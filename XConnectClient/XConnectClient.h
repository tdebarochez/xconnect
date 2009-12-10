// XCONNECTCLIENT.h : main header file for the XCONNECTCLIENT application
//

#if !defined(AFX_XCONNECTCLIENT_H__B0627AF7_6782_4204_9FB2_E510F4C2B549__INCLUDED_)
#define AFX_XCONNECTCLIENT_H__B0627AF7_6782_4204_9FB2_E510F4C2B549__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CXConnectClientApp:
// See XCONNECTCLIENT.cpp for the implementation of this class
//

class CXConnectClientApp : public CWinApp
{
public:
	CXConnectClientApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXConnectClientApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	BOOL openIniFile();

	CString getServerName() { return m_ServerName; };

private:
	CString m_ServerName;

	//{{AFX_MSG(CXConnectClientApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XCONNECTCLIENT_H__B0627AF7_6782_4204_9FB2_E510F4C2B549__INCLUDED_)
