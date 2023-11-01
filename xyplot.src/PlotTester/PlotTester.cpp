// PlotTester.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "PlotTester.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "PlotTesterDoc.h"
#include "PlotTesterView.h"

#include "..\src\dnp_checkout.h"
#include "..\src\xyplotwrapper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace xyplot;

// CPlotTesterApp

BEGIN_MESSAGE_MAP(CPlotTesterApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
END_MESSAGE_MAP()


// CPlotTesterApp construction

CPlotTesterApp::CPlotTesterApp()
{
}


// The one and only CPlotTesterApp object

CPlotTesterApp theApp;

// CPlotTesterApp initialization





BOOL CPlotTesterApp::InitInstance()
{
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();

	//_CrtSetBreakAlloc(14895);


#ifdef NDEBUG
	#ifdef _M_X64
		char szLibPath[] = "D:\\Source\\Cpp\\xyplot\\x64\\Release\\xyplot.dll";
	#else
		char szLibPath[] = "D:\\Source\\Cpp\\xyplot\\win32\\Release\\xyplot.dll";
	#endif
#else
	#ifdef _M_X64
		char szLibPath[] = "D:\\Source\\Cpp\\xyplot\\x64\\Debug\\xyplot.dll";
		//char szLibPath[] = "D:\\Source\\Cpp\\xyplot\\x64\\Release\\xyplot.dll";
	#else
		char szLibPath[] = "D:\\Source\\Cpp\\xyplot\\win32\\Debug\\xyplot.dll";
	#endif
#endif

	//char szLibPath[] = "xyplot.dll";
	if (!XYPlotManager::Instance().Initialize(szLibPath, GetDesktopWindow()))
		return FALSE;

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("PlotTester"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_PlotTesterTYPE,
		RUNTIME_CLASS(CPlotTesterDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CPlotTesterView));
	AddDocTemplate(pDocTemplate);
	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;
	// call DragAcceptFiles only if there's a suffix
	//  In an MDI app, this should occur immediately after setting m_pMainWnd
	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	((CMDIFrameWnd*)m_pMainWnd)->ActivateFrame( SW_SHOW );
	
	return TRUE;
}

// CAboutDlg dialog used for App About
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

// Implementation
protected:
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
};


CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


BOOL CAboutDlg::OnInitDialog()
{
	VERIFY( CDialog::OnInitDialog() );

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// App command to run the dialog
void CPlotTesterApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CPlotTesterApp message handlers

