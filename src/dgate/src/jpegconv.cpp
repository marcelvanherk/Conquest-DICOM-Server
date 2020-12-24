//
//  jpegconv.cpp
//
/*
20120710 bcb Created.
20130218 mvh JPEG compression did not set right sopclass!!!!
20130226 bcb Replaced gpps with IniValue class, fixed size warnings.
                Version to 1.4.18a and b.
20131207 bcb add jpeg-ls decode
20160220 mvh Made to compile in mvh's build environment: bcb please check these changes were applied
	20121208	mvh	Block deplane if SamplesPerPixel is not 3 - checked 20170430 is in nkiqrsop
	20130823	mvh	Fixed lossy jpeg compression for 12 bits data - likely in nkiqrsop
	20140209	mvh 	Fix decompressjpegl here: fix lossless colorspace to RGB - fixed 20170430 (was in bcb's newer version)
	20140309	mvh 	Catch error if no image VR present in jpeg2000 decompression - likely in imagedata
	20160229        mvh     potential issue: calloc/malloc on this side free in opj_image_destroy - fixed by using static libraries
20160306 mvh use CHARLS_STATIC and statically link libraries to avoid calloc/opj_image_destroy issue above
20170205 mvh Define guard as extra space for botched compression
20170205 mvh Move image creating inside frame loop of jpeg2000 compression to avoid crash on multiframe
             Known issue: 12 bits images with too many bit not error free in jpegLS and jpeg2000
20170208 mvh Check input pixel value against precision; refuse to compress if outside
20170209 mvh Adapted for charls1x
20170430 mvh Reapplied 20140209 change: lossless RGB jpeg is not YCR coded (was in newer incompatible bcb version)
20170522 mvh Reapplied 20150619 change: copy color space information
20170528 mvh Test on JFIF marker in 20140209 change for lossless RGB jpeg is not YCR coded
20170827 mvh No longer use IU type for transfer syntax must be UI
20181123 mvh Changed TransferSyntaxUID back to IU (is in preamble)
20200314 mvh Added JPEGIGNORELOSSLESSJFIFCOLORSPACE flag, ignores color space in JFIF header for lossless
20200722 mvh Note: lossless JPEG compression converts color space to YBR_FULL which is incorrect
20200722 mvh Note: lossless JPEG decompression ignores YBR_FULL which leads to old conquest color images to show green
20201224 mvh Added StopOnJPEGWarnings configuration to fail decompress on corrupted images
*/

#define guard 265536

#include "jpegconv.hpp"
#include "nkiqrsop.hpp"
#include "dprintf.hpp"
#include "uncompconv.hpp"
#include <time.h>

#ifdef HAVE_LIBJASPER
#include "jasper/jasper.h"
#endif
#ifdef HAVE_LIBOPENJPEG
#include "openjpeg.h"
#ifdef HAVE_LIBOPENJPEG2
#include "cio.h"
#include "event.h"
#endif //HAVE_LIBOPENJPEG2
#endif //HAVE_LIBOPENJPEG
#ifdef HAVE_LIBCHARLS
#ifdef _WIN32
//Can also be CHARLS_STATIC
#define CHARLS_STATIC
#endif
#include "interface.h"
//#include "encoderstrategy.h"
#endif //HAVE_LIBCHARLS

#ifndef UINT32_MAX
#define UINT32_MAX 0xFFFFFFFF
#endif
#ifndef INT32_MAX
#define INT32_MAX 0x7FFFFFFF
#endif

#define MIN_QUALITY 1

extern	int	DebugLevel;

struct size_struct
{
    INT     frames;
    INT     currFrame;
    UINT    height;
    UINT    width;
    UINT8   components;
    UINT8   precision_allocated;
    UINT8   precision;
    UINT8   byteWidth;
    UINT32  frameSize;
    UINT32  total;
};
BOOL getImageSizes(DICOMDataObject* pDDO, size_struct *size, const char *who)
{
    // Are there frames?
    if(!(size->frames = pDDO->Getatoi(0x0028, 0x0008))) size->frames = 1;
    size->currFrame = 0;
    // Width, height, and componets for uncompressed memory allocation.
    if(!(size->height = pDDO->GetUINT(0x0028, 0x0010)))
    {
        SystemDebug.printf("***[%s]: Failed to get image height.\n", who);
        return(FALSE);
    }
    // Get the Columns VR and check size.
    if(!(size->width = pDDO->GetUINT(0x0028, 0x0011)))
    {
        OperatorConsole.printf("***[%s]: Failed to get image width.\n", who);
        return(FALSE);
    }
    // Get the number of samples per pixel VR.
    if(!(size->components = pDDO->GetBYTE(0x0028, 0x0002)))
        size->components = 1;  // Gray default.
    // Get the number of bits allocated.
    if(!(size->precision_allocated = pDDO->GetBYTE(0x0028, 0x0100)))
        size->precision_allocated = 8; // 8 bit default.
    // Get the number of bits stored.
    if(!(size->precision = pDDO->GetBYTE(0x0028, 0x0101)))
        size->precision = 8; // 8 bit default.
    size->byteWidth = 1;
    if(size->precision_allocated > 8 || size->precision > 8) size->byteWidth = 2;
    // Set the size of each image
    size->frameSize = size->width * size->height * size->components *  size->byteWidth;
    size->total = size->frameSize * size->frames;
    if(size->total & 1 ) size->total++;//Odd length, make it even.
    return TRUE;
}

#ifdef HAVE_LIBJPEG //The jpeg common stuff.
/* Here's the extended error handler struct for libjpeg:
 (from jpeg example.c)*/
struct jerror_mgr 
{ 
    struct jpeg_error_mgr pub;	/* "public" fields */
    jmp_buf setjmp_buffer;	/* for return to caller */
};
typedef struct jerror_mgr * jerror_ptr;

typedef struct 
{ 
    struct jpeg_source_mgr pub;	/* public fields */
} jsource_mgr;

typedef jsource_mgr * jsrc_ptr;
#endif //HAVE_LIBJPEG

#ifdef HAVE_LIBJPEG //The jpeg common stuff.
/* This is a replacement for the libjpeg message printer that normally
 *  goes to stderr. */
METHODDEF(void)
joutput_message (j_common_ptr cinfo)
{ 
    char buffer[JMSG_LENGTH_MAX];
    
    /* Create the message */
    (*cinfo->err->format_message) (cinfo, buffer);
    
    OperatorConsole.printf("***[JPEG Library]: %s\n", buffer);
}

/* This is a replacement for the libjpeg error handler that normally
 *  halts the program. */
METHODDEF(void)
jerror_exit (j_common_ptr cinfo)
{
    /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
    jerror_ptr jerr = (jerror_ptr) cinfo->err;
    
    /* Always display the message */
    (*cinfo->err->output_message) (cinfo);
    
    /* Return control to the setjmp point */
    longjmp(jerr->setjmp_buffer, 1);
}

//Just for routines we do not need or use.
METHODDEF(void)
jdoes_nothing_decomp (j_decompress_ptr cinfo)
{
    //Nothing
    UNUSED_ARGUMENT(cinfo);
    return;
}

METHODDEF(void)
jdoes_nothing_comp (j_compress_ptr cinfo)
{
    //Nothing
    UNUSED_ARGUMENT(cinfo);
    return;
}

METHODDEF(boolean)
jreturns_true_comp (j_compress_ptr cinfo)
{
    //Nothing
    UNUSED_ARGUMENT(cinfo);
    return TRUE;
}

#else// Use jpeg_encoder.cpp
UINT32 encode_image (UINT8 *input_ptr, UINT8 *output_ptr, UINT32 quality_factor, UINT32 image_format, UINT32 image_width, UINT32 image_height);
#endif

BOOL ToJPG(DICOMDataObject* pDDO, char *filename, int size, int append, int level, int window, unsigned int frame)
{ int 			dimx, dimy;
    unsigned char 	*out;//, c;
    FILE 			*f;
    int 			i, Ratefps;
#ifdef HAVE_LIBJPEG
    struct                jpeg_compress_struct cinfo;
    unsigned int          rowWidth;
    struct jerror_mgr     jerr;
    JSAMPROW     row_pointer;
#endif
    
    if (frame>10000 && GetNumberOfFrames(pDDO)>1)
    { char file[256], file2[256];
        DICOMDataObject *DDO2 = MakeCopy(pDDO);
        NewTempFile(file);
        for (i=0; i<GetNumberOfFrames(pDDO); i++)
        { sprintf(file2, "%s.%05d", file, i);
            ToJPG(DDO2, file2, size, 0, level, window, i);
            delete DDO2;
        }
        
        Ratefps = (frame % 100);
        if (Ratefps==0) Ratefps=1;
        
        // to convert a row of jpeg images to mpeg2 using mjpegtools:
        // jpeg2yuv -f 25 -I p -j dgate%03d.jpg | mpeg2enc -f 3 -b 1500 -o mpegfile.m2v
        //          rate  progr files                      mpg2  bitrate   file
        sprintf(file2, "jpeg2yuv -f %d -I p -j %s%%05d | mpeg2enc -f 3 -b 1500 -o %s", Ratefps, file, filename);
        system(file2);
        
        for (i=0; i<GetNumberOfFrames(pDDO); i++)
        { sprintf(file2, "%s.%05d", file, i);
            unlink(file2);
        }
        return TRUE;
    }
    
    if (To8bitMonochromeOrRGB(pDDO, size, &dimx, &dimy, &out, 2, level, window, frame))
    { if (append) f = fopen(filename, "ab");
    else        f = fopen(filename, "wb");
        if (!f)
        { free(out);
            return FALSE;
        }
#ifdef HAVE_LIBJPEG
        
        // Init the default handler.
        cinfo.err = jpeg_std_error(&jerr.pub);
        // change the error exit so libjpeg can't kill us.
        jerr.pub.error_exit = jerror_exit;
        // Use our methode for outputting messages.
        jerr.pub.output_message = joutput_message;
        if (setjmp(jerr.setjmp_buffer))
        {
            /* If we get here, the JPEG code has signaled an error.
             * We need to clean up the JPEG object and return. */
            jpeg_destroy_compress(&cinfo);
            if(f != NULL)
            {
                fclose(f);
                if (!append)unlink(filename);
                free(out);
            }
            return (FALSE);
        }
        //Look for multi-byte version 6c (63) from www.suelix.com
        jpeg_CreateCompress(&cinfo, 63, (size_t) sizeof(struct jpeg_compress_struct));
        // Color space must be set before a call to jpeg_set_defaults.
        cinfo.in_color_space = JCS_RGB; //To8bitMonochromeOrRGB is set for rgb, gray could be faster.
        jpeg_set_defaults(&cinfo);
        // Get all the image size stuff.
        cinfo.image_height =  dimy;
        cinfo.image_width =  dimx;
        cinfo.num_components = 3;
        cinfo.input_components = cinfo.num_components;
        cinfo.data_precision = 8;
        cinfo.data_precision_other = cinfo.data_precision;
        jpeg_set_quality(&cinfo, 95, TRUE);//100 is silly in lossy and the same as 95.
        cinfo.jpeg_color_space = cinfo.in_color_space;
        jpeg_default_colorspace(&cinfo);
        // Set where to put it.
        jpeg_stdio_dest(&cinfo, f);
        rowWidth =  cinfo.image_width * cinfo.num_components;
        jpeg_start_compress(&cinfo, TRUE);
        while (cinfo.next_scanline < cinfo.image_height)
        {
            row_pointer = &(((JSAMPROW)out)[cinfo.next_scanline * rowWidth]);
            jpeg_write_scanlines(&cinfo, &row_pointer, 1);
        }
        // If here, finished the compression.
        jpeg_finish_compress(&cinfo);
        jpeg_destroy_compress(&cinfo);
#else // Old way.
        unsigned char *output = (UINT8 *)malloc(dimx * dimy * 3);
        UINT32 len;
        len = encode_image (out, output, 256, 14, dimx, dimy);
        fwrite (output, 1, len, f);
        free(output);
#endif
        free(out);
        fclose(f);
        return TRUE;
    }
    
    return FALSE;
}

#ifndef SUELIX
extern int gJpegQuality;// The quality of the lossy jpeg or j2k image from dicom.ini.
#else
#include "dbsql.hpp"
#endif

int LossyQuality()
{ 
#ifdef SUELIX
  return IniValue::GetInstance()->sscscpPtr->LossyQualityreturn;
#else
  return gJpegQuality;
#endif
}

#ifdef HAVE_LIBJPEG
/* This routine will take all of the listed color spaces in 08_03 of the standard
 *  in little endian, uncompressed format and convert it to YBR_FULL and than compress
 *  it to jpeg.  YBR_FULL is the IJG standard for the Jpeg-6b library used everywhere.
 *  I did not use the Jpeg-6b version because it had to be compiled for 8, 12 or 16
 *  only.  So I wrote Jpeg-6c that can change bit width on the fly.  You can get it here:
 *  http://www.suelix.com/downloads/jpeg-6c.zip
 *  I included spectral sel "1.2.840.10008.1.2.4.53" and progressive
 *  "1.2.840.10008.1.2.4.55", but both are obsolete and maybe I should remove them.
 *  jpegQuality is meaningless for lossless and  0 for use "iniValuePtr->sscscpPtr->LossyQuality" in IniValue Class.
 *  Maybe one day all of the format decoding should be moved into it's own routine.
 *  If I have made some mistakes (most likely) you can contact me bruce.barton
 *  (the mail symbol goes here) me.con.  Let me know where I can find a sample of
 *  the image that didn't work. */
