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
//    Name    : convert.cpp
//    Details : Conversion routine from RGB24 to YUV420 & YUV420 to RGB24.
//
/////////////////////////////////////////////////////////////////////////////


#include "convert.h"
#include <string.h>

// Conversion from RGB to YUV420
int RGB2YUV_YR[256], RGB2YUV_YG[256], RGB2YUV_YB[256];
int RGB2YUV_UR[256], RGB2YUV_UG[256], RGB2YUV_UBVR[256];
int RGB2YUV_VG[256], RGB2YUV_VB[256];

// Conversion from YUV420 to RGB24
static long int crv_tab[256];
static long int cbu_tab[256];
static long int cgu_tab[256];
static long int cgv_tab[256];
static long int tab_76309[256];
static unsigned char clp[1024];			//for clip in CCIR601

#define RGB565_MASK_RED 0xF800
#define RGB565_MASK_GREEN 0x07E0
#define RGB565_MASK_BLUE 0x001F

//
// Table used for RGB to YUV420 conversion
//
void InitLookupTable()
{
	int i;

	for (i = 0; i < 256; i++) RGB2YUV_YR[i] = (float)65.481 * (i<<8);
	for (i = 0; i < 256; i++) RGB2YUV_YG[i] = (float)128.553 * (i<<8);
	for (i = 0; i < 256; i++) RGB2YUV_YB[i] = (float)24.966 * (i<<8);
	for (i = 0; i < 256; i++) RGB2YUV_UR[i] = (float)37.797 * (i<<8);
	for (i = 0; i < 256; i++) RGB2YUV_UG[i] = (float)74.203 * (i<<8);
	for (i = 0; i < 256; i++) RGB2YUV_VG[i] = (float)93.786 * (i<<8);
	for (i = 0; i < 256; i++) RGB2YUV_VB[i] = (float)18.214 * (i<<8);
	for (i = 0; i < 256; i++) RGB2YUV_UBVR[i] = (float)112 * (i<<8);
}


void ReverseBMP(char *data, int width, int height)
{
	int i;
	char *tmp = new char[width * 3];
	for(i = 0; i < height / 2; i++)
	{
		memcpy(tmp, &data[i * width * 3], width * 3);
		memcpy(&data[i * width * 3], &data[(height - 1 - i) * width * 3], width * 3);
		memcpy(&data[(height - 1 - i) * width * 3], tmp, width * 3);
	}
	delete[] tmp;
}

//
// Convert from  RGB24 to YUV420
//
int ConvertRGB2YUV(int w,int h,unsigned char *bmp,unsigned int *yuv)
{
	//ReverseBMP((char*)bmp,w,h);
	
	unsigned int *u,*v,*y,*uu,*vv;
	unsigned int *pu1,*pu2,*pu3,*pu4;
	unsigned int *pv1,*pv2,*pv3,*pv4;
	unsigned char *r,*g,*b;
	int i,j;

	uu=new unsigned int[w*h];
	vv=new unsigned int[w*h];

	if(uu==NULL || vv==NULL)
		return 0;

	y=yuv;
	u=uu;
	v=vv;

	// Get r,g,b pointers from bmp image data....
	r=bmp;
	g=bmp+1;
	b=bmp+2;


//Get YUV values for rgb values...

	for(i=0;i<h;i++)
	{
	
		for(j=0;j<w;j++)
		{
			*y++=( RGB2YUV_YR[*r]  +RGB2YUV_YG[*g]+RGB2YUV_YB[*b]+1048576)>>16;
			*u++=(-RGB2YUV_UR[*r]  -RGB2YUV_UG[*g]+RGB2YUV_UBVR[*b]+8388608)>>16;
			*v++=( RGB2YUV_UBVR[*r]-RGB2YUV_VG[*g]-RGB2YUV_VB[*b]+8388608)>>16;

			r+=3;
			g+=3;
			b+=3;
		}

	}



	// Now sample the U & V to obtain YUV 4:2:0 format

	// Sampling mechanism...
/*	  @  ->  Y
	  #  ->  U or V
	  
	  @   @   @   @
		#       #
	  @   @   @   @
	
	  @   @   @   @
		#       #
	  @   @   @   @

*/

	// Get the right pointers...
	u=yuv+w*h;
	v=u+(w*h)/4;

	// For U
	pu1=uu;
	pu2=pu1+1;
	pu3=pu1+w;
	pu4=pu3+1;

	// For V
	pv1=vv;
	pv2=pv1+1;
	pv3=pv1+w;
	pv4=pv3+1;

	// Do sampling....
	for(i=0;i<h;i+=2)
	{
	
		for(j=0;j<w;j+=2)
		{
		*u++=(*pu1+*pu2+*pu3+*pu4)>>2;
		*v++=(*pv1+*pv2+*pv3+*pv4)>>2;

		pu1+=2;
		pu2+=2;
		pu3+=2;
		pu4+=2;

		pv1+=2;
		pv2+=2;
		pv3+=2;
		pv4+=2;
		}
	
	pu1+=w;
	pu2+=w;
	pu3+=w;
	pu4+=w;

	pv1+=w;
	pv2+=w;
	pv3+=w;
	pv4+=w;
		
	}


	delete[] uu;
	delete[] vv;

	return 1;
}





