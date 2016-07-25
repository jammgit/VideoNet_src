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
//    Name    : VideoNetDlg.cpp
//    Details : Main dialog class
//
//
/////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "VideoNet.h"
#include "VideoNetDlg.h"
#include "Resource.h"
#include "Yuv_RGB.h"
//#include "H264Encoder.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// 频道配置结构
static const SVideoNetConfig g_SpeedConfig[2][SEND_CONFIG_MAX] = 
{
	{	// 优先画质  跳帧率 最大100 
		//		B/s			跳帧率		压缩参数
		{ 0,	10000,		0,			3,			},
		{ 1,	15000,		10,			4,			},
		{ 2,	20000,		20,			4,			},
		{ 3,	25000,		40,			8,			},
		{ 4,	30000,		50,			8,			},
		{ 5,	35000,		60,			8,			},
		{ 6,	40000,		70,			16,			},
		{ 7,	45000,		80,			16,			},
		{ 8,	50000,		90,			16,			},
		{ 9,	55000,		95,			16,			},
	},
	{	// 优先流畅
		//		B/s			跳			压缩参数
		{ 0,	10000,		0,			6,			},
		{ 1,	15000,		0,			8,			},
		{ 2,	20000,		10,			8,			},
		{ 3,	25000,		20,			10,			},
		{ 4,	30000,		40,			10,			},
		{ 5,	35000,		50,			10,			},
		{ 6,	40000,		60,			24,			},
		{ 7,	45000,		70,			24,			},
		{ 8,	50000,		80,			24,			},
		{ 9,	55000,		90,			24,			},
		}	
};


const SVideoNetConfig & GetSpeedConfig(DWORD dwSpeed,bool bIsGoodPic)
{
	int nType = bIsGoodPic?0:1;
	for (DWORD i=0;i<SEND_CONFIG_MAX;++i)
	{
		if (dwSpeed<g_SpeedConfig[nType][i].dwSpeed)
		{
			return g_SpeedConfig[nType][i];
		}
	}
	return g_SpeedConfig[nType][SEND_CONFIG_MAX-1];
}

const SVideoNetConfig & GetSpeedConfigByID(int nID,bool bIsGoodPic)
{
	int nType = bIsGoodPic?0:1;
	if (nID>=0 && nID<SEND_CONFIG_MAX)
	{
		return g_SpeedConfig[nType][nID];
	}
	return g_SpeedConfig[nType][0];
}

//static int g_SkipFrameInfo[100][2];	// 算好的数据
//static bool g_bIniSkipFrameInfo = false;