BOOL CompressJPEGL(DICOMDataObject* pDDO, int comp, int jpegQuality)
{
    ImageData                   imageData;
    VR				*vrs;
    BOOL			progressive;
    unsigned int		rowWidth;
    clock_t                     starttime;
    JSAMPROW                    row_pointer[1];
    struct jpeg_compress_struct	cinfo;
    struct jerror_mgr		jerr;
    
//If debug > 0, get start time.
    if (DebugLevel > 0)starttime = clock();
    if (DebugLevel > 1) SystemDebug.printf("JPEG compress started.\n");
//Get the image info.
    if(!imageData.GetImageData(pDDO, "CompressJPEG", TRUE)) return FALSE;
//See if its real.
    if (!imageData.CalculateImageLengths())
    {//Should of had it, look for arrays
        if(!imageData.GetEncapsulatedDDO(NULL)) return FALSE;//no marker
    }
//Make the input standard.
    if (imageData.components == 1)
    {
        if (!imageData.MakeStdGray()) return FALSE;
    }
    else if (!imageData.MakeYBR_FULL_OrRGB(FALSE)) return FALSE;//No planes!
#if NATIVE_ENDIAN != LITTLE_ENDIAN //Big Endian, not sure why!  bcb--untested.
    if(imageData.precision_allocated > 8)SwapEndian((unsigned char *)imageData.imageVR->Data, imageData.imageVR->Length);// Byte swap time
#endif //Big Endian
//Create the encapsulation array.
    if (!imageData.CreateOutputArray()) return FALSE;
//Check and set the quality for lossy.
    if(jpegQuality < MIN_QUALITY)// Set to 0 to use dicom.ini value.
    {
        jpegQuality = LossyQuality();//Use the default or dicom.ini value.
    }
    if(jpegQuality > 100) jpegQuality = 100;
//Init the default handler.
    cinfo.err = jpeg_std_error(&jerr.pub);
//Change the error exit so libjpeg can't kill us.
    jerr.pub.error_exit = jerror_exit;
//Use our methode for outputting messages.
    jerr.pub.output_message = joutput_message;
//Set the error entry point.
    if (setjmp(jerr.setjmp_buffer))
    {
    //If we get here, the JPEG code has signaled an error.
    // We need to clean up the JPEG object and return.
        jpeg_destroy_compress(&cinfo);
        return (FALSE);
    }
//Look for multi-byte version 6c (63) from www.suelix.com.
    jpeg_CreateCompress(&cinfo, 63,
                        (size_t) sizeof(struct jpeg_compress_struct));
// Set the destination.
    cinfo.dest = (struct jpeg_destination_mgr *)
    (*cinfo.mem->alloc_small) ((j_common_ptr) &cinfo, JPOOL_PERMANENT,
                               sizeof(struct jpeg_destination_mgr));
    cinfo.dest->init_destination = jdoes_nothing_comp;
    cinfo.dest->empty_output_buffer = jreturns_true_comp;
//Pass buffer big enough for the whole image, so this is an error that should not happen.
    cinfo.dest->term_destination = jdoes_nothing_comp;
//Color space must be set before a call to jpeg_set_defaults.
    cinfo.in_color_space = JCS_RGB;
    switch (imageData.colorSpace)
    {
        case CLRSPC_FAM_GRAYSCALE:
            cinfo.in_color_space = JCS_GRAYSCALE;
            break;
        case CLRSPC_FAM_YBR_FULL:
            cinfo.in_color_space = JCS_YCbCr;// Y/Cb/Cr (also known as YUV).
            break;
        default:// red/green/blue.
            break;
    }
    jpeg_set_defaults(&cinfo);
//Get all the image size stuff.
    if((cinfo.image_height = imageData.height) > JPEG_MAX_DIMENSION)
    {
        OperatorConsole.printf("***[CompressJPEGL]: %d too high for the jpeg standard.\n",
                               cinfo.image_height);
        jpeg_destroy_compress(&cinfo);
        return(FALSE);
    }
    if((cinfo.image_width = imageData.width ) > JPEG_MAX_DIMENSION)
    {
        OperatorConsole.printf("***[CompressJPEGL]: %d too wide for the jpeg standard.\n",
                               cinfo.image_width);
        jpeg_destroy_compress(&cinfo);
        return(FALSE);
    }
//Get the number of samples per pixel VR.
    cinfo.num_components = imageData.components;
    cinfo.input_components = cinfo.num_components;
//Get the number of bits allocated.
    cinfo.data_precision_other = imageData.precision_allocated;//Made into 8 or 16 make standard.
//Get the number of bits stored, jpeg only knows 8,12, &16.
    if (imageData.precision > 12) cinfo.data_precision = 16;
    else if (imageData.precision > 8) cinfo.data_precision = 12;
    else cinfo.data_precision = 8; // 8 bit default.
    progressive = FALSE;
    switch(comp)
    {
        case '2':// lossless SV6 "1.2.840.10008.1.2.4.57" = jpeg 14 (33%)
            jpegQuality = 100;
            if(cinfo.data_precision > 8) cinfo.lossless = TRUE;// Tell 8 bit after colorspace
            comp = 2;
            break;
        case '3':// baseline   (8 bits ) "1.2.840.10008.1.2.4.50" = jpeg 1  (15%)
        case '4':// extended   (12 bits) "1.2.840.10008.1.2.4.51" = jpeg2, 4  (15%)
            if(cinfo.data_precision <= 12)// Else fall though 5 and 6 to 1.
            {
                if(cinfo.data_precision > 8)
                {
                    cinfo.data_precision = 12;
                    jpeg_set_quality(&cinfo, jpegQuality, false);
                    comp = 4;
                }
                else
                {
                    cinfo.data_precision = 8;
                    jpeg_set_quality(&cinfo, jpegQuality, true);
                    comp = 3;
                }
                break;
            }
        case '5': // spectral sel 	"1.2.840.10008.1.2.4.53" = jpeg 8  (15%) Obsolete!
        case '6': // progressive		"1.2.840.10008.1.2.4.55" = jpeg 12 (14%) Obsolete!
            if(cinfo.data_precision <= 12)// Else fall though.
            {
                progressive = TRUE;
                if(cinfo.data_precision > 8)
                {
                    cinfo.data_precision = 12;
                    comp = 6;
                }
                else
                {
                    cinfo.data_precision = 8;
                    comp = 5;
                }
                jpeg_set_quality(&cinfo, jpegQuality, false);
                jpeg_simple_progression(&cinfo);
                break;
            }
        default: //default lossless SV1 "1.2.840.10008.1.2.4.70" = jpeg 14 (33%)
            //16 bit jpeg id always lossless.
            comp = 1;
            jpegQuality = 100;
            if(cinfo.data_precision > 8)cinfo.lossless = TRUE;// Tell 8 bit after colorspace
    }
// Setup for compress.
    cinfo.jpeg_color_space = cinfo.in_color_space; // merged from bcb 20150619
    jpeg_default_colorspace(&cinfo);
//lossless must be set after colorspace, but color space need to know lossless if not 8 bits.
    if(comp <= 2)// Lossless J1 or J2.
    {
        cinfo.lossless = TRUE;
        cinfo.lossless_scaling = FALSE;
        if(comp == 2)jpeg_simple_lossless(&cinfo, 6, 0);//(J2)Predictor = 6, point_transform = 0
        else jpeg_simple_lossless(&cinfo, 1, 0);//(J1)Predictor = 1, point_transform = 0
    //Color space for RGB is YCbCr, set if needed, lossless messes it up.
        if(cinfo.in_color_space == JCS_RGB) jpeg_set_colorspace(&cinfo, JCS_YCbCr);
    }
//Set the obsolete progressive if called for.
    if(progressive)jpeg_simple_progression(&cinfo);
//Print out some info for debug.
    if (DebugLevel > 2)
    {
        if(cinfo.lossless)SystemDebug.printf("JPEG Lossless");
        else SystemDebug.printf("JPEG Lossy Quality = %d", jpegQuality);
        if (DebugLevel > 3)
        {
            if(progressive)SystemDebug.printf(", progressive");
        }
        SystemDebug.printf( "\n, H = %d, W = %d, Bits = %d in %d, Frames = %d\n",
                           cinfo.image_width, cinfo.image_height, cinfo.data_precision,
                           cinfo.data_precision_other, imageData.frames);
    }
//Start the frames loop
    while(TRUE)
    {
    //Check for enough data.
        if (!imageData.CheckEnoughData(TRUE)) break;
    //Set where to put it. Make it as big as uncompressed data.
        if (!imageData.CreateOutputBuffer(imageData.frameSize+guard))
        {
            jpeg_destroy_compress(&cinfo);
            return ( FALSE );
        }
        cinfo.dest->free_in_buffer = imageData.frameSize+guard;//The max that can be stored.
        cinfo.dest->next_output_byte = (JOCTET *)imageData.outBuffer;
    //Get the library ready for data.
        jpeg_start_compress(&cinfo, TRUE);
    //Jpeg library is feed one row at a time.
        rowWidth =  cinfo.image_width * cinfo.num_components * imageData.byteWidth;
        while (cinfo.next_scanline < cinfo.image_height)
        {
            row_pointer[0] = &(((JSAMPROW)imageData.inPtr)[cinfo.next_scanline * rowWidth]);
            jpeg_write_scanlines(&cinfo, &row_pointer[0], 1);
        }
    //If here, finished the compression without errors.
        jpeg_finish_compress(&cinfo);
    //Time to handle errors on our own again, the cinfo still exists.
        if (DebugLevel > 1)
            SystemDebug.printf("JPEG Library compress %d bytes to %d bytes\n", imageData.frameSize,
                               imageData.frameSize+guard - cinfo.dest->free_in_buffer);
    //Encapsulate an image object.
        imageData.AddToOutputArray(imageData.frameSize+guard - (cinfo.dest->free_in_buffer & (UINT32)-1));
        imageData.currFrame++;
        if(imageData.currFrame >= imageData.frames)break;//Finished while() loop
    //Still here, setup for the next frame
        imageData.inPtr += imageData.frameSize;
    //If enough data, next loop.
        if(imageData.CheckEnoughData(FALSE)) continue;
    // Deal with silly input arrays
        if(!imageData.GetEncapsulatedVR(NULL)) break;
    //New data, time to fix, make the input standard.
        if (imageData.components == 1)
        {
            if (!imageData.MakeStdGray()) return FALSE;
        }
        else if (!imageData.MakeYBR_FULL_OrRGB(FALSE)) return FALSE;//No planes!
#if NATIVE_ENDIAN != LITTLE_ENDIAN //Big Endian, not sure why!  bcb--untested.
        if(imageData.precision_allocated > 8)SwapEndian((unsigned char *)imageData.imageVR->Data, imageData.imageVR->Length);// Byte swap time
#endif //Big Endian
    }//End of while(TRUE), go back for the next frame.

    // report failure on warnings
    if (jerr.pub.num_warnings)
        {
        char szRootSC[64], buffer[64];
	if (MyGetPrivateProfileString(RootConfig, "MicroPACS", RootConfig, szRootSC, 64, ConfigFile))
		{
		MyGetPrivateProfileString(szRootSC, "StopOnJPEGWarnings", "0", buffer, 64, ConfigFile);
		if (atoi(buffer))          
                        {
                        jpeg_destroy_compress(&cinfo);
                        return (FALSE);
                        }
                }
        }

//All frames compressed, done with cinfo.
    jpeg_destroy_compress(&cinfo);
//Should we kill it and keep the uncompressed data?
    imageData.CheckEnoughFrames();
//The end object.
    imageData.AddToOutputArray((UINT32)-1);
//Save the new data.
    imageData.SwitchToOutputArray();
//Needed for anything that stores <= 8 in 16 (now 8 in 8).
    imageData.FixPrecision(cinfo.data_precision);
//Change the transfer syntax to JPEG!  Many choices, do it old way.
    vrs = pDDO->GetVR( 0x0002, 0x0010 );
    if(vrs)
    {
        if(vrs->Length != 22)vrs->ReAlloc(22);
    }
    else
    {
        vrs = new VR( 0x0002, 0x0010, 22, TRUE);
        vrs->TypeCode = 'IU';
        pDDO->Push(vrs);
    }
    memcpy((char *)vrs->Data, "1.2.840.10008.1.2.4.", 20);
    switch(comp)// here was a big bug, used e.g. '2' instead of 2
    {
        case 2: memcpy(&((char *)vrs->Data)[20], "57", 2);//Lossless J2, SV6
            break;
        case 3: memcpy(&((char *)vrs->Data)[20], "50", 2);//Lossy baseline (8 bits)
            break;
        case 4: memcpy(&((char *)vrs->Data)[20], "51", 2);//Lossy extended (12 bits)
            break;
        case 5: memcpy(&((char *)vrs->Data)[20], "53", 2);// Obsolete! Progressive (8 bits)
            break;
        case 6: memcpy(&((char *)vrs->Data)[20], "55", 2);// Obsolete! Progressive (12 bits)
            break;
        default:  memcpy(&((char *)vrs->Data)[20], "70", 2);//Lossless J1, SV1
    }
//Change the Photometric Interpretation if needed.
    if(imageData.components > 1) imageData.SetColorTo("YBR_FULL\0",FALSE);
    if (DebugLevel > 0)
    {
        SystemDebug.printf("JPEG compress time %u milliseconds.\n",clock() - starttime);
    }
    return (TRUE);
}

//This is a replacement for the get more data for the buffer routine.
// Because it is passed the whole image, it should never ask for more.
METHODDEF(boolean)
jfill_input_buffer (j_decompress_ptr cinfo)
{
    // Passed libjpeg the whole image, there is no more data, error exit.
    OperatorConsole.printf("***[LIBJPEG]: Ran out of data in image.\n");
    ERREXIT(cinfo, JERR_INPUT_EOF);
    
    return TRUE;
}

// This was taken directly from the library with the error message changed.
METHODDEF(void)
jskip_input_data (j_decompress_ptr cinfo, long num_bytes)
{
    jpeg_source_mgr *src = cinfo->src;
//Just a dumb implementation for now.  Could use fseek() except
// it doesn't work on pipes.  Not clear that being smart is worth
// any trouble anyway --- large skips are infrequent.
    if (num_bytes > 0)
        if (num_bytes > (long) src->bytes_in_buffer)
        {
            OperatorConsole.printf("***[LIBJPEG]: Tried to skip past the end of the image.\n");
            ERREXIT(cinfo, JERR_INPUT_EOF);
        }
    src->next_input_byte += (size_t) num_bytes;
    src->bytes_in_buffer -= (size_t) num_bytes;
}

/* This routine will take in a jpeg image and convert it to little endian, uncompressed,
 *  RGB or YBR_FULL format.  RGB and YBR_FULL is the IJG standard output for the Jpeg-6b
 *  library used everywhere.  As stated before, I did not use the Jpeg-6b version because
 *  it had to be compiled for 8, 12 or 16 only.  So I wrote Jpeg-6c that can change bit
 *  width on the fly.  You can get it here:
 *  http://www.suelix.com/downloads/jpeg-6c.zip
 *  If I have made some mistakes (most likely) you can contact me bruce.barton
 *  (the mail symbol goes here) me.com.  Let me know where I can find a sample of
 *  the image that didn't work. */

BOOL DecompressJPEGL(DICOMDataObject* pDDO)
{
    ImageData                   imageData;
    const unsigned char         jpegSOImarker[3] = { 0xFF, 0xD8, 0 };
#ifdef FUJI_FIX
    const unsigned char         fujiJpegSOImarker[10] = { 0xFF, 0xD8, 0xFF, 0xE0,
                                    0x00, 0x10, 'J','F', 'I', 'F' };
    int                         cnt;
#endif
    char                        *uncompImageBptr;
    unsigned int                outBytes;
    BOOL                        leExplict;
    clock_t                     starttime;
//libjpeg stuff
    JSAMPARRAY16                    outBuffer;
    JDIMENSION                      num_scanlines, row, rowWidth, pixcnt;
    struct jpeg_decompress_struct   cinfo;
    struct jerror_mgr               jerr;

    // If debug > 0, get start time.
    if (DebugLevel > 0)starttime = clock();
    if (DebugLevel > 1) SystemDebug.printf("JPEG decompress started.\n");
    leExplict = TRUE; // Only changed by FUJI_FIX
//Get the image data.
    if(!imageData.GetImageData(pDDO, "DecompressJPEGL", FALSE)) return FALSE;
#ifdef FUJI_FIX
//See if its not encapsulated data.
    if (imageData.CalculateImageLengths())
    {
     //Look for a Jfif header. (LittleEndianImplicit, jpeg compressed,not encapsulated, how evil)
        for(cnt = 0; cnt < 10; cnt++) if(*(unsigned char *)(imageData.inPtr)++ != fujiJpegSOImarker[cnt])break;;
        if (cnt == 10)
        {
            SystemDebug.printf("Warn[DecompressJPEGL]: Applying Fuji Fix\n");
            leExplict = FALSE;
        }
    }
#endif
    if(leExplict)//Not FUJI image or no FUJI_FIX code.
    {//Should be in an array, look for it there.
        if(!imageData.GetEncapsulatedDDO(jpegSOImarker))return FALSE;//No Jpeg data found
    }
//Init the default handler
    cinfo.err = jpeg_std_error(&jerr.pub);
//Change the error exit so libjpeg can't kill us.
    jerr.pub.error_exit = jerror_exit;
//Use our methode for outputting messages.
    jerr.pub.output_message = joutput_message;
    if (setjmp(jerr.setjmp_buffer))
    {
    // If we get here, the JPEG code has signaled an error.
    // We need to clean up the JPEG object and return.
        jpeg_destroy_decompress(&cinfo);
        return (FALSE);
    }
    
//Look for multi-byte version 6c (63) from www.suelix.com
    jpeg_CreateDecompress(&cinfo, 63,
                (size_t) sizeof(struct jpeg_decompress_struct));
//Set the source
    cinfo.src = (struct jpeg_source_mgr *)
    (*cinfo.mem->alloc_small) ((j_common_ptr) &cinfo, JPOOL_PERMANENT,
                               sizeof(struct jpeg_source_mgr));
    cinfo.src->init_source = jdoes_nothing_decomp;
    cinfo.src->skip_input_data = jskip_input_data;
    cinfo.src->resync_to_restart = jpeg_resync_to_restart; //Use default method.
    cinfo.src->term_source = jdoes_nothing_decomp; //Term_source does nothing.
//Passing the whole image, so this is an error that should not happen.
    cinfo.src->fill_input_buffer = jfill_input_buffer;
//Get data for the first or only frame.
    cinfo.src->bytes_in_buffer = imageData.imageVR->Length; //Forces fill_input_buffer on first read.
    cinfo.src->next_input_byte = (JOCTET *)imageData.imageVR->Data;//The image data.
//Find out what we have.
    jpeg_read_header(&cinfo, TRUE);
//Set all of the cinfo information.
    jpeg_calc_output_dimensions(&cinfo);

//The default is 8 bits, set it to the image size.
    cinfo.data_precision_other = cinfo.data_precision;

//Should be changed for an alpha chanel?
    if (cinfo.out_color_components != 1 && cinfo.out_color_components!=3)
    {
        OperatorConsole.printf("***[DecompressJPEGL]: Not grayscale, RGB or YCbCr. Number of components = %d\n",
                               cinfo.out_color_components);
        jpeg_destroy_decompress(&cinfo);
        return (FALSE);
    }

// 20200722 Check Conquest lossless jpeg used YBR_FULL (which is wrong, but set the PhotoMetricInterpretation according)
    VR* pVR = pDDO->GetVR(0x0028,0x0004);		// PhotometricInterpretation
    BOOL    isYBR_FULL=false;
    if (pVR)
    { if (pVR->Length==8)
      { if (strncmp((char*)pVR->Data, "YBR_FULL", 8) == 0)
        isYBR_FULL=true;
      }
    }
	
// 20140209: lossless color jpeg should be RGB, but is not in Conquest implementation
    if (cinfo.process == JPROC_LOSSLESS &&
        cinfo.out_color_components == 3 &&

	cinfo.jpeg_color_space != JCS_RGB &&
	!isYBR_FULL &&		 // added 20200722: Conquest does not adhere to the standard! Uses YBR and sets PhotoMetricInterpretation so
#ifdef JPEGIGNORELOSSLESSJFIFCOLORSPACE
	true)
#else	
	!cinfo.saw_JFIF_marker)  // added 20170528
