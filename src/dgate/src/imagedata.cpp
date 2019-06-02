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
 bcb 20160203: Fix error to work with OsiriX JPEG-LS and strncpy warning.
 mvh 20160619: bcb added sgnd flag
 mvh 20161204: mvh catch imageVR not present
 mvh 20190602: mvh Fix conversion, reload after To8BitGray and To16BitGray
*/

#ifdef _WIN32
#include <stdlib.h>
#define STRNCPY(o,i,c) strncpy_s(o,i,c)
#else 
#define STRNCPY(o,i,c) strncpy(o,i,c)
#endif

#include "imagedata.hpp"
#include "uncompconv.hpp"
#include "nkiqrsop.hpp"
#include "util.h"
#include "dprintf.hpp"

extern	int	DebugLevel;

//Creates the class and sets the defaults.
ImageData   ::   ImageData():frames(1), currFrame(0), height(0), colorSpace(CLRSPC_FAM_GRAYSCALE),
width(0), components(1), sgnd(0), precision_allocated(8), precision(8), byteWidth(1), singleColorFrameSize(0), 
frameSize(0), outputImageLengthEven(0), planes(0), frameBuffer(NULL), spareBuffer(NULL),
outBuffer(NULL), imageVR(NULL), currSQObject(-1), inArrayPtr(NULL), outArrayPtr(NULL), who(NULL),
image7fe0_10(NULL), ascColorLength(0), inPtr(NULL), inBuffer(NULL), inArrayCnt(0)

{
    ascColor[0] = 0;
}

ImageData   ::   ~ImageData()
{
    if (inBuffer) free(inBuffer);
    if (spareBuffer) free(spareBuffer);
    if (outBuffer) free(outBuffer);
    if (frameBuffer) free(frameBuffer);
    if (outArrayPtr)
    {
        while(outArrayPtr->GetSize())//bcb Leak! Added 14/4/14.
        {
            delete outArrayPtr->Get(0);
            outArrayPtr->RemoveAt(0);
        }
        delete outArrayPtr;
    }
//Input arrays distroyed by image7fe0_10->Reset.
}

BOOL ImageData   ::   AddToOutputArray(UINT32 size)
{
    DICOMDataObject	*DDO;
    VR			*vrs;
    char                *outTemp;

//Check for good call.
    if(outArrayPtr == NULL) return FALSE;
    if(size == 0) vrs = new VR(0xfffe, 0xe000, 0, (void *)NULL, FALSE);//An empty vr.
    else if(size == (UINT32)-1) vrs = new VR(0xfffe, 0xe0dd, 0, (void *)NULL, FALSE);//Ending vr.
    else
    {//Data vr.
    //Check for good call.
        if(outBuffer == NULL || size > outputImageLengthEven) return FALSE;
        if(size & 1) outBuffer[size++] = 0;//Make even
    //If the size change is big, the location may move, freeing outBuffer!
        if((outTemp = (char *)realloc(outBuffer, size)) == NULL)//Make it the cottect size.
        {
            OperatorConsole.printf("***[%s]:: Failed to reallocate a new smaller buffer.\n", who);
            return false;
        }
        vrs = new VR(0xfffe, 0xe000, size, (void *)outTemp, TRUE);
    //The VR now owns the output buffer.
        outBuffer = NULL;
    }
    if(vrs == NULL)
    {
        OperatorConsole.printf("***[%s]:: Failed to allocate a new VR.\n", who);
        return false;
    }
//Save the image object.
    if((DDO = new DICOMDataObject) == NULL)
    {
        OperatorConsole.printf("***[%s]:: Failed to allocate a new DDO.\n", who);
        return false;
    }
    DDO->Push(vrs);
    outArrayPtr->Add(DDO);
    return TRUE;
}

