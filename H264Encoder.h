#pragma once
extern "C"{
	#include <libavutil/opt.h>
#include "libavcodec/avcodec.h"
#include "libavutil/frame.h"
#include "libavutil/imgutils.h"
}

class CH264Encoder
{
public:
	CH264Encoder(void);
public:
	~CH264Encoder(void);
public:
	int init();
	int Encode(char* data);
	int close();
private:
	AVCodecContext *encContext;
	AVCodec *encoder;
	int fd_write;
};