#endif
	
    {
        OperatorConsole.printf("DecompressJPEGL: forced jpeg colorspace for lossless to RGB\n");
        cinfo.jpeg_color_space = JCS_RGB;
    }

//Time to make an output image buffer.
    if(cinfo.data_precision_other > 8) outBytes = 2;
    else outBytes = 1;
    rowWidth =  cinfo.output_width * cinfo.output_components * outBytes;
    imageData.outputImageLengthEven = (rowWidth * cinfo.output_height * imageData.frames) & (UINT32)-1;
    if(!imageData.CreateOutputBuffer(imageData.outputImageLengthEven))
    {
        jpeg_destroy_decompress(&cinfo);
        return (FALSE);
    }
//The size of a pixel is JSAMPLE16, 16 bits, no matter what the size of the original data.
// This is done because the size of the data in the jpeg image is not known to the library until
// jpeg_read_header(). So if the buffer is 8 bit, the library will create an internal 16 bit
// buffer to hold the image data and copy it to the smaller buffer wasting time.
// So just make a 16 bit buffer array now the height of DCTSIZE (jpeg standard = 8).
    outBuffer = (JSAMPARRAY16)(*cinfo.mem->alloc_sarray)
	((j_common_ptr) &cinfo,  JPOOL_PERMANENT,//JPOOL_IMAGE,
     (JDIMENSION) cinfo.output_width * cinfo.output_components
     * sizeof(JSAMPLE16), (JDIMENSION) cinfo.rec_outbuf_height);
    if(!outBuffer)
    {
        OperatorConsole.printf("***[DecompressJPEGL]: Libjpeg could not allocate image buffer memory.\n");
        jpeg_destroy_decompress(&cinfo);
        return (FALSE);
    }
//Start the frames loop.
    while(TRUE)
    {
    //The library will convert the internal YCbCr to RGB.
        if (cinfo.out_color_components==3) cinfo.out_color_space = JCS_RGB;
        else cinfo.out_color_space = JCS_GRAYSCALE;
    //Ready to decompress.
        jpeg_start_decompress(&cinfo);
    //Tell libjpeg we will use a 16 bit buffer.
        cinfo.buffer_size_char = FALSE;
    //Never scale.
        cinfo.shft = 0;
    //Read the Image loop.
        while (cinfo.output_scanline < cinfo.output_height)
        {
            num_scanlines = jpeg_read_scanlines(&cinfo,(JSAMPARRAY) outBuffer, 
                                                (JDIMENSION) cinfo.rec_outbuf_height);
        //Transfer rows decomrpessed into the output buffer.
            for(row = 0; row < num_scanlines; row++)
            {
                uncompImageBptr = (char *)outBuffer[row];
                if (outBytes == 1)// char
                {
                    for( pixcnt = 0; pixcnt < rowWidth;  pixcnt++)
                    {
#if NATIVE_ENDIAN == BIG_ENDIAN //Big Endian like PPC
                        uncompImageBptr++;
#endif
                        *imageData.outPtr++ = *uncompImageBptr++;
#if NATIVE_ENDIAN == LITTLE_ENDIAN //Little Endian
                        uncompImageBptr++;
#endif
                    }
                }
                else //words
                { /* Row with is 2 x, MSB, LSB */
                    for( pixcnt = 0; pixcnt < rowWidth;  pixcnt++)
                    {
#if NATIVE_ENDIAN == LITTLE_ENDIAN //Little Endian
                        *imageData.outPtr++ = *uncompImageBptr++;
#else //Big Endian like PPC
                        *imageData.outPtr++ = uncompImageBptr[1];
                        *imageData.outPtr++ = *uncompImageBptr++;
                        uncompImageBptr++;
                        pixcnt++;
#endif
                    }
                }
            }
        }
        jpeg_finish_decompress(&cinfo);
    //Check for the end. Fuji is a single frame, will exit here.
        imageData.currFrame++;
        if(imageData.currFrame >= imageData.frames )break;
    //More images to read
        if(!imageData.GetEncapsulatedVR(jpegSOImarker)) break;
    //OK, have another image now, reset decompressor
    //Get data for next frame
        cinfo.src->bytes_in_buffer = imageData.imageVR->Length; // forces fill_input_buffer on first read
        cinfo.src->next_input_byte = (JOCTET *)imageData.imageVR->Data; // Image buffer
    //Read the new header, nothing should change (should we check?)
        jpeg_read_header(&cinfo, TRUE);
    //The default is 8 bits, set it back to the image size.
        cinfo.data_precision_other = cinfo.data_precision;
    }//Loop back to jpeg_start_decompress (while(TRUE)).

    // report failure on warnings
    if (jerr.pub.num_warnings)
        {
        char szRootSC[64], buffer[64];
	if (MyGetPrivateProfileString(RootConfig, "MicroPACS", RootConfig, szRootSC, 64, ConfigFile))
		{
		MyGetPrivateProfileString(szRootSC, "StopOnJPEGWarnings", "0", buffer, 64, ConfigFile);
		if (atoi(buffer))          
                        {
                        jpeg_destroy_decompress(&cinfo);
                        return (FALSE);
                        }
                }
        }

//Should we kill it and keep the uncompressed data?
    imageData.CheckEnoughFrames();
//Should have image(s) here, time to save it.
    imageData.SwitchToOutputBuffer();
//Needed for anything that stores <= 8 in 16 (now 8 in 8).
    imageData.FixPrecision(cinfo.data_precision);
//Done with libjpeg
    jpeg_destroy_decompress(&cinfo);
//Change the Photometric Interpretation, will only change color.
    imageData.SetColorToRGB(FALSE);
//Change the transfer syntax to LittleEndianExplict!
    if(leExplict)// Only false for FUJI_FIX w/ Fuji Image
        pDDO->ChangeVR( 0x0002, 0x0010, "1.2.840.10008.1.2.1\0", 'IU');
// If debug > 0, print decompress time.
    if (DebugLevel > 0) SystemDebug.printf("JPEG decompress time %u milliseconds.\n",clock() - starttime);
    
    // Done!
    return(TRUE);
}
#endif//End HAVE_LIBJPEG
#ifdef HAVE_J2K
//Jpeg 2000 is strange, should be just a stream, but could hav a header.
BOOL HaveNoJ2kStream(ImageData *imageDataPtr)
{
    const unsigned char     jpeg2kHeader[6] = { 0, 0, 0, 0x0C, 0x6A, 0x50 };
    int                     cnt;
    
    //Look for j2k marker or jp2 file header.
    if(*(unsigned char *)(imageDataPtr->inPtr) == 0xFF && imageDataPtr->inPtr[1] == 0x4F) return TRUE;//Jpeg stream.
    //Not a stream, look for a header.  Note: inPtr changes.
    for(cnt = 0; cnt < 6; cnt++) if(*(unsigned char *)(imageDataPtr->inPtr)++ != jpeg2kHeader[cnt]) return FALSE;
    //Header found, look for stream in first 512 bytes.
    for(cnt = 0; cnt < 512; cnt++)//I just picked 512.
    { // Look for stream header.
        if(*(unsigned char *)(imageDataPtr->inPtr) == 0xFF && imageDataPtr->inPtr[1] == 0x4F) return TRUE;
        imageDataPtr->inPtr++;
    }
    return FALSE;
}

#endif
#ifdef HAVE_LIBJASPER
/* This routine will take all of the listed color spaces in 08_03 of the standard
 *  in little endian, uncompressed format and  compress it to jpeg2000 in whatever
 *  format it came in.  I don't know if dicom viewers can support this.  It uses the
 *  Jasper library by Michael D. Adams from the Department of Electrical and
 *  Computer Engineering at the University of Victoria.  You can get it here:
 *  http://www.ece.uvic.ca/~mdadams/jasper/
 *  You can also get it here with a few minor changes to use the my Jpeg-6c
 *  library, but none of the changes are used by dgate: 
 *  http://www.suelix.com/downloads/jasper-1.900.1-6ct.zip
 *  Jasper can compress anything, with any size for each plane, but the standard only
 *  allows YBR_RCT lossless and YBR_ICT lossy.  So conversions are needed to YCbCr lossy
 *  and RGB for lossless.
 *  If I have made some mistakes (most likely), you can contact me bruce.barton
 *  (the mail symbol goes here) me.com.  Let me know where I can find a sample of
 *  the image that didn't work. */
BOOL CompressJPEG2K(DICOMDataObject* pDDO, int j2kQuality)
{
    ImageData                   imageData;
    clock_t                     starttime;
    jas_stream_t                *out;
    jas_image_t                 *image;
    jas_image_cmpt_t            *cmpt;
    jas_image_coord_t           byteCnt;
    UINT8                       sgnd;
    UINT16                      cmptno;
    char                        *colorBuffer_ptr[3];
    char                        option[20];

//If debug > 0, get start time and set the level.
    if (DebugLevel > 0)
    {
        starttime = clock();
        jas_setdbglevel(DebugLevel - 1);
    }
    if (DebugLevel > 0) SystemDebug.printf("Jasper compress started.\n");
//Checked if the data is signed.
    if(!(sgnd = pDDO->GetBYTE(0x0028, 0x0103)))sgnd = 0;
//Get the data.
    if(!imageData.GetImageData(pDDO, "Jasper compress", TRUE)) return FALSE;
//See if its real.
    if (!imageData.CalculateImageLengths())
    {//Should of had it, look for arrays
        if(!imageData.GetEncapsulatedDDO(NULL)) return FALSE;//no marker
    }
//Check and set the quality for lossy.
    if(j2kQuality < MIN_QUALITY)// Set to 0 to use dicom.ini value.
    {
        j2kQuality = LossyQuality();//Use the default or dicom.ini value.
    }
    if(j2kQuality > 100) j2kQuality = 100;
//Make the input standard.
    if (imageData.components == 1)
    {
        if (!imageData.MakeStdGray()) return FALSE;
    //For sone reason jasper wants the data in big endian on both a LE and BE system.
    //  There may be an option switch for this, but I just swpped endian.
        if(imageData.precision_allocated > 8)SwapEndian((unsigned char *)imageData.imageVR->Data, imageData.imageVR->Length);
    }
    else if (!imageData.MakeRGB(TRUE))return FALSE;//Planes perfered, but not needed.
//Create the encapsulation array.
    if (!imageData.CreateOutputArray()) return FALSE;
// Uninitialized warnings.
    cmpt = NULL;
//Init the jasper library.
    if(jas_init())
    {
        OperatorConsole.printf("***[CompressJPEG2K]: Cannot init the jasper library\n");
        return (FALSE);
    }
//Created from jas_image_create and jas_image_create0.
// It was done so the buffers are not allocated if not needed.
    if (!(image = (jas_image_t *)malloc(sizeof(jas_image_t))))
    {
        OperatorConsole.printf("***[CompressJPEG2K]: Could not allocate an image structure.\n");
        return (FALSE);
    }
//Set the some defaults for the outer box.
    image->tlx_ = 0;
    image->tly_ = 0;
//Set colors.
    if(imageData.components == 1) image->clrspc_ = JAS_CLRSPC_FAM_GRAY;
    else image->clrspc_ = JAS_CLRSPC_FAM_RGB;
    image->numcmpts_ = 0;//Set later
    image->maxcmpts_ = imageData.components;
    image->bry_ = imageData.height;
    image->brx_ = imageData.width;
    if((imageData.planes || image->maxcmpts_ == 1))
    {// No need for buffers, just pointers, grayscale is one plane.
        for (cmptno = 0; cmptno < image->maxcmpts_; ++cmptno)
        {
            colorBuffer_ptr[cmptno] = (char *)imageData.imageVR->Data + (imageData.singleColorFrameSize * cmptno) ;
        }
    }
    else//A buffer is needed.
    {
        if(!imageData.CreateFrameBuffer(imageData.frameSize))
        {
            free(image);
            return FALSE;
        }
        for (cmptno = 0; cmptno < image->maxcmpts_; ++cmptno)
            colorBuffer_ptr[cmptno] = imageData.frameBuffer + (imageData.singleColorFrameSize * cmptno);
    }
//Allocate memory for the per-component pointer table.
    if (!(image->cmpts_ = ((jas_image_cmpt_t **)jas_malloc(image->maxcmpts_ *
                                                           sizeof(jas_image_cmpt_t *)))))
    {
        OperatorConsole.printf("***[CompressJPEG2K]: Could not create component pointers.\n");
        free(image);
        return (FALSE);
    }
//Initialize in case of failure so jas_image_destroy can be used.
    for (cmptno = 0; cmptno < image->maxcmpts_; ++cmptno)
        image->cmpts_[cmptno] = 0;
    image->inmem_ = TRUE;
    image->cmprof_ = 0;
//Allocate memory for the per-component information and init.
    for (cmptno = 0; cmptno < image->maxcmpts_; ++cmptno)
    {
        if (!(cmpt = (jas_image_cmpt_t *)jas_malloc(sizeof(jas_image_cmpt_t))))
        {
            jas_image_destroy(image);
            OperatorConsole.printf("***[CompressJPEG2K]: Could not create empty components.\n");
            return (FALSE);
        }
        image->cmpts_[cmptno] =cmpt;
        cmpt->type_ = JAS_IMAGE_CT_UNKNOWN;//Set next.
        cmpt->tlx_ = 0;
        cmpt->tly_ = 0;
        cmpt->hstep_ = 1;
        cmpt->vstep_ = 1;
        cmpt->width_ = image->brx_;
        cmpt->height_ = image->bry_;
        cmpt->prec_ = imageData.precision;
        cmpt->sgnd_ = sgnd;
        cmpt->stream_ = 0;
        cmpt->cps_ = imageData.byteWidth;
        if(!(cmpt->stream_ = jas_stream_memopen(colorBuffer_ptr[cmptno], imageData.singleColorFrameSize)))
        {
            jas_free(cmpt);
            jas_image_destroy(image);
            OperatorConsole.printf("***[CompressJPEG2K]: Jasper could not open the memory as stream.\n");
            return (FALSE);
        }
        ++(image->numcmpts_);
    }
    // Now set the color type
    if (image->clrspc_ == JAS_CLRSPC_FAM_GRAY)
    {
        jas_image_setclrspc(image, JAS_CLRSPC_SGRAY);
        jas_image_setcmpttype(image, 0, JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_GRAY_Y));
    }
    else
    {
        jas_image_setclrspc(image, JAS_CLRSPC_SRGB);
        jas_image_setcmpttype(image, 0, JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_RGB_R));
        jas_image_setcmpttype(image, 1, JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_RGB_G));
        jas_image_setcmpttype(image, 2, JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_RGB_B));
    }
    // Lossy (ICT) or lossless (RCT)?
    if((j2kQuality < 100))
    {
        if(j2kQuality > 95)j2kQuality = 95;
        memcpy(option, "mode=real rate=0.", 17);
        option[17] = (j2kQuality / 10) + '0';//ICT j2kQuality msb
        option[18] = (j2kQuality % 10) + '0';//ICT j2kQuality lsb
        option[19] = 0;//end the string.
        if (DebugLevel > 1)SystemDebug.printf("JPEG2K Lossy Quality rate = %s", &option[15]);
    }
    else
    {
        memcpy(option, "mode=int\0", 9);//RCT
        if (DebugLevel > 1)SystemDebug.printf("JPEG2K Lossless");
    }
