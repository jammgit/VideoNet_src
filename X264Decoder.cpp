#include "stdafx.h"
#include "X264Decoder.h"
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <io.h>

#define DECODED_OUTPUT_FORMAT  AV_PIX_FMT_YUV420P  
#define INPUT_FILE_NAME "f:/test.h264"  
#define OUTPUT_FILE_NAME "f:/x264out.yuv"  
#define IMAGE_WIDTH 320
#define IMAGE_HEIGHT 240


CX264Decoder::CX264Decoder(void)
{
}

CX264Decoder::~CX264Decoder(void)
{
}

bool CX264Decoder::init()
{
	av_register_all(); 

	codec=avcodec_find_decoder(AV_CODEC_ID_H264); 

	decContext = avcodec_alloc_context3(codec);

	if(codec->capabilities&CODEC_CAP_TRUNCATED)
		decContext->flags|= CODEC_FLAG_TRUNCATED; /* we dont send complete frames */

    decContext->lowres = 0;
    decContext->idct_algo = FF_IDCT_AUTO;
    decContext->flags2 |= CODEC_FLAG2_FAST;
    decContext->skip_frame = AVDISCARD_DEFAULT;
    decContext->skip_idct = AVDISCARD_DEFAULT;
    decContext->skip_loop_filter = AVDISCARD_DEFAULT;
//    decContext->error_resilience = FF_ER_CAREFUL;
    decContext->error_concealment = 3;
	/* for some codecs, such as msmpeg4 and mpeg4, width and height
	   MUST be initialized there because these info are not available
	   in the bitstream */

	decContext->width = IMAGE_WIDTH;
	decContext->height = IMAGE_HEIGHT;

	if (avcodec_open2(decContext, codec,NULL) < 0)
	{
		return false;
	}
}

bool CX264Decoder::decode(char* buf,int size,char* outbuf)
{
	AVPacket avp;
	av_init_packet(&avp);
	avp.data = (uint8_t *)buf;
	avp.size = size;

	int got_picture, len;
	AVFrame *remotepic;
	byte *inbuf_ptr;
	inbuf_ptr = (byte*)buf;
	remotepic = avcodec_alloc_frame();
	len = avcodec_decode_video2(decContext, remotepic, &got_picture, &avp);

	if (len<=0)
	{
		return false;
	}

	if(got_picture)
	{
		int nbyte = 0;
		for (int i=0;i<3;i++)
		{
			int width = IMAGE_WIDTH;
			int height = IMAGE_HEIGHT;
			if(i!=0) {
				height /= 2;
				width /= 2;
			}
			for (int j=0;j<height;j++)
			{
				memcpy(outbuf + nbyte, remotepic->data[i]+j*remotepic->linesize[i], width);
				nbyte = nbyte + width;
			}
		}
	}
	

	av_free(remotepic);
	return 0;
}

bool CX264Decoder::close()
{
	if(decContext != NULL)
	{
		avcodec_close(decContext);
		av_free(decContext);
	}
	return 0;
}
