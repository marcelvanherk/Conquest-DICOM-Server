/*
20030117	ljz	Created
20030415	ljz	Better exception handling
20030417	ljz	Support for multi-frame images
20041221	ljz	Changed include files
20050215	ljz	Fixed for fragmented frames in a sequence
20050901	mvh	Renamed errorcodes.h to errcodes.h
20071001	mvh	output of JPEG error messages to OperatorConsole
20080819	mvh	Suppress large amounts of jpeg errors; added \n
20081116	mvh	Adapt for very large objects (use unsigned int lenght)
*/

#include <setjmp.h>
#include "dicom.hpp"
#include "dprintf.hpp"
#include "errcodes.h"


#define		Uint8	unsigned char
#define		Uint16	unsigned short
#define		Uint32	unsigned int

typedef struct
{ int				iCurFragment;
  int				iSequenceSize;
  Array<DICOMDataObject*>*	pADDO;
  int                           error_count;
} JPEG_SEQUENCE;


extern "C"
{

#define XMD_H
#define NEED_FAR_POINTERS
#define boolean	hooligan
/* Note in ijg libraries, boolean is defined as an int */

#include "libijg12\\jpeglib12.h"

static void DcmJpegInitSource(j_decompress_ptr cinfo)
{ /* If no code here, then the next routine will be called */
}

static boolean DcmJpegFillInputBuffer(j_decompress_ptr cinfo)
{ VR*				pVR;
  struct jpeg_source_mgr*	src;
  Array<DICOMDataObject*>*	pADDO;
  DICOMDataObject*		pDDO;
  JPEG_SEQUENCE*		pSequence;

  src = cinfo->src;

  pSequence = (JPEG_SEQUENCE*)cinfo->client_data;
  pADDO = pSequence->pADDO;

  if (!pADDO)
    return (int)FALSE;
  if (pSequence->iCurFragment >= pSequence->iSequenceSize)
    return (int)FALSE;

  /* Get the VR containing the compressed pixels of current Frame */
  pDDO = pADDO->Get(pSequence->iCurFragment++);
  if (!pDDO)
    return (int)FALSE;
  pVR  = pDDO->GetVR(0xfffe, 0xe000);
  if (!pVR)
    return (int)FALSE;
    
  src->next_input_byte = (unsigned char*)pVR->Data;
  src->bytes_in_buffer = pVR->Length;
  return (int)TRUE;
}

static void DcmJpegSkipInputData(j_decompress_ptr cinfo, long num_bytes)
{
}

static void DcmJpegTermSource(j_decompress_ptr cinfo)
{
}


}

// private error handler struct
struct IJG_ErrorStruct
{
    // the standard IJG error handler object
    struct jpeg_error_mgr pub;
    // our jump buffer
    jmp_buf setjmp_buffer;
};

// error handler, executes longjmp
static void IJG_ErrorExit(j_common_ptr cinfo)
{
  IJG_ErrorStruct *myerr = (IJG_ErrorStruct *)cinfo->err;
  longjmp(myerr->setjmp_buffer, 1);
}

METHODDEF(void)
my_output_message (j_common_ptr cinfo)
{
  JPEG_SEQUENCE*		pSequence;
  pSequence = (JPEG_SEQUENCE*)cinfo->client_data;

  char buffer[JMSG_LENGTH_MAX];
  strcpy(buffer, "***");

  /* Create the message */
  (*cinfo->err->format_message) (cinfo, buffer+3);
  strcat(buffer, "\n");

  if (pSequence->error_count<10) OperatorConsole.printf(buffer);
  pSequence->error_count++;
}