// Print out some more info for debug.
    if (DebugLevel > 1)
    {
        if (DebugLevel > 2)
        {
            if(imageData.frameBuffer == NULL)SystemDebug.printf(", unbuffered data");
            else SystemDebug.printf(", buffered data");
            if(imageData.planes)SystemDebug.printf(", planes");
        }
        SystemDebug.printf(
                           "\n, H = %d, W = %d, Bits = %d in %d, Frames = %d, ",
                           image->brx_, image->brx_, imageData.precision, imageData.precision_allocated, imageData.frames);
        if(imageData.ascColor[0] != 0) SystemDebug.printf("color = %s\n", imageData.ascColor);
    }
//Frames loop.
    while(TRUE)
    {
        if(imageData.frameBuffer != NULL)//JAS_CLRSPC_FAM_RGB or JAS_CLRSPC_SYCBCR
        {
            for(byteCnt = 0; byteCnt < imageData.singleColorFrameSize; byteCnt++)
            {
                *(colorBuffer_ptr[0])++ = *imageData.inPtr++;
                *(colorBuffer_ptr[1])++ = *imageData.inPtr++;
                *(colorBuffer_ptr[2])++ = *imageData.inPtr++;
            }
        }// End of buffers.
    //A memory buffer for the output data, too big.
        if (!imageData.CreateOutputBuffer(imageData.frameSize+guard))
        {
            jas_image_destroy(image);
            return (FALSE);
        }
    // Open the too big buffer to hold output data.
        if((out = jas_stream_memopen(imageData.outBuffer, imageData.frameSize+guard)) == NULL)
        {
            jas_image_destroy(image);
            OperatorConsole.printf("***[CompressJPEG2K]: Jasper could not open the output buffer.\n");
            return (FALSE);
        }
    //Time to compress.
        if(jpc_encode(image, out, option))
        {
            jas_stream_close(out);
            jas_image_destroy(image);
            OperatorConsole.printf("***[CompressJPEG2K]: Jasper could not encode the image.\n");
            return (FALSE);
        }
    //Encapsulate an image object.
        imageData.AddToOutputArray((out->rwcnt_) & (UINT32)-1);
        jas_stream_close(out);// Jasper is done with the buffer.
        imageData.currFrame++;
        if(imageData.currFrame >= imageData.frames)break;//Done with all of the frames.
    //Still here, setup for the next frame, if frame buffer not used, move pointer next image.
        if (imageData.frameBuffer == NULL )imageData.inPtr += imageData.frameSize;
    //If not enough data, look in silly arrays.
        if (!imageData.CheckEnoughData(FALSE))
        {
        //Deal with silly input arrays.
            if(!imageData.GetEncapsulatedVR(NULL)) break;//No marker
        //New data, time to fix, make the input standard.
            if (imageData.components == 1)
            {
                if (!imageData.MakeStdGray())
                {
                    jas_image_destroy(image);
                    return FALSE;
                }
            }
            else if (!imageData.MakeRGB(TRUE));
            {
                jas_image_destroy(image);
                return FALSE;
            }
        //jasper still wants big endian.
            if(imageData.precision_allocated > 8)SwapEndian((unsigned char *)imageData.imageVR->Data, imageData.imageVR->Length);// Byte swap time
        }
    //Reset some jasper stuff.
        for (cmptno = 0; cmptno < image->numcmpts_; ++cmptno)
        {
            if(imageData.frameBuffer != NULL)
            {//Reset the buffers and streams.
                jas_stream_rewind( image->cmpts_[cmptno]->stream_ );
            //Reset the buffers pointers.
                colorBuffer_ptr[cmptno] = imageData.frameBuffer + (imageData.singleColorFrameSize * cmptno);
            }
            else
            {// Close and reopen the streams at the new locations.
                jas_stream_close( image->cmpts_[cmptno]->stream_);
                image->cmpts_[cmptno]->stream_ = NULL;
                if(!(cmpt->stream_ = jas_stream_memopen( imageData.inPtr + (imageData.singleColorFrameSize * cmptno),
                                                        imageData.singleColorFrameSize )))
                {
                    jas_image_destroy(image);
                    OperatorConsole.printf("***[CompressJPEG2K]: Jasper could not open the memory as stream.\n");
                    return (FALSE);
                }
            }
        }
    }//Back for the next frame, end of while(TRUE)
//Done with Jasper.
    jas_image_destroy(image);
    jas_image_clearfmts();//bcb Needed to stop leaks?
//Should we kill it and keep the uncompressed data?
    imageData.CheckEnoughFrames();
//The end object.
    imageData.AddToOutputArray((UINT32)-1);
//Save the new data.
    imageData.SwitchToOutputArray();
//Needed for anything that stores <= 8 in 16 (now 8 in 8).
    imageData.FixPrecision(imageData.precision);
//Set color info if needed.
    if(imageData.components > 1)
    {
    //Reset the plane's VR, if there.
        imageData.SetPlanesToNone();
    //Set the color profile.
        if(j2kQuality < 100)
            pDDO->ChangeVR( 0x0028, 0x0004, "YBR_ICT\0", 'CS');
        else pDDO->ChangeVR( 0x0028, 0x0004, "YBR_RCT\0", 'CS');
    }
//Change the transfer syntax to JPEG2K!
    if(j2kQuality < 100)
        pDDO->ChangeVR( 0x0002, 0x0010, "1.2.840.10008.1.2.4.91\0", 'IU');
    else pDDO->ChangeVR( 0x0002, 0x0010, "1.2.840.10008.1.2.4.90\0", 'IU');
//If debug > 0, print when finished.
    if (DebugLevel > 0) SystemDebug.printf("Jasper compress time %u milliseconds.\n",clock() - starttime);
    return (TRUE);
}

/* This routine will take in a jpeg2000 image and convert it to little endian, uncompressed,
 *  RGB or grayscale.  It uses the Jasper library by Michael D. Adams from the
 *  Department of Electrical and Computer Engineering at the University of Victoria.
 *  You can get it here:
 *  http://www.ece.uvic.ca/~mdadams/jasper/
 *  You can also get it here with a few minor changes to use the my Jpeg-6c
 *  library, but none of the changes are used by dgate: 
 *  http://www.suelix.com/downloads/jasper-1.900.1-6ct.zip
 *  Jasper can compress anything, with any size for each plane.  So that means any
 *  of the color spaces and formats can be in it.  The dicom standard removes the header
 *  and color box infromation and sends just the data stream.  The standard states only
 *  MONO1,2 PALETTE_COLOR YBR_RCT and YBR_ICT can be use.  For the color space and
 *  image format, I have to trust jasper.
 *  If I have made some mistakes (most likely), you can contact me bruce.barton
 *  (the mail symbol goes here) me.com.  Let me know where I can find a sample of
 *  the image that didn't work. */
BOOL DecompressJPEG2K(DICOMDataObject* pDDO)
{
    ImageData           imageData;
    clock_t             starttime;
    BOOL                moreImages;
    jas_stream_t	*jas_in, *jas_out[4];
    jas_image_t		*decompImage;
    size_t              cnt;
    UINT32		stream_len;
    
//If debug > 0, get start time and set the level.
    if (DebugLevel > 0)
    {
        starttime = clock();
        jas_setdbglevel(DebugLevel - 1);
    }
    if (DebugLevel > 0) SystemDebug.printf("Jasper decompress started.\n");
//Get the image VR.
    if(!imageData.GetImageData(pDDO, "Jasper decompress", TRUE)) return FALSE;
//Jpeg 2000 is strange, should be just a stream, but could hav a header.
    while (TRUE)
    {
        if(!imageData.GetEncapsulatedDDO(NULL)) return FALSE;
        if(HaveNoJ2kStream(&imageData))break;
    }
//Init the jasper library.
    if(jas_init())
    {
        OperatorConsole.printf("***[DecompressJPEG2K]: cannot init the jasper library\n");
        return (FALSE);
    }
    if (DebugLevel > 3) SystemDebug.printf("Jasper library init completed.\n");
//Start the frames loop.
    moreImages = TRUE;
    while(moreImages)
    {
#ifdef NOVARAD_FIX
        if (((unsigned char *)(imageData.imageVR->Data))[imageData.imageVRLength -2] != 0xFF
            && ((unsigned char *)(imageData.imageVR->Data))[imageData.imageVRLength -2] != 0xD9)
        {
        //Put the end back on.
            if (!imageData.FixJpegEnd()) return FALSE;
        //Put the inPtr at the start of the stream.
            if (HaveNoJ2kStream(&imageData)) return FALSE;
        }
#endif
    //Open the memory as a stream.
        if(!(jas_in = jas_stream_memopen(imageData.inPtr, (imageData.inEndPtr - imageData.inPtr) & (int)-1)))
        {
            OperatorConsole.printf("***[DecompressJPEG2K]: cannot open jpeg 2K data\n");
            return (FALSE);
        }
    //Decopress the stream.
        if (!(decompImage = jpc_decode(jas_in, NULL)))
        {
            OperatorConsole.printf("***[DecompressJPEG2K]: cannot decode the jpeg 2K code stream\n");
            jas_stream_close(jas_in);
            return ( FALSE );
        }
        jas_stream_close(jas_in);
    //Do this the first time only.
        if(!imageData.currFrame)
        {
        //Check for color.
            if(imageData.components > 1)
            {
                if((imageData.components != 3) || (decompImage->numcmpts_ != 3))
                {
                    OperatorConsole.printf("***[DecompressJPEG2K]: Should be 3 colors, DICOM: %d ,J2K: %d \n",
                                           imageData.components , decompImage->numcmpts_);
                    jas_image_destroy(decompImage);
                    return ( FALSE );
                }
            }
        //Get the total uncompressed length and rewind all streams and get pointers.
            stream_len = 0;
            for ( cnt = 0; cnt < imageData.components ; cnt++ )
            {
                jas_out[cnt] = decompImage->cmpts_[cnt]->stream_;
                jas_stream_rewind(jas_out[cnt]);
                stream_len += jas_stream_length(jas_out[cnt]);
            }
            imageData.precision = decompImage->cmpts_[0]->prec_;//Save for outside loop
            imageData.byteWidth = decompImage->cmpts_[0]->cps_;// Bytes or words.
            imageData.outputImageLengthEven = stream_len * imageData.frames;
            if (DebugLevel > 1)SystemDebug.printf(
                "Info[DecompressJPEG2K] stream_len = %d, frames = %d, total_len = %d, prec = %d,cps_jas = %d.\n",
                stream_len,imageData.frames, imageData.outputImageLengthEven, imageData.precision, imageData.byteWidth);
        //Allocate the bigger uncompressed and unencapsulated image.
            if (!imageData.CreateOutputBuffer(imageData.outputImageLengthEven))//Will make even.
            {
                jas_image_destroy(decompImage);
                return ( FALSE );
            }
        }//End of currFrame = 0 or first time.
        else
        {//Not first, just rewind streams
            for ( cnt = 0; cnt < imageData.components ; cnt++ )
            {
                jas_out[cnt] = decompImage->cmpts_[cnt]->stream_;
                jas_stream_rewind(jas_out[cnt]);
            }
        }
    //Image copy loops.
        if(imageData.components == 1)
        {
        //Again, for some reason, Jasper outputs big endian.  There may be a switch.
            if(imageData.byteWidth == 2)
            {
                for(cnt = 0; cnt < stream_len; cnt++)
                {
                    imageData.outPtr[1] = jas_stream_getc(jas_out[0]);
                    *imageData.outPtr++ = jas_stream_getc(jas_out[0]);
                    imageData.outPtr++;
                    cnt++;
                }
            }
            else for(cnt = 0; cnt < stream_len; cnt++)
                    *imageData.outPtr++ = jas_stream_getc(jas_out[0]);
        }
        else //RGB, jasper put out planes, deplane
        {
            stream_len = (UINT32)(jas_stream_length(jas_out[0]) &UINT32_MAX);
            for(cnt = 0; cnt < stream_len; cnt++)
            {
                *imageData.outPtr++ = jas_stream_getc(jas_out[0]);
                *imageData.outPtr++ = jas_stream_getc(jas_out[1]);
                *imageData.outPtr++ = jas_stream_getc(jas_out[2]);
            }
        }
        jas_image_destroy(decompImage);//Done with the image.
    //Check for the end.
        imageData.currFrame++;
        if(imageData.currFrame >= imageData.frames)break;
    //More images to read.
        while (TRUE)
        {
            if(!imageData.GetEncapsulatedDDO(NULL))
            {//Not good, can't find the next image!
                moreImages = FALSE;//Break the image loop.
                break;
            }
            if(HaveNoJ2kStream(&imageData))break;
        }
    //Loop back to open the memory as a stream.
    }//End of the frames loop
//Should we kill it and keep the uncompressed data?
    imageData.CheckEnoughFrames();
//Change the image vr to the bigger uncompressed and unencapsulated image.
    imageData.SwitchToOutputBuffer();
    if( imageData.components > 1)//color.
        imageData.SetColorToRGB(FALSE);
//Change the transfer syntax to LittleEndianExplict!
    pDDO->ChangeVR( 0x0002, 0x0010, "1.2.840.10008.1.2.1\0", 'IU');
//If debug > 0, print when finished
    if (DebugLevel > 0) SystemDebug.printf("Jasper decompress time %u milliseconds.\n",clock() - starttime);
    return (TRUE);
}
#endif //End for LIBJASPER

#ifdef HAVE_LIBOPENJPEG
#ifndef HAVE_LIBOPENJPEG2
/* Error and message callback does not use the FILE* client object. */
void error_callback(const char *msg, void *client_data)
{
	UNUSED_ARGUMENT(client_data);
	OperatorConsole.printf("***[Libopenjpeg(J2k)]: %s\n", msg);
}

/* Warning callback expecting a FILE* client object. */
void warning_callback(const char *msg, void *client_data)
{
	UNUSED_ARGUMENT(client_data);
	OperatorConsole.printf("Warn[Libopenjpeg(J2k)]: %s\n", msg);
}

/* Debug callback expecting no client object. */
void info_callback(const char *msg, void *client_data)
{
	UNUSED_ARGUMENT(client_data);
	if(DebugLevel > 1)SystemDebug.printf("Info[Libopenjpeg(J2k)]: %s\n", msg);
}

/* This routine will take all of the listed color spaces in 08_03 of the standard
 *  in little endian, uncompressed format and  compress it to jpeg2000 in whatever
 *  format it came in.  I don't know if dicom viewers can support this. It uses the 
 *  openjpeg library from Communications and Remote Sensing Lab (TELE) in the
 *  Universitait Catholique de Louvain (UCL), and CNES with the support of the CS company,
 *  version 1.3.  You can get it here:
 *  http://code.google.com/p/openjpeg/downloads/list
 *  The library was built with USE_JPWL defined.
 *  OpenJPEG can compress anything, with any size for each plane, but the standard only
 *  allows YBR_RCT lossless and YBR_ICT lossy.
 *  If I have made some mistakes (most likely), you can contact me bruce.barton
 *  (the mail symbol goes here) me.com.  Let me know where I can find a sample of
 *  the image that didn't work. */
