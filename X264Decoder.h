#pragma once

extern "C"{
#include <libavcodec/avcodec.h>  
#include <libavformat/avformat.h>  
#include <libavutil/mathematics.h>  
}

class CX264Decoder
{
public:
	CX264Decoder(void);
public:
	~CX264Decoder(void);
public:
	bool init();
	bool decode(char* buf,int size,char* outbuf);
	bool close();

private:
	int  ret,videoStream;  
	AVFormatContext * formatContext;  
	AVCodec * codec;  
	AVCodecContext * codecContext; 
	AVCodecContext * decContext;
	AVFrame * decodedFrame;  
	AVPacket packet;  
	uint8_t *decodedBuffer;  
	unsigned int decodedBufferSize;  
	int finishedFrame; 
};