void GetSpeedSkipFrame(int nPer,int &nSkip,int &nSend,bool bUseCache)
{
	// 初始化一下
	//if (!g_bIniSkipFrameInfo)
	//{
	//	g_bIniSkipFrameInfo = true;
	//	int nMySkip = 0;
	//	int nMySend = 0;
	//	for (int i=0;i<100;i++)
	//	{
	//		GetSpeedSkipFrame(i,nMySkip,nMySend,false);
	//		g_SkipFrameInfo[i][0] = nMySkip;
	//		g_SkipFrameInfo[i][1] = nMySend;
	//	}
	//}
	if (nPer<=0 || nPer>=100)
	{
		nSkip = 0;
		nSend = 1;
		return;
	}
	// 用已算好的加快点性能
	//if (bUseCache)
	//{
	//	nSkip = g_SkipFrameInfo[nPer][0];
	//	nSend = g_SkipFrameInfo[nPer][0];
	//	return;
	//}
	
	int nFindSkip[FSKIP_CONFIG_MAX] = {0};
	int nFindSend[FSKIP_CONFIG_MAX] = {0};
	int nFindPer[FSKIP_CONFIG_MAX] = {0};

	if (nPer<=50)
	{
		for (int i=0;i<FSKIP_CONFIG_MAX;i++)
		{
			nFindSkip[i] = i+1;
			nFindSend[i] = (nFindSkip[i]*(100-nPer))/nPer;
			if (nFindSend[i]>SKIP_FRAME_MAX)
			{
				nFindSend[i] = SKIP_FRAME_MAX;
			}
			nFindPer[i] = (100*nFindSkip[i])/(nFindSkip[i]+nFindSend[i]);
		}
	}
	else
	{
		for (int i=0;i<FSKIP_CONFIG_MAX;i++)
		{
			nFindSend[i] = i+1;
			nFindSkip[i] = (nFindSend[i]*nPer)/(100-nPer);
			if (nFindSkip[i]>SKIP_FRAME_MAX)
			{
				nFindSkip[i] = SKIP_FRAME_MAX;
			}
			nFindPer[i] = (100*nFindSkip[i])/(nFindSkip[i]+nFindSend[i]);
		}
	}

	nSkip = nFindSkip[0];
	nSend = nFindSend[0];
	int nMinRang = 10000;
	// 找一个误差最小的
	for (int i=0;i<FSKIP_CONFIG_MAX;i++)
	{
		int nRang = abs(nFindPer[i]-nPer);
		if (nRang<nMinRang)
		{
			nMinRang = nRang;
			nSkip = nFindSkip[i];
			nSend = nFindSend[i];
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()










/////////////////////////////////////////////////////////////////////////////
// CVideoNetDlg dialog




// Global varialbes...
int count=0;
unsigned char cdata[1000000];
int cbuffer_size=1000000;
unsigned char rgbdata[8000000];
int buffersize=8000000;




// Callback function gets invoked during compression...
// It returns the compressed data byte by byte...
void OwnWriteFunction(int byte)
{
	if(count<cbuffer_size)
	cdata[count]=(unsigned char)byte;
	
	count++;

}



CVideoNetDlg::CVideoNetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVideoNetDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVideoNetDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	memset(yuv,0,sizeof(yuv));
	memset(rgbBuf,0,sizeof(rgbBuf));
	dwSendSpeed = 0;
	dwAVGSendSpeed = 0;

	dwAllSpeed =0;
	dwAllFrames =0;
	dwLastTick =0;

	bIsSkiping = false;
	nSkipIndex = 0; 
	nSendFrames = 0;
	nSkipFrames = 0;

	dwFramesPer = 0;
	m_LastGetFrameTicks = 0;

	dwRecvSpeed = 0;
	dwRecvFramesPer = 0;
	dwAllRecvSpeed = 0;
	dwRecvLastTick = 0;
	dwAllRecvFrames= 0;
	m_nSendQ = 8;
	m_nQIndex = 1;
	nSkipPerRate = 0;

	m_dwLastRestTick = 0;
	m_dwLastChangeQTick = 0;

	m_bIsGoodPic = TRUE;
	m_bIsNetSpeed = FALSE;
	m_bOpenSkipMode = true;
	m_bOpenVideoCapture = true;

	m_ptLastMouse.x = 0;
	m_ptLastMouse.y = 0;
	m_dwLastMouseTime = 0;
	m_pDeskTopDC = NULL;

	m_hdcDeskTop = NULL;

}

void CVideoNetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVideoNetDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}





BEGIN_MESSAGE_MAP(CVideoNetDlg, CDialog)
	//{{AFX_MSG_MAP(CVideoNetDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	
	// Conference menu handlers
	ON_COMMAND(CONF_CONNECT,OnConnect)
	ON_COMMAND(CONF_DISCONNECT,OnDisconnect)
	ON_COMMAND(CONF_EXIT,OnCancel)
	
	// View menu handlers
	ON_COMMAND(VIEW_LOCAL,OnViewLocal)
	ON_COMMAND(VIEW_REMOTE,OnViewRemote)
	
	// Audio menu handlers
	ON_COMMAND(AUDIO_CONTROL,OnVolumeControl)
	ON_COMMAND(AUDIO_SEND,OnAudioSend)
	ON_COMMAND(AUDIO_RECEIVE,OnAudioReceive)
	

	// Video menu handlers
	ON_COMMAND(VIDEO_SEND,OnVideoSend)
	ON_COMMAND(VIDEO_RECEIVE,OnVideoReceive)
	

	// Help menu handlers
	ON_COMMAND(HELP_ABOUT,OnAbout)

	ON_COMMAND(IDC_BUTTON1,OnSendMesg)
	

	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
	ON_COMMAND(ID_VIDEO_GOODPIC, &CVideoNetDlg::OnVideoGoodpic)
	ON_COMMAND(ID_VIDEO_SPEED, &CVideoNetDlg::OnVideoSpeed)
	ON_COMMAND(ID_VIDEO_SNAP, &CVideoNetDlg::OnVideoSnap)
	ON_COMMAND(ID_VIDEO_OPENSKIP, &CVideoNetDlg::OnVideoOpenskip)
END_MESSAGE_MAP()










BOOL CVideoNetDlg::OnInitDialog()
{

	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	

	// Open log file...
	log.Open("main.log",CFile::modeCreate | CFile::modeWrite);



	// Disable the menu items...at startup
	menu=this->GetMenu();
	menu->EnableMenuItem(CONF_CONNECT,MF_ENABLED);
	menu->EnableMenuItem(CONF_DISCONNECT,MF_DISABLED | MF_GRAYED);
	menu->EnableMenuItem(AUDIO_SEND,MF_GRAYED);
	menu->EnableMenuItem(AUDIO_RECEIVE,MF_GRAYED);
	menu->EnableMenuItem(VIDEO_SEND,MF_GRAYED);
	menu->EnableMenuItem(VIDEO_RECEIVE,MF_GRAYED);
	menu->EnableMenuItem(VIEW_LOCAL,MF_GRAYED);
	menu->EnableMenuItem(VIEW_REMOTE,MF_GRAYED);

	menu->CheckMenuItem(ID_VIDEO_GOODPIC,MF_BYCOMMAND | MF_CHECKED);
	menu->CheckMenuItem(ID_VIDEO_SPEED,MF_BYCOMMAND | MF_UNCHECKED);
	menu->CheckMenuItem(ID_VIDEO_SNAP,MF_BYCOMMAND | MF_UNCHECKED);
	menu->CheckMenuItem(ID_VIDEO_OPENSKIP,MF_BYCOMMAND | MF_CHECKED);

	list=(CListBox*)GetDlgItem(IDC_LIST1);
	bsend=(CButton*)GetDlgItem(IDC_BUTTON1);

	// Disable the send button
	bsend->EnableWindow(FALSE);

		
	// Create Recording  thread
	record=new RecordSound(this);
	record->CreateThread();

	// Create Player thread
	play=new PlaySound1(this);
	play->CreateThread();


	// Prepare for capture video from webcam...
	vidcap=new VideoCapture();
	vidcap->SetDialog(this);

	if(vidcap->Initialize()) // success
	{	
		//this->m_bmpinfo=&vidcap->m_bmpinfo;

		isVideo=TRUE;

	}
	else
	{
		isVideo=FALSE;
			
		// Disable menu items...
		menu->EnableMenuItem(VIEW_LOCAL,MF_DISABLED | MF_GRAYED);
		menu->EnableMenuItem(VIDEO_SEND,MF_DISABLED | MF_GRAYED);
	
	}

	// Setup bmpinfo structure yourself
	m_bmpinfo=new BITMAPINFO;
	m_bmpinfo->bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	m_bmpinfo->bmiHeader.biWidth=IMAGE_WIDTH;
	m_bmpinfo->bmiHeader.biHeight=IMAGE_HEIGHT;
	m_bmpinfo->bmiHeader.biPlanes=1;
	m_bmpinfo->bmiHeader.biBitCount=24;
	m_bmpinfo->bmiHeader.biCompression=0;
	m_bmpinfo->bmiHeader.biSizeImage=0;
	m_bmpinfo->bmiHeader.biXPelsPerMeter=0;
	m_bmpinfo->bmiHeader.biYPelsPerMeter=0;
	m_bmpinfo->bmiHeader.biClrUsed=0;
	m_bmpinfo->bmiHeader.biClrImportant=0;

	// Initialize the compressor

	// Initialize table for RGB to YUV conversion
	InitLookupTable();

	// Initialize the compressor
	cparams.format = CPARAM_QCIF;//CPARAM_QCIF;
	InitH263Encoder(&cparams);  

	// Set up the callback function
	WriteByteFunction = OwnWriteFunction;
	
	// Capturing is not yet started...
	isCaptureStarted=FALSE;

	isViewLocalVideo=TRUE;
	isViewRemoteVideo=TRUE;

	isAudioSend=TRUE;
	isAudioReceive=TRUE;

	isVideoSend=TRUE;
	isVideoReceive=TRUE;


	
	// Initialize decompressor
	InitH263Decoder();
	
	
	// Adjust display windows
	CWnd *wnd,*bwnd;
	CRect rect,brect;

	// For local video display window
	wnd=this->GetDlgItem(IDC_LOCALVIDEO);
    bwnd=this->GetDlgItem(IDC_LOCALBORDER);
   	bwnd->GetWindowRect(brect);
	ScreenToClient(brect);

	local_wnd_x=brect.TopLeft().x+(brect.Width()-IMAGE_WIDTH)/2;
	local_wnd_y=brect.TopLeft().y+(brect.Height()-IMAGE_HEIGHT)/2;

	//local_wnd_x=brect.TopLeft().x+2;
	//local_wnd_y=brect.TopLeft().y+2;

	local_wnd_w = -1;//brect.Width()-4;
	local_wnd_h = -1;//brect.Height()-4; 
	
	// Centre the local video window
	wnd->SetWindowPos(&wndTop,local_wnd_x-2,local_wnd_y-2,IMAGE_WIDTH+4,IMAGE_HEIGHT+4,SWP_SHOWWINDOW | SWP_DRAWFRAME);

	// For remote video display window
	wnd=this->GetDlgItem(IDC_REMOTEVIDEO);	// Video display window
   	bwnd=this->GetDlgItem(IDC_REMOTEBORDER); // Border window...
   	bwnd->GetWindowRect(brect);
	ScreenToClient(brect);

	remote_wnd_x=brect.TopLeft().x+(brect.Width()-IMAGE_WIDTH)/2;
	remote_wnd_y=brect.TopLeft().y+(brect.Height()-IMAGE_HEIGHT)/2;

	//remote_wnd_x=brect.TopLeft().x+2;
	//remote_wnd_y=brect.TopLeft().y+2;

	remote_wnd_w = -1;//brect.Width()-4;
	remote_wnd_h = -1;//brect.Height()-4;
	
	// Centre the remote video window
	wnd->SetWindowPos(&wndTop,remote_wnd_x-2,remote_wnd_y-2,IMAGE_WIDTH+4,IMAGE_HEIGHT+4,SWP_SHOWWINDOW | SWP_DRAWFRAME);

	
	m_pDeskTopDC = this->GetDesktopWindow()->GetDC(); 
	// Get Dialog DC
	m_hdc = this->GetDC()->m_hDC;
	

	//获取桌面窗口句柄
	HWND hwndDesk = ::GetDesktopWindow();
	//获取桌面窗口DC
	m_hdcDeskTop = ::GetWindowDC(hwndDesk);

	//::DrawDibInit();
    
	// Initialize DIB for drawing...
	hdib=::DrawDibOpen();
	::DrawDibBegin(hdib,
		m_hdc,
		-1,				// don't stretch
		-1,				// don't stretch
		&m_bmpinfo->bmiHeader,
		IMAGE_WIDTH,         // width of image
		IMAGE_HEIGHT,        // height of image
		DDF_SAME_DRAW				
		);

	// Setup the connect dialog box
	connectdlg=new ConnectDlg(NULL);
	connectdlg->SetParent(this);

	
	// Setup the parent for differnt datagram sockets
	dcontrol.SetParent(this);
	daudio.SetParent(this);
	dvideo.SetParent(this);
	dtext.SetParent(this);

	// Create  control datagram socket for handling messages
	dcontrol.CreateSocket(PORT_CONTROL,TYPE_CONTROL);

	// Conference not yet started...
	isStarted=FALSE;

	// 网络处理定时器
	SetTimer( 1, 50, NULL );

	m_x264Encoder.Init();
	m_x264Decoder.init();
//	m_x264Decoder.close();
//	m_h264Encoder.init();
	
	//log.WriteString("跳百分比,跳,发,实际比,相差\n");
	//char szBuf[256] = {0};
	//for (int i=0;i<101;i++)
	//{
	//	int nSkip = 0;
	//	int nSend = 0;
	//	GetSpeedSkipFrame(i,nSkip,nSend);
	//	int nPer = 100*nSkip/(nSkip+nSend);
	//	sprintf_s(szBuf,sizeof(szBuf),"%d,%d,%d,%d,%d\n",i,nSkip,nSend,nPer,nPer-i);
	//	log.WriteString(szBuf);
	//}
	//
	return TRUE;  // return TRUE  unless you set the focus to a control

}

void CVideoNetDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 在此处添加消息处理程序代码

	// Initialize DIB for drawing...
	if (hdib)
	{
		::DrawDibClose(hdib);
	}
}