BOOL CompressJPEG2Ko(DICOMDataObject* pDDO, int j2kQuality)
{
    ImageData           imageData;
    clock_t             starttime;
    int                 codestream_length;
    UINT8		sgnd;
    INT32		byteCnt;
    char		*bufferg_ptr, *bufferb_ptr, *out_ptr;
    unsigned int	mask;
    int 		*colorBuffer_ptr[3], *colorBuffer[3];
    unsigned int	cnt;
//OpenJPEG Stuff
    opj_cparameters_t	parameters;	// compression parameters.
    opj_event_mgr_t	event_mgr;	// event manager.
    opj_cio_t*		cio;
    opj_cinfo_t*	cinfo;
    opj_image_t		*image = NULL;
    opj_image_comp_t	*comp;
    const char		comment[] = "Created by OpenJPEG with JPWL version ";
    size_t		clen, vlen;
    const char		*version = opj_version();
    
//If debug > 0, get start time and print who.
    if (DebugLevel > 0)
    {
        starttime = clock();
	SystemDebug.printf("openJPEG %s compress started.\n", version);
    }
//Checked if the data is signed.
    if(!(sgnd = pDDO->GetBYTE(0x0028, 0x0103)))sgnd = 0;
//Get the data.
    if(!imageData.GetImageData(pDDO, "openJPEG compress", TRUE)) return FALSE;
//See if its real.
    if (!imageData.CalculateImageLengths())
    {//Should of had it, look for arrays
        if(!imageData.GetEncapsulatedDDO(NULL)) return FALSE;//no marker
    }
//Create the encapsulation array.
    if(!imageData.CreateOutputArray()) return FALSE;
//Check and set the quality for lossy.
    if(j2kQuality < MIN_QUALITY)// Set to 0 to use dicom.ini value.
    {
        j2kQuality = LossyQuality();//Use the default or dicom.ini value.
    }
    if(j2kQuality > 100) j2kQuality = 100;
//Make the input standard.
    if (imageData.components == 1)
    {
        if (!imageData.MakeStdGray()) return FALSE;
    }
    else if (!imageData.MakeYBR_FULL_OrRGB(TRUE))return FALSE;
//Allocate all of the memory needed.
//Create an image struct.
    if((image = (opj_image_t*)calloc(1, sizeof(opj_image_t))) == NULL)
    {
        OperatorConsole.printf("***[openJPEG compress]: Could not allocate an image structure.\n");
        return (FALSE);
    }
//Create comment for codestream buffer.
    clen = strlen(comment);//Made comment above, strnlen not needed.
    vlen = strnlen(version, 10);
    if((parameters.cp_comment = (char*)malloc(clen+vlen+1)) == NULL)
    {
        free(image);
        OperatorConsole.printf("***[openJPEG compress]: Could allocate the coment buffer.\n");
        return (FALSE);
    }
    memcpy(parameters.cp_comment, comment, clen);
    memcpy(&parameters.cp_comment[clen], version, vlen);
    parameters.cp_comment[clen + vlen] = 0;
//Allocate memory for the per-component information.
    if((image->comps = (opj_image_comp_t*)calloc(imageData.components, sizeof(opj_image_comp_t))) == NULL)
    {
        free(image);
        free(parameters.cp_comment);
        parameters.cp_comment = NULL;
        OperatorConsole.printf("***[openJPEG compress]: Could not create empty components.\n");
        return FALSE;
    }
//Buffers are always needed for ints.
    for (cnt = 0; cnt < imageData.components; cnt++)
        if((colorBuffer[cnt] = (int *)malloc(imageData.singleColorFrameSize * sizeof(int))) == NULL)
        {
            free(image->comps);
            image->comps = NULL;
            free(image);
            free(parameters.cp_comment);
            parameters.cp_comment = NULL;
            OperatorConsole.printf("***[openJPEG compress]: Could allocate the component buffer.\n");
            return FALSE;
        }
//Set image offset
    image->x0 = 0;
    image->y0 = 0;
//How many colors.
    image->numcomps = imageData.components;
//Decide if MCT should be used.
    parameters.tcp_mct = image->numcomps == 3 ? 1 : 0;
//Set the dimensions
    image->y1 = imageData.height;
    image->x1 = imageData.width;
//Check color.
    image->color_space = CLRSPC_GRAY;// Set the default.
    if(image->numcomps == 3) image->color_space = CLRSPC_SRGB;
//Created from image_to_j2k.c and convert.c.
//Configure the event callbacks.
    memset(&event_mgr, 0, sizeof(opj_event_mgr_t));
    event_mgr.error_handler = error_callback;
    event_mgr.warning_handler = warning_callback;
    event_mgr.info_handler = info_callback;
//Set encoding parameters values.
    opj_set_default_encoder_parameters(&parameters);
    if(j2kQuality < 100)parameters.tcp_rates[0] = 100/j2kQuality;
    else parameters.tcp_rates[0] = 0;// MOD antonin : losslessbug
    parameters.tcp_numlayers++;
    parameters.cp_disto_alloc = 1;
// Lossy (ICT) or lossless (RCT)?
    if(j2kQuality < 100) parameters.irreversible = 1;//ICT
//Create the individual image components.
    for(cnt = 0; cnt < image->numcomps; cnt++)
    {
        comp = &image->comps[cnt];
        comp->dx = parameters.subsampling_dx;
        comp->dy = parameters.subsampling_dy;
        comp->w = image->x1;
        comp->h = image->y1;
        comp->x0 = 0;
        comp->y0 = 0;
        comp->prec = imageData.precision;
        comp->bpp = imageData.precision_allocated;
        comp->sgnd = sgnd;
    // From now, forward, opj_image_destroy(image) will free the buffers.
        comp->data = colorBuffer[cnt];
    }
//Print out some more info for debug.
    if (DebugLevel > 1) imageData.PrintInfo();
    mask = ((1 << imageData.precision) - 1);
//Frames loop.
    while(TRUE)
    {
    //Almost never used (planes)
        bufferg_ptr = imageData.inPtr + imageData.singleColorFrameSize;
        bufferb_ptr = bufferg_ptr + imageData.singleColorFrameSize;
    //(Re)set the buffer pointers.
        for( cnt = 0; cnt < image->numcomps; cnt++)
            colorBuffer_ptr[cnt] = (int *)colorBuffer[cnt];
    // Get a J2K compressor handle.
        cinfo = opj_create_compress(CODEC_J2K);
    //Catch events using our callbacks and give a local context.
        opj_set_event_mgr((opj_common_ptr)cinfo, &event_mgr, stderr);
    // Setup the encoder parameters using the current image and user parameters.
        opj_setup_encoder(cinfo, &parameters, image);
    // Open a byte stream for writing.
        if(!(cio = opj_cio_open((opj_common_ptr)cinfo, NULL, 0)))
        {
            OperatorConsole.printf("***[openJPEG compress]: Failed to allocate output stream memory.\n");
            opj_destroy_compress(cinfo);
            opj_image_destroy(image);
            free(parameters.cp_comment);
            parameters.cp_comment = NULL;
            return ( FALSE );
        }
        switch(image->color_space)
        {
            case CLRSPC_GRAY:
                    if(imageData.precision_allocated == 8) // 8 Bits, char to int.
                    {
                        for(byteCnt = 0; byteCnt < imageData.frameSize; byteCnt++)
                        {
                            *(colorBuffer_ptr[0])++ = ((int)*imageData.inPtr & mask);
                            imageData.inPtr++;
                        }
                        break;
                    }
            //16 Bits
                    if(imageData.precision > 8 && imageData.precision <= 16)//Byte swap
                    {
                        for(byteCnt = 0; byteCnt < imageData.frameSize; byteCnt += 2)
                        { unsigned short s=((imageData.inPtr[0])&0xFF)|(((imageData.inPtr[1])&0xFF)<<8);
		          if (s>mask)
			  {
			    OperatorConsole.printf("***[openJPEG compress]: Error: input data out of range.\n");
			    opj_destroy_compress(cinfo);
			    opj_image_destroy(image);
			    free(parameters.cp_comment);
			    parameters.cp_comment = NULL;
			    return ( FALSE );
			  }
                            *(colorBuffer_ptr[0]) = ((int)*imageData.inPtr & 0xFF);
                            imageData.inPtr++;
                            *(colorBuffer_ptr[0])++ |= (((int)*imageData.inPtr & 0xFF) << 8) & mask;
                            imageData.inPtr++;
                        }
                        break;
                    }
                    for(byteCnt = 0; byteCnt < imageData.frameSize; byteCnt += 2)// 8 in 16.
                    {
                        *(colorBuffer_ptr[0]) = ((int)*imageData.inPtr  & mask);
                        imageData.inPtr++;
                        imageData.inPtr++;
                    }
                    break;
                default: //CLRSPC_SRGB:
                    if(imageData.planes)
                    {
                        for(byteCnt = 0; byteCnt < imageData.singleColorFrameSize; byteCnt++)
                        {
                            *(colorBuffer_ptr[0])++ = (int)*imageData.inPtr & mask;
                            imageData.inPtr++;
                            *(colorBuffer_ptr[1])++ = (int)*bufferg_ptr & mask;
                            bufferg_ptr++;
                            *(colorBuffer_ptr[2])++ = (int)*bufferb_ptr & mask;
                            bufferb_ptr++;
                        }
                        break;
                    }//Normal RGB
                    for(byteCnt = 0; byteCnt < imageData.singleColorFrameSize; byteCnt++)
                    {
                        *(colorBuffer_ptr[0])++ = (int)*imageData.inPtr & mask;
                        imageData.inPtr++;
                        *(colorBuffer_ptr[1])++ = (int)*imageData.inPtr & mask;
                        imageData.inPtr++;
                        *(colorBuffer_ptr[2])++ = (int)*imageData.inPtr & mask;
                        imageData.inPtr++;
                    }
                    break;
        }//Filled the buffers and ready to go.
    //encode the image.
        if(!(opj_encode(cinfo, cio, image, NULL)))
        {
            OperatorConsole.printf("***[openJPEG compress]: OpenJpeg could not encode the image.\n");
            opj_destroy_compress(cinfo);
            opj_image_destroy(image);
            opj_cio_close(cio);
            free(parameters.cp_comment);
            parameters.cp_comment = NULL;
            return FALSE;
        }
        opj_destroy_compress(cinfo);//Done with cinfo.
        codestream_length = cio_tell(cio);
    //Jpeg2k is encapsulated, make a new vr to encapsulate.
        if(!imageData.CreateOutputBuffer(codestream_length & (UINT32)-1))//Sets outputImageLengthEven.
        {
            opj_image_destroy(image);
            opj_cio_close(cio);
            free(parameters.cp_comment);
            parameters.cp_comment = NULL;
            return ( FALSE );
        }
    //Copy the Image data.
        out_ptr = (char *)cio->buffer;
        for(cnt = codestream_length; cnt > 0; --cnt) *imageData.outPtr++ = *out_ptr++;
    //Done with the code stream and cinfo.
        opj_cio_close(cio);
    //Save the image object.
        if(!imageData.AddToOutputArray(imageData.outputImageLengthEven))
        {
            opj_image_destroy(image);
            free(parameters.cp_comment);
            parameters.cp_comment = NULL;
            return ( FALSE );
        }
    //Done with all of the frames.
        imageData.currFrame++;
        if(imageData.currFrame >= imageData.frames) break;
    //If not enough data, look in silly arrays.
        if (!imageData.CheckEnoughData(FALSE))
        {//Deal with silly input arrays.
            if(!imageData.GetEncapsulatedVR(NULL)) break;//No marker
        //New data, time to fix, make the input standard.
            if (imageData.components == 1)
            {
                if (!imageData.MakeStdGray())
                {
                    opj_image_destroy(image);
                    free(parameters.cp_comment);
                    parameters.cp_comment = NULL;
                    return ( FALSE );
                }
            }
            else if (!imageData.MakeRGB(TRUE))
            {
                opj_image_destroy(image);
                free(parameters.cp_comment);
                parameters.cp_comment = NULL;
                return ( FALSE );
            }
        }
        if(imageData.planes) imageData.inPtr = bufferb_ptr;
    }//Back for the next frame, end of while(TRUE)
//At last, done with the image struct.
    opj_image_destroy(image); 
//Done with the comments.
    free(parameters.cp_comment);
    parameters.cp_comment = NULL;
//Should we kill it and keep the uncompressed data?
    imageData.CheckEnoughFrames();
//Finish encapsulating.
    imageData.AddToOutputArray((UINT32)-1);
//Save the new data.
    imageData.SwitchToOutputArray();
//Change the dicom parameters.
//Needed for anything that stores <= 8 in 16 (now 8 in 8).
    imageData.FixPrecision(imageData.precision);
//Set color if needed.
    if(imageData.components > 1)
    {
    //Reset the plane's VR, if there.
        imageData.SetPlanesToNone();
    //Set the color profile
        if((j2kQuality < 100))
            pDDO->ChangeVR( 0x0028, 0x0004, "YBR_ICT\0", 'CS');
        else pDDO->ChangeVR( 0x0028, 0x0004, "YBR_RCT\0", 'CS');
	}
    //Change the transfer syntax to JPEG2K!
    if(j2kQuality < 100)
        pDDO->ChangeVR( 0x0002, 0x0010, "1.2.840.10008.1.2.4.91\0", 'IU');
    else pDDO->ChangeVR( 0x0002, 0x0010, "1.2.840.10008.1.2.4.90\0", 'IU');
    // If debug > 0, print when finished
    //If debug > 0, print when finished
    if (DebugLevel > 0) SystemDebug.printf("OpenJPEG compress time %u milliseconds.\n",clock() - starttime);
    return (TRUE);
}

/* This routine will take in a jpeg2000 image and convert it to little endian, uncompressed,
 *  RGB or grayscale.  It uses the openjpeg library from Communications and Remote Sensing Lab (TELE)
 *  in the Universitait Catholique de Louvain (UCL), and CNES with the support of the CS company.
 *  You can get it here:
 *  http://code.google.com/p/openjpeg/downloads/list
 *  The library was built with USE_JPWL defined.
 *  JPEG200 can compress almost anything, with any size for each plane.  So that means any
 *  of the color spaces and formats can be in it.  The dicom standard removes the header
 *  and color box infromation and sends just the data stream.  The standard states only
 *  MONO1,2 PALETTE_COLOR YBR_RCT and YBR_ICT can be use.  For the color space and
 *  image format, I have to trust libopenjpeg.
 *  If I have made some mistakes (most likely) you can contact me bruce.barton
 *  (the mail symbol goes here) me.com.  Let me know where I can find a sample of
 *  the image that didn't work. */