//
//Initialize conversion table for YUV420 to RGB
//
void InitConvertTable()
{
   long int crv,cbu,cgu,cgv;
   int i,ind;   
     
   crv = 104597; cbu = 132201;  /* fra matrise i global.h */
   cgu = 25675;  cgv = 53279;
  
   for (i = 0; i < 256; i++) {
      crv_tab[i] = (i-128) * crv;
      cbu_tab[i] = (i-128) * cbu;
      cgu_tab[i] = (i-128) * cgu;
      cgv_tab[i] = (i-128) * cgv;
      tab_76309[i] = 76309*(i-16);
   }
	 
   for (i=0; i<384; i++)
	  clp[i] =0;
   ind=384;
   for (i=0;i<256; i++)
	   clp[ind++]=i;
   ind=640;
   for (i=0;i<384;i++)
	   clp[ind++]=255;
}


//
//  Convert from YUV420 to RGB24
//
void ConvertYUV2RGB(unsigned char *src0,unsigned char *src1,unsigned char *src2,unsigned char *dst_ori,
								 int width,int height)
{
	int y1,y2,u,v; 
	unsigned char *py1,*py2;
	int i,j, c1, c2, c3, c4;
	unsigned char *d1, *d2;

	py1=src0;
	py2=py1+width;
	d1=dst_ori;
	d2=d1+3*width;
 	for (j = 0; j < height; j += 2) { 
		for (i = 0; i < width; i += 2) {

			u = *src1++;
			v = *src2++;

			c1 = crv_tab[v];
			c2 = cgu_tab[u];
			c3 = cgv_tab[v];
			c4 = cbu_tab[u];

			//up-left
            y1 = tab_76309[*py1++];	
			*d1++ = clp[384+((y1 + c1)>>16)];  
			*d1++ = clp[384+((y1 - c2 - c3)>>16)];
            *d1++ = clp[384+((y1 + c4)>>16)];

			//down-left
			y2 = tab_76309[*py2++];
			*d2++ = clp[384+((y2 + c1)>>16)];  
			*d2++ = clp[384+((y2 - c2 - c3)>>16)];
            *d2++ = clp[384+((y2 + c4)>>16)];

			//up-right
			y1 = tab_76309[*py1++];
			*d1++ = clp[384+((y1 + c1)>>16)];  
			*d1++ = clp[384+((y1 - c2 - c3)>>16)];
			*d1++ = clp[384+((y1 + c4)>>16)];

			//down-right
			y2 = tab_76309[*py2++];
			*d2++ = clp[384+((y2 + c1)>>16)];  
			*d2++ = clp[384+((y2 - c2 - c3)>>16)];
            *d2++ = clp[384+((y2 + c4)>>16)];
		}
		d1 += 3*width;
		d2 += 3*width;
		py1+=   width;
		py2+=   width;
	}       


}



void Tran565to24(unsigned char* src, int w, int h, unsigned char* dst)
{
	int x = 0, y = 0;
	int pos = 0;
	unsigned char val0 =0, val1 = 0;
	for (y = 0; y < h; y++)
	{
		for (x = 0; x < w; x++)
		{
			pos = (y * w  + x) * 2;
			val0 = *(src + pos);
			val1 = *(src + pos + 1);

			pos =  3 * w * y + x * 3;
			*(dst + pos + 0) = val0 << 3;
			*(dst + pos + 1) = (val1 << 5) | ((val0 >> 3) & 0x1C);
			*(dst + pos + 2) = val1 & 0xF8;
		}
	}
}

void Tran555to24(unsigned char* src, int w, int h, unsigned char* dst)
{
	int x = 0, y = 0;
	int pos = 0;
	unsigned char val0 = 0, val1 = 0;
	for (y = 0; y < h; y++)
	{
		for (x = 0; x < w; x++)
		{
			pos = (y * w  + x) * 2;
			val0 = *(src + pos);
			val1 = *(src + pos + 1);

			pos =   3 * w * y + x * 3;
			*(dst + pos + 0) = val0 << 3;
			*(dst + pos + 1) = (val1 << 6) | ((val0 >> 2) & 0x38);
			*(dst + pos + 2) = (val1<<1) & 0xF8;
		}
	}
}


//YUY2视频转化RGB格式 by hardy 2007-10-17
//http://www.fourcc.org/fccyvrgb.php