BOOL ImageData   ::   CalculateImageLengths()
{
    // Set the size of each image
    singleColorFrameSize = width * height * byteWidth;
    frameSize = singleColorFrameSize * components;
    outputImageLengthEven = frameSize * frames;
    if(outputImageLengthEven & 1 ) outputImageLengthEven++;//Odd length, make it even.
    // Fix the stream length.
    imageVRLength = imageVR->Length;
    if (imageVRLength == 0 || imageVRLength == (UINT32)-1) return FALSE;//Not real, must have data in array
    //Check for uncompressed data that ends in 0.
    if (imageVRLength != frameSize &&  imageVRLength != frameSize * frames)
        if(((char *)(imageVR->Data))[imageVRLength - 1] == 0) imageVRLength--;
    inPtr = (char *)imageVR->Data;
    inEndPtr = inPtr + imageVRLength;
    return TRUE;
}

BOOL ImageData   ::   CheckEnoughData(BOOL warn)
{
    if((inPtr + frameSize) > inEndPtr)
    {
        if (warn)OperatorConsole.printf("Warn[$s]: Ran out of image data on frame %d of %d.\n",
                               who, currFrame, frames);
        return FALSE;
    }
    return TRUE;
}

void ImageData   ::   CheckEnoughFrames()
{
    if(currFrame < frames)
        OperatorConsole.printf( "Warn[%s]: Only found %d of %d frames.\n", who, currFrame, frames);
}

BOOL ImageData   ::   CreateOutputArray()
{
// Create the encapsulation array.
    if((outArrayPtr = new Array < DICOMDataObject * >) == NULL)
    {
        OperatorConsole.printf("***[%s]:: Failed to allocate a new image array.\n", who);
        return false;
    }
// The first blank object.
    return AddToOutputArray(0);
}

BOOL ImageData   ::   CreateFrameBuffer(UINT32 size)
{
    if((frameBuffer = (char *)malloc(size)) == NULL)
    {
        OperatorConsole.printf("***[%s]: Could not allocate %d bytes of memory.\n", who, (size));
        return FALSE;
    }
    return TRUE;
}


BOOL ImageData   ::   CreateOutputBuffer(INT32 bufferSize)
{
    outputImageLengthEven = bufferSize;
    if(outputImageLengthEven & 1)outputImageLengthEven++;
    if((outBuffer = (char *)malloc(outputImageLengthEven)) == NULL)
    {
        OperatorConsole.printf("***[%s]: Could not allocate %d bytes of memory for an output buffer.\n", who, outputImageLengthEven);
        return FALSE;
    }
    outBuffer[outputImageLengthEven - 1] = 0;//Never hurts!
    outPtr = outBuffer;
    return TRUE;
}

BOOL ImageData   ::   DePlaneImage()
{
    char    *tempBuff;

    if((tempBuff = DePlane((char *)imageVR->Data, (int)imageVR->Length)) == NULL)
    {
        OperatorConsole.printf("***[%s]: Could not allocate DePlaning memory.\n", who);
        return FALSE;
    }
    free(imageVR->Data);
    imageVR->Data = tempBuff;
    SetPlanesToNone();
    return TRUE;
}

void ImageData   ::   FixPrecision(UINT8 data_precision)
{
    if(data_precision <= 8 && precision_allocated > 8) pddo->ChangeVR( 0x0028, 0x0100, (UINT8)8, 'US');
    if(data_precision > precision_allocated)//Must always be less.
    {
        pddo->ChangeVR( 0x0028, 0x0101, data_precision, 'US');
        pddo->ChangeVR( 0x0028, 0x0102, (UINT8)(data_precision - 1), 'US');
    }
}

BOOL ImageData   ::   GetEncapsulatedDDO(const unsigned char *hexHeader)
{
//Check for bad call.
    if(!image7fe0_10) return (FALSE);
//Get the encapsulated data object.
    if(!inArrayPtr)
        inArrayPtr = (Array<DICOMDataObject *> *) image7fe0_10->SQObjectArray;
    if(!inArrayPtr)
    {
        OperatorConsole.printf("***[%s]: No image array found\n", who);
        return (FALSE);
    }
    if ((inArrayCnt = inArrayPtr->GetSize()) <= 0)
    {
        OperatorConsole.printf("***[%s]: No image array data found\n", who);
        return (FALSE);
    }
// currSQObject starts as -1 and is inc. to start value of 0.
    return ( GetEncapsulatedVR(hexHeader) );
}