BOOL DecompressJPEG2Ko(DICOMDataObject* pDDO)
{
    ImageData                   imageData;
    clock_t                     starttime;
    BOOL                        moreImages;
    UINT16			*out16_ptr;
    unsigned int                cnt;
    int		*jpc_out[3], mask;
    int				bytes_jpc, prec_jpc;
    UINT32			stream_len;
#if NATIVE_ENDIAN == BIG_ENDIAN //Big Endian like Apple power pc
    int		masked;
#endif //Big Endian
    // openJPEG stuff.
    opj_dparameters_t		parameters;	// decompression parameters.
    opj_image_t			*decompImage = NULL;
    opj_event_mgr_t		event_mgr;		// event manager.
    opj_dinfo_t			*dinfo = NULL;	// handle to a decompressor.
    opj_cio_t			*cio = NULL;
    const char		*version = opj_version();
	
//If debug > 0, get start time.
    if (DebugLevel > 0)starttime = clock();
    if (DebugLevel > 0) SystemDebug.printf("openJPEG decompress started.\n");
// Get the image info.
    if (!imageData.GetImageData(pDDO, "openJPEG decompress", TRUE)) return FALSE;
//Jpeg 2000 is strange, should be just a stream, but could hav a header.
    while (TRUE)
    {
        if(!imageData.GetEncapsulatedDDO(NULL)) return FALSE;
        if(HaveNoJ2kStream(&imageData))break;
    }
    if(!imageData.CreateOutputArray()) return FALSE;
//Init some variables for uninitialized warnings.
    out16_ptr = NULL;
    mask = 0;
    bytes_jpc = 1;
    prec_jpc = 8;
    stream_len = 0;
//Configure the event callbacks.
    memset(&event_mgr, 0, sizeof(opj_event_mgr_t));
    event_mgr.error_handler = error_callback;
    event_mgr.warning_handler = warning_callback;
    event_mgr.info_handler = info_callback;
//Set decoding parameters to default values.
    opj_set_default_decoder_parameters(&parameters);
//Set decoding parameter format to stream.
    parameters.decod_format = CODEC_J2K;
    //Start the frames loop.
    moreImages = TRUE;
    while(moreImages)
    {
#ifdef NOVARAD_FIX
        if (((unsigned char *)(imageData.imageVR->Data))[imageData.imageVRLength -2] != 0xFF
            && ((unsigned char *)(imageData.imageVR->Data))[imageData.imageVRLength -2] != 0xD9)
        {
        //Put the end back on.
            if (!imageData.FixJpegEnd()) return FALSE;
        //Put the inPtr at the start of the stream.
            if (HaveNoJ2kStream(&imageData)) return FALSE;
        }
#endif
    //Get a decoder handle.
        dinfo = opj_create_decompress(CODEC_J2K);//Bad openjpeg, can't be reused!
    //Catch events using our callbacks and give a local context.
        opj_set_event_mgr((opj_common_ptr)dinfo, &event_mgr, stderr);
    //Setup the decoder decoding parameters using user parameters.
        opj_setup_decoder(dinfo, &parameters);
    //Open the memory as a stream.
        if(!(cio = opj_cio_open((opj_common_ptr)dinfo, (unsigned char *)imageData.inPtr,
                                (imageData.inEndPtr - imageData.inPtr) & (int)-1)))
        {
            opj_destroy_decompress(dinfo);
            OperatorConsole.printf("***[openJPEG decompress]: cannot open jpeg 2K data\n");
            return (FALSE);
        }
    //Decode the stream and fill the image structure
        decompImage = opj_decode(dinfo, cio);
    //Check the image
        if (!decompImage)
        {
            OperatorConsole.printf("***[openJPEG decompress]: Jpeg 2K code stream decode did not create an image\n");
            opj_destroy_decompress(dinfo);
            opj_cio_close(cio);
            return FALSE;
        }
    //Close the byte stream
        opj_cio_close(cio);
    //Do this the first time only
        if(!imageData.currFrame)
        {//Make the buffer.
        //Check for color.
            if(imageData.components > 1)
            {
                if((imageData.components != 3) || (decompImage->numcomps != 3))
                {
                    OperatorConsole.printf("***[DecompressJPEG2K]: Should be 3 colors, DICOM: %d ,J2K: %d \n",
                                           imageData.components , decompImage->numcomps);
                    opj_image_destroy(decompImage);
                    return ( FALSE );
                }
            }
        //Get the total uncompressed length.
            prec_jpc = decompImage->comps[0].prec;//Need it at the end
            bytes_jpc = ((prec_jpc -1) / 8) + 1;// Bytes or words.
            stream_len = (decompImage->comps[0].w * decompImage->comps[0].h);
            if(!imageData.CreateOutputBuffer(stream_len * bytes_jpc * imageData.components * imageData.frames))
            {
                opj_image_destroy(decompImage);
                return ( FALSE );
            }
			out16_ptr = (UINT16 *)imageData.outBuffer;
        //The same for all images
            mask = (1 << prec_jpc) - 1;
        }//End of make the data buffer
        //Get the data pointer(s)
        for ( cnt = 0; cnt < imageData.components; cnt++ )
        {
            jpc_out[cnt] = decompImage->comps[cnt].data;
        }
    //Image copy loops, open JPEG outputs ints.
        if( imageData.components == 1)
        {
            if(bytes_jpc == 2)
            {
                for(cnt = 0; cnt < stream_len; cnt++)
                {
#if NATIVE_ENDIAN == BIG_ENDIAN //Big Endian like Apple power pc
                    masked = (UINT16)(*jpc_out[0]++ & mask);
                    *(imageData.outPtr)++ = (unsigned char)(masked & 0xFF);
                    *(imageData.outPtr)++ = (unsigned char)((masked >> 8) & 0xFF);
#else //Little Endian
                    *out16_ptr++ = (UINT16)(*jpc_out[0]++ & mask);
#endif //Big Endian
                }
            }
            else for(cnt = 0; cnt < stream_len; cnt++)
                        *(imageData.outPtr)++ = (unsigned char)(*jpc_out[0]++ & mask);
        }
        else //RGB
        {
            for(cnt = 0; cnt < stream_len; cnt++)
            {
                *(imageData.outPtr)++ = (unsigned char)(*jpc_out[0]++ & mask);
                *(imageData.outPtr)++ = (unsigned char)(*jpc_out[1]++ & mask);
                *(imageData.outPtr)++ = (unsigned char)(*jpc_out[2]++ & mask);
            }
        }
    //Done with libopenjpeg for this loop.
        opj_destroy_decompress(dinfo);
        opj_image_destroy(decompImage);
    //Done with all of the frames.
        imageData.currFrame++;
        if(imageData.currFrame >= imageData.frames)break;
        //More images to read.
        while (TRUE)
        {
            if(!imageData.GetEncapsulatedDDO(NULL))
            {//Not good, can't find the next image!
                moreImages = FALSE;//Break the image loop.
                break;
            }
            if(HaveNoJ2kStream(&imageData))break;
        }
        // Loop back to open the memory as a stream.
    }//End of the frames loop
//Should we kill it and keep the uncompressed data?
    imageData.CheckEnoughFrames();
//Change the image vr to the bigger uncompressed and unencapsulated image.
    imageData.SwitchToOutputBuffer();
//The color stuff.
    if( imageData.components > 1) imageData.SetColorToRGB(FALSE);
//Change the transfer syntax to LittleEndianExplict!
    pDDO->ChangeVR( 0x0002, 0x0010, "1.2.840.10008.1.2.1\0", 'IU');
// If debug > 0, print decompress time.
    if (DebugLevel > 0) SystemDebug.printf("OpenJPEG %s decompress time %u milliseconds.\n",
                                           version, clock() - starttime);
    return (TRUE);
}
#else //Have libopenjpeg2
/* Error and message callback does not use the FILE* client object. */
void error_callback(const char *msg, void *client_data)
{
    UNUSED_ARGUMENT(client_data);
    OperatorConsole.printf("***[OpenJP2(J2k)]: %s\n", msg);
}

/* Warning callback expecting a FILE* client object. */
void warning_callback(const char *msg, void *client_data)
{
    UNUSED_ARGUMENT(client_data);
    OperatorConsole.printf("Warn[OpenJP2(J2k)]: %s\n", msg);
}

/* Debug callback expecting no client object. */
void info_callback(const char *msg, void *client_data)
{
    UNUSED_ARGUMENT(client_data);
    if(DebugLevel > 1)SystemDebug.printf("Info[OpenJP2(J2k)]: %s\n", msg);
}
// These routines are added to use memory instead of a file for input and output.
//Structure need to treat memory as a stream.
typedef struct
{
	OPJ_UINT8* pData; //Our data.
	OPJ_SIZE_T dataSize; //How big is our data.
	OPJ_SIZE_T offset; //Where are we currently in our data.
}opj_memory_stream;
//This will read from our memory to the buffer.
static OPJ_SIZE_T opj_memory_stream_read(void * p_buffer, OPJ_SIZE_T p_nb_bytes, void * p_user_data)
{
	opj_memory_stream* l_memory_stream = (opj_memory_stream*)p_user_data;//Our data.
	OPJ_SIZE_T l_nb_bytes_read = p_nb_bytes;//Amount to move to buffer.
	//Check if the current offset is outside our data buffer.
	if (l_memory_stream->offset >= l_memory_stream->dataSize) return (OPJ_SIZE_T)-1;
	//Check if we are reading more than we have.
	if (p_nb_bytes > (l_memory_stream->dataSize - l_memory_stream->offset))
		l_nb_bytes_read = l_memory_stream->dataSize - l_memory_stream->offset;//Read all we have.
	//Copy the data to the internal buffer.
	memcpy(p_buffer, &(l_memory_stream->pData[l_memory_stream->offset]), l_nb_bytes_read);
	l_memory_stream->offset += l_nb_bytes_read;//Update the pointer to the new location.
	return l_nb_bytes_read;
}
//This will write from the buffer to our memory.
static OPJ_SIZE_T opj_memory_stream_write(void * p_buffer, OPJ_SIZE_T p_nb_bytes, void * p_user_data)
{
	opj_memory_stream* l_memory_stream = (opj_memory_stream*)p_user_data;//Our data.
	OPJ_SIZE_T l_nb_bytes_write = p_nb_bytes;//Amount to move to buffer.
	//Check if the current offset is outside our data buffer.
	if (l_memory_stream->offset >= l_memory_stream->dataSize) return (OPJ_SIZE_T)-1;
	//Check if we are write more than we have space for.
	if (p_nb_bytes > (l_memory_stream->dataSize - l_memory_stream->offset))
		l_nb_bytes_write = l_memory_stream->dataSize - l_memory_stream->offset;//Write the remaining space.
	//Copy the data from the internal buffer.
	memcpy(&(l_memory_stream->pData[l_memory_stream->offset]), p_buffer, l_nb_bytes_write);
	l_memory_stream->offset += l_nb_bytes_write;//Update the pointer to the new location.
	return l_nb_bytes_write;
}
//Moves the pointer forward, but never more than we have.
static OPJ_OFF_T opj_memory_stream_skip(OPJ_OFF_T p_nb_bytes, void * p_user_data)
{
	opj_memory_stream* l_memory_stream = (opj_memory_stream*)p_user_data;
	OPJ_SIZE_T l_nb_bytes;

	if (p_nb_bytes < 0) return -1;//No skipping backwards.
	l_nb_bytes = (OPJ_SIZE_T)p_nb_bytes;//Allowed because it is positive.
	// Do not allow jumping past the end.
	if (l_nb_bytes >l_memory_stream->dataSize - l_memory_stream->offset)
		l_nb_bytes = l_memory_stream->dataSize - l_memory_stream->offset;//Jump the max.
	//Make the jump.
	l_memory_stream->offset += l_nb_bytes;
	//Returm how far we jumped.
	return l_nb_bytes;
}
//Sets the pointer to anywhere in the memory.
static OPJ_BOOL opj_memory_stream_seek(OPJ_OFF_T p_nb_bytes, void * p_user_data)
{
	opj_memory_stream* l_memory_stream = (opj_memory_stream*)p_user_data;

	if (p_nb_bytes < 0) return OPJ_FALSE;//No before the buffer.
	if (p_nb_bytes >(OPJ_OFF_T)l_memory_stream->dataSize) return OPJ_FALSE;//No after the buffer.
	l_memory_stream->offset = (OPJ_SIZE_T)p_nb_bytes;//Move to new position.
	return OPJ_TRUE;
}
//The system need a routine to do when finished, the name tells you what I want it to do.
static void opj_memory_stream_do_nothing(void * p_user_data)
{
	OPJ_ARG_NOT_USED(p_user_data);
}
//Create a stream to use memory as the input or output.
opj_stream_t* opj_stream_create_default_memory_stream(opj_memory_stream* p_memoryStream, OPJ_BOOL p_is_read_stream)
{
	opj_stream_t* l_stream;

	if (!(l_stream = opj_stream_default_create(p_is_read_stream))) return (NULL);
	//Set how to work with the frame buffer.
	if(p_is_read_stream)
		opj_stream_set_read_function(l_stream, opj_memory_stream_read);
	else
		opj_stream_set_write_function(l_stream, opj_memory_stream_write);
	opj_stream_set_seek_function(l_stream, opj_memory_stream_seek);
	opj_stream_set_skip_function(l_stream, opj_memory_stream_skip);
	opj_stream_set_user_data(l_stream, p_memoryStream, opj_memory_stream_do_nothing);
	opj_stream_set_user_data_length(l_stream, p_memoryStream->dataSize);
	return l_stream;
}
/* This routine will take all of the listed color spaces in 08_03 of the standard
 *  in little endian, uncompressed format and  compress it to jpeg2000 in whatever
 *  format it came in.  I don't know if dicom viewers can support this. It uses the
 *  openjpeg library version 2.1  You can get it here:
 *  https://github.com/uclouvain/openjpeg
 *  OpenJPEG can compress anything, with any size for each plane, but the standard only
 *  allows YBR_RCT lossless and YBR_ICT lossy.
 *  If I have made some mistakes (most likely), you can contact me bruce.barton
 *  (the mail symbol goes here) me.com.  Let me know where I can find a sample of
 *  the image that didn't work. */
