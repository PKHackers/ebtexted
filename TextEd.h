// TextEd.h : main header file for the TEXTED application
//

#if !defined(AFX_TEXTED_H__B8E4E9BC_24C1_49DA_9FAF_B3EB8B3C912D__INCLUDED_)
#define AFX_TEXTED_H__B8E4E9BC_24C1_49DA_9FAF_B3EB8B3C912D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CTextEdApp:
// See TextEd.cpp for the implementation of this class
//

class CTextEdApp : public CWinApp
{
public:
	CTextEdApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTextEdApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTextEdApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXTED_H__B8E4E9BC_24C1_49DA_9FAF_B3EB8B3C912D__INCLUDED_)
