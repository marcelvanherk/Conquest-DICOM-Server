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
 bcb 20140513: Created this file.
 mvh 20190602: Fix To8BitGray and To16BitGray, shifts were reversed
 */

#include "uncompconv.hpp"
#ifdef _WIN32
#include <stdlib.h>
#endif

UINT32 To8BitGray(unsigned char** dataH,  UINT32 length, UINT16 dataWidth, BOOL freeBuf)
{
    unsigned char *inPtr, *outPtr, *outData, *endPtr;
    unsigned int  mask, shiftCnt, data;
    UINT32  newLength;
    
    if(dataH == NULL || *dataH == NULL || dataWidth == 0 || dataWidth > 7) return 0;
    newLength = length * 8 / dataWidth;
    if(newLength & 1)newLength++;//Make even.
    if((outData = (unsigned char*)malloc(newLength)) == NULL) return 0;
    outPtr = outData;
    inPtr = *dataH;
    endPtr = inPtr + length;
    mask = (1 << dataWidth) - 1; //20190601, made >> into <<
    shiftCnt = 0;
    while (inPtr < endPtr)
    {
        data = *inPtr++;
        data += *inPtr * 0x100;
    //If dataWidth <= 4 will loop, else twice.
        while (shiftCnt < 8)//Never shift more than 8 places.
        {
            *outPtr++ = ((data & mask) >> shiftCnt) & 0xFF;//Get and align the data. //20190601, made << into >>
            mask = mask << dataWidth;//Move the mask up to the mext group. //20190601, made >> into <<
            shiftCnt += dataWidth;//Further to shift.
        }
    //Realign for the next 8 bits of data.
        shiftCnt -= 8;
        mask = mask >> 8; //20190601, made << into >>
    }
    if(freeBuf) free(*dataH);
    *dataH = outData;
    return newLength;
}

UINT32 To16BitGray(unsigned char** dataH,  UINT32 length, UINT16 dataWidth, BOOL freeBuf)
{
    UINT16 *inPtr, *outPtr, *outData, *endPtr;
    unsigned int  mask, shiftCnt, data;
    UINT32  newLength;
    
    if(dataH == NULL || *dataH == NULL || dataWidth < 9 || dataWidth > 15) return 0;
    if(dataH == NULL || *dataH == NULL || dataWidth == 0 || dataWidth > 7) return 0;
    newLength = length * 16 / dataWidth;
    if(newLength & 1)newLength++;//Make even.
    if((outData = (UINT16*)malloc(newLength)) == NULL) return 0;
    outPtr = outData;
    inPtr = (UINT16*)*dataH;
    endPtr = inPtr + length/2;
    mask = (1 << dataWidth) - 1; //20190601, made >> into <<
    shiftCnt = 0;
    while (inPtr < endPtr)
    {
        data = *inPtr++;
        data += *inPtr++ * 0x10000;
        while (shiftCnt < 16)//Never shift more than 16 places.
        {
            *outPtr++ = ((data & mask) >> shiftCnt) & 0xFFFF;//Get and align the data. //20190601, made << into >>
            mask = mask << dataWidth;//Move the mask up to the mext group. //20190601, made >> into <<
            shiftCnt += dataWidth;//Further to shift.
        }
        //Realign for the next 8 bits of data.
        shiftCnt -= 16;
        mask = mask >> 16; //20190601, made << into >>
    }
    if(freeBuf) free(*dataH);
    *dataH = (unsigned char*)outData;
    return newLength;
}

void SwapEndian(unsigned char* dataPtr, UINT32 length)
{
    unsigned char temp;
    
    length /= 2;
    while (length-- > 0)
    {
        temp = *dataPtr;
        *dataPtr = dataPtr[1];
        dataPtr++;
        *dataPtr++ = temp;
    }
}

//There are 2 y values for every Cb and Cr.
//Can do one frame or all frames.
UINT32 YBR_FULL_422_to_FULL(unsigned char** dataH, UINT32 length, BOOL freeBuf)
{
    unsigned char *inPtr, *outPtr, *outData, *endPtr, y2;
    UINT32 newLength;
    
    if(dataH == NULL || *dataH == NULL || length == 0) return 0;
    newLength = length * 3 / 2;
    if(newLength & 1)newLength++;//Make even.
    if((outData = (unsigned char*)malloc(newLength)) == NULL) return 0;
    outPtr = outData;
    inPtr = *dataH;
    endPtr = inPtr + length;
    length /= 4; //Each Cx values.
    while (inPtr < endPtr)
    {
        //First Y.
        *outPtr++ = *inPtr++;//y1
        y2 = *inPtr++;//y2
        //Cx
        *outPtr++ = *inPtr++;
        *outPtr++ = *inPtr--;
        //Second Y
        *outPtr++ = y2;
        //Cx Again
        *outPtr++ = *inPtr++;
        *outPtr++ = *inPtr++;
    }
    if(freeBuf) free(*dataH);
    *dataH = outData;
    return newLength;
}