BOOL CompressJPEG2Ko(DICOMDataObject* pDDO, int j2kQuality)
{
    ImageData           imageData;
    clock_t             starttime;
    UINT8		sgnd;
    UINT32		byteCnt;
    char		*bufferg_ptr, *bufferb_ptr, *out_ptr;
    unsigned int	mask;
    int 		*colorBuffer_ptr[3], *colorBuffer[3];
    size_t		cnt;
//Kind of Openjpeg Stuff (shound be)
	opj_memory_stream frameBufferStream;
//OpenJPEG Stuff
	OPJ_BOOL bSuccess;
	opj_cparameters_t	parameters;	// compression parameters.
	opj_stream_t *l_stream = 00;
	opj_codec_t* l_codec = 00;
	opj_image_t		*image = NULL;
    opj_image_comp_t	*comp;
    const char		comment[] = "Created by OpenJPEG version ";
    size_t		clen, vlen;
    const char		*version = opj_version();
    
//If debug > 0, get start time and print who.
    if (DebugLevel > 0)
    {
        starttime = clock();
        SystemDebug.printf("OpenJP2 %s compress started.\n", version);
    }
//Checked if the data is signed.
    if(!(sgnd = pDDO->GetBYTE(0x0028, 0x0103)))sgnd = 0;
//Get the data.
    if(!imageData.GetImageData(pDDO, "OpenJP2 compress", TRUE)) return FALSE;
//See if its real.
    if (!imageData.CalculateImageLengths())
    {//Should of had it, look for arrays
        if(!imageData.GetEncapsulatedDDO(NULL)) return FALSE;//no marker
    }
//Create the encapsulation array.
    if(!imageData.CreateOutputArray()) return FALSE;
//Check and set the quality for lossy.
    if(j2kQuality < MIN_QUALITY)// Set to 0 to use dicom.ini value.
    {
        j2kQuality = LossyQuality();//Use the default or dicom.ini value.
    }
    if(j2kQuality > 100) j2kQuality = 100;
//Make the input standard.
    if (imageData.components == 1)
    {
        if (!imageData.MakeStdGray()) return FALSE;
    }
    else if (!imageData.MakeYBR_FULL_OrRGB(TRUE))return FALSE;
//Allocate all of the memory needed.
//Make an frame buffer as big as an uncompressed image to hold the compressed stream.
	if (!imageData.CreateFrameBuffer(imageData.frameSize+guard))
	{
		OperatorConsole.printf("***[OpenJP2 compress]: Could not allocate an image frame buffer.\n");
		return (FALSE);
	}
//Set the "OpenJpeg like" stream data.
	frameBufferStream.pData = (OPJ_UINT8 *)imageData.frameBuffer;
	frameBufferStream.dataSize = imageData.frameSize+guard;
	frameBufferStream.offset = 0;
//Create comment for codestream buffer.
    clen = strlen(comment);//Made comment above, strnlen not needed.
    vlen = strnlen(version, 10);
    if((parameters.cp_comment = (char*)malloc(clen+vlen+1)) == NULL)
    {
        free(image);
        OperatorConsole.printf("***[OpenJP2 compress]: Could allocate the coment buffer.\n");
        return (FALSE);
    }
    memcpy(parameters.cp_comment, comment, clen);
    memcpy(&parameters.cp_comment[clen], version, vlen);
    parameters.cp_comment[clen + vlen] = 0;
//Created from image_to_j2k.c and convert.c.
//Set encoding parameters values.
    opj_set_default_encoder_parameters(&parameters);
    if(j2kQuality < 100)parameters.tcp_rates[0] = 100.0f/j2kQuality;
    else parameters.tcp_rates[0] = 0;// MOD antonin : losslessbug
    parameters.tcp_numlayers++;
    parameters.cp_disto_alloc = 1;
// Lossy (ICT) or lossless (RCT)?
    if(j2kQuality < 100) parameters.irreversible = 1;//ICT

//Print out some more info for debug.
    if (DebugLevel > 1) imageData.PrintInfo();
    mask = ((1 << imageData.precision) - 1);
//Frames loop.

    while(TRUE)
    {

//Create an image struct.
    if((image = (opj_image_t*)calloc(1, sizeof(opj_image_t))) == NULL)
    {
        OperatorConsole.printf("***[OpenJP2 compress]: Could not allocate an image structure.\n");
        return (FALSE);
    }
//Set image offset
    image->x0 = 0;
    image->y0 = 0;
//How many colors.
    image->numcomps = imageData.components;
//Decide if MCT should be used.
    parameters.tcp_mct = image->numcomps == 3 ? 1 : 0;
//Set the dimensions
    image->y1 = imageData.height;
    image->x1 = imageData.width;
//Check color.
    image->color_space = OPJ_CLRSPC_GRAY;// Set the default.
    if(image->numcomps == 3) image->color_space = OPJ_CLRSPC_SRGB;

//Allocate memory for the per-component information.
    if((image->comps = (opj_image_comp_t*)calloc(imageData.components, sizeof(opj_image_comp_t))) == NULL)
    {
        free(image);
        free(parameters.cp_comment);
        parameters.cp_comment = NULL;
        OperatorConsole.printf("***[OpenJP2 compress]: Could not create empty components.\n");
        return FALSE;
    }
    //Buffers are always needed for ints.
    for (cnt = 0; cnt < imageData.components; cnt++)
        if((colorBuffer[cnt] = (int *)malloc(imageData.singleColorFrameSize * sizeof(int))) == NULL)
        {
            free(image->comps);
            image->comps = NULL;
            free(image);
            free(parameters.cp_comment);
            parameters.cp_comment = NULL;
            OperatorConsole.printf("***[OpenJP2 compress]: Could allocate the component buffer.\n");
            return FALSE;
        }
//Create the individual image components.
    for(cnt = 0; cnt < image->numcomps; cnt++)
    {
        comp = &image->comps[cnt];
        comp->dx = parameters.subsampling_dx;
        comp->dy = parameters.subsampling_dy;
        comp->w = image->x1;
        comp->h = image->y1;
        comp->x0 = 0;
        comp->y0 = 0;
        comp->prec = imageData.precision;
        comp->bpp = imageData.precision_allocated;
        comp->sgnd = sgnd;
    // From now, forward, opj_image_destroy(image) will free the buffers.
        comp->data = colorBuffer[cnt];
    }

    //Almost never used (planes)
        bufferg_ptr = imageData.inPtr + imageData.singleColorFrameSize;
        bufferb_ptr = bufferg_ptr + imageData.singleColorFrameSize;
	//(Re)set the buffer pointers.
        for( cnt = 0; cnt < image->numcomps; cnt++)
            colorBuffer_ptr[cnt] = (int *)colorBuffer[cnt];

    // Get a J2K compressor handle.
        l_codec = opj_create_compress(OPJ_CODEC_J2K);

	//Catch events using our callbacks and give a local context.
		opj_set_info_handler(l_codec, info_callback, 00);
		opj_set_warning_handler(l_codec, warning_callback, 00);
		opj_set_error_handler(l_codec, error_callback, 00);
	//opj_set_event_mgr((opj_common_ptr)cinfo, &event_mgr, stderr);
	// Setup the encoder parameters using the current image and user parameters.
        opj_setup_encoder(l_codec, &parameters, image);
	
	// Open a byte stream for writing.
	if(!(l_stream = opj_stream_create_default_memory_stream(&frameBufferStream, OPJ_FALSE)))
        {
            OperatorConsole.printf("***[OpenJP2 compress]: Failed to allocate output stream memory.\n");
			opj_destroy_codec(l_codec);
            opj_image_destroy(image);
		free(parameters.cp_comment);
            parameters.cp_comment = NULL;
            return ( FALSE );
        }

	//Set the color stuff.
        switch(image->color_space)
        {
            case OPJ_CLRSPC_GRAY:
                if(imageData.precision_allocated == 8) // 8 Bits, char to int.
                {
                    for(byteCnt = 0; byteCnt < imageData.singleColorFrameSize; byteCnt++)
                    {
                        *(colorBuffer_ptr[0])++ = ((int)*imageData.inPtr & mask);
                        imageData.inPtr++;
                    }
                    break;
                }
            //16 Bits
                if(imageData.precision > 8 && imageData.precision <= 16)//Byte swap
                {
                    for(byteCnt = 0; byteCnt < imageData.singleColorFrameSize; byteCnt += 2)
                    {
			unsigned short s=((imageData.inPtr[0])&0xFF)|(((imageData.inPtr[1])&0xFF)<<8);
		          if (s>mask)
			  {
			    OperatorConsole.printf("***[OpenJP2 compress]: Error: input data out of range.\n");
					opj_destroy_codec(l_codec);
			    opj_image_destroy(image);
				free(parameters.cp_comment);
			    parameters.cp_comment = NULL;
			    return ( FALSE );
			  }                        
			  *(colorBuffer_ptr[0]) = ((int)*imageData.inPtr & 0xFF);
                        imageData.inPtr++;
                        *(colorBuffer_ptr[0])++ |= (((int)*imageData.inPtr & 0xFF) << 8) & mask;
                        imageData.inPtr++;
                    }
                    break;
                }
                //for(byteCnt = 0; byteCnt < imageData.frameSize; byteCnt += 2)// 8 in 16.
                //{
                //    *colorBuffer_ptr[0] = ((int)*imageData.inPtr  & mask);
                //    imageData.inPtr++;
                //    imageData.inPtr++;
               // }
                break;
            default: //OPJ_CLRSPC_SRGB:
                if(imageData.planes)
                {
                    for(byteCnt = 0; byteCnt < imageData.singleColorFrameSize; byteCnt++)
                    {
                        *(colorBuffer_ptr[0])++ = (int)*imageData.inPtr & mask;
                        imageData.inPtr++;
                        *(colorBuffer_ptr[1])++ = (int)*bufferg_ptr & mask;
                        bufferg_ptr++;
                        *(colorBuffer_ptr[2])++ = (int)*bufferb_ptr & mask;
                        bufferb_ptr++;
                    }
                    break;
                }//Normal RGB
                for(byteCnt = 0; byteCnt < imageData.singleColorFrameSize; byteCnt++)
                {
                    *(colorBuffer_ptr[0])++ = (int)*imageData.inPtr & mask;
                    imageData.inPtr++;
                    *(colorBuffer_ptr[1])++ = (int)*imageData.inPtr & mask;
                    imageData.inPtr++;
                    *(colorBuffer_ptr[2])++ = (int)*imageData.inPtr & mask;
                    imageData.inPtr++;
                }
                break;
        }//Filled the buffers and ready to go.
	//encode the image.

	bSuccess = opj_start_compress(l_codec, image, l_stream);
		if (!bSuccess)
		{
			OperatorConsole.printf("***[OpenJP2 compress]:OpenJpeg could not start encoding the image.\n");
		}
		else
		{
			bSuccess = opj_encode(l_codec, l_stream);
			if (!bSuccess)
			{
				OperatorConsole.printf("***[OpenJP2 compress]:OpenJpeg could not encode the image.\n");
			}
			else
			{
				bSuccess = opj_end_compress(l_codec, l_stream);
				if (!bSuccess)
				{
					OperatorConsole.printf("***[OpenJP2 compress]:OpenJpeg could not end encoding the image.\n");
				}
			}
		}

		//Done with not reusables.
                opj_destroy_codec(l_codec);
		opj_stream_destroy(l_stream);
		if (!bSuccess)
		{

			opj_image_destroy(image);
			free(parameters.cp_comment);
			parameters.cp_comment = NULL;
			return FALSE;
        }
        //Jpeg2k is encapsulated, make a new vr to encapsulate.
        if(!imageData.CreateOutputBuffer(frameBufferStream.offset & (UINT32)-1))//Sets outputImageLengthEven.
        {
  	    opj_image_destroy(image);
		free(parameters.cp_comment);
            parameters.cp_comment = NULL;
            return ( FALSE );
        }
    //Copy the Image data.
        out_ptr = (char *)frameBufferStream.pData;
        for(cnt = frameBufferStream.offset; cnt > 0; --cnt) *imageData.outPtr++ = *out_ptr++;
    //Done with the frameBuffer for now, reset it if more frames
		frameBufferStream.offset = 0;
    //Save the image object.
        if(!imageData.AddToOutputArray(imageData.outputImageLengthEven))
        {
            opj_image_destroy(image);
            free(parameters.cp_comment);
            parameters.cp_comment = NULL;
            return ( FALSE );
        }

    //Done with all of the frames.
        imageData.currFrame++;
        if(imageData.currFrame >= imageData.frames) break;
    //If not enough data, look in silly arrays.
        if (!imageData.CheckEnoughData(FALSE))
        {//Deal with silly input arrays.
            if(!imageData.GetEncapsulatedVR(NULL)) break;//No marker
        //New data, time to fix, make the input standard.
            if (imageData.components == 1)
            {
                if (!imageData.MakeStdGray())
                {
                    opj_image_destroy(image);
                    free(parameters.cp_comment);
                    parameters.cp_comment = NULL;
                    return ( FALSE );
                }
            }
            else if (!imageData.MakeRGB(TRUE))
            {
                opj_image_destroy(image);
                free(parameters.cp_comment);
                parameters.cp_comment = NULL;
                return ( FALSE );
            }
        }
        if(imageData.planes) imageData.inPtr = bufferb_ptr;
//At last, done with the image struct.  
opj_image_destroy(image);
    }//Back for the next frame, end of while(TRUE)

//Done with the comments.
    free(parameters.cp_comment);
    parameters.cp_comment = NULL;
//Should we kill it and keep the uncompressed data?
    imageData.CheckEnoughFrames();
//Finish encapsulating.
    imageData.AddToOutputArray((UINT32)-1);
//Save the new data.
    imageData.SwitchToOutputArray();
//Change the dicom parameters.
//Needed for anything that stores <= 8 in 16 (now 8 in 8).
    imageData.FixPrecision(imageData.precision);
//Set color if needed.
    if(imageData.components > 1)
    {
    //Reset the plane's VR, if there.
        imageData.SetPlanesToNone();
    //Set the color profile
        if((j2kQuality < 100))
            pDDO->ChangeVR( 0x0028, 0x0004, "YBR_ICT\0", 'CS');
        else pDDO->ChangeVR( 0x0028, 0x0004, "YBR_RCT\0", 'CS');
    }
//Change the transfer syntax to JPEG2K!
    if(j2kQuality < 100)
        pDDO->ChangeVR( 0x0002, 0x0010, "1.2.840.10008.1.2.4.91\0", 'IU');
    else pDDO->ChangeVR( 0x0002, 0x0010, "1.2.840.10008.1.2.4.90\0", 'IU');
// If debug > 0, print when finished
    if (DebugLevel > 0) SystemDebug.printf("OpenJP2 compress time %u milliseconds.\n",clock() - starttime);
    return (TRUE);
}

/* This routine will take in a jpeg2000 image and convert it to little endian, uncompressed,
 *  RGB or grayscale.  It uses the openjpeg library 2.1. You can get it here:
 *  https://github.com/uclouvain/openjpeg
 *  JPEG200 can compress almost anything, with any size for each plane.  So that means any
 *  of the color spaces and formats can be in it.  The dicom standard removes the header
 *  and color box infromation and sends just the data stream.  The standard states only
 *  MONO1,2 PALETTE_COLOR YBR_RCT and YBR_ICT can be use.  For the color space and
 *  image format, I have to trust libopenjpeg.
 *  If I have made some mistakes (most likely) you can contact me bruce.barton
 *  (the mail symbol goes here) me.com.  Let me know where I can find a sample of
 *  the image that didn't work. */
