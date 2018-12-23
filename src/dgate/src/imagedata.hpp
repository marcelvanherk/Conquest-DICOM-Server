/****************************************************************************
 
 THIS SOFTWARE IS MADE AVAILABLE, AS IS, AND IMAGE RETERIVAL SERVICES
 DOES NOT MAKE ANY WARRANTY ABOUT THE SOFTWARE, ITS
 PERFORMANCE, ITS MERCHANTABILITY OR FITNESS FOR ANY PARTICULAR
 USE, FREEDOM FROM ANY COMPUTER DISEASES OR ITS CONFORMITY TO ANY
 SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND PERFORMANCE OF
 THE SOFTWARE IS WITH THE USER.
 
 Copyright of the software and supporting documentation is
 owned by the Image Reterival Services, and free access
 is hereby granted as a license to use this software, copy this
 software and prepare derivative works based upon this software.
 However, any distribution of this software source code or
 supporting documentation or derivative works (source code and
 supporting documentation) must include this copyright notice.
 ****************************************************************************/

/***************************************************************************
 *
 * Image Reterival Services
 * Version 0.1 Beta
 *
 ***************************************************************************/

/*
 bcb 20131224: Created this file.
 bcb 20151203: Moved some dependent defines from total.cpp to here.
 mvh 20160619: bcb added sgnd flag
*/
#ifndef imagedata_hpp
#define imagedata_hpp

#include "dicom.hpp"

// dependent defines
#ifdef BCB_FIX // Became tired of retyping on different systems.
#define FUJI_FIX // Allows decompressing Fuji AC3 jpeg compressed,not encapsulated images.
#define NOVARAD_FIX // Works around the intentional corruptions NovaPacs (NovaRad) adds to the images.
#endif
// dependent defines
#define CLRSPC_FAM_UNKNOWN 0
#define CLRSPC_FAM_GRAYSCALE 1
#define CLRSPC_FAM_RGB 2
#define CLRSPC_FAM_YBR_FULL 3
//Skipped numbers for Jasper //
#define CLRSPC_FAM_YCBCR_ICT 6
#define CLRSPC_FAM_YCBCR_RCT 7
#define CLRSPC_FAM_YCBCR_F422 8
#define CLRSPC_FAM_YCBCR_422 9
#define CLRSPC_FAM_YCBCR_420 10

#define ASC_COLOR_MAX_LENGTH 18
#define MAX_HEADER_LEN 20

class   ImageData
        {
        private:
            const char *who;//Name of the calling code.
            DICOMDataObject* pddo;//Passed DDO
            size_t  ascColorLength;//Vr(0x0028, 0x0004) length
        public://Image dicriptions
            Array < DICOMDataObject *> *outArrayPtr;//Output Image Encapsulating DDO
            Array < DICOMDataObject *> *inArrayPtr;//Input Image Encapsulating DDO
            unsigned int inArrayCnt;
            VR      *image7fe0_10;//The image or encapsulating Vr
            INT     frames;
            UINT    height;
            UINT    width;
            UINT8   components;
            UINT8	sgnd;
            INT     colorSpace;
            UINT8   precision_allocated;
            UINT8   precision;
            UINT8   byteWidth;
            UINT32  singleColorFrameSize;
            UINT32  frameSize;
            UINT32  outputImageLengthEven;//Cannot be odd.
            UINT    planes;//0x0028, 0x0006
            UINT32  imageVRLength;//Can be odd.
            char    ascColor[ASC_COLOR_MAX_LENGTH];//0x0028, 0x0004
            //Data pointers.
            char    *inBuffer;//Used to make an input to a compressor useable.
            char    *inPtr;
            char    *inEndPtr;
            char    *outBuffer;//Output of a decompressor or compressor.
            char    *outPtr;
            char    *frameBuffer;//Fits an uncompressed frame.
            char    *spareBuffer;//Used by compressors and decompressors
            VR      *imageVR;
            //Data counters.
            INT     currFrame;
            int     currSQObject;
            ImageData();
            ~ImageData();
        //Add a compressed image to the array and create the array if needed.
            BOOL    AddToOutputArray(UINT32 size);
        //Sets singleColorFrameSize, frameSize, outputImageLengthEven, inPtr, and inEndPtr.
        //Check imageVRLength if needed to odd. Returns false if VR lenght 0 or -1.
            BOOL    CalculateImageLengths();
        //Checks if the current frame has enough data.
            BOOL    CheckEnoughData(BOOL warn);
        //Checks the number of frames done and reports if to little.
            void    CheckEnoughFrames();
        //Creates an output encapsulation array.
            BOOL    CreateOutputArray();
        //Allocates a buffer the size of uncompressed frame.
            BOOL    CreateFrameBuffer(UINT32 size);
        //Allocates an output buffer.
            BOOL    CreateOutputBuffer(INT32 bufferSize);
        //Checks the precision, precision allocated and fixes if needed.
        //All routines should fix the <= 8 in 16, call this to do it.
            void    FixPrecision(UINT8 data_precision);
            BOOL    DePlaneImage();
            BOOL    GetEncapsulatedDDO(const unsigned char *hexHeader);
            BOOL    GetEncapsulatedVR(const unsigned char *hexHeader);
        //Compress needs color information.
            BOOL    GetImageData(DICOMDataObject* pDDO, const char *fromWho, BOOL compress);
            BOOL    MakeRGB(BOOL planesAllowed);
            BOOL    MakeStdGray();
            BOOL    MakeYBR_FULL();
            BOOL    MakeYBR_FULL_OrRGB(BOOL planesAllowed);
#ifdef NOVARAD_FIX
            BOOL    FixJpegEnd();
#endif
            BOOL    SetColorTo(const char *name, BOOL planesAllowed);
            BOOL    SetColorToRGB(BOOL planesAllowed);//0x0028, 0x0004
            void    SetPlanesToNone();
            BOOL    SwitchToOutputArray();//Changes the image VR to to compressed data.
            BOOL    SwitchToOutputBuffer();//Changes the image VR to to output.
            void    PrintInfo();
            
        private:// This will prevent it from being copied (it has pointers)
            ImageData(const ImageData&);
            const	ImageData & operator = (const ImageData&);
        };
#endif