/**
*
*	Send the video frame to the remote host...
*
*/
void CVideoNetDlg::SendVideo(BYTE *data,int size,bool bIsCapture)
{

	if(vidcap->isOver==TRUE)
		return;

	if(data==NULL)
		return;

	// 如果是录屏幕,就不处理摄像
	if (!m_bOpenVideoCapture && bIsCapture)
	{
		return;
	}

	CRect wndrect;
	Bits bits;		// Various count about compression

	unsigned char * pSrcData = data;

	// 如果是YUY2的,换称成RBG24
	if (bIsCapture && vidcap->m_bmpinfo.bmiHeader.biCompression==0x32595559)
	{
		// YUV2RGB
		// pYUV			point to the YUV data
		// pRGB			point to the RGB data
		// width		width of the picture
		// height		height of the picture
		// alphaYUV		is there an alpha channel in YUV
		// alphaRGB		is there an alpha channel in RGB
		YUV2RGB(data, rgbBuf, IMAGE_WIDTH, IMAGE_HEIGHT, false, false);

		pSrcData = rgbBuf;

	}


	// Display the captured frame...
	if(isViewLocalVideo)
	{
		if(hdib!=NULL)
		{

			::DrawDibDraw(hdib,
				m_hdc,
				local_wnd_x,		// dest : left pos
				local_wnd_y,		// dest : top pos
				local_wnd_w,					 // don't zoom x
				local_wnd_h,					 // don't zoom y
				&m_bmpinfo->bmiHeader,			 // bmp header info
				pSrcData,					 // bmp data
				0,					 // src :left
				0,					 // src :top
				IMAGE_WIDTH,				 // src : width
				IMAGE_HEIGHT,				 // src : height
				DDF_SAME_DRAW			 // use prev params....
				);

		}


	}



	DWORD dwNowTick = (DWORD)::GetTickCount();

	if (m_bOpenSkipMode)
	{
		DWORD dwCoolTicks = m_bIsNetSpeed?80:60;	// 节省带宽,fps下来
		// 控制最快只能跑16fps
		if ( dwNowTick<m_LastGetFrameTicks+dwCoolTicks)
		{
			return;
		}

		m_LastGetFrameTicks = dwNowTick;
		//计算跳帧
		if (bIsSkiping || dwSendSpeed>MAX_SEND_SPEED||dwAVGSendSpeed>MAX_SEND_AVGSPEED)
		{
			nSkipIndex++;
			// 正在跳
			if (bIsSkiping)
			{
				// 如果速度下来了
				if(dwAVGSendSpeed<MIN_SEND_SPEED)
				{
					// 并走完一圈跳帧
					if (nSkipIndex>=nSendFrames)
					{

						const SVideoNetConfig &speedinf = GetSpeedConfig(dwAVGSendSpeed,m_bIsGoodPic);
						bIsSkiping = false;
						GetSpeedSkipFrame(speedinf.nSkipPer,nSkipFrames,nSendFrames);
						nSkipPerRate = speedinf.nSkipPer;
						m_nSendQ = speedinf.nQinter;
						m_nQIndex = speedinf.nIndex;
						nSkipIndex = 1;
						m_dwLastChangeQTick = dwNowTick;
					}
				}
				else
				{
					// 并走完一圈跳帧,并要冷却一秒
					if (nSkipIndex>=nSendFrames && dwNowTick>m_dwLastChangeQTick+1000)
					{
						const SVideoNetConfig &speednew = GetSpeedConfig(dwAVGSendSpeed,m_bIsGoodPic);
						// 策略改变
						if (speednew.nIndex!=m_nQIndex)
						{
							int nNewQIndex = speednew.nIndex;
							int nStepQ = speednew.nIndex-m_nQIndex;
							bool bChange = true;

							if ( nStepQ < 0 )
							{
								// 相差太大,要过渡
								if ( dwNowTick>m_dwLastChangeQTick+ 3000  ) 
								{
									nNewQIndex = m_nQIndex - 1;
								}
								else
								{
									bChange = false;
								}
							}

							if ( bChange )
							{
								const SVideoNetConfig &speedinf = GetSpeedConfigByID(nNewQIndex,m_bIsGoodPic);
								GetSpeedSkipFrame(speedinf.nSkipPer,nSkipFrames,nSendFrames);
								nSkipPerRate = speedinf.nSkipPer;
								m_nSendQ = speedinf.nQinter;
								m_nQIndex = speedinf.nIndex;
								nSkipIndex = -1*nSkipFrames;
								m_dwLastChangeQTick = dwNowTick;
							}
						}
					}
				}
			}
			else
			{
				// 如没没跳,超速度,就启动
				if (dwAVGSendSpeed>MAX_SEND_AVGSPEED|| dwSendSpeed>MAX_SEND_SPEED)
				{
					bIsSkiping = true;
					const SVideoNetConfig &speedinf = GetSpeedConfig(dwAVGSendSpeed,m_bIsGoodPic);
					GetSpeedSkipFrame(speedinf.nSkipPer,nSkipFrames,nSendFrames);
					nSkipPerRate = speedinf.nSkipPer;
					m_nSendQ = speedinf.nQinter;
					m_nQIndex = speedinf.nIndex;
					nSkipIndex = -1*nSkipFrames;
					m_dwLastChangeQTick = dwNowTick;
				}
			}

			// <0表示要跳
			if (nSkipIndex<=0)
			{
				return;
			}
			else if(nSkipIndex>=nSendFrames)
			{
				// 走完一圈跳帧回来再跳
				nSkipIndex = -1*nSkipFrames;
			}
		}

	}
	else
	{
		m_nSendQ = 8;
		nSkipPerRate = 0;
	}


	if(isVideoSend)
	{

		//log.WriteString("\n Converting to YUV format..");

		memset(yuv,0,sizeof(yuv));

		//Convert the data from rgb24 format to YUV format	
		//ConvertRGB2YUV(IMAGE_WIDTH,IMAGE_HEIGHT,pSrcData,yuv);



		// 直接转 YUV
		//CYuv_RGB::getYUV420DataFromRGBData((unsigned char*)yuv,pSrcData,352,288);
		//ConvertRGB2YUV(IMAGE_WIDTH,IMAGE_HEIGHT,pSrcData,yuv);

		unsigned long len;
		CYuv_RGB::getYUV420DataFromRGBData((unsigned char*)yuv,pSrcData,IMAGE_WIDTH,IMAGE_HEIGHT); 

		memset(cdata,0,sizeof(cdata));
		m_x264Encoder.Encode((char*)yuv,(char*)cdata,len);
		
		// Reset the counter
		count=0;


		int nSendQ = m_bIsNetSpeed?((int)m_nSendQ*1.5):m_nSendQ;	// 节省带宽,压缩高点

		//Compress the data...to h263
		cparams.format= CPARAM_QCIF;//CPARAM_QCIF;
		cparams.inter = CPARAM_INTER;//CPARAM_INTER;//CPARAM_INTRA;  
		cparams.Q_intra = 8;
		cparams.Q_inter = nSendQ;
		cparams.data=yuv;		//  Data in YUV format...

		// 每5秒发一个无前后动态的,防中途看时老花屏
		if (dwNowTick>m_dwLastRestTick+10000)
		{
			m_dwLastRestTick = dwNowTick;
			cparams.inter = CPARAM_INTRA;
		}
		//CompressFrame(&cparams, &bits);

		// Transmit the compressed frame
		//log.WriteString("Transmitting the frame");
		//dvideo.SendVideoData(cdata,count);
		//memcpy(cdata,yuv,sizeof(yuv));
		DisplayRemoteFrame(cdata,len);
		

		DWORD dwNowTick = (DWORD)time(NULL);//::GetTickCount();
		dwAllSpeed += len;
		dwAllFrames++;

		if (dwNowTick>dwLastTick)
		{
			dwSendSpeed = dwAllSpeed;
			dwFramesPer = dwAllFrames;
			dwLastTick = dwNowTick;
			dwAllSpeed = 0;
			dwAllFrames = 0;
			dwAVGSendSpeed = dwAVGSendSpeed*0.6+dwSendSpeed*0.4;	//平滑速度
		}

		char szBuf[512] = {0};
		sprintf_s(szBuf,sizeof(szBuf),"发:%.2f|%.2f KB/S %dfps %d%% I:%d Q:%d",(float)dwAVGSendSpeed/1000.0,(float)dwSendSpeed/1000.0,dwFramesPer,nSkipPerRate,m_nQIndex,nSendQ);
		GetDlgItem(IDC_STATIC_SPEED)->SetWindowText(szBuf);

		//DisplayRemoteFrame(cdata,count);


	}

}



