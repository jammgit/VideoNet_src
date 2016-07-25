#include "stdafx.h"
#include "H264Encoder.h"
#include <fcntl.h>
#include <stdio.h>
#include <io.h>

#define IMAGE_WIDTH 176
#define IMAGE_HEIGHT 144

CH264Encoder::CH264Encoder(void)
{

}

CH264Encoder::~CH264Encoder(void)
{
}

int CH264Encoder::init()
{
	encContext = NULL;

	avcodec_register_all();

	// Initialize the compressor
	/* find the H263 video encoder */
	encoder = avcodec_find_encoder(CODEC_ID_H264);
	if (!encoder) {
		return 0;  //can't find encoder
	}

	encContext = avcodec_alloc_context3(encoder);
	/* put sample parameters */

	/* put sample parameters */
	encContext->bit_rate = 400000;
	/* resolution must be a multiple of two */
	encContext->width = IMAGE_WIDTH;
	encContext->height = IMAGE_HEIGHT;
	/* frames per second */
	AVRational AVRationalTemp;
	AVRationalTemp.den = 25;
	AVRationalTemp.num = 1;

	encContext->time_base= AVRationalTemp;
	encContext->gop_size = 10; /* emit one intra frame every ten frames */
//	encContext->max_b_frames=1;
	encContext->pix_fmt = AV_PIX_FMT_YUV420P;

	av_opt_set(encContext->priv_data, "preset", "slow", 0);

	/* open it */
	if (avcodec_open2(encContext, encoder,NULL) < 0) {
		//avcodec_close(encContext);
		//av_free(encContext);
		encContext = NULL;
		return 0;
	}

	if((fd_write=open("f:/h264.h264",O_WRONLY | O_TRUNC | O_CREAT,0777))<0){  
		exit(EXIT_FAILURE);  
	} 

	//Encode(NULL);

	FILE * fd_read = fopen("f:/akiyo_qcif.yuv","rb");

	fseek(fd_read,0,SEEK_SET);

	char buffe[IMAGE_WIDTH*IMAGE_HEIGHT*3/2];
	char buffetemp[IMAGE_WIDTH*IMAGE_HEIGHT*3/2];
	int byte = 0;
	while(byte < IMAGE_WIDTH*IMAGE_HEIGHT*3/2)
	{

		int ret = fread(buffetemp,1,IMAGE_WIDTH*IMAGE_HEIGHT*3/2,fd_read);
		if(ret <=0) break;

		memcpy(&buffe[byte],buffetemp,ret);
		byte = byte + ret;
		if(byte >= IMAGE_WIDTH*IMAGE_HEIGHT*3/2)
		{
			Encode(buffe);
			byte = 0;
		}
	}
	fclose(fd_read);
}

int CH264Encoder::Encode(char* data)
{
	AVFrame* frame = avcodec_alloc_frame();
    if (!frame) {
        exit(1);
    }
    frame->format = encContext->pix_fmt;
    frame->width  = encContext->width;
    frame->height = encContext->height;

    /* the image can be allocated by any means and av_image_alloc() is
     * just the most convenient way if av_malloc() is to be used */
   int ret = av_image_alloc(frame->data, frame->linesize, encContext->width, encContext->height,
                         encContext->pix_fmt, 32);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate raw picture buffer\n");
        exit(1);
    }

	memcpy(frame->data[0],data,encContext->width*encContext->height);
	memcpy(frame->data[1],data + encContext->width*encContext->height,encContext->width*encContext->height/4);
	memcpy(frame->data[2],data + encContext->width*encContext->height*5/4,encContext->width*encContext->height/4);

	
	//int out_size = avcodec_encode_video(encContext, cdata, 32*1024, picture);

	AVPacket pkt;
	av_init_packet(&pkt);
	pkt.data = NULL;    // packet data will be allocated by the encoder
	pkt.size = 0;

	frame->pts++;

	int got_output;
	av_init_packet(&pkt);
	pkt.data = NULL;    // packet data will be allocated by the encoder
	pkt.size = 0;
	ret = avcodec_encode_video2(encContext, &pkt, frame, &got_output);

	static int kind = 0;
	
	if(got_output>0)
	{
		kind ++;
		write(fd_write,pkt.data,pkt.size); 
		av_free_packet(&pkt);
	}

	av_freep(&frame->data[0]);
	avcodec_free_frame(&frame);
}

int CH264Encoder::close()
{
	uint8_t endcode[] = { 0, 0, 1, 0xb7 };
	write(fd_write, endcode, 4);
	::close(fd_write);
	if(encContext != NULL)
	{
		avcodec_close(encContext);
		av_free(encContext);
	}
	return 0;
}