BOOL ImageData   ::   GetEncapsulatedVR(const unsigned char *hexHeader)
{
    DICOMDataObject*    ddo;
    size_t              cnt, headerLength;
    
//Check for bad call.
    if(!inArrayPtr || inArrayCnt == 0) return (FALSE);
//Are we looking for something? Normal string functions do not always work, unsigned vs signed.
    headerLength = 0;//To undo dec later.
    if(hexHeader != NULL)
        while (hexHeader[headerLength] != 0 && headerLength <= MAX_HEADER_LEN) headerLength++;
    if (headerLength > MAX_HEADER_LEN)
    {
        OperatorConsole.printf("***[%s]: Made a bad call. Header length > %d\n", who, MAX_HEADER_LEN);
        return (FALSE);
    }
//Look though the array.
    while(++currSQObject < (int)inArrayCnt)
    {
        ddo = inArrayPtr->Get(currSQObject);//Get the array.
        imageVR = ddo->GetVR(0xfffe, 0xe000);//Get the data
        if(!imageVR) continue;//No VR, try again.
        if(!imageVR->Length) continue;//No length, try again.
    //If requested, look for the jpeg SOI marker, 0 headerLength falls though "for".
        for( cnt = 0; cnt < headerLength; cnt++)
            if(((unsigned char *)(imageVR->Data))[cnt] != hexHeader[cnt])break;
        if (headerLength == cnt && CalculateImageLengths()) return (TRUE);
    }//Loop back.
//Ran out of objects.
    OperatorConsole.printf("***[%s]: No image data found.\n", who);
    return (FALSE);
}

