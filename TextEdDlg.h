// TextEdDlg.h : header file
//

#if !defined(AFX_TEXTEDDLG_H__65B9AC1A_6301_47C8_8B79_1278F5BC3660__INCLUDED_)
#define AFX_TEXTEDDLG_H__65B9AC1A_6301_47C8_8B79_1278F5BC3660__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "textstuff.h"


/////////////////////////////////////////////////////////////////////////////
// CTextEdDlg dialog

class CTextEdDlg : public CDialog
{
// Construction
public:
	void FillListBox(String strlist[], int nstr);
	void DisplayString(String s, int pos = -1, int maxpos = -1);
	void LoadFile(CString szFileName);
	CTextEdDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CTextEdDlg)
	enum { IDD = IDD_TEXTED_DIALOG };
	CListBox	m_ListBox;
	CTabCtrl	m_TabMenu;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTextEdDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CTextEdDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnFileOpen();
	afx_msg void OnSelectString();
	afx_msg void OnSelectTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnApply();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXTEDDLG_H__65B9AC1A_6301_47C8_8B79_1278F5BC3660__INCLUDED_)
