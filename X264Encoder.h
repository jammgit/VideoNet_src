#pragma once
#include <inttypes.h>

extern "C"{
#include "x264.h"
}

class CX264Encoder
{
public:
	CX264Encoder(void);
public:
	~CX264Encoder(void);
public:
	bool Init();
	int Encode(char* buf,char* outbuf,unsigned long& size);
	void close();

private:
	x264_param_t  * x264_parameter;  
	char parameter_preset[20];  
	char parameter_tune[20];  
	char parameter_profile[20];  
	x264_t  * x264_encoder;  
	x264_picture_t * yuv420p_picture;  
	long colorspace;  
	unsigned char *yuv;  
	x264_nal_t * nal; 
};