/**
*	  Display frame got from remote host
*/
void CVideoNetDlg::DisplayRemoteFrame(unsigned char *data,int size)
{
int retvalue;

	// If remote view disabled then return...
	if(!isViewRemoteVideo)
		return;

	// If video reception disabled..then return..
	if(!isVideoReceive)
		return;

	DWORD dwNowTick = (DWORD)time(NULL);//::GetTickCount();
	dwAllRecvSpeed += size;
	dwAllRecvFrames++;
	if (dwNowTick>dwRecvLastTick)
	{
		dwRecvSpeed = dwAllRecvSpeed;
		dwRecvFramesPer = dwAllRecvFrames;
		dwRecvLastTick = dwNowTick;
		dwAllRecvSpeed = 0;
		dwAllRecvFrames = 0;
	}

	char szBuf[512] = {0};
	sprintf_s(szBuf,sizeof(szBuf),"收:%.2f KB/S %dfps",(float)dwRecvSpeed/1000.0,dwRecvFramesPer);
	GetDlgItem(IDC_STATIC_SPEED2)->SetWindowText(szBuf);

	//memcpy(rgbdata,data,IMAGE_WIDTH*IMAGE_HEIGHT*3);
	//retvalue=DecompressFrame(data,size,rgbdata,buffersize);
	//retvalue = DecompressFrameWithoutDeCode(data,size,rgbdata,buffersize);
	memset((char*)yuv,0,sizeof(yuv));
	m_x264Decoder.decode((char*)data,size,(char*)yuv);
	unsigned long len;
	//CYuv_RGB::YUV2RGB((unsigned char*)yuv,IMAGE_WIDTH,IMAGE_HEIGHT,rgbdata,&len);
	CYuv_RGB::getRgb24DataFromYUV420Data((unsigned char*)yuv, rgbdata, IMAGE_WIDTH, IMAGE_HEIGHT);

	//log.WriteString("\n Decompression succeeded");

	if (hdib)
	{
		::DrawDibDraw(hdib,
			m_hdc,
			remote_wnd_x,		// dest : left pos
			remote_wnd_y,		// dest : top pos
			remote_wnd_w,					 // don't zoom x
			remote_wnd_h,					 // don't zoom y
			&m_bmpinfo->bmiHeader,
			rgbdata,					 // bmp data
			0,					 // src :left
			0,					 // src :top
			IMAGE_WIDTH,				 // src : width
			IMAGE_HEIGHT,				 // src : height
			DDF_SAME_DRAW			 // use prev params....
			);

	}
}






void CVideoNetDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}






//  If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CVideoNetDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}






HCURSOR CVideoNetDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


void CVideoNetDlg::OnTimer(UINT_PTR nIDEvent)
{

	CDialog::OnTimer(nIDEvent);

	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent==1)
	{
		OnTimeSnapScreen();
	}

}

void CVideoNetDlg::OnTimeSnapScreen0()
{


	if (m_bOpenVideoCapture)
	{
		return;
	}
		if (NULL==m_pDeskTopDC)
	{
		return;
	}

	DWORD pid = ::GetCurrentThreadId();

	// 当前位
	POINT ptMouseNow;
	GetCursorPos(&ptMouseNow);

	// 一秒才更新位置
	DWORD dwNowTimes = (DWORD)time(NULL);
	if (dwNowTimes!=m_dwLastMouseTime)
	{
		m_dwLastMouseTime = dwNowTimes;
		m_ptLastMouse = ptMouseNow;
	}

	// 动了就抓
	if ( abs(m_ptLastMouse.x-ptMouseNow.x)>1 ||  abs(m_ptLastMouse.y-ptMouseNow.y)>1 )
	{
		return;
	}

	POINT ptMouse = m_ptLastMouse;

	//获取桌面窗口句柄
	HWND hwndDesk = ::GetDesktopWindow();

	int Width = IMAGE_WIDTH;
	int Height = IMAGE_HEIGHT;

	::ScreenToClient(hwndDesk,&ptMouse);
	POINT ptCenter;
	ptCenter.x = ptMouse.x-IMAGE_WIDTH/2;
	ptCenter.y = ptMouse.y-IMAGE_HEIGHT/2;

	//内存DC
	HDC memdc = ::CreateCompatibleDC(m_hdcDeskTop);

	//建立和屏幕兼容的bitmap
	HBITMAP memBitmap = ::CreateCompatibleBitmap(m_hdcDeskTop, Width, Height);

	HBITMAP oldmemBitmap = (HBITMAP)::SelectObject(memdc,memBitmap);//将memBitmap选入内存DC
	::BitBlt(memdc,0, 0, Width, Height, m_hdcDeskTop, ptCenter.x, ptCenter.y, SRCCOPY);//复制屏幕图像到内存DC


	//以下代码保存memDC中的位图到文件
	BITMAP bmp;
	//获得位图信息
	::GetObject(memBitmap,sizeof(bmp),&bmp);  

	BITMAPINFOHEADER bih = {0};				//位图信息头
	bih.biBitCount		= 24;				// bmp.bmBitsPixel;//每个像素字节大小
	bih.biCompression	= BI_RGB;
	bih.biHeight		= bmp.bmHeight;		//高度
	bih.biPlanes		= 1;
	bih.biSize			= sizeof(BITMAPINFOHEADER);
	bih.biSizeImage		= bmp.bmWidthBytes * bmp.bmHeight;//图像数据大小
	bih.biWidth			= bmp.bmWidth;		//宽度

	int nMemoryBmpSize = bmp.bmWidthBytes * bmp.bmHeight;

	BYTE * p = new BYTE[nMemoryBmpSize*2];//申请内存保存位图数据

	::GetDIBits(memdc, memBitmap, 0, Height, p, (LPBITMAPINFO) &bih, DIB_RGB_COLORS);	//获取位图数据

	SendVideo(p,nMemoryBmpSize,false);

	delete [] p;
	::SelectObject(memdc,oldmemBitmap);

	//绘图完成后的清理 
	::DeleteObject(memBitmap); 

	::DeleteDC(memdc);
}


void CVideoNetDlg::OnTimeSnapScreen()
{


	if (m_bOpenVideoCapture)
	{
		return;
	}
	if (NULL==m_pDeskTopDC)
	{
		return;
	}

	DWORD pid = ::GetCurrentProcessId();

	// 当前位
	POINT ptMouseNow;
	GetCursorPos(&ptMouseNow);

	// 一秒才更新位置
	DWORD dwNowTimes = (DWORD)time(NULL);
	if (dwNowTimes!=m_dwLastMouseTime)
	{
		m_dwLastMouseTime = dwNowTimes;
		m_ptLastMouse = ptMouseNow;
	}

	// 动了就抓
	if ( abs(m_ptLastMouse.x-ptMouseNow.x)>1 ||  abs(m_ptLastMouse.y-ptMouseNow.y)>1 )
	{
		return;
	}

	POINT ptMouse = m_ptLastMouse;


	//屏幕DC
	CWnd* pDesktopWin = this->GetDesktopWindow(); 
	int Width = IMAGE_WIDTH;
	int Height = IMAGE_HEIGHT;

	pDesktopWin->ScreenToClient(&ptMouse);
	POINT ptCenter;
	ptCenter.x = ptMouse.x-IMAGE_WIDTH/2;
	ptCenter.y = ptMouse.y-IMAGE_HEIGHT/2;

	CDC memDC;//内存DC
	memDC.CreateCompatibleDC(m_pDeskTopDC);

	CBitmap memBitmap;	//建立和屏幕兼容的bitmap
	memBitmap.CreateCompatibleBitmap(m_pDeskTopDC, Width, Height);

	CBitmap* oldmemBitmap = memDC.SelectObject(&memBitmap);//将memBitmap选入内存DC
	memDC.BitBlt(0, 0, Width, Height, m_pDeskTopDC, ptCenter.x, ptCenter.y, SRCCOPY);//复制屏幕图像到内存DC


	//// 创建画笔
	//CPen Pen( PS_SOLID,1, RGB( 255, 0, 0 ) );
	//// 选取新的画笔,并保存旧的画笔,画完时恢复
	//CPen *pOldPen = memDC.SelectObject( &Pen );

	//// 绘制线段用于标识鼠标当前位置
	//POINT ptStart;
	//POINT ptEnd;

	//// 画垂直线
	//ptStart.x	= IMAGE_WIDTH/2-7;
	//ptStart.y	= IMAGE_HEIGHT/2+1;
	//ptEnd.x	= IMAGE_WIDTH/2+9;
	//ptEnd.y = IMAGE_HEIGHT/2+1;
	//memDC.MoveTo(ptStart);
	//memDC.LineTo(ptEnd);

	//// 画水平线
	//ptStart.x = IMAGE_WIDTH/2+1;
	//ptStart.y = IMAGE_HEIGHT/2-7;
	//ptEnd.x = IMAGE_WIDTH/2+1;
	//ptEnd.y = IMAGE_HEIGHT/2+10;
	//memDC.MoveTo(ptStart);
	//memDC.LineTo(ptEnd);

	//memDC.SelectObject( pOldPen );
	//Pen.DeleteObject();


	//以下代码保存memDC中的位图到文件
	BITMAP bmp;
	memBitmap.GetBitmap(&bmp);//获得位图信息

	BITMAPINFOHEADER bih = {0};//位图信息头
	bih.biBitCount =24;// bmp.bmBitsPixel;//每个像素字节大小

	bih.biCompression = BI_RGB;
	bih.biHeight = bmp.bmHeight;//高度

	bih.biPlanes = 1;
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biSizeImage = bmp.bmWidthBytes * bmp.bmHeight;//图像数据大小
	bih.biWidth = bmp.bmWidth;//宽度

	int nMemoryBmpSize = bmp.bmWidthBytes * bmp.bmHeight;

	BYTE * p = new BYTE[nMemoryBmpSize*2];//申请内存保存位图数据

	GetDIBits(memDC.m_hDC, (HBITMAP) memBitmap.m_hObject, 0, Height, p, (LPBITMAPINFO) &bih, DIB_RGB_COLORS);//获取位图数据

	SendVideo(p,nMemoryBmpSize,false);

	delete [] p;
	memDC.SelectObject(oldmemBitmap);

	//绘图完成后的清理 
	memBitmap.DeleteObject(); 

	memDC.DeleteDC();
}


void CVideoNetDlg::OnCancel() 
{
	
	// If conference is going on...then 
	// it must be destroyed...
	if(isStarted)
	DestroyConference();

	// Close the control session
	dcontrol.CloseSocket();

	// End recording and player thread

	record->PostThreadMessage(WM_RECORDSOUND_ENDTHREAD,0,0);
	
	play->PostThreadMessage(WM_PLAYSOUND_ENDTHREAD,0,0);

	// Close the video capture device
	vidcap->Destroy();

		
	// Close graphics....
	if(hdib!=NULL)
	{
	::DrawDibEnd(hdib);
	::DrawDibClose(hdib);
	}
	
	// Close the encoder
	if(isVideo)
	ExitH263Encoder(&cparams);

	// close decoder ......
	ExitH263Decoder();
	

	CDialog::OnCancel();
}



/**
*	  Connect to remote host
*	  Invoked when user presses connect menu item
*	  Displays the connect dialog box
*/
void CVideoNetDlg::OnConnect()
{

	// Show the connect dialog box
	connectdlg->DoModal();

}




/**
*	  Disconnect the remote host
*	  Invoked when user presses disconnect menu item
*	  Stops the current conference
*/
void CVideoNetDlg::OnDisconnect()
{
	// Send notification to the remote host...
	dcontrol.SendControlMessage(MESG_DISCONNECT,NULL);

	DestroyConference();

}



/**
*	  Show / Hide the local video screen
*/
void CVideoNetDlg::OnViewLocal()
{

	CMenu *menu=this->GetMenu();
	
	if(isViewLocalVideo)
	{
	isViewLocalVideo=FALSE;
	menu->CheckMenuItem(VIEW_LOCAL,MF_BYCOMMAND | MF_UNCHECKED);
	}
	else
	{
	isViewLocalVideo=TRUE;
	menu->CheckMenuItem(VIEW_LOCAL,MF_BYCOMMAND | MF_CHECKED);
	}



}




/**
*	  Show / Hide the remote video screen
*/
void CVideoNetDlg::OnViewRemote()
{

	CMenu *menu=this->GetMenu();
	
	if(isViewRemoteVideo)
	{
	isViewRemoteVideo=FALSE;
	menu->CheckMenuItem(VIEW_REMOTE,MF_BYCOMMAND | MF_UNCHECKED);
	}
	else
	{
	isViewRemoteVideo=TRUE;
	menu->CheckMenuItem(VIEW_REMOTE,MF_BYCOMMAND | MF_CHECKED);
	}





}
	

	
/**
*	  Displays the volume control dialog box
*/
void CVideoNetDlg::OnVolumeControl()
{
	
	Volume vol(IDD_DIALOG3);

 vol.DoModal();


}




/**
*	  Enables/Disables transmission of audio
*     Invoked when user presses Audio -> Send menu item
*/
void CVideoNetDlg::OnAudioSend()
{

	if(isAudioSend)
	{
		// Stop recording audio
		record->SuspendThread();
		//>PostThreadMessage(WM_RECORDSOUND_STOPRECORDING,0,0);
		isAudioSend=FALSE;
		menu->CheckMenuItem(AUDIO_SEND,MF_BYCOMMAND | MF_UNCHECKED);

	}
	else
	{
		// Start recording audio
		record->ResumeThread();
		//record->PostThreadMessage(WM_RECORDSOUND_STARTRECORDING,0,0);
		isAudioSend=TRUE;
		menu->CheckMenuItem(AUDIO_SEND,MF_BYCOMMAND | MF_CHECKED);
	}
}



/**
*	  Enables/Disables reception of audio
*     Invoked when user presses Audio -> Receive menu item
*/
void CVideoNetDlg::OnAudioReceive()
{


	if(isAudioReceive)
	{
		isAudioReceive=FALSE;
		menu->CheckMenuItem(AUDIO_RECEIVE,MF_BYCOMMAND | MF_UNCHECKED);
		
	
	}
	else
	{
		isAudioReceive=TRUE;
		menu->CheckMenuItem(AUDIO_RECEIVE,MF_BYCOMMAND | MF_CHECKED);
	}





}
	

/**
*	  Enables/Disables transmission of video
*     Invoked when user presses Video-> Send menu item
*/
void CVideoNetDlg::OnVideoSend()
{

	if(isVideoSend)
	{
		isVideoSend=FALSE;
		menu->CheckMenuItem(VIDEO_SEND,MF_BYCOMMAND | MF_UNCHECKED);
	
	}
	else
	{
		isVideoSend=TRUE;
		menu->CheckMenuItem(VIDEO_SEND,MF_BYCOMMAND | MF_CHECKED);
	}




}




/**
*	  Enables/Disables reception of video
*     Invoked when user presses Video -> Receive menu item
*/
void CVideoNetDlg::OnVideoReceive()
{

	if(isVideoReceive)
	{
		isVideoReceive=FALSE;
		menu->CheckMenuItem(VIDEO_RECEIVE,MF_BYCOMMAND | MF_UNCHECKED);

	
	}
	else
	{
		isVideoReceive=TRUE;
		menu->CheckMenuItem(VIDEO_RECEIVE,MF_BYCOMMAND | MF_CHECKED);
	}



}
	


/**
*	  Display the about dialog box
*/
void CVideoNetDlg::OnAbout()
{

		CAboutDlg dlgAbout;
		dlgAbout.DoModal();

}




/**
*	  Send the message to remote host
*/
void CVideoNetDlg::OnSendMesg()
{
unsigned char data[500];

	GetDlgItemText(IDC_EDIT1,(char*)data,500);
	
	
	if(strlen((const char*)data)>0)
	{
	dtext.SendTextData(data,strlen((const char*)data));
	SetDlgItemText(IDC_EDIT1,"");
	}
		
}




/**
*	  Display mesg received from the remote host
*/
void CVideoNetDlg::DisplayMesg(char *mesg)
{

	list->AddString(mesg);
	// Highlight the currently added string
	// This will scroll the window automatically
	list->SetCurSel(list->GetCount()-1);

	
}






/**
*	  Start the conference ....
*
*/
void CVideoNetDlg::StartConference()
{
	
	// If already started....then return
	if(isStarted)
	return;

	isStarted=TRUE;
	isViewLocalVideo=TRUE;
	isViewRemoteVideo=TRUE;
	
	// Enable / Disable menu items...
	menu->EnableMenuItem(CONF_CONNECT,MF_DISABLED | MF_GRAYED);
	menu->EnableMenuItem(CONF_DISCONNECT,MF_ENABLED);
	
	menu->EnableMenuItem(AUDIO_SEND,MF_ENABLED);
	menu->EnableMenuItem(AUDIO_RECEIVE,MF_ENABLED);
	
	menu->EnableMenuItem(VIDEO_SEND,MF_ENABLED);
	menu->EnableMenuItem(VIDEO_RECEIVE,MF_ENABLED);

	menu->EnableMenuItem(VIEW_LOCAL,MF_ENABLED);
	menu->EnableMenuItem(VIEW_REMOTE,MF_ENABLED);

	menu->CheckMenuItem(VIEW_LOCAL,MF_BYCOMMAND | MF_CHECKED);
	menu->CheckMenuItem(VIEW_REMOTE,MF_BYCOMMAND | MF_CHECKED);

	// Start text Session
	dtext.CreateSocket(PORT_TEXT,TYPE_TEXT);

	// Enable the send button
	bsend->EnableWindow(TRUE);


	// Start audio Session
	daudio.CreateSocket(PORT_AUDIO,TYPE_AUDIO);


	// Start video Session
	dvideo.CreateSocket(PORT_VIDEO,TYPE_VIDEO);


	// Start recording audio
	record->PostThreadMessage(WM_RECORDSOUND_STARTRECORDING,0,0);

	
	// Start playing audio
	play->PostThreadMessage(WM_PLAYSOUND_STARTPLAYING,0,0);



	// Start capturing video
	if(isVideo)
	{
	
		if(vidcap->StartCapture()==FALSE)
		{
		MessageBox("Unable to start the capture");
		isCaptureStarted=FALSE;
		}
		else
		{
		isCaptureStarted=TRUE;
		}

	}

}




/**
*	  Destroy the current conference
*/
void CVideoNetDlg::DestroyConference()
{

	// If not started / already destroyed..then return;
	if(isStarted==FALSE)
	return;

	isStarted=FALSE;
	isViewLocalVideo=FALSE;
	isViewRemoteVideo=FALSE;


	// Enable/disable menu items
	menu->EnableMenuItem(CONF_CONNECT,MF_ENABLED);
	menu->EnableMenuItem(CONF_DISCONNECT,MF_DISABLED | MF_GRAYED);


	// Disable Text session
	dtext.CloseSocket();

	// Disable the send button...
	bsend->EnableWindow(FALSE);

	// Clear the contents of text chat windows
	SetDlgItemText(IDC_EDIT1,"");
	list->ResetContent();

		
	// If recording is going on ...then stop it
	if(isAudioSend)
	record->SuspendThread();

	record->PostThreadMessage(WM_RECORDSOUND_STOPRECORDING,0,0);	 

	// stop audio session
	daudio.CloseSocket();

	// stop playing audio
	play->PostThreadMessage(WM_PLAYSOUND_STOPPLAYING,0,0);


	// stop capturing video
	if(isVideo && isCaptureStarted)
	vidcap->StopCapture();	
	
	// stop video session
	dvideo.CloseSocket();

	// Update local and remote viewer..
	CWnd *wnd=(CWnd*)this->GetDlgItem(IDC_LOCALVIDEO);
    wnd->Invalidate(TRUE);

	wnd=(CWnd*)this->GetDlgItem(IDC_REMOTEVIDEO);
    wnd->Invalidate(TRUE);

	// Reset the status..
	record->ResumeThread();

	isAudioSend=TRUE;
	menu->CheckMenuItem(AUDIO_SEND,MF_BYCOMMAND | MF_CHECKED);

	isAudioReceive=TRUE;
	menu->CheckMenuItem(AUDIO_RECEIVE,MF_BYCOMMAND | MF_CHECKED);
	
	isVideoSend=TRUE;
	menu->CheckMenuItem(VIDEO_SEND,MF_BYCOMMAND | MF_CHECKED);

	isVideoReceive=TRUE;
	menu->CheckMenuItem(VIDEO_RECEIVE,MF_BYCOMMAND | MF_CHECKED);

	m_bIsGoodPic = TRUE;
	menu->CheckMenuItem(ID_VIDEO_GOODPIC,MF_BYCOMMAND | MF_CHECKED);

	m_bIsNetSpeed = FALSE;
	menu->CheckMenuItem(ID_VIDEO_SPEED,MF_BYCOMMAND | MF_UNCHECKED);


	menu->EnableMenuItem(AUDIO_SEND,MF_GRAYED);
	menu->EnableMenuItem(AUDIO_RECEIVE,MF_GRAYED);
	
	menu->EnableMenuItem(VIDEO_SEND,MF_GRAYED);
	menu->EnableMenuItem(VIDEO_RECEIVE,MF_GRAYED);

	menu->EnableMenuItem(VIEW_LOCAL,MF_GRAYED);
	menu->EnableMenuItem(VIEW_REMOTE,MF_GRAYED);





}

void CVideoNetDlg::OnVideoGoodpic()
{
	// TODO: 在此添加命令处理程序代码


	if(m_bIsGoodPic)
	{
		m_bIsGoodPic=FALSE;
		menu->CheckMenuItem(ID_VIDEO_GOODPIC,MF_BYCOMMAND | MF_UNCHECKED);


	}
	else
	{
		m_bIsGoodPic=TRUE;
		menu->CheckMenuItem(ID_VIDEO_GOODPIC,MF_BYCOMMAND | MF_CHECKED);
	}

}

void CVideoNetDlg::OnVideoSpeed()
{
	// TODO: 在此添加命令处理程序代码
	if(m_bIsNetSpeed)
	{
		m_bIsNetSpeed=FALSE;
		menu->CheckMenuItem(ID_VIDEO_SPEED,MF_BYCOMMAND | MF_UNCHECKED);


	}
	else
	{
		m_bIsNetSpeed=TRUE;
		menu->CheckMenuItem(ID_VIDEO_SPEED,MF_BYCOMMAND | MF_CHECKED);
	}

}

void CVideoNetDlg::OnVideoSnap()
{
	// TODO: 在此添加命令处理程序代码
	if(m_bOpenVideoCapture)
	{
		m_bOpenVideoCapture=false;
		menu->CheckMenuItem(ID_VIDEO_SNAP,MF_BYCOMMAND | MF_CHECKED);


	}
	else
	{
		m_bOpenVideoCapture=true;
		menu->CheckMenuItem(ID_VIDEO_SNAP,MF_BYCOMMAND | MF_UNCHECKED);
	}
}

void CVideoNetDlg::OnVideoOpenskip()
{
	// TODO: 在此添加命令处理程序代码
	if(m_bOpenSkipMode)
	{
		m_bOpenSkipMode=false;
		menu->CheckMenuItem(ID_VIDEO_OPENSKIP,MF_BYCOMMAND | MF_UNCHECKED);
	}
	else
	{
		m_bOpenSkipMode=true;
		menu->CheckMenuItem(ID_VIDEO_OPENSKIP,MF_BYCOMMAND | MF_CHECKED);
	}

}