BOOL ImageData   ::   GetImageData(DICOMDataObject* pDDO, const char *fromWho, BOOL compress)
{
//Save the caller.
    if((who = fromWho) == NULL)
        who = "Unknown";
//Check for a good call.
    if((pddo = pDDO) == NULL)
    {
        SystemDebug.printf("***[ImageData]: Passed a bad data dicom object from: %s.\n", who);
        return(FALSE);
    }
// Are there frames?
    if(!(frames = pddo->Getatoi(0x0028, 0x0008))) frames = 1;
// Width, height, and componets for uncompressed memory allocation.
    if(!(height = pddo->GetUINT(0x0028, 0x0010)))
    {
        SystemDebug.printf("***[%s]: Failed to get image height.\n", who);
        return(FALSE);
    }
// Get the Columns VR and check size.
    if(!(width = pddo->GetUINT(0x0028, 0x0011)))
    {
        OperatorConsole.printf("***[%s]: Failed to get image width.\n", who);
        return(FALSE);
    }
// Get the number of samples per pixel VR.
    if(!(components = pddo->GetBYTE(0x0028, 0x0002))) components = 1;  // Gray default.
// Get the number of bits allocated.
    precision_allocated = pddo->GetBYTE(0x0028, 0x0100);
// Get the number of bits stored.
    if(!(precision = pddo->GetBYTE(0x0028, 0x0101))) precision = precision_allocated;
    if(!precision_allocated)
    {
        if(precision) precision_allocated = precision;
        else
        {// All 0, set the defaults.
            precision_allocated = 8;
            precision = 8;
        }
    }
    if(precision_allocated > 8 || precision > 8) byteWidth = 2;
// Color is needed for compression
    if(compress)
    {
        char *tempStr;
        switch(components)
        {
            case 1:// Leave the defaults, CLRSPC_FAM_GRAYSCALE.
                break;
            case 3:// Look for the color type
                if((tempStr = pddo->GetCString (0x0028, 0x0004, ASC_COLOR_MAX_LENGTH)) == NULL || // Get the color profile.
                   (ascColorLength = strlen(ascColor)) < 3) SetColorToRGB(FALSE);  //Check it length.
                else STRNCPY(ascColor, tempStr, ASC_COLOR_MAX_LENGTH);
                if (tempStr) free(tempStr);
                if(strnieq(ascColor, "RGB",3))
                {
                    colorSpace = CLRSPC_FAM_RGB;// RGB
                    break;
                }
                if (ascColorLength > 6 && strnieq(ascColor, "YBR_",4))
                {
                    if (ascColorLength > 7 && strnieq(&ascColor[4], "FULL",4))// YBR_FULL(_422)
                    {
                        if (ascColorLength > 11 && strnieq(&ascColor[8], "_422",4))// YBR_FULL_422
                            colorSpace = CLRSPC_FAM_YCBCR_F422;
                        else colorSpace = CLRSPC_FAM_YBR_FULL;// YBR_FULL
                        break;
                    }
                    if (ascColorLength > 14 && strnieq(&ascColor[4], "PARTIAL_42",10))// YBR_PARTIAL_42x
                    {
                        if(ascColor[14] == '0')colorSpace = CLRSPC_FAM_YCBCR_420;
                        else colorSpace = CLRSPC_FAM_YCBCR_422;
                        break;
                    }
                    if ((strnieq(&ascColor[4], "ICT",3))  || (strnieq(&ascColor[4], "RCT",3)))
                    {
                        colorSpace = CLRSPC_FAM_RGB;
                        SetColorToRGB(TRUE);//Don't touch planes data.
                        SystemDebug.printf
                            ("Warn[%s]: Uncompressed colorspace can not be YBR_ICT or YBR_RCT, Trying RGB\n", who);
                        break;
                    }
                }// End of YBR_
            // Add more colors here.
                if(colorSpace == CLRSPC_FAM_GRAYSCALE)
                {
                    colorSpace = CLRSPC_FAM_RGB;
                    SetColorToRGB(TRUE);//Don't touch planes data.
                    SystemDebug.printf("***[%s]: Unknown or unsuported color space %s, Trying RGB\n", who, ascColor);
                }
            // Only RGB can be in planes or R,G,B format, check.
                if(colorSpace == CLRSPC_FAM_RGB) planes = pddo->GetBYTE(0x0028, 0x0006);
                if (DebugLevel > 2 && planes != 0) SystemDebug.printf("Planes = %d\n");
                break;
            default:
                OperatorConsole.printf("***[%s]: Unsuported number of components: %d.\n",
                                       who, components );
                return(FALSE);
        }
    }
// Output if needed.
    if (DebugLevel > 1) PrintInfo();
// Get the data.
    image7fe0_10 = pddo->GetVR(0x7fe0, 0x0010); // Get the Image VR.
    if(!image7fe0_10)
    {
        OperatorConsole.printf("***[%s]: No image VR\n", who);
        return (FALSE);
    }
    imageVR = image7fe0_10;//imageVR can change for encapsulated data.
//    if(!fixImageLength()) return GetEncapsulatedDDO(notJpeg);//If vr length invalid, look for encapsulated data
    return TRUE;
}

BOOL ImageData   ::   MakeRGB(BOOL planesAllowed)
{
//Check for bad call.
    if (components < 3 || imageVR == NULL ||
        imageVR->Data == NULL || imageVR->Length == 0) return FALSE;
    if (MakeYBR_FULL_OrRGB(planesAllowed) == FALSE) return FALSE;
    if (colorSpace == CLRSPC_FAM_RGB) return TRUE;
    //YBR FULL
    DeYBRFULL((char *)imageVR->Data, imageVR->Length);
    SetColorToRGB(FALSE);
    return TRUE;
}

