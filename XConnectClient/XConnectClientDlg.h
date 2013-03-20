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

// XCONNECTCLIENTDlg.h : header file
//

#if !defined(AFX_XCONNECTCLIENTDLG_H__6093697C_27CE_4BEF_B1F8_2159A138463F__INCLUDED_)
#define AFX_XCONNECTCLIENTDLG_H__6093697C_27CE_4BEF_B1F8_2159A138463F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CXCONNECTCLIENTDlg dialog

#define XC_CALL_CONNECTION			"FSASMLIB:IPC"

class CXCONNECTCLIENTDlg : public CDialog
{
// Construction
public:
	CXCONNECTCLIENTDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CXCONNECTCLIENTDlg)
	enum { IDD = IDD_XCONNECTCLIENT_DIALOG };
	CString	m_Static1;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXCONNECTCLIENTDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
	LRESULT OnFDSCall(WPARAM wParam, LPARAM lParam);
private:

	BOOL connected;

protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CXCONNECTCLIENTDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XCONNECTCLIENTDLG_H__6093697C_27CE_4BEF_B1F8_2159A138463F__INCLUDED_)