unsigned char FULL_from_PARTIAL_Y(unsigned char data)
{
    int tmp = 1.1636 * (data - 16);
    if (tmp > 255) tmp = 255;
    if (tmp < 0) tmp = 0;
    return (tmp & 0xFF);
}

unsigned char FULL_from_PARTIAL_Cx(unsigned char data)
{//Cx, 128 is no color in full and partial
    int tmp = (1.133 * (data - 128)) + 128;
    if (tmp > 255) tmp = 255;
    if (tmp < 0) tmp = 0;
    return (tmp & 0xFF);
}

//There are 2 y values for every Cb and Cr or
// Total length = 1/2Y + 1/4Cb + 1/4Cr
//Y= + .2568R + .5041G + .0979B + 16
//CB = - .1482R - .2910G + .4392B + 128
//CR = + .4392R - .3678G - .0714B + 128
void YBR_PARTIAL_422_to_FULL_422(unsigned char* data, int length)
{
    if(data == NULL) return;
    length /= 4; //Two Ys, a Cr and a Cb.
    for ( ;length > 0; length-- )
    {
        //First Y.
        *data = FULL_from_PARTIAL_Y(*data);
        data++;
        //Second Y.
        *data = FULL_from_PARTIAL_Y(*data);
        data++;
        //Cr
        *data = FULL_from_PARTIAL_Cx(*data);
        data++;
        //Cb
        *data = FULL_from_PARTIAL_Cx(*data);
        data++;
    }
    return;
}

// What a strange standard. See page 323 of 08_03pu.pdf.
// line 1 = Y1CrCbY2Y3CrCbY4...
// Line 2 = Y1Y2Y3Y4...
// For each CrCb there are 4 Ys.
UINT32 YBR_PARTIAL_420_to_FULL(unsigned char** dataH, UINT32 length, UINT32 width, BOOL freeBuf)
{
    unsigned char *inPtr, *outPtr, *cxPtr, *outData, *endPtr, *cxrow;
    UINT32 newLength, cnt;
    
    if (dataH == NULL || *dataH == NULL || length == 0 || width == 0) return 0;
    if ((cxrow = (unsigned char *)malloc(width * 2)) == NULL) return 0;//Just save Cxs.
    // Need to add 3 CrCb pairs to the 3 other Ys
    newLength = length * 2;
    if (newLength & 1) newLength++;
    if ((outData = (unsigned char *)malloc(newLength)) == NULL)
    {
        free(cxrow);
        return 0;
    }
    inPtr = *dataH;
    outPtr = outData;
    endPtr = inPtr + length;
    while (inPtr < endPtr)
    {
        cxPtr = cxrow;
        for (cnt = width / 2; cnt > 0; cnt--)//2 pixels each loop.
        {//Rows with CrCbs, two Ys at a time.
            *outPtr++ = FULL_from_PARTIAL_Y(*inPtr++); //Save the first Y.
            *cxPtr = FULL_from_PARTIAL_Cx(*inPtr++); //Save Cr in buffer.
            *outPtr++ = *cxPtr++; //Save Cr.
            *cxPtr = FULL_from_PARTIAL_Cx(*inPtr++); //Save Cb in buffer.
            *outPtr++ = *cxPtr--; //Save Cb, back to Cr.
            *outPtr++ = FULL_from_PARTIAL_Y(*inPtr++); //Save the next Y.
            *outPtr++ = *cxPtr++; //Save Cr again.
            *outPtr++ = *cxPtr++; //Save Cb again.
        }
        cxPtr = cxrow;
        for (cnt = width / 2; cnt > 0; cnt--)
        {
            *outPtr++ = FULL_from_PARTIAL_Y(*inPtr++); //Save the third Y.
            *outPtr++ = *cxPtr++; //Save Cr a third time.
            *outPtr++ = *cxPtr--; //Save Cb a third time, back to Cr.
            *outPtr++ = FULL_from_PARTIAL_Y(*inPtr++); //Save the forth Y.
            *outPtr++ = *cxPtr++; //Save Cr a forth time.
            *outPtr++ = *cxPtr++; //Save Cb a forth time.
        }
    }
    free(cxrow);
    if(freeBuf) free(*dataH);
    *dataH = outData;
    return length;
}

void RGB_ToYBRFULL(char* data, int length)
{
    unsigned char *nPtr;
    int y, cb, cr;
    
    length /= 3; // each color
    nPtr = (unsigned char *)data;
    for ( ;length > 0; length-- )
    {
        y  = .2990 * nPtr[0] + .5870 * nPtr[1] + .1140 * nPtr[2];
        cb = .1687 * nPtr[0] + .3313 * nPtr[1] + .5000 * nPtr[2] + 128;
        cr = .5000 * nPtr[0] + .4187 * nPtr[1] + .0813 * nPtr[2] + 128;
        if (y>255) y=255;
        if (cb>255) cb=255;
        if (cr>255) cr=255;
        if (y<0) y=0;
        if (cb<0) cb=0;
        if (cr<0) cr=0;
        *nPtr++ = y;
        *nPtr++ = cb;
        *nPtr++ = cr;
    }
}