BOOL ImageData   ::   MakeStdGray()
{
    UINT32 newLength;
    
    if (components > 1 || imageVR == NULL ||
        imageVR->Data == NULL || imageVR->Length == 0) return FALSE;
    if (precision_allocated == 8 || precision_allocated == 16 )return TRUE;
    if (precision_allocated < 8)
    {
        if ((newLength = To8BitGray((unsigned char **)&(imageVR->Data), imageVR->Length, precision_allocated , TRUE)) == 0)
        {
            OperatorConsole.printf("***[%s]: Can not change data to 8 bits wide.\n", who);
            return FALSE;
        }
        imageVR->Length = newLength;
	inPtr = (char *)imageVR->Data; // 20190601
	inEndPtr = (char *)imageVR->Data+newLength; // 20190601

        precision_allocated = 8;
        if (pddo->ChangeVR( 0x0028, 0x0100, (UINT8)8, 'US') > 0) return TRUE;
        OperatorConsole.printf("***[%s]: Can not change VR 0x0028, 0x0100 to 8 bits wide.\n", who);
        return FALSE;
    }
//8 < precision_allocated < 16
    if ((newLength = To16BitGray((unsigned char **)&(imageVR->Data), imageVR->Length, precision_allocated , TRUE)) == 0)
        {
            OperatorConsole.printf("***[%s]: Can not change data to 16 bits wide.\n", who);
            return FALSE;
        }
    imageVR->Length = newLength;
    inPtr = (char *)imageVR->Data; // 20190601
    inEndPtr = (char *)imageVR->Data+newLength; // 20190601
    precision_allocated = 16;
    if (pddo->ChangeVR( 0x0028, 0x0100, (UINT8)16, 'US') == 0)
    {
        OperatorConsole.printf("***[%s]: Can not change VR 0x0028, 0x0100 to 16 bits wide.\n", who);
        return FALSE;
    }
    return TRUE;
}

BOOL ImageData   ::   MakeYBR_FULL()
{
    //Check for bad call.
    if (components < 3 || imageVR == NULL ||
        imageVR->Data == NULL || imageVR->Length == 0) return FALSE;
    if (MakeYBR_FULL_OrRGB(FALSE) == FALSE) return FALSE;
    if (colorSpace == CLRSPC_FAM_YBR_FULL) return TRUE;
    //RGB
    RGB_ToYBRFULL((char *)imageVR->Data, imageVR->Length);
    SetColorTo("YBR_FULL)", FALSE);
    return TRUE;
}

BOOL ImageData   ::   MakeYBR_FULL_OrRGB(BOOL planesAllowed)
{
    UINT32  length;

//Check for bad call.
    if (components < 3 || imageVR == NULL || imageVR->Data == NULL ||
        imageVR->Length == 0 || ascColor == NULL) return FALSE;
    switch (colorSpace)
    {
        case CLRSPC_FAM_RGB:
        // Planar configuration
            if (planesAllowed  || planes == 0) return TRUE;
            return DePlaneImage();
        case CLRSPC_FAM_YBR_FULL:
            return TRUE;
        case CLRSPC_FAM_YCBCR_420:
            if ((length = YBR_PARTIAL_420_to_FULL((unsigned char **)&(imageVR->Data), imageVR->Length, width, TRUE)) == 0)
            {
                OperatorConsole.printf(
                    "***[%s]: Could not allocate YBR_PARTIAL_420 to YBR_FULL conversion memory.\n", who);
                return FALSE;
            }
            break;
        case CLRSPC_FAM_YCBCR_422:
            YBR_PARTIAL_422_to_FULL_422((unsigned char*)inBuffer, frameSize);
        case CLRSPC_FAM_YCBCR_F422://Partial fall through.
            if ((length = YBR_FULL_422_to_FULL((unsigned char **)&(imageVR->Data), imageVR->Length, TRUE)) == 0)
            {
                OperatorConsole.printf("***[%s]: Could not allocate YBR_FULL_422 to YBR_FULL conversion memory.\n", who);
                return FALSE;
            }
            break;
        default:
            SystemDebug.printf("***[%s]: Unknown or unsuported color space %s.\n", who, ascColor);
            return FALSE;
    }
    imageVR->Length = length;
    SetColorTo("YBR_FULL", FALSE);
    colorSpace = CLRSPC_FAM_YBR_FULL;
    return TRUE;
}

