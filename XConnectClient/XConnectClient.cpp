/*
This program is free software: you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free
Software Foundation, either version 3 of the License, or (at your option) any
later version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
details.

You should have received a copy of the GNU Lesser General Public License along
with this program. If not, see <http://www.gnu.org/licenses/>.
*/

// XCONNECTCLIENT.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "XConnectClient.h"
#include "XConnectClientDlg.h"
#include "parse.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

UINT XC_CALL = 0;

/////////////////////////////////////////////////////////////////////////////
// CXConnectClientApp

BEGIN_MESSAGE_MAP(CXConnectClientApp, CWinApp)
	//{{AFX_MSG_MAP(CXConnectClientApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXConnectClientApp construction

CXConnectClientApp::CXConnectClientApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CXConnectClientApp object

CXConnectClientApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CXConnectClientApp initialization

BOOL CXConnectClientApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	FILE* str = fopen("XConnectClient.out", "w");
	if (str) {
		fprintf(str, "XCONNECTCLIENT is running...\n\n");
		fclose(str);
	}

	if (!openIniFile()) {
		AfxMessageBox("Error opening XConnectClient.ini");
		return FALSE;
	}

	XC_CALL = RegisterWindowMessage(XC_CALL_CONNECTION);

	WNDCLASS wc;

	::GetClassInfo(AfxGetInstanceHandle(), "#32770", &wc);

	wc.lpszClassName = "FS98MAIN";

	AfxRegisterClass(&wc);

	CXCONNECTCLIENTDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

BOOL CXConnectClientApp::openIniFile()
{
	FILE* str = fopen("XConnectClient.ini", "r");

	if (!str)
		return FALSE;

	ReadToken(str);
	if (!TokenIs("ServerName")) {
		fclose(str);
		return FALSE;
	}
	ReadToken(str);
	m_ServerName = GetTokenBuffer();

	fclose(str);
	return TRUE;
}