void YUY2_RGB(unsigned char *YUY2buff,unsigned char *RGBbuff,int dwSize)
{
	//B = 1.164(Y - 16)         + 2.018(U - 128)
	//G = 1.164(Y - 16) - 0.813(V - 128) - 0.391(U - 128)
	//R = 1.164(Y - 16) + 1.596(V - 128)
	unsigned char *orgRGBbuff = RGBbuff;
	for(int count=0;count<dwSize;count+=4)
	{
		//Y0 U0 Y1 V0
		float Y0 = *YUY2buff;
		float U = *(++YUY2buff);
		float Y1 = *(++YUY2buff);
		float V = *(++YUY2buff);
		++YUY2buff;
		*(RGBbuff) =   (unsigned char)(Y0 + (1.370705 * (V-128)));          
		*(++RGBbuff) = (unsigned char)(Y0 - (0.698001 * (V-128)) - (0.337633 * (U-128)));  
		*(++RGBbuff) = (unsigned char)(Y0 + (1.732446 * (U-128)));        
		*(++RGBbuff) = (unsigned char)(Y1 + (1.370705 * (V-128)));          
		*(++RGBbuff) = (unsigned char)(Y1 - (0.698001 * (V-128)) - (0.337633 * (U-128)));  
		*(++RGBbuff) = (unsigned char)(Y1 + (1.732446 * (U-128)));        
		++RGBbuff;
	}
}
void YUY2_RGB2(unsigned char *YUY2buff,unsigned char *RGBbuff,int dwSize)
{
	//R = Y + 1.14V
	//G = Y - 0.39U - 0.58V
	//B = Y + 2.03U
	// R = Y + 1.403V'
	//G = Y - 0.344U' - 0.714V'
	//B = Y + 1.770U'
	unsigned char *orgRGBbuff = RGBbuff;
	for(int count=0;count<dwSize;count+=4)
	{
		//Y0 U0 Y1 V0
		float Y0 = *YUY2buff;
		float U = *(++YUY2buff);
		float Y1 = *(++YUY2buff);
		float V = *(++YUY2buff);
		++YUY2buff;
		float R,G,B;

		R = (Y0 + 1.14f*V);
		G=(Y0 - 0.39f*U-0.58f*V);  
		B=(Y0 +2.03f*U);
		if(R<0) R =0;
		if(R>255) R=255;
		if(G<0) G =0;
		if(G>255) G=255;
		if(B<0) B =0;
		if(B>255) B=255;
		*(RGBbuff) =     (unsigned char)B;         
		*(++RGBbuff) =  (unsigned char)G;
		*(++RGBbuff) =  (unsigned char)R;
		R = (Y1 + 1.14f*V);
		G=(Y1 - 0.39f*U-0.58f*V);  
		B=(Y1 +2.03f*U)   ;
		if(R<0) R =0;
		if(R>255) R=255;
		if(G<0) G =0;
		if(G>255) G=255;
		if(B<0) B =0;
		if(B>255) B=255;
		*(++RGBbuff) =     (unsigned char)B;         
		*(++RGBbuff) =  (unsigned char)G;
		*(++RGBbuff) =  (unsigned char)R;
		++RGBbuff;
	}
}

void YUY2_RGB3(unsigned char *YUY2buff,unsigned char *RGBbuff,int dwSize)
{
	//B = 1.164(Y - 16)         + 2.018(U - 128)
	//G = 1.164(Y - 16) - 0.813(V - 128) - 0.391(U - 128)
	//R = 1.164(Y - 16) + 1.596(V - 128)
	unsigned char *orgRGBbuff = RGBbuff;
	for(int count=0;count<dwSize;count+=4)
	{
		//Y0 U0 Y1 V0
		float Y0 = *YUY2buff;
		float U = *(++YUY2buff);
		float Y1 = *(++YUY2buff);
		float V = *(++YUY2buff);
		++YUY2buff;
		*(RGBbuff) =   (unsigned char)(1.164f*(Y0 - 16)+ 2.018f*(U - 128));          
		*(++RGBbuff) = (unsigned char)(1.164f*(Y0 - 16) - 0.813f*(V - 128) - 0.391f*(U - 128));  
		*(++RGBbuff) = (unsigned char)(1.164f*(Y0 - 16) + 1.596f*(V - 128));        
		*(++RGBbuff) =   (unsigned char)(1.164f*(Y0 - 16)+ 2.018f*(U - 128));          
		*(++RGBbuff) = (unsigned char)(1.164f*(Y0 - 16) - 0.813f*(V - 128) - 0.391f*(U - 128));  
		*(++RGBbuff) = (unsigned char)(1.164f*(Y0 - 16) + 1.596f*(V - 128));         
		++RGBbuff;

	}
}
void YUY2_RGB4(unsigned char *YUY2buff,unsigned char *RGBbuff,int dwSize)
{

	// R = Y + 1.403V'
	//G = Y - 0.344U' - 0.714V'
	//B = Y + 1.770U'
	unsigned char *orgRGBbuff = RGBbuff;
	for(int count=0;count<dwSize;count+=4)
	{
		//Y0 U0 Y1 V0
		float Y0 = *YUY2buff;
		float U = *(++YUY2buff);
		float Y1 = *(++YUY2buff);
		float V = *(++YUY2buff);
		++YUY2buff;
		float R,G,B;

		R = (Y0 + 1.403f*V);
		G=(Y0 - 0.344f*U-0.714f*V);  
		B=(Y0 +1.77f*U);
		if(R<0) R =0;
		if(R>255) R=255;
		if(G<0) G =0;
		if(G>255) G=255;
		if(B<0) B =0;
		if(B>255) B=255;
		*(RGBbuff) =     (unsigned char)R;         
		*(++RGBbuff) =  (unsigned char)G;
		*(++RGBbuff) =  (unsigned char)B;
		R = (Y1 + 1.403f*V);
		G=(Y1 - 0.344f*U-0.714f*V);  
		B=(Y1 +1.77f*U);
		if(R<0) R =0;
		if(R>255) R=255;
		if(G<0) G =0;
		if(G>255) G=255;
		if(B<0) B =0;
		if(B>255) B=255;
		*(++RGBbuff) =     (unsigned char)R;         
		*(++RGBbuff) =  (unsigned char)G;
		*(++RGBbuff) =  (unsigned char)B;
		++RGBbuff;
	}
}