#ifdef NOVARAD_FIX
BOOL ImageData   ::   FixJpegEnd()
// Put the end of file back if not present.
// Even if a decompression fails, NovaRad fix is done.
{
    void            *fixPtr;
    unsigned char   *fix1Ptr, *fix2Ptr;
    UINT32          cnt;

//Check for bad call.
    if(!imageVR) return (FALSE);
    if(!(fixPtr = malloc(imageVR->Length + 2)))
    {
        OperatorConsole.printf("***[%s]: Can not create memory fix buffer.\n", who);
        return (FALSE);
    }
    fix1Ptr = (unsigned char*)fixPtr;
    fix2Ptr = (unsigned char*)imageVR->Data;
    for( cnt = imageVRLength; cnt > 0; --cnt ) *fix1Ptr++ = *fix2Ptr++;
    *fix1Ptr++ = 0xFF;
    *fix1Ptr++ = 0xD9;
    if(imageVRLength & 1 ) *fix1Ptr = 0;
    free(imageVR->Data);
    imageVR->Data = fixPtr;
    imageVRLength += 2;
    imageVR->Length += 2;
    inPtr = (char*)imageVR->Data;
    inEndPtr = inPtr + imageVR->Length;
    if(DebugLevel > 0)SystemDebug.printf("Warn[%s]: Novarad fix, EOC marker added.\n", who);
    return TRUE;
}
#endif

void ImageData   ::   PrintInfo()
{
    SystemDebug.printf("Debug[%s]: H = %d, W = %d, Bits = %d in %d, Components = %d, Frames = %d\n",
                           who, height, width, precision, precision_allocated, components, frames);
    if(components > 1)
    {
        if(ascColor) SystemDebug.printf("Color = %s\n", ascColor);
        else SystemDebug.printf("Unknown color space record.\n");
    }
}

BOOL ImageData   ::   SetColorTo(const char *name, BOOL planesAllowed)
{
    UINT32  length;
    
    if (name == NULL || (length = strnlen32u(name, ASC_COLOR_MAX_LENGTH)) == 0) return FALSE;
    if (!planesAllowed) SetPlanesToNone();
    ascColorLength = length;
    strcpylim(ascColor, name, ASC_COLOR_MAX_LENGTH);
    if (pddo->ChangeVR( 0x0028, 0x0004, ascColor, 'CS') > 0) return TRUE;
    OperatorConsole.printf("***[%s]: Can not change VR 0x0028, 0x0004 to %s\n", who, ascColor);
    return (FALSE);
}

BOOL ImageData   ::   SetColorToRGB(BOOL planesAllowed)
{
    if (components != 3)return FALSE;
    return SetColorTo("RGB", planesAllowed) == FALSE;
}

void ImageData   ::   SetPlanesToNone()
{
    VR *pvr;
    
    pvr = pddo->GetVR(0x0028, 0x0006); // Fix the planes VR if there.
    if(pvr && pvr->Length && *(char *)(pvr->Data) == 1)
    {
        *(UINT16 *)pvr->Data = 0;
    }
    planes = 0;
    return;
}

BOOL ImageData   ::   SwitchToOutputArray()
{
    //Check for bad call.
    if(!image7fe0_10 || !outArrayPtr) return (FALSE);
    // Change the image vr to the smaller encapsulated image.
    image7fe0_10->Reset();// Delete the old data including the array.
    image7fe0_10->SQObjectArray = outArrayPtr;
    outArrayPtr = NULL;//Stop ImageData from freeing the freed array.
    // Set the image type.
    if(byteWidth > 1)image7fe0_10->TypeCode ='OW';
    else image7fe0_10->TypeCode ='OB';
    return (TRUE);
}


BOOL ImageData   ::   SwitchToOutputBuffer()
{
//Check for bad call.
    if(!image7fe0_10 || !outBuffer) return (FALSE);
// Change the image vr to the bigger uncompressed and unencapsulated image.
    image7fe0_10->Reset();// Delete the old data including the array.
    image7fe0_10->Length = outputImageLengthEven;
    image7fe0_10->Data = (void *)outBuffer;// The new uncompressed data.
    image7fe0_10->ReleaseMemory = TRUE;//Give the memory to the vr.
    outBuffer = NULL;//Stop ImageData from freeing the freed memory.
// Set the image type.
    if(byteWidth == 1)// 8 bits
    {
        image7fe0_10->TypeCode ='OB';
    }
    else
    {
        image7fe0_10->TypeCode ='OW';
    }
    return (TRUE);
}