int DecompressJPEG12(SLICE_INFO* pSliceInfo, VR* pSequence,
	void** ppResult, unsigned int* piResultSize)
{ int				i, rc = DCM_E_UNSUPPORTED_COMPRESSION;
  int				iNbFrames, iNbRows;
  Array<DICOMDataObject*>*	pADDO;
  struct jpeg_decompress_struct	cinfo;
  struct IJG_ErrorStruct	jerr;
  struct jpeg_source_mgr	src;
  void*				pDecompressed = NULL;
  char*				pCurFrame;
  size_t			rowsize = 0;
  JPEG_SEQUENCE			JpegSequence;

  JpegSequence.error_count=0;

  pADDO = (Array<DICOMDataObject*>*)pSequence->SQObjectArray;

  iNbFrames = pSliceInfo->iNbFrames;
  if (iNbFrames <= 0)
    iNbFrames = 1;

  /* Jump out of JPEG lib in case of trouble */
  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = IJG_ErrorExit;
  jerr.pub.output_message = my_output_message;
  if (setjmp(jerr.setjmp_buffer))
  { /* This code is executed when IJG_ErrorExit executes a longjmp */
    jpeg_destroy_decompress(&cinfo);
    return DCM_E_UNSUPPORTED_COMPRESSION;
  }

  rc = DCM_E_OK;

  JpegSequence.iCurFragment = 1;
  JpegSequence.iSequenceSize = pADDO->GetSize();
  JpegSequence.pADDO = pADDO;

  for (i=0; i<iNbFrames; i++)
  { /* JPEG lib interface */
    jpeg_create_decompress(&cinfo);
    memset(&src, 0, sizeof(src));
    src.init_source       = DcmJpegInitSource;
    src.fill_input_buffer = DcmJpegFillInputBuffer;
    src.skip_input_data   = DcmJpegSkipInputData;
    src.resync_to_restart = jpeg_resync_to_restart;
    src.term_source       = DcmJpegTermSource;
    cinfo.src = &src;
    cinfo.client_data = (void*)&JpegSequence;

    /* GO ! */
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    JSAMPARRAY buffer = NULL;
    int bufsize = 0;

    bufsize = cinfo.output_width * cinfo.output_components; // number of JSAMPLEs per row
    rowsize = bufsize * sizeof(JSAMPLE); // number of bytes per row
    buffer = (cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, bufsize, 1);
    if (!buffer)
    { rc = DCM_E_MEMORY;
      goto FINISH;
    }
    iNbRows = cinfo.output_height;
    if (!pDecompressed)
    { /* Allocate the output */
      pDecompressed = malloc(iNbFrames * iNbRows * rowsize + 1);
      if (!pDecompressed)
      { rc = DCM_E_MEMORY;
	goto FINISH;
      }
    }

    /* Following loop decompresses one frame */
    pCurFrame = (char*)pDecompressed + i * iNbRows * rowsize;
    while (cinfo.output_scanline < cinfo.output_height)
    { if (jpeg_read_scanlines(&cinfo, buffer, 1) == 0)
      { rc = DCM_E_UNSUPPORTED_COMPRESSION;
	goto FINISH;
      }
      memcpy(pCurFrame + (cinfo.output_scanline-1) * rowsize, *buffer, rowsize);
    }

    if (i < iNbFrames - 1)
    { /* Still frames to go */
      if (jpeg_finish_decompress(&cinfo) == FALSE) 
      { rc = DCM_E_UNSUPPORTED_COMPRESSION;
	goto FINISH;
      }
      jpeg_destroy_decompress(&cinfo);
    }
  }
FINISH:
  if (jpeg_finish_decompress(&cinfo) == FALSE) 
    rc = DCM_E_UNSUPPORTED_COMPRESSION;
  jpeg_destroy_decompress(&cinfo);

  if (rc == DCM_E_OK)
  { *ppResult = pDecompressed;
    *piResultSize = iNbFrames * iNbRows * rowsize;
  }
  else
  { *ppResult = NULL;
    *piResultSize = 0;
  }

  if (JpegSequence.error_count>10) OperatorConsole.printf("***suppressed %d further messages for this object\n", JpegSequence.error_count-10); 

  return rc;
}



