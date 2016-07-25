// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__B0986308_5F44_11D6_8897_000B2B0F84B6__INCLUDED_)
#define AFX_STDAFX_H__B0986308_5F44_11D6_8897_000B2B0F84B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxsock.h>		// MFC socket extensions

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

// for Thread messages
#define ON_THREAD_MESSAGE_MY(message, memberFxn) \
	{ message, 0, 0, 0, AfxSig_vwl, \
	(AFX_PMSG)(AFX_PMSGT) \
	(static_cast< LRESULT (AFX_MSG_CALL CWinThread::*)(WPARAM, LPARAM) > \
	(memberFxn)) },


#endif // !defined(AFX_STDAFX_H__B0986308_5F44_11D6_8897_000B2B0F84B6__INCLUDED_)
