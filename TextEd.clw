; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CTextEdDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "TextEd.h"

ClassCount=3
Class1=CTextEdApp
Class2=CTextEdDlg
Class3=CAboutDlg

ResourceCount=4
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_TEXTED_DIALOG
Resource4=IDR_MENU

[CLS:CTextEdApp]
Type=0
HeaderFile=TextEd.h
ImplementationFile=TextEd.cpp
Filter=N

[CLS:CTextEdDlg]
Type=0
HeaderFile=TextEdDlg.h
ImplementationFile=TextEdDlg.cpp
Filter=D
LastObject=CTextEdDlg
BaseClass=CDialog
VirtualFilter=dWC

[CLS:CAboutDlg]
Type=0
HeaderFile=TextEdDlg.h
ImplementationFile=TextEdDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_TEXTED_DIALOG]
Type=1
Class=CTextEdDlg
ControlCount=8
Control1=IDC_EDIT_TEXT,edit,1352732676
Control2=IDC_APPLY,button,1342242816
Control3=IDC_TEXTLIST,listbox,1352733697
Control4=IDC_TEXT_CURPOS,static,1342308352
Control5=IDC_TEXT_CURADDRESS,static,1342308354
Control6=IDC_TAB,SysTabControl32,1342193728
Control7=IDC_ERROR,static,1342308352
Control8=IDC_ERRORT,static,1342308352

[MNU:IDR_MENU]
Type=1
Class=?
Command1=ID_FILE_OPENROM
Command2=ID_FILE_CLOSEROM
Command3=ID_FILE_EXIT
Command4=ID_EDIT_UNDO
Command5=ID_EDIT_CUT
Command6=ID_EDIT_COPY
Command7=ID_EDIT_PASTE
Command8=ID_EDIT_SEARCH
Command9=ID_TOOLS_EXPANDROM
Command10=ID_TOOLS_TPTEDITOR
CommandCount=10

