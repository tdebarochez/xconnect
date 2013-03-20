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

// XCONNECTCLIENTDlg.cpp : implementation file
//

#include "stdafx.h"
#include "XCONNECTCLIENT.h"
#include "XCONNECTCLIENTDlg.h"
#include "XConnectionTypes.h"
#include "SocketHandling.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CXConnectClientApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CXCONNECTCLIENTDlg dialog

CXCONNECTCLIENTDlg::CXCONNECTCLIENTDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CXCONNECTCLIENTDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CXCONNECTCLIENTDlg)
	m_Static1 = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	connected = FALSE;
}

void CXCONNECTCLIENTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CXCONNECTCLIENTDlg)
	DDX_Text(pDX, IDC_STATIC1, m_Static1);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CXCONNECTCLIENTDlg, CDialog)
	//{{AFX_MSG_MAP(CXCONNECTCLIENTDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_REGISTERED_MESSAGE(XC_CALL, OnFDSCall)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXCONNECTCLIENTDlg message handlers

BOOL CXCONNECTCLIENTDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	FILE* str = fopen("XCONNECTCLIENT.log", "w");
	fprintf(str, "XCONNECTCLIENT is running...\n");
	fclose(str);

	m_Static1 = "XCONNECTCLIENT is running...";
	UpdateData(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CXCONNECTCLIENTDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	CloseConnection();
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CXCONNECTCLIENTDlg::OnPaint() 
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

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CXCONNECTCLIENTDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

LRESULT CXCONNECTCLIENTDlg::OnFDSCall(WPARAM wParam, LPARAM lParam)
{
	if (!connected) {
		if (OpenConnection(theApp.getServerName().GetBuffer(0)))
			connected = TRUE;
	}
	XC_ACTION_READ_HDR* pHdrR = NULL;
	XC_ACTION_WRITE_HDR* pHdrW = NULL;
	XC_ACTION_READTOKEN_HDR* pHdrRT = NULL;
	XC_ACTION_WRITETOKEN_HDR* pHdrWT = NULL;

	ATOM atom = wParam;
	LRESULT result = XC_RETURN_FAILURE;
	char szName[MAX_PATH];

	GlobalGetAtomName(atom, szName, MAX_PATH);
	HANDLE hMap = OpenFileMapping(FILE_MAP_WRITE, FALSE, szName);
	if (hMap) {
		BYTE* pView = (BYTE*)MapViewOfFile(hMap, FILE_MAP_WRITE, 0, 0, 0);
		if (pView) {
			BYTE* pNext = pView;
			DWORD* pdw = (DWORD*)pView;
			while (*pdw) {
				switch (*pdw) {
					case XC_ACTION_READ:
					case XC_ACTION_READTRUE:
						pHdrR = (XC_ACTION_READ_HDR*)pdw;
						pNext += sizeof(XC_ACTION_READ_HDR);
						pNext += pHdrR->size;
						pdw = (DWORD*)pNext;
						break;
					case XC_ACTION_WRITE:
					case XC_ACTION_WRITETRUE:
						pHdrW = (XC_ACTION_WRITE_HDR*)pdw;
						pNext += sizeof(XC_ACTION_WRITE_HDR) + pHdrW->size;
						pdw = (DWORD*)pNext;
						break;
					default:
						*pdw = 0;
						break;
				}
			}
			pNext += 4;
			if (CallServer(pView, pNext-pView)) {
				result = XC_RETURN_SUCCESS;
				m_Static1 = "Communication with XConnectServer OK";
			}
			else {
				m_Static1 = "Communication with XConnectServer Failed!";
				if (connected) {
					CloseConnection();
					connected = FALSE;
				}
			}

			UnmapViewOfFile((LPVOID)pView);
			UpdateData(FALSE);
		}
	}

	return result;
}
