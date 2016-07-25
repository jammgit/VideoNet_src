#pragma once

class CYuv_RGB
{
public:
	CYuv_RGB(void);
public:
	~CYuv_RGB(void);
public:
	static bool getYUV420DataFromRGBData(unsigned char* yuvbuf,unsigned char* rgbbuf,unsigned int width,unsigned int height);
	static bool getRgb24DataFromYUV420Data(unsigned char* rgbbuf,unsigned char* yuvbuf ,unsigned int width,unsigned int height);
	static bool RGB2YUV(unsigned char* RgbBuf,unsigned int nWidth,unsigned int nHeight,unsigned char* yuvBuf,unsigned long *len); 
	static bool YUV2RGB(unsigned char* yuvBuf,unsigned int nWidth,unsigned int nHeight,unsigned char* pRgbBuf,unsigned long *len); 
	static bool OpenYuvFile();
};
