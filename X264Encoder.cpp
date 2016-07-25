#include "stdafx.h"
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <io.h>
#include "X264Encoder.h"


#define CLEAR(x) (memset((&x),0,sizeof(x)))  
#define IMAGE_WIDTH   320  
#define IMAGE_HEIGHT  240  
#define ENCODER_PRESET "veryfast"  
#define ENCODER_TUNE   "zerolatency"  
#define ENCODER_PROFILE  "baseline"  
#define ENCODER_COLORSPACE X264_CSP_I420 //X264_CSP_I420  

CX264Encoder::CX264Encoder(void)
{
}

CX264Encoder::~CX264Encoder(void)
{
}

bool CX264Encoder::Init()
{
	strcpy(parameter_preset,ENCODER_PRESET);  
	strcpy(parameter_tune,ENCODER_TUNE);  

	x264_parameter=(x264_param_t *)malloc(sizeof(x264_param_t));  
	if(!x264_parameter){  
		printf("malloc x264_parameter error!\n");  
		exit(EXIT_FAILURE);  
	}  
	CLEAR(*(x264_parameter));  
	x264_param_default(x264_parameter);  

	int ret;
	if((ret=x264_param_default_preset(x264_parameter,parameter_preset,parameter_tune))<0){  
		printf("x264_parameter_default_preset error!\n");  
		exit(EXIT_FAILURE);  
	}  

//	//可疑参数
//	// x264_parameter->i_bframe_adaptive = X264_B_ADAPT_TRELLIS;  
//
//	x264_parameter->i_fps_den         =1;  //* 帧率分母 
//	x264_parameter->i_fps_num         =25;  //* 帧率分子
//	x264_parameter->i_width       =IMAGE_WIDTH;//* 要编码的图像宽度.
//	x264_parameter->i_height      =IMAGE_HEIGHT;  //* 要编码的图像高度 
////	x264_parameter->i_threads         =1;  
//	x264_parameter->b_intra_refresh =1;  
//	x264_parameter->b_annexb      =1;  
//
//     //* 流参数  
//	 x264_parameter->i_keyint_max = 250;
//	 x264_parameter->i_keyint_min = 25;
//
//     //* 速率控制参数  
//     x264_parameter->rc.i_bitrate = 40000;//* 码率(比特率,单位Kbps)  
     //* muxing parameters  

 //    x264_parameter->i_timebase_den = x264_parameter->i_fps_num;  
 //    x264_parameter->i_timebase_num = x264_parameter->i_fps_den;  

	 x264_parameter->i_fps_den         =1;  
	 x264_parameter->i_fps_num         =25;  
	 x264_parameter->i_width       =IMAGE_WIDTH;  
	 x264_parameter->i_height      =IMAGE_HEIGHT;  
	 x264_parameter->i_threads         =1;  
	 x264_parameter->i_keyint_max    =25;  
	 x264_parameter->b_intra_refresh =1;  
	 x264_parameter->b_annexb      =1; 
	 x264_parameter->rc.i_bitrate = 800;

	 /*use default setting*/
	 x264_parameter->i_fps_den=1;
	 x264_parameter->i_fps_num=10;  /*--keyint 100*/
	 x264_parameter->b_vfr_input=0; /*--force-cfr*//*fimxe,这个功能比较有意思*/

	 x264_parameter->i_keyint_max=100; /*--keyint 100*/
	 x264_parameter->i_bframe=0; /*--bframes 0*/
	 x264_parameter->i_frame_reference=3; /*--ref 3*/
	 /*slow-firstpass: b_turbo=0*/

	 x264_parameter->rc.i_rc_method=X264_RC_CQP;
	 x264_parameter->rc.i_qp_constant=30;
	 
	 //x264_parameter->rc.i_vbv_max_bitrate=200; /*--vbv-maxrate 200*/
	 //x264_parameter->rc.i_vbv_buffer_size=100; /*--vbv-bufsize 2000*/

	 x264_parameter->vui.b_fullrange = 1;

	strcpy(parameter_profile,ENCODER_PROFILE);
	
	if((ret=x264_param_apply_profile(x264_parameter,parameter_profile))<0){  
		printf("x264_parameter_apply_profile error!\n");  
		exit(EXIT_FAILURE);  
	}  

	x264_encoder=x264_encoder_open(x264_parameter);  

	colorspace=ENCODER_COLORSPACE; 

	yuv420p_picture=(x264_picture_t *)malloc(sizeof(x264_picture_t ));  
	if(!yuv420p_picture){  
		exit(EXIT_FAILURE);  
	}  
	if((ret=x264_picture_alloc(yuv420p_picture,colorspace,IMAGE_WIDTH,IMAGE_HEIGHT))<0){  
		exit(EXIT_FAILURE);  
	}  

	yuv420p_picture->img.i_csp=colorspace;  
	yuv420p_picture->img.i_plane=3;  
	yuv420p_picture->i_type=X264_TYPE_AUTO;  
	yuv420p_picture->i_qpplus1 = 0;

	yuv=(uint8_t *)malloc(IMAGE_WIDTH*IMAGE_HEIGHT*3/2);  
	if(!yuv){  
		exit(EXIT_FAILURE);  
	}  
	CLEAR(*(yuv));  

	yuv420p_picture->img.plane[0]=yuv;
	yuv420p_picture->img.plane[1]=yuv+IMAGE_WIDTH*IMAGE_HEIGHT;
	yuv420p_picture->img.plane[2]=yuv+IMAGE_WIDTH*IMAGE_HEIGHT*5/4;   
	//yuv420p_picture->img.plane[1]=yuv+IMAGE_WIDTH*IMAGE_HEIGHT;  
	//yuv420p_picture->img.plane[2]=yuv+IMAGE_WIDTH*IMAGE_HEIGHT+IMAGE_WIDTH*IMAGE_HEIGHT/4; 


	nal=(x264_nal_t *)malloc(sizeof(x264_nal_t ));  
	if(!nal){  
		exit(EXIT_FAILURE);  
	}

	/*FILE * fd_read = fopen("f:/akiyo_qcif.yuv","rb");

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
	this->close();*/
}

int CX264Encoder::Encode(char* buf,char* outbuf,unsigned long& size)
{
	CLEAR(*(nal)); 
	memcpy(yuv,buf,IMAGE_WIDTH*IMAGE_HEIGHT*3/2);

	static int m_iFrameNum = 0;
	yuv420p_picture->i_pts++;

	int ret;
	int n_nal;
	x264_picture_t pic_out; 
	if((ret=x264_encoder_encode(x264_encoder,&nal,&n_nal,yuv420p_picture,&pic_out))<=0){  
		printf("x264_encoder_encode error!\n");  
		exit(EXIT_FAILURE);  
	} 

	size = 0;
	x264_nal_t *my_nal;
	unsigned int length=0;  
	for(my_nal=nal;my_nal<nal+n_nal;++my_nal){
		memcpy(outbuf + size,my_nal->p_payload,my_nal->i_payload);
		size += my_nal->i_payload;
	}  

	

	/*clean_up functions*/  
	//x264_picture_clean(encoder->yuv420p_picture);  
	//free(encoder->nal);//???? confused conflict with x264_encoder_close(encoder->x264_encoder);  

	return 0;  
}

void CX264Encoder::close()
{
	free(yuv);  
	free(yuv420p_picture);  
	free(x264_parameter);  
	x264_encoder_close(x264_encoder);
}
