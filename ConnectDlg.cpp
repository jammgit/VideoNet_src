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
//    Name    : ConnectDlg.cpp
//    Details : Dialog box for connecting to remote host.
//
//
/////////////////////////////////////////////////////////////////////////////



#include "stdafx.h"
#include "VideoNet.h"
#include "Connectdlg.h"
#include "VideoNetDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



ConnectDlg::ConnectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ConnectDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(ConnectDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void ConnectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ConnectDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ConnectDlg, CDialog)
	//{{AFX_MSG_MAP(ConnectDlg)
	ON_BN_CLICKED(IDC_BUTTON1, OnConnect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()




/**
*    Connect to remote host
*	 It sends the connect notification to remote host	
*
*/
void ConnectDlg::OnConnect() 
{
char hostname[200];


this->GetDlgItemText(IDC_EDIT1,hostname,200);
this->OnCancel();

((CVideoNetDlg*)mdlg)->dcontrol.SendControlMessage(MESG_CONNECT,hostname);

}



/**
*    Set the reference of main dialog 
*
*/
void ConnectDlg::SetParent(CDialog *dlg) 
{

mdlg=dlg;

}




/**
*    Close the connect dialog box
*
*/
void ConnectDlg::OnCancel() 
{
	

	this->SetDlgItemText(IDC_EDIT1,"");
	
	CDialog::OnCancel();
}

BOOL ConnectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	this->SetDlgItemText(IDC_EDIT1,"127.0.0.1");

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