/** 翻转图象
@param  BYTE * pDest:输出数据buffer
@param  DWORD dwDestSize: 输出数据buffer大小
@param  BYTE * pSrc:输入数据buffer
@param  DWORD dwSrcSize:输入数据buffer大小
@param  const int nHeight: Bmp图像高度
@param  const int nLineByteWidth:每一行的占字节数
@return  
*/	
void FlipDIBImage(unsigned char * pDest,const int dwDestSize,unsigned char * pSrc,const int dwSrcSize,const int nHeight,const int nLineByteWidth)
{
	if (pDest==NULL || pSrc==NULL || dwDestSize==0 || dwSrcSize==0 || nHeight<=0 || nLineByteWidth<=0)
	{
		return;
	}
	int nSize=nLineByteWidth*nHeight;
	if ( nSize>dwSrcSize || nSize>dwDestSize)
	{
		return;
	}

	unsigned char * pOut=NULL;
	unsigned char * pIn=NULL;

	for(int i=0;i<nHeight;i++)
	{
		pOut=pDest+i*nLineByteWidth;
		pIn=pSrc+(nHeight-i-1)*nLineByteWidth;
		memcpy(pOut,pIn,nLineByteWidth);
	}
}



//////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////
// YUV2RGB
// pYUV			point to the YUV data
// pRGB			point to the RGB data
// width		width of the picture
// height		height of the picture
// alphaYUV		is there an alpha channel in YUV
// alphaRGB		is there an alpha channel in RGB
//////////////////////////////////////////////////////////////////////////
int YUV2RGB(void* pYUV, void* pRGB, int width, int height, bool alphaYUV, bool alphaRGB)
{
	if (NULL == pYUV || NULL== pRGB)
	{
		return -1;
	}
	unsigned char* pYUVData = (unsigned char *)pYUV;
	unsigned char* pRGBData = (unsigned char *)pRGB;
	//if (NULL == pRGBData)
	//{
	//	if (alphaRGB)
	//	{
	//		pRGBData = new unsigned char[width*height*4];
	//	}
	//	else
	//		pRGBData = new unsigned char[width*height*3];
	//}
	int Y1, U1, V1, Y2, alpha1, alpha2, R1, G1, B1, R2, G2, B2;
	int C1, D1, E1, C2;
	if (alphaRGB)
	{
		if (alphaYUV)
		{
			for (int i=0; i<height; ++i)
			{
				for (int j=0; j<width/2; ++j)
				{
					Y1 = *(pYUVData+i*width*3+j*6);
					U1 = *(pYUVData+i*width*3+j*6+1);
					Y2 = *(pYUVData+i*width*3+j*6+2);
					V1 = *(pYUVData+i*width*3+j*6+3);
					alpha1 = *(pYUVData+i*width*3+j*6+4);
					alpha2 = *(pYUVData+i*width*3+j*6+5);
					C1 = Y1-16;
					C2 = Y2-16;
					D1 = U1-128;
					E1 = V1-128;
					R1 = ((298*C1 + 409*E1 + 128)>>8>255 ? 255 : (298*C1 + 409*E1 + 128)>>8);
					G1 = ((298*C1 - 100*D1 - 208*E1 + 128)>>8>255 ? 255 : (298*C1 - 100*D1 - 208*E1 + 128)>>8);	
					B1 = ((298*C1+516*D1 +128)>>8>255 ? 255 : (298*C1+516*D1 +128)>>8);	
					R2 = ((298*C2 + 409*E1 + 128)>>8>255 ? 255 : (298*C2 + 409*E1 + 128)>>8);
					G2 = ((298*C2 - 100*D1 - 208*E1 + 128)>>8>255 ? 255 : (298*C2 - 100*D1 - 208*E1 + 128)>>8);
					B2 = ((298*C2 + 516*D1 +128)>>8>255 ? 255 : (298*C2 + 516*D1 +128)>>8);	
					*(pRGBData+(height-i-1)*width*4+j*8+2) = R1<0 ? 0 : R1;
					*(pRGBData+(height-i-1)*width*4+j*8+1) = G1<0 ? 0 : G1;
					*(pRGBData+(height-i-1)*width*4+j*8) = B1<0 ? 0 : B1;
					*(pRGBData+(height-i-1)*width*4+j*8+3) = alpha1;	
					*(pRGBData+(height-i-1)*width*4+j*8+6) = R2<0 ? 0 : R2;
					*(pRGBData+(height-i-1)*width*4+j*8+5) = G2<0 ? 0 : G2;
					*(pRGBData+(height-i-1)*width*4+j*8+4) = B2<0 ? 0 : B2;
					*(pRGBData+(height-i-1)*width*4+j*8+7) = alpha2;	
				}
			}	
		}
		else
		{
			int alpha = 255;
			for (int i=0; i<height; ++i)
			{
				for (int j=0; j<width/2; ++j)
				{
					Y1 = *(pYUVData+i*width*2+j*4);
					U1 = *(pYUVData+i*width*2+j*4+1);
					Y2 = *(pYUVData+i*width*2+j*4+2);
					V1 = *(pYUVData+i*width*2+j*4+3);
					C1 = Y1-16;
					C2 = Y2-16;
					D1 = U1-128;
					E1 = V1-128;
					R1 = ((298*C1 + 409*E1 + 128)>>8>255 ? 255 : (298*C1 + 409*E1 + 128)>>8);
					G1 = ((298*C1 - 100*D1 - 208*E1 + 128)>>8>255 ? 255 : (298*C1 - 100*D1 - 208*E1 + 128)>>8);	
					B1 = ((298*C1+516*D1 +128)>>8>255 ? 255 : (298*C1+516*D1 +128)>>8);	
					R2 = ((298*C2 + 409*E1 + 128)>>8>255 ? 255 : (298*C2 + 409*E1 + 128)>>8);
					G2 = ((298*C2 - 100*D1 - 208*E1 + 128)>>8>255 ? 255 : (298*C2 - 100*D1 - 208*E1 + 128)>>8);
					B2 = ((298*C2 + 516*D1 +128)>>8>255 ? 255 : (298*C2 + 516*D1 +128)>>8);	
					*(pRGBData+(height-i-1)*width*4+j*8+2) = R1<0 ? 0 : R1;
					*(pRGBData+(height-i-1)*width*4+j*8+1) = G1<0 ? 0 : G1;
					*(pRGBData+(height-i-1)*width*4+j*8) = B1<0 ? 0 : B1;
					*(pRGBData+(height-i-1)*width*4+j*8+3) = alpha;	
					*(pRGBData+(height-i-1)*width*4+j*8+6) = R2<0 ? 0 : R2;
					*(pRGBData+(height-i-1)*width*4+j*8+5) = G2<0 ? 0 : G2;
					*(pRGBData+(height-i-1)*width*4+j*8+4) = B2<0 ? 0 : B2;
					*(pRGBData+(height-i-1)*width*4+j*8+7) = alpha;	
				}
			}	
		}
	}
	else
	{
		if (alphaYUV)
		{
			for (int i=0; i<height; ++i)
			{
				for (int j=0; j<width/2; ++j)
				{
					Y1 = *(pYUVData+i*width*3+j*4);
					U1 = *(pYUVData+i*width*3+j*4+1);
					Y2 = *(pYUVData+i*width*3+j*4+2);
					V1 = *(pYUVData+i*width*3+j*4+3);
					C1 = Y1-16;
					C2 = Y2-16;
					D1 = U1-128;
					E1 = V1-128;
					R1 = ((298*C1 + 409*E1 + 128)>>8>255 ? 255 : (298*C1 + 409*E1 + 128)>>8);
					G1 = ((298*C1 - 100*D1 - 208*E1 + 128)>>8>255 ? 255 : (298*C1 - 100*D1 - 208*E1 + 128)>>8);	
					B1 = ((298*C1+516*D1 +128)>>8>255 ? 255 : (298*C1+516*D1 +128)>>8);	
					R2 = ((298*C2 + 409*E1 + 128)>>8>255 ? 255 : (298*C2 + 409*E1 + 128)>>8);
					G2 = ((298*C2 - 100*D1 - 208*E1 + 128)>>8>255 ? 255 : (298*C2 - 100*D1 - 208*E1 + 128)>>8);
					B2 = ((298*C2 + 516*D1 +128)>>8>255 ? 255 : (298*C2 + 516*D1 +128)>>8);	
					*(pRGBData+(height-i-1)*width*3+j*6+2) = R1<0 ? 0 : R1;
					*(pRGBData+(height-i-1)*width*3+j*6+1) = G1<0 ? 0 : G1;
					*(pRGBData+(height-i-1)*width*3+j*6) = B1<0 ? 0 : B1;
					*(pRGBData+(height-i-1)*width*3+j*6+5) = R2<0 ? 0 : R2;
					*(pRGBData+(height-i-1)*width*3+j*6+4) = G2<0 ? 0 : G2;
					*(pRGBData+(height-i-1)*width*3+j*6+3) = B2<0 ? 0 : B2;
				}
			}
		}
		else
		{
			for (int i=0; i<height; ++i)
			{
				for (int j=0; j<width/2; ++j)
				{
					Y1 = *(pYUVData+i*width*2+j*4);
					U1 = *(pYUVData+i*width*2+j*4+1);
					Y2 = *(pYUVData+i*width*2+j*4+2);
					V1 = *(pYUVData+i*width*2+j*4+3);
					C1 = Y1-16;
					C2 = Y2-16;
					D1 = U1-128;
					E1 = V1-128;
					R1 = ((298*C1 + 409*E1 + 128)>>8>255 ? 255 : (298*C1 + 409*E1 + 128)>>8);
					G1 = ((298*C1 - 100*D1 - 208*E1 + 128)>>8>255 ? 255 : (298*C1 - 100*D1 - 208*E1 + 128)>>8);	
					B1 = ((298*C1+516*D1 +128)>>8>255 ? 255 : (298*C1+516*D1 +128)>>8);	
					R2 = ((298*C2 + 409*E1 + 128)>>8>255 ? 255 : (298*C2 + 409*E1 + 128)>>8);
					G2 = ((298*C2 - 100*D1 - 208*E1 + 128)>>8>255 ? 255 : (298*C2 - 100*D1 - 208*E1 + 128)>>8);
					B2 = ((298*C2 + 516*D1 +128)>>8>255 ? 255 : (298*C2 + 516*D1 +128)>>8);	
					*(pRGBData+(height-i-1)*width*3+j*6+2) = R1<0 ? 0 : R1;
					*(pRGBData+(height-i-1)*width*3+j*6+1) = G1<0 ? 0 : G1;
					*(pRGBData+(height-i-1)*width*3+j*6) = B1<0 ? 0 : B1;
					*(pRGBData+(height-i-1)*width*3+j*6+5) = R2<0 ? 0 : R2;
					*(pRGBData+(height-i-1)*width*3+j*6+4) = G2<0 ? 0 : G2;
					*(pRGBData+(height-i-1)*width*3+j*6+3) = B2<0 ? 0 : B2;
				}
			}	
		}
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////
// RGB2YUV
// pRGB			point to the RGB data
// pYUV			point to the YUV data
// width		width of the picture
// height		height of the picture
// alphaYUV		is there an alpha channel in YUV
// alphaRGB		is there an alpha channel in RGB
//////////////////////////////////////////////////////////////////////////
int RGB2YUV(void* pRGB, void* pYUV, int width, int height, bool alphaYUV, bool alphaRGB)
{
	if (NULL == pRGB || NULL== pYUV)
	{
		return -1;
	}
	unsigned char* pRGBData = (unsigned char *)pRGB;
	unsigned char* pYUVData = (unsigned char *)pYUV;
	//if (NULL == pYUVData)
	//{
	//	if (alphaYUV)
	//	{
	//		pYUVData = new unsigned char[width*height*3];
	//	}
	//	else
	//		pYUVData = new unsigned char[width*height*2];
	//}
	int R1, G1, B1, R2, G2, B2, Y1, U1, Y2, V1;
	int alpha1, alpha2;
	if (alphaYUV)
	{
		if (alphaRGB)
		{
			for (int i=0; i<height; ++i)
			{
				for (int j=0; j<width/2; ++j)
				{
					B1 = *(pRGBData+(height-i-1)*width*4+j*8);
					G1 = *(pRGBData+(height-i-1)*width*4+j*8+1);
					R1 = *(pRGBData+(height-i-1)*width*4+j*8+2);
					alpha1 = *(pRGBData+(height-i-1)*width*4+j*8+3);
					B2 = *(pRGBData+(height-i-1)*width*4+j*8+4);
					G2 = *(pRGBData+(height-i-1)*width*4+j*8+5);
					R2 = *(pRGBData+(height-i-1)*width*4+j*8+6);
					alpha2 = *(pRGBData+(height-i-1)*width*4+j*8+7);
					Y1 = (((66*R1+129*G1+25*B1+128)>>8) + 16) > 255 ? 255 : (((66*R1+129*G1+25*B1+128)>>8) + 16);
					U1 = ((((-38*R1-74*G1+112*B1+128)>>8)+((-38*R2-74*G2+112*B2+128)>>8))/2 + 128)>255 ? 255 : ((((-38*R1-74*G1+112*B1+128)>>8)+((-38*R2-74*G2+112*B2+128)>>8))/2 + 128);
					Y2 = (((66*R2+129*G2+25*B2+128)>>8) + 16)>255 ? 255 : ((66*R2+129*G2+25*B2+128)>>8) + 16;
					V1 = ((((112*R1-94*G1-18*B1+128)>>8) + ((112*R2-94*G2-18*B2+128)>>8))/2 + 128)>255 ? 255 : ((((112*R1-94*G1-18*B1+128)>>8) + ((112*R2-94*G2-18*B2+128)>>8))/2 + 128);
					*(pYUVData+i*width*3+j*6) = Y1;
					*(pYUVData+i*width*3+j*6+1) = U1;
					*(pYUVData+i*width*3+j*6+2) = Y2;
					*(pYUVData+i*width*3+j*6+3) = V1;
					*(pYUVData+i*width*3+j*6+4) = alpha1;
					*(pYUVData+i*width*3+j*6+5) = alpha2;
				}
			}	
		}
		else
		{
			unsigned char alpha = 255;
			for (int i=0; i<height; ++i)
			{
				for (int j=0; j<width/2; ++j)
				{
					B1 = *(pRGBData+(height-i-1)*width*3+j*6);
					G1 = *(pRGBData+(height-i-1)*width*3+j*6+1);
					R1 = *(pRGBData+(height-i-1)*width*3+j*6+2);
					B2 = *(pRGBData+(height-i-1)*width*3+j*6+3);
					G2 = *(pRGBData+(height-i-1)*width*3+j*6+4);
					R2 = *(pRGBData+(height-i-1)*width*3+j*6+5);
					Y1 = ((66*R1+129*G1+25*B1+128)>>8) + 16;
					U1 = ((-38*R1-74*G1+112*B1+128)>>8+(-38*R2-74*G2+112*B2+128)>>8)/2 + 128;
					Y2 = ((66*R2+129*G2+25*B2+128)>>8) + 16;
					V1 = ((112*R1-94*G1-18*B1+128)>>8 + (112*R2-94*G2-18*B2+128)>>8)/2 + 128;
					Y1 = (((66*R1+129*G1+25*B1+128)>>8) + 16) > 255 ? 255 : (((66*R1+129*G1+25*B1+128)>>8) + 16);
					U1 = ((((-38*R1-74*G1+112*B1+128)>>8)+((-38*R2-74*G2+112*B2+128)>>8))/2 + 128)>255 ? 255 : ((((-38*R1-74*G1+112*B1+128)>>8)+((-38*R2-74*G2+112*B2+128)>>8))/2 + 128);
					Y2 = (((66*R2+129*G2+25*B2+128)>>8) + 16)>255 ? 255 : ((66*R2+129*G2+25*B2+128)>>8) + 16;
					V1 = ((((112*R1-94*G1-18*B1+128)>>8) + ((112*R2-94*G2-18*B2+128)>>8))/2 + 128)>255 ? 255 : ((((112*R1-94*G1-18*B1+128)>>8) + ((112*R2-94*G2-18*B2+128)>>8))/2 + 128);
					*(pYUVData+i*width*3+j*6) = Y1;
					*(pYUVData+i*width*3+j*6+1) = U1;
					*(pYUVData+i*width*3+j*6+2) = Y2;
					*(pYUVData+i*width*3+j*6+3) = V1;
					*(pYUVData+i*width*3+j*6+4) = alpha;
					*(pYUVData+i*width*3+j*6+5) = alpha;
				}
			}	
		}
	}
	else
	{
		if (alphaRGB)
		{
			for (int i=0; i<height; ++i)
			{
				for (int j=0; j<width/2; ++j)
				{
					B1 = *(pRGBData+(height-i-1)*width*4+j*8);
					G1 = *(pRGBData+(height-i-1)*width*4+j*8+1);
					R1 = *(pRGBData+(height-i-1)*width*4+j*8+2);
					B2 = *(pRGBData+(height-i-1)*width*4+j*8+4);
					G2 = *(pRGBData+(height-i-1)*width*4+j*8+5);
					R2 = *(pRGBData+(height-i-1)*width*4+j*8+6);
					Y1 = (((66*R1+129*G1+25*B1+128)>>8) + 16) > 255 ? 255 : (((66*R1+129*G1+25*B1+128)>>8) + 16);
					U1 = ((((-38*R1-74*G1+112*B1+128)>>8)+((-38*R2-74*G2+112*B2+128)>>8))/2 + 128)>255 ? 255 : ((((-38*R1-74*G1+112*B1+128)>>8)+((-38*R2-74*G2+112*B2+128)>>8))/2 + 128);
					Y2 = (((66*R2+129*G2+25*B2+128)>>8) + 16)>255 ? 255 : ((66*R2+129*G2+25*B2+128)>>8) + 16;
					V1 = ((((112*R1-94*G1-18*B1+128)>>8) + ((112*R2-94*G2-18*B2+128)>>8))/2 + 128)>255 ? 255 : ((((112*R1-94*G1-18*B1+128)>>8) + ((112*R2-94*G2-18*B2+128)>>8))/2 + 128);
					*(pYUVData+i*width*2+j*4) = Y1;
					*(pYUVData+i*width*2+j*4+1) = U1;
					*(pYUVData+i*width*2+j*4+2) = Y2;
					*(pYUVData+i*width*2+j*4+3) = V1;
				}
			}	
		}
		else
		{
			for (int i=0; i<height; ++i)
			{
				for (int j=0; j<width/2; ++j)
				{
					B1 = *(pRGBData+(height-i-1)*width*3+j*6);
					G1 = *(pRGBData+(height-i-1)*width*3+j*6+1);
					R1 = *(pRGBData+(height-i-1)*width*3+j*6+2);
					B2 = *(pRGBData+(height-i-1)*width*3+j*6+3);
					G2 = *(pRGBData+(height-i-1)*width*3+j*6+4);
					R2 = *(pRGBData+(height-i-1)*width*3+j*6+5);
					Y1 = (((66*R1+129*G1+25*B1+128)>>8) + 16) > 255 ? 255 : (((66*R1+129*G1+25*B1+128)>>8) + 16);
					U1 = ((((-38*R1-74*G1+112*B1+128)>>8)+((-38*R2-74*G2+112*B2+128)>>8))/2 + 128)>255 ? 255 : ((((-38*R1-74*G1+112*B1+128)>>8)+((-38*R2-74*G2+112*B2+128)>>8))/2 + 128);
					Y2 = (((66*R2+129*G2+25*B2+128)>>8) + 16)>255 ? 255 : ((66*R2+129*G2+25*B2+128)>>8) + 16;
					V1 = ((((112*R1-94*G1-18*B1+128)>>8) + ((112*R2-94*G2-18*B2+128)>>8))/2 + 128)>255 ? 255 : ((((112*R1-94*G1-18*B1+128)>>8) + ((112*R2-94*G2-18*B2+128)>>8))/2 + 128);
					*(pYUVData+i*width*2+j*4) = Y1;
					*(pYUVData+i*width*2+j*4+1) = U1;
					*(pYUVData+i*width*2+j*4+2) = Y2;
					*(pYUVData+i*width*2+j*4+3) = V1;
				}
			}	
		}
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////
// pGBYUV			point to the background YUV data
// pFGYUV			point to the foreground YUV data
// width			width of the picture
// height			height of the picture
// alphaBG			is there an alpha channel in background YUV data
// alphaFG			is there an alpha channel in fourground YUV data
//////////////////////////////////////////////////////////////////////////
int YUVBlending(void* pBGYUV, void* pFGYUV, int width, int height, bool alphaBG, bool alphaFG)
{
	if (NULL == pBGYUV || NULL == pFGYUV)
	{
		return -1;
	}
	unsigned char* pBGData = (unsigned char*)pBGYUV;
	unsigned char* pFGData = (unsigned char*)pFGYUV;
	if (!alphaFG)
	{
		if (!alphaBG)
		{
			memcpy(pBGData, pFGData, width*height*2);
		}
		else
		{
			for (int i=0; i<height; ++i)
			{
				for (int j=0; j<width/2; ++j)
				{
					*(pBGData+i*width*2+j*4) = *(pFGData+i*width*2+j*4);
					*(pBGData+i*width*2+j*4+1) = *(pFGData+i*width*2+j*4+1);
					*(pBGData+i*width*2+j*4+2) = *(pFGData+i*width*2+j*4+2);
					*(pBGData+i*width*2+j*4+3) = *(pFGData+i*width*2+j*4+3);
				}
			}
		}
	}
	int Y11, U11, V11, Y12, Y21, U21, V21, Y22;
	int alpha1, alpha2;
	if (!alphaBG)
	{
		for (int i=0; i<height; ++i)
		{
			for (int j=0; j<width/2; ++j)
			{
				Y11 = *(pBGData+i*width*2+j*4);
				U11 = *(pBGData+i*width*2+j*4+1);
				Y12 = *(pBGData+i*width*2+j*4+2);
				V11 = *(pBGData+i*width*2+j*4+3);

				Y21 = *(pFGData+i*width*3+j*6);
				U21 = *(pFGData+i*width*3+j*6+1);
				Y22 = *(pFGData+i*width*3+j*6+2);
				V21 = *(pFGData+i*width*3+j*6+3);
				alpha1 = *(pFGData+i*width*3+j*6+4);
				alpha2 = *(pFGData+i*width*3+j*6+5);

				*(pBGData+i*width*2+j*4) = (Y21-16)*alpha1/255+(Y11-16)*(255-alpha1)/255+16;
				*(pBGData+i*width*2+j*4+1) = ((U21-128)*alpha1/255+(U11-128)*(255-alpha1)/255 + (U21-128)*alpha2/255+(U11-128)*(255-alpha2)/255)/2+128;
				*(pBGData+i*width*2+j*4+3) = ((V21-128)*alpha1/255+(V11-128)*(255-alpha1)/255 + (V21-128)*alpha2/255+(V11-128)*(255-alpha2)/255)/2+128;
				*(pBGData+i*width*2+j*4+2) = (Y22-16)*alpha2/255+(Y12-16)*(255-alpha2)/255+16;
			}
		}
	}
	else
	{
		for (int i=0; i<height; ++i)
		{
			for (int j=0; j<width/2; ++j)
			{
				Y11 = *(pBGData+i*width*3+j*6);
				U11 = *(pBGData+i*width*3+j*6+1);
				Y12 = *(pBGData+i*width*3+j*6+2);
				V11 = *(pBGData+i*width*3+j*6+3);

				Y21 = *(pFGData+i*width*3+j*6);
				U21 = *(pFGData+i*width*3+j*6+1);
				Y22 = *(pFGData+i*width*3+j*6+2);
				V21 = *(pFGData+i*width*3+j*6+3);
				alpha1 = *(pFGData+i*width*3+j*6+4);
				alpha2 = *(pFGData+i*width*3+j*6+5);

				*(pBGData+i*width*3+j*6) = (Y21-16)*alpha1/255+(Y11-16)*(255-alpha1)/255+16;
				*(pBGData+i*width*3+j*6+1) = ((U21-128)*alpha1/255+(U11-128)*(255-alpha1)/255 + (U21-128)*alpha2/255+(U11-128)*(255-alpha2)/255)/2+128;
				*(pBGData+i*width*3+j*6+3) = ((V21-128)*alpha1/255+(V11-128)*(255-alpha1)/255 + (V21-128)*alpha2/255+(V11-128)*(255-alpha2)/255)/2+128;
				*(pBGData+i*width*3+j*6+2) = (Y22-16)*alpha2/255+(Y12-16)*(255-alpha2)/255+16;
			}
		}
	}
	return 0;
}

