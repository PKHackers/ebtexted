// TextEdDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TextEd.h"
#include "TextEdDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define TPT 0
#define RAW 1
#define EXP 2


// global properties

char* comprTable[768];	// compression string table

CCNode ccTable;	// initial node

// pointer table text
String tpttext[20000];
int num_tpt;

// raw text
String rawtext[20000];
int num_raw;

String exptext[20000];
int num_exp;

String *strlist;
int curlist = TPT;
int curstring = 0;


FILE* RomFile;

CString FileName;



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
// CTextEdDlg dialog

CTextEdDlg::CTextEdDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTextEdDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTextEdDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTextEdDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTextEdDlg)
	DDX_Control(pDX, IDC_TEXTLIST, m_ListBox);
	DDX_Control(pDX, IDC_TAB, m_TabMenu);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTextEdDlg, CDialog)
	//{{AFX_MSG_MAP(CTextEdDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(ID_FILE_OPENROM, OnFileOpen)
	ON_LBN_SELCHANGE(IDC_TEXTLIST, OnSelectString)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, OnSelectTab)
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTextEdDlg message handlers

BOOL CTextEdDlg::OnInitDialog()
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	CreateCCTable(&ccTable);

	m_TabMenu.InsertItem(0, "Text Pointer Table");
	m_TabMenu.InsertItem(1, "Raw Text");
	m_TabMenu.InsertItem(2, "Expanded Area");

	m_ListBox.SetItemHeight(0, 16);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTextEdDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTextEdDlg::OnPaint() 
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
HCURSOR CTextEdDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}



void CTextEdDlg::OnFileOpen() 
{
	// Display OpenFileName dialog, get filename,
	// Call LoadFile to load data
	OPENFILENAME ofn;       // common dialog box structure
	char szFile[260];       // buffer for filename

	szFile[0] = NULL;

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "SNES ROM files (*.smc)\0*.SMC\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	CFileDialog dlg(TRUE, "smc", NULL, OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST, "SNES ROM files (*.smc)\0*.SMC\0", this);

	dlg.m_ofn = ofn;

	// Display the Open dialog box
	int nResult = dlg.DoModal();

	if(nResult == IDOK) {
		if(CheckFile(szFile)) {
			LoadFile(szFile);
			//theApp.AddToRecentFileList(szFile);
			FileName = szFile;
		}
	}	
}



void CTextEdDlg::OnSelectTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	int sel = m_TabMenu.GetCurSel();

	switch(sel)
	{
	case TPT:
		curlist = TPT;
		FillListBox(tpttext, num_tpt);
		break;
	case RAW:
		curlist = RAW;
		FillListBox(rawtext, num_raw);
		break;
	case EXP:
		curlist = EXP;
		FillListBox(exptext, num_exp);
		break;
	}

	*pResult = 0;
}



void CTextEdDlg::LoadFile(CString szFileName)
{
	RomFile = fopen(szFileName, "rb+");

	CreateCompressionTable(RomFile, comprTable);

	num_tpt = LoadTPTText(RomFile, tpttext, ccTable);
	num_raw = LoadRawText(RomFile, rawtext, ccTable, comprTable);
	num_exp = LoadEXPText(RomFile, exptext, ccTable);

	FillListBox(tpttext, num_tpt);
}





void CTextEdDlg::DisplayString(String s, int pos, int maxpos)
{
	curstring = pos-1;

	SetDlgItemText(IDC_EDIT_TEXT, ParseGrouping(ParseString(s.str, s.len, ccTable, comprTable)));

	CString str;

	str.Format("%d/%d", pos, maxpos);
	SetDlgItemText(IDC_TEXT_CURPOS, str);

	str.Format("$%X", s.address);
	SetDlgItemText(IDC_TEXT_CURADDRESS, str);
}


void CTextEdDlg::OnSelectString() 
{
	// TODO: Add your control notification handler code here
	int pos = m_ListBox.GetCurSel();

	String s;

	if(curlist == TPT)
	{
		s = tpttext[pos];
		DisplayString(s, pos+1, num_tpt);
	}
	if(curlist == RAW)
	{
		s = rawtext[pos];
		DisplayString(s, pos+1, num_raw);
	}
	if(curlist == EXP)
	{
		s = exptext[pos];
		DisplayString(s, pos+1, num_exp);
	}


}

void CTextEdDlg::FillListBox(String strings[], int nstr)
{
	strlist = strings;

	m_ListBox.SetRedraw(FALSE);
	m_ListBox.ResetContent();

	for(int i = 0; i < nstr; i++)
	{

		//SetDlgItemInt(IDC_ERROR, i);

		char* s = ParseHeader(strings[i].str, strings[i].len, ccTable, comprTable);
		//char *s = ParseString(strings[i].str, strings[i].len, ccTable, comprTable);

		if(strlen(s) == 0)
			s = "      ";

		m_ListBox.AddString(s);
	}
	m_ListBox.SetRedraw(TRUE);
}





void CTextEdDlg::OnApply() 
{
	// TODO: Add your control notification handler code here
	char str[8192];
	GetDlgItemText(IDC_EDIT_TEXT, str, 8192);

	char *s = DeparseString(str);
	int len = GetStringLength(str);

	if(len > strlist[curstring].len)
	{
		AfxMessageBox("WARNING: Massive borkage is about to ensue.");
	}

	strlist[curstring].str = s;
	strlist[curstring].len = len;

	WriteString(RomFile, strlist[curstring], strlist[curstring].address);

	if(curlist == TPT)
	{
		num_tpt = LoadTPTText(RomFile, tpttext, ccTable);
		FillListBox(strlist, num_tpt);
	}
	if(curlist == RAW)
	{
		num_raw = LoadRawText(RomFile, rawtext, ccTable, comprTable);
		FillListBox(strlist, num_raw);
	}
	if(curlist == EXP)
	{
		num_exp = LoadEXPText(RomFile, exptext, ccTable);
		FillListBox(strlist, num_exp);
	}

	m_ListBox.SetCurSel(curstring);
}