BOOL DecompressJPEG2Ko(DICOMDataObject* pDDO)
{
    ImageData                   imageData;
    clock_t                     starttime;
    BOOL                        moreImages;
    UINT16			*out16_ptr;
    unsigned int                cnt;
    int		*jpc_out[3], mask;
    int				bytes_jpc, prec_jpc;
    UINT32			stream_len;
#if NATIVE_ENDIAN == BIG_ENDIAN //Big Endian like Apple power pc
    int		masked;
#endif //Big Endian
//Kind of Openjpeg Stuff (shound be)
	opj_memory_stream inputStream;
// OpenJPEG stuff.
	OPJ_BOOL bSuccess;
	opj_dparameters_t	parameters;	// decompression parameters.
    opj_image_t			*decompImage = NULL;
	opj_codec_t*		l_codec = 00;
	opj_stream_t		*l_stream = NULL;
	const char			*version = opj_version();
    
//If debug > 0, get start time.
    if (DebugLevel > 0)starttime = clock();
    if (DebugLevel > 0) SystemDebug.printf("OpenJP2 decompress started.\n");
// Get the image info.
    if (!imageData.GetImageData(pDDO, "OpenJP2 decompress", TRUE)) return FALSE;
//Jpeg 2000 is strange, should be just a stream, but could have a header.
    while (TRUE)
    {
        if(!imageData.GetEncapsulatedDDO(NULL)) return FALSE;
        if(HaveNoJ2kStream(&imageData))break;
    }
    if(!imageData.CreateOutputArray()) return FALSE;
//Init some variables for uninitialized warnings.
    out16_ptr = NULL;
    mask = 0;
    bytes_jpc = 1;
    prec_jpc = 8;
    stream_len = 0;
//Set decoding parameters to default values.
    opj_set_default_decoder_parameters(&parameters);
//Set decoding parameter format to stream.
    parameters.decod_format = OPJ_CODEC_J2K;
//Start the frames loop.
    moreImages = TRUE;
    while(moreImages)
    {
#ifdef NOVARAD_FIX
        if (((unsigned char *)(imageData.imageVR->Data))[imageData.imageVRLength -2] != 0xFF
            && ((unsigned char *)(imageData.imageVR->Data))[imageData.imageVRLength -2] != 0xD9)
        {
        //Put the end back on.
            if (!imageData.FixJpegEnd()) return FALSE;
        //Put the inPtr at the start of the stream.
            if (HaveNoJ2kStream(&imageData)) return FALSE;
        }
#endif
    //Get a decoder handle.
		l_codec = opj_create_decompress(OPJ_CODEC_J2K);//Bad openjpeg, can't be reused!
    //Catch events using our callbacks and give a local context.
		opj_set_info_handler(l_codec, info_callback, 00);
		opj_set_warning_handler(l_codec, warning_callback, 00);
		opj_set_error_handler(l_codec, error_callback, 00);
    //Setup the decoder decoding parameters using user parameters.
		if (!opj_setup_decoder(l_codec, &parameters))
		{
			opj_destroy_codec(l_codec);
			OperatorConsole.printf("***[OpenJP2 decompress]: failed to setup the decoder\n");
			return (FALSE);
		}
	//Set up the input buffer as a stream
		inputStream.pData = (OPJ_UINT8 *)imageData.inPtr;
		inputStream.dataSize = imageData.inEndPtr - imageData.inPtr;
		inputStream.offset = 0;
	//Open the memory as a stream.
		if (!(l_stream = opj_stream_create_default_memory_stream(&inputStream, OPJ_TRUE)))
		{
			OperatorConsole.printf("***[OpenJP2 compress]: Failed to allocate output stream memory.\n");
			opj_destroy_codec(l_codec);
			return (FALSE);
		}
	// Read the main header of the codestream, if necessary the JP2 boxes, and create decompImage.
		if (!opj_read_header(l_stream, l_codec, &decompImage)) {
			OperatorConsole.printf("***[OpenJP2 decompress]: failed to read the header\n");
			opj_stream_destroy(l_stream);
			opj_destroy_codec(l_codec);
			opj_image_destroy(decompImage);
			return (FALSE);
		}
		//Decode the stream and fill the image structure
		bSuccess = opj_decode(l_codec, l_stream, decompImage);
		if(!bSuccess) OperatorConsole.printf("***[OpenJP2 decompress]: Jpeg 2K code stream did not decode.\n");
		else
		{
			bSuccess = opj_end_decompress(l_codec, l_stream);
			if (!bSuccess) OperatorConsole.printf("***[OpenJP2 decompress]: Jpeg 2K code stream did not end decode.\n");
		}
	//Done with the input stream and the decoder.
		opj_stream_destroy(l_stream);
		opj_destroy_codec(l_codec);
		if (!bSuccess)// Clean up if errored occured.
		{
			opj_image_destroy(decompImage);
			return FALSE;
		}
    //Do this the first time only
        if(!imageData.currFrame)
        {//Make the buffer.
        //Check for color.
            if(imageData.components > 1)
            {
                if((imageData.components != 3) || (decompImage->numcomps != 3))
                {
                    OperatorConsole.printf("***[OpenJP2 decompress]: Should be 3 colors, DICOM: %d ,J2K: %d \n",
                                           imageData.components , decompImage->numcomps);
                    opj_image_destroy(decompImage);
                    return ( FALSE );
                }
            }
        //Get the total uncompressed length.
            prec_jpc = decompImage->comps[0].prec;//Need it at the end
            bytes_jpc = ((prec_jpc -1) / 8) + 1;// Bytes or words.
            stream_len = (decompImage->comps[0].w * decompImage->comps[0].h);
            if(!imageData.CreateOutputBuffer(stream_len * bytes_jpc * imageData.components * imageData.frames))
            {
                opj_image_destroy(decompImage);
                return ( FALSE );
            }
            out16_ptr = (UINT16 *)imageData.outBuffer;
        //The same for all images
            mask = (1 << prec_jpc) - 1;
        }//End of make the data buffer
    //Get the data pointer(s)
        for ( cnt = 0; cnt < imageData.components; cnt++ )
        {
            jpc_out[cnt] = decompImage->comps[cnt].data;
        }
    //Image copy loops, open JPEG outputs ints.
        if( imageData.components == 1)
        {
            if(bytes_jpc == 2)
            {
                for(cnt = 0; cnt < stream_len; cnt++)
                {
#if NATIVE_ENDIAN == BIG_ENDIAN //Big Endian like Apple power pc
                    masked = (UINT16)(*jpc_out[0]++ & mask);
                    *(imageData.outPtr)++ = (unsigned char)(masked & 0xFF);
                    *(imageData.outPtr)++ = (unsigned char)((masked >> 8) & 0xFF);
#else //Little Endian
                    *out16_ptr++ = (UINT16)(*jpc_out[0]++ & mask);
#endif //Big Endian
                }
            }
            else for(cnt = 0; cnt < stream_len; cnt++)
                *(imageData.outPtr)++ = (unsigned char)(*jpc_out[0]++ & mask);
        }
        else //RGB
        {
            for(cnt = 0; cnt < stream_len; cnt++)
            {
                *(imageData.outPtr)++ = (unsigned char)(*jpc_out[0]++ & mask);
                *(imageData.outPtr)++ = (unsigned char)(*jpc_out[1]++ & mask);
                *(imageData.outPtr)++ = (unsigned char)(*jpc_out[2]++ & mask);
            }
        }
    //Done with libopenjpeg for this loop.
        opj_image_destroy(decompImage);
    //Done with all of the frames.
        imageData.currFrame++;
        if(imageData.currFrame >= imageData.frames)break;
    //More images to read.
        while (TRUE)
        {
            if(!imageData.GetEncapsulatedDDO(NULL))
            {//Not good, can't find the next image!
                moreImages = FALSE;//Break the image loop.
                break;
            }
            if(HaveNoJ2kStream(&imageData))break;
        }
    // Loop back to open the memory as a stream.
    }//End of the frames loop
//Should we kill it and keep the uncompressed data?
    imageData.CheckEnoughFrames();
//Change the image vr to the bigger uncompressed and unencapsulated image.
    imageData.SwitchToOutputBuffer();
//The color stuff.
    if( imageData.components > 1) imageData.SetColorToRGB(FALSE);
//Change the transfer syntax to LittleEndianExplict!
    pDDO->ChangeVR( 0x0002, 0x0010, "1.2.840.10008.1.2.1\0", 'IU');
//If debug > 0, print decompress time.
    if (DebugLevel > 0) SystemDebug.printf("OpenJP2 %s decompress time %u milliseconds.\n",
                                           version, clock() - starttime);
    return (TRUE);
}
#endif //End for no libopenjpeg2 else
#endif //End for libopenjpeg
#ifdef HAVE_LIBCHARLS
void jlsErrorOut(int jlsError, const char *errorFrom, const char *errorMessage)
{
//See if CharLS gave us a written message.
	if (errorMessage[0] != 0)
	{// Have something
		if (strnlen(errorMessage, 257) < 256)//Don't know if I trust charLS to terminate.
		{
			OperatorConsole.printf("%s%s\n", errorFrom, errorMessage);
			return;
		}
	}
    switch(jlsError)
    {
        case InvalidJlsParameters:
            OperatorConsole.printf("%sParameters are invalid\n", errorFrom);
            break;
        case ParameterValueNotSupported:
            OperatorConsole.printf("%sParameters are not supported\n", errorFrom);
            break;
        case UncompressedBufferTooSmall:
            OperatorConsole.printf("%sUncompressed buffer too small\n", errorFrom);
            break;
        case CompressedBufferTooSmall:
            OperatorConsole.printf("%sCompressed buffer too small\n", errorFrom);
            break;
        case InvalidCompressedData:
            OperatorConsole.printf("%sInvalid compressed data\n", errorFrom);
            break;
		case TooMuchCompressedData:
			OperatorConsole.printf("%sToo much compressed date\n", errorFrom);
			break;
		case ImageTypeNotSupported:
            OperatorConsole.printf("%sImage type not supported\n", errorFrom);
            break;
        case UnsupportedBitDepthForTransform:
            OperatorConsole.printf("%sUnsupported bit depth for transform\n", errorFrom);
            break;
        case UnsupportedColorTransform:
            OperatorConsole.printf("%sUnsupported color for transform\n", errorFrom);
            break;
        case UnsupportedEncoding:
            OperatorConsole.printf("%sNot a JPEG-LS Image\n", errorFrom);
            break;
		case UnknownJpegMarker:
			OperatorConsole.printf("%sUnknown JPEG-LS marker\n", errorFrom);
			break;
		case MissingJpegMarkerStart:
			OperatorConsole.printf("%sNo start marker found\n", errorFrom);
			break;
		default: //CharlsApiResultType::UnexpectedFailure: or CharlsApiResultType::OK w/ too long text.
			OperatorConsole.printf("%sUnknown failure\n", errorFrom);
			break;
	}
}

BOOL CompressJPEGLS(DICOMDataObject* pDDO, int jpegQuality)
{
    ImageData                   imageData;
    size_t                      bytesWriten;
    clock_t                     starttime;
// CharLS stuff
    struct JlsParameters        jlsParamaters;
	int         jlsError;
	char                        jlsErrorMessage[256];

//If debug > 0, get start time.
    if (DebugLevel > 0)starttime = clock();
    if (DebugLevel > 1) SystemDebug.printf("JPEG-LS compress started.\n");
// Get the image info.
    if (!imageData.GetImageData(pDDO, "CompressJPEGLS", TRUE)) return FALSE;
//See if its real.
    if (!imageData.CalculateImageLengths())
    {//Should of had it, look for arrays
        if(!imageData.GetEncapsulatedDDO(NULL)) return FALSE;//no marker
    }
//Clear the structure.
    memset(&jlsParamaters, 0, sizeof(jlsParamaters));
// Check and set the quality for lossy.
    if(jpegQuality < MIN_QUALITY)// Set to 0 to use dicom.ini value.
    {
        jpegQuality = LossyQuality();//Use the default or dicom.ini value.
    }
    if(jpegQuality > 100) jpegQuality = 100;
// 0 is lossless and 9 is really bad
    jlsParamaters.allowedlossyerror = 10 - (int)((jpegQuality + 9) / 10);
// Set all the image size stuff.
    jlsParamaters.width = imageData.width;
    jlsParamaters.height = imageData.height;
    jlsParamaters.components = imageData.components;
    jlsParamaters.bitspersample = imageData.precision;//2-16 allowed.
    if(imageData.planes == 0)
    {
        jlsParamaters.ilv = ILV_NONE;
        jlsParamaters.bytesperline = imageData.width * imageData.byteWidth;
    }
    else
    {
        jlsParamaters.ilv = ILV_LINE;
        jlsParamaters.bytesperline = imageData.width;//Planes, never more than 8 bits.
    }
//CharLS Only works with 8 or 16 allocated .
    if(imageData.components == 1)
    {
        if(!imageData.MakeStdGray()) return FALSE;
    }
//CharLS Only works with RGB regular or planes.
    else if(!imageData.MakeRGB(TRUE)) return FALSE;
//The output is an array of DDOs.
    if(!imageData.CreateOutputArray()) return FALSE;
//Start of data.
    imageData.inPtr = ((char *)imageData.imageVR->Data);
//End of data.
    imageData.inEndPtr = ((char *)imageData.imageVR->Data) + imageData.imageVR->Length;
//The frames loop.
    while (TRUE)
    {
    //Check for enough data.
        if (!imageData.CheckEnoughData(TRUE)) break;
    //Set where to put it. Make it as big as uncompressed data.
        if(!imageData.CreateOutputBuffer(imageData.frameSize+guard)) return FALSE;//Always too big for compressed data.
	//Clear jsl error text.
	jlsErrorMessage[0] = 0;
        int mask = ((1 << imageData.precision) - 1);
	if (imageData.precision > 8 && imageData.precision <= 16)
          for(int byteCnt = 0; byteCnt < imageData.frameSize; byteCnt += 2)
          { unsigned short s=((imageData.inPtr[byteCnt])&0xFF)|(((imageData.inPtr[byteCnt+1])&0xFF)<<8);
	    if (s>mask)
            { OperatorConsole.printf("***[CompressJPEGLS]: Error: input data out of range.\n");
	      return ( FALSE );
            }
	  }

    //Make it smaller.
	if((jlsError = JpegLsEncode((void *)imageData.outBuffer, imageData.frameSize+guard, &bytesWriten,
            (const void *)imageData.inPtr, imageData.frameSize, &jlsParamaters)) !=
			OK)
        {
            jlsErrorOut(jlsError, "***[CompressJPEGLS] Jpeg LS Encode", "");
            return false;
        }
        if (DebugLevel > 1)
            SystemDebug.printf("JPEG-LS compress %d bytes to %d bytes\n", imageData.frameSize, bytesWriten);
    //Encapsulate an image object.
        if(!imageData.AddToOutputArray(bytesWriten & (UINT32)-1)) return FALSE;
    //Still here, setup for the next frame
        imageData.currFrame++;
        if(imageData.currFrame >= imageData.frames)break;//Finished while(TRUE) loop
        imageData.inPtr += imageData.frameSize;//Next frame.
    //If enough data, next loop.
        if (imageData.CheckEnoughData(FALSE)) continue;
    //Deal with silly input arrays
        if(!imageData.GetEncapsulatedVR(NULL)) break;//No markers
    //New data, fix again, CharLS Only works with 8 or 16 allocated .
        if(imageData.components == 1)
        {
            if(!imageData.MakeStdGray()) return FALSE;
        }
    //CharLS Only works with RGB regular or planes.
        else if(!imageData.MakeRGB(TRUE)) return FALSE;
    }//End of while(TRUE), go back for the next frame.
//Should we kill it and keep the uncompressed data?
    imageData.CheckEnoughFrames();
//End the array.
    if(!imageData.AddToOutputArray((UINT32)-1)) return FALSE;
// Change the image vr to the encapsulated image(s).
    imageData.SwitchToOutputArray();
// Change the dicom parameters
    //Needed for anything that stores <= 8 in 16 (now 8 in 8).
    imageData.FixPrecision(imageData.precision);
//Change the transfer syntax to JPEGLS!
    if(jpegQuality < 100)
        pDDO->ChangeVR( 0x0002, 0x0010, "1.2.840.10008.1.2.4.81\0", 'IU');
    else pDDO->ChangeVR( 0x0002, 0x0010, "1.2.840.10008.1.2.4.80\0", 'IU');
// Done with CharLS.
    if (DebugLevel > 0) SystemDebug.printf("JPEGLS compress time %u milliseconds.\n",clock() - starttime);
    return TRUE;
}

BOOL DecompressJPEGLS(DICOMDataObject* pDDO)
{
    int                        outBytes;
    ImageData                  imageData;
    const unsigned char        jpegSOImarker[3] = { 0xFF, 0xD8, 0 };
    clock_t                    starttime;
// CharLS stuff
    struct JlsParameters       jlsParamaters;
	int        jlsError;
	char                       jlsErrorMessage[256];
    
//If debug > 0, get start time.
    if (DebugLevel > 0)starttime = clock();
    if (DebugLevel > 1) SystemDebug.printf("JPEGLS decompress started.\n");
//Get the image info and encapsulated data. JpegLS starts with jpeg tag FF D8.
    if(!imageData.GetImageData(pDDO, "DecompressJPEGLS", FALSE)) return FALSE;
    if(!imageData.GetEncapsulatedDDO(jpegSOImarker))return FALSE;//No Jpeg data found
//Get memory for the image.
    if(!imageData.CreateOutputBuffer(imageData.outputImageLengthEven)) return (FALSE);
//Clear jsl error text.
	jlsErrorMessage[0] = 0;
    jlsParamaters.outputBgr = 0;
    while (TRUE)
    {
//Check image header
        if((jlsError = JpegLsReadHeader((const void*)imageData.imageVR->Data, imageData.imageVRLength, &jlsParamaters)) != OK)
        {
			jlsErrorOut(jlsError, "***[DecompressJPEGLS] Read Header: ", "");
            return false;
        }
        if (DebugLevel > 2) {
            SystemDebug.printf("Debug[DecompressJPEGLS] Header values: H = %d, W = %d, Bits = %d, Components = %d\n",
                               jlsParamaters.height, jlsParamaters.width, jlsParamaters.bitspersample, jlsParamaters.components);
        }
    //Check frame size
        outBytes = 1;
        if( jlsParamaters.bitspersample > 8) outBytes = 2;
        if(imageData.outputImageLengthEven < 
			(UINT32)((jlsParamaters.width * jlsParamaters.height * jlsParamaters.components * outBytes) & 0x7FFFFFFF))
        {
            OperatorConsole.printf("***[DecompressJPEGLS]: Uncompressed image size of %d bytes bigger than %d bytes allocated.\n",
                                   (jlsParamaters.width * jlsParamaters.height * jlsParamaters.components * outBytes),
                                   imageData.outputImageLengthEven);
            return false;
        }
    //Get memory for the thumbnail.  Use the imageData.reformatBuf pointer, freed when we return.
//        if((imageData.spareBuffer = (char *)malloc(jlsParamaters.jfif.Xthumbnail * jlsParamaters.jfif.Xthumbnail * 3)) == NULL)
//        {
            //OperatorConsole.printf("***[DecompressJPEGLS]: Could not allocate %d bytes of memory.\n",
//                                   (jlsParamaters.jfif.Xthumbnail * jlsParamaters.jfif.Xthumbnail * 3));
//            return false;
//        }
//        jlsParamaters.jfif.thumbnail = imageData.spareBuffer;
    //Set rgb output.
        jlsParamaters.ilv = ILV_NONE;
    //Decode the image.
        jlsError = JpegLsDecode(imageData.outBuffer + (imageData.frameSize * imageData.currFrame), imageData.frameSize,
                                imageData.imageVR->Data, imageData.imageVRLength, &jlsParamaters);
    //Check for errors
        if(jlsError != OK)
        {
            jlsErrorOut(jlsError, "***[DecompressJPEGLS] Jpeg LS Decode: ", "");
            return false;
        }
    // check for the end
        if( ++(imageData.currFrame) >= imageData.frames )break;//Normal way out.
    // More images to read
        if(!imageData.GetEncapsulatedVR(jpegSOImarker))break;
    // OK, have another image now.
    //Free thumbnail memory
        //if(imageData.spareBuffer != NULL) free(imageData.spareBuffer);
        //jlsParamaters.jfif.thumbnail = imageData.spareBuffer = NULL;
    }// Loop back to decompress (while(TRUE))
//Done decompressing.
    if(imageData.currFrame < imageData.frames)
        OperatorConsole.printf( "Warn[DecompressJPEGLS]: Found %d of %d frames.\n",imageData.currFrame, imageData.frames);
// Change the image vr to the bigger uncompressed and unencapsulated image.
    imageData.SwitchToOutputBuffer();
//Needed for anything that stores <= 8 in 16 (now 8 in 8).
    imageData.FixPrecision(jlsParamaters.bitspersample);
// Set color stuff if needed.
    if( imageData.components > 1) imageData.SetColorToRGB(FALSE);
//Change the transfer syntax to LittleEndianExplict!
    pDDO->ChangeVR( 0x0002, 0x0010, "1.2.840.10008.1.2.1\0", 'IU');
// If debug > 0, print decompress time.
    if (DebugLevel > 0) SystemDebug.printf("JPEGLS decompress time %u milliseconds.\n",clock() - starttime);
    return TRUE;
}
#endif //End for libCharLS