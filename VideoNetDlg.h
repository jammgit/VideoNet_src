// VideoNetDlg.h : header file
//

#if !defined(AFX_VIDEONETDLG_H__B0986306_5F44_11D6_8897_000B2B0F84B6__INCLUDED_)
#define AFX_VIDEONETDLG_H__B0986306_5F44_11D6_8897_000B2B0F84B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "DSocket.h"
#include "ConnectDlg.h"
#include "RecordSound.h"
#include "VideoCapture.h"
#include "PlaySound.h"
#include "Volume.h"
#include "Encoder/libr263.h"
#include "Decoder/tmndec.h"
#include "X264Encoder.h"
#include "X264Decoder.h"
#include "H264Encoder.h"


#define  MAX_SEND_SPEED 20000
#define  MAX_SEND_AVGSPEED 10000
#define  MIN_SEND_SPEED 5000

#define  MAX_SEND_COUNT 3000

#define  CHANGE_SPEED_COOLTIME 3000

#define  SEND_CONFIG_MAX 10
#define  FSKIP_CONFIG_MAX 2
#define  SKIP_FRAME_MAX 100

// 频道配置结构
struct SVideoNetConfig
{
	int     nIndex;
	DWORD	dwSpeed;
	int		nSkipPer;
	int     nQinter;
};

const SVideoNetConfig & GetSpeedConfig(DWORD dwSpeed,bool bIsGoodPic=true);
const SVideoNetConfig & GetSpeedConfigByID(int nID,bool bIsGoodPic=true);
void GetSpeedSkipFrame(int nPer,int &nSkip,int &nSend,bool bUseCache=true);

class CVideoNetDlg : public CDialog
{

	// Attributes
	public:
	
	CX264Encoder m_x264Encoder;
	CX264Decoder m_x264Decoder;
//	CH264Encoder m_h264Encoder;

	VideoCapture *vidcap;
	BOOL isVideo , isAudio , isStarted , isCaptureStarted;
	BOOL isViewLocalVideo,isViewRemoteVideo;
	BOOL isAudioSend,isAudioReceive;
	BOOL isVideoSend,isVideoReceive;
	BOOL m_bIsGoodPic;
	BOOL m_bIsNetSpeed;

	PBITMAPINFO m_bmpinfo;
	BITMAPINFO compbmp;
	RecordSound *record;
	PlaySound1 *play;
	HDC m_hdc;

	CMenu *menu;
	CButton *bsend;
	CListBox *list;
	CStdioFile log;
	CDC * m_pDeskTopDC; 

	//获取桌面窗口DC
	HDC m_hdcDeskTop;


	HICON m_hIcon;
	HWND hwnd;

	ConnectDlg *connectdlg;	
	
	DSocket dcontrol,daudio,dvideo,dtext;

	HDRAWDIB hdib;
	int local_wnd_x , local_wnd_y, local_wnd_w, local_wnd_h;
	int remote_wnd_x , remote_wnd_y,remote_wnd_w,remote_wnd_h;


	// Compression parameters....
	CParam cparams;
	
	// Buffer for storing YUV data....
	unsigned int yuv[ QCIF_WIDTH*QCIF_HEIGHT*2 ];
	unsigned char rgbBuf[ QCIF_WIDTH*QCIF_HEIGHT*5 ];
	unsigned char rgbBuf1[ QCIF_WIDTH*QCIF_HEIGHT*5 ];

	DWORD m_dwLastChangeQTick;
	int  m_nSendQ;
	int  m_nQIndex;
	DWORD dwFramesPer;
	DWORD dwSendSpeed;
	DWORD dwAVGSendSpeed;
	int nSkipFrames;
	int nSendFrames;
	int nSkipIndex;
	int nSkipPerRate;
	bool bIsSkiping;

	int dwAllSpeed;
	int dwAllFrames;
	DWORD dwLastTick;
	DWORD m_LastGetFrameTicks;

	DWORD dwRecvSpeed;
	DWORD dwRecvFramesPer;
	int dwAllRecvSpeed;
	int dwAllRecvFrames;
	DWORD dwRecvLastTick;

	DWORD m_dwLastRestTick;
	bool m_bOpenSkipMode;
	bool m_bOpenVideoCapture;

	POINT m_ptLastMouse;
	DWORD m_dwLastMouseTime;


	
// Construction
public:
	CVideoNetDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CVideoNetDlg)
	enum { IDD = IDD_VIDEONET_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVideoNetDlg)
public:	
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation




	// Generated message map functions
	//{{AFX_MSG(CVideoNetDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual void OnCancel();
	
	// Conference menu handlers
	void OnConnect();
	void OnDisconnect();
	
	// View menu handlers
	void OnViewLocal();
	void OnViewRemote();
	
	
	//Audio menu handlers
	void OnVolumeControl();
	void OnAudioSend();
	void OnAudioReceive();
	

	// Video menu handlers
	void OnVideoSend();
	void OnVideoReceive();
	

	// Help menu handlers
	void OnAbout();
		
	void StartConference();
	
	void DestroyConference();

	void OnSendMesg();

	void DisplayMesg(char *str);

	void SendVideo(BYTE *data,int size,bool bIsCapture=true);

	void DisplayRemoteFrame(unsigned char *data,int size);

	void OnTimeSnapScreen0();
	void OnTimeSnapScreen();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	afx_msg void OnDestroy();
	afx_msg void OnVideoGoodpic();
	afx_msg void OnVideoSpeed();
	afx_msg void OnVideoSnap();
	afx_msg void OnVideoOpenskip();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIDEONETDLG_H__B0986306_5F44_11D6_8897_000B2B0F84B6__INCLUDED_)
