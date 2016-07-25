////////////////////////////////////////////////////////////////////////////
//
//
//    Project     : VideoNet version 1.1.
//    Description : Peer to Peer Video Conferencing over the LAN.
//	  Author      :	Nagareshwar Y Talekar ( nsry2002@yahoo.co.in)
//    Date        : 15-6-2004.
//
//
//    File description : 
//    Name    : convert.h
//    Details : Conversion routine from RGB24 to YUV420 & YUV420 to RGB24.
//
/////////////////////////////////////////////////////////////////////////////

#if !defined _CONVERT_H
#define _CONVERT_H

#include<stdio.h>

// Conversion from RGB24 to YUV420
void InitLookupTable();
int  ConvertRGB2YUV(int w,int h,unsigned char *rgbdata,unsigned int *yuv);


// Conversion from YUV420 to RGB24
void InitConvertTable();
void ConvertYUV2RGB(unsigned char *src0,unsigned char *src1,unsigned char *src2,unsigned char *dst_ori,int width,int height);

void Tran565to24(unsigned char* src, int w, int h, unsigned char* dst);
void Tran555to24(unsigned char* src, int w, int h, unsigned char* dst);

void YUY2_RGB(unsigned char *YUY2buff,unsigned char *RGBbuff,int dwSize);
void YUY2_RGB2(unsigned char *YUY2buff,unsigned char *RGBbuff,int dwSize);
void YUY2_RGB3(unsigned char *YUY2buff,unsigned char *RGBbuff,int dwSize);
void YUY2_RGB4(unsigned char *YUY2buff,unsigned char *RGBbuff,int dwSize);

/** 翻转图象
@param  BYTE * pDest:输出数据buffer
@param  DWORD dwDestSize: 输出数据buffer大小
@param  BYTE * pSrc:输入数据buffer
@param  DWORD dwSrcSize:输入数据buffer大小
@param  const int nHeight: Bmp图像高度
@param  const int nLineByteWidth:每一行的占字节数
@return  
*/	
void FlipDIBImage(unsigned char * pDest,const int dwDestSize,unsigned char * pSrc,const int dwSrcSize,const int nHeight,const int nLineByteWidth);

//////////////////////////////////////////////////////////////////////////
// YUV2RGB
// pYUV			point to the YUV data
// pRGB			point to the RGB data
// width		width of the picture
// height		height of the picture
// alphaYUV		is there an alpha channel in YUV
// alphaRGB		is there an alpha channel in RGB
//////////////////////////////////////////////////////////////////////////
int YUV2RGB(void* pYUV, void* pRGB, int width, int height, bool alphaYUV, bool alphaRGB);

//////////////////////////////////////////////////////////////////////////
// RGB2YUV
// pRGB			point to the RGB data
// pYUV			point to the YUV data
// width		width of the picture
// height		height of the picture
// alphaYUV		is there an alpha channel in YUV
// alphaRGB		is there an alpha channel in RGB
//////////////////////////////////////////////////////////////////////////
int RGB2YUV(void* pRGB, void* pYUV, int width, int height, bool alphaYUV, bool alphaRGB);

//////////////////////////////////////////////////////////////////////////
// pGBYUV			point to the background YUV data
// pFGYUV			point to the foreground YUV data
// width			width of the picture
// height			height of the picture
// alphaBG			is there an alpha channel in background YUV data
// alphaFG			is there an alpha channel in fourground YUV data
//////////////////////////////////////////////////////////////////////////
int YUVBlending(void* pBGYUV, void* pFGYUV, int width, int height, bool alphaBG, bool alphaFG);


#endif