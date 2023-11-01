// NanoTubesControlCenter.cpp : Defines the class behaviors for the application.
//
#include "stdafx.h"

#include "NanoTubesControlCenter.h"
#include "MainFrm.h"

#include "ModuleFrm.h"
#include "FSDoc.h"
#include "FSView.h"

#include "ProjectDoc.h"
#include "ProjectFrame.h"
#include "ProjectView.h"

#include "PlotDlg.h"
#include "DLGs/UserLogonDlg.h"
#include "DLGs/UserDataEditDlg.h"

#include "ah_project.h"
#include "ah_users.h"
#include "ah_msgprovider.h"
#include "ah_loadcfg.h"
#include "ah_fonts.h"

#include "Automation/WatchDogs.h"
#include "Automation/Timers.h"
#include "Automation/DataSrc.h"
#include "Automation/Converters.h"

#include "Automation/NTCCDataSrv.h"

//#include "Automation/v_pid.h"

#define USE_ADMIN_LOGIN

#ifdef _DEBUG
	#define new DEBUG_NEW	
	//#define USE_SCREEN_RESOLUTION_1280X1024
#endif


using namespace xyplot;




// CNanoTubesControlCenterApp

BEGIN_MESSAGE_MAP(CNanoTubesControlCenterApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CNanoTubesControlCenterApp::OnAppAbout)
	ON_COMMAND(ID_ADD_USER, &CNanoTubesControlCenterApp::OnAddUser)
	ON_COMMAND(ID_LOCK_APP, &CNanoTubesControlCenterApp::OnLockApp)

	// Standard print setup command
	//ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinApp::OnFilePrintSetup)
	ON_COMMAND(ID_APP_EXIT, &CNanoTubesControlCenterApp::OnAppExit)
	ON_COMMAND(ID_VIEW_LOAD_RESULTS, &CNanoTubesControlCenterApp::OnViewLoadResults)
	ON_COMMAND(ID_VIEW_CONFIG, &CNanoTubesControlCenterApp::OnViewActualConfig)
	
END_MESSAGE_MAP()


// CNanoTubesControlCenterApp construction

CNanoTubesControlCenterApp::CNanoTubesControlCenterApp() : pDataSrv(NULL) , pPlotDlgWnd(NULL)
{

	
	log.open("ntcc.log");

	log << "Set locale..." << endl;

	EnableHtmlHelp();

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

int CNanoTubesControlCenterApp::Run( )
{
	int res = 0;
	try {
		 res = CWinApp::Run();
	}
	catch( CMemoryException* mfcEx )
	{
		// Handle the out-of-memory exception here.
		mfcEx->ReportError();
		mfcEx->Delete();
	}
	catch( CFileException* mfcEx )
	{
		// Handle the file exceptions here.
		mfcEx->ReportError();
		mfcEx->Delete();
	}
	catch( CException* mfcEx )
	{
		// Handle all other types of exceptions here.
		mfcEx->ReportError();
		mfcEx->Delete();
	}
	catch(std::exception stdEx)
	{
		
		MessageBox(m_pMainWnd->m_hWnd, stdEx.what(), "Unhandled exception", MB_ICONEXCLAMATION|MB_OK);
	}
	return res;
}

// The one and only CNanoTubesControlCenterApp object

CNanoTubesControlCenterApp theApp;

std::vector<CDocument*> CNanoTubesControlCenterApp::m_vSubDocs;

// CNanoTubesControlCenterApp initialization

/////////////////////////////////////////////////////////////////////////////
// Prof-UIS advanced options

void CNanoTubesControlCenterApp::SetupUiAdvancedOptions()
{

	// General UI look
		

	// Popup menu option: Display menu shadows
	CExtPopupMenuWnd::g_bMenuWithShadows = false;

	// Popup menu option: Display menu cool tips
	CExtPopupMenuWnd::g_bMenuShowCoolTips = false;

	// Popup menu option: Initially hide rarely used items (RUI)
	CExtPopupMenuWnd::g_bMenuExpanding = false;

	// Popup menu option: Display RUI in different style
	CExtPopupMenuWnd::g_bMenuHighlightRarely = true;

	// Popup menu option: Animate when expanding RUI (like Office XP)
	CExtPopupMenuWnd::g_bMenuExpandAnimation = false;

	// Popup menu option: Align to desktop work area (false - to screen area)
	CExtPopupMenuWnd::g_bUseDesktopWorkArea = true;

	// Popup menu option: Popup menu animation effect (when displaying)
	//CExtPopupMenuWnd::g_DefAnimationType = CExtPopupMenuWnd::__AT_FADE;
		//$$__PROFUISAPPWIZ_KEY_MENU_ANIM_DISPMS$$
	CExtControlBar::g_bDisableAutoHideAnimationCollapsing = true;

	CExtControlBar::g_bDisableAutoHideAnimationExpanding = true;

}

BOOL CNanoTubesControlCenterApp::SelectProject() const
{
	
/////////////////////////////////
	CString strCustomDialogCaption;
	CString strCustomOkButtonCaption;
	CString strTextTitle;
	CString strCustomCancelButtonCaption;
	CString strCustomNewFolderButtonCaption;
	CString strCustomEditLabelCaption;

	CExtShellDialogBrowseFor dlgShellBrowseFor( NULL );
	if( ! strCustomDialogCaption.IsEmpty() )
		dlgShellBrowseFor.m_strCustomDialogCaption = LPCTSTR(strCustomDialogCaption);
	if( ! strCustomOkButtonCaption.IsEmpty() )
		dlgShellBrowseFor.m_strCustomOkButtonCaption = LPCTSTR(strCustomOkButtonCaption);
	if( ! strCustomCancelButtonCaption.IsEmpty() )
		dlgShellBrowseFor.m_strCustomCancelButtonCaption = LPCTSTR(strCustomCancelButtonCaption);
	if( ! strCustomNewFolderButtonCaption.IsEmpty() )
		dlgShellBrowseFor.m_strCustomNewFolderButtonCaption = LPCTSTR(strCustomNewFolderButtonCaption);
	if( ! strCustomEditLabelCaption.IsEmpty() )
		dlgShellBrowseFor.m_strCustomEditLabelCaption = LPCTSTR(strCustomEditLabelCaption);
	if( ! strTextTitle.IsEmpty() )
		dlgShellBrowseFor.m_strLabelAtTop = LPCTSTR(strTextTitle);
	
	if( TRUE )
		dlgShellBrowseFor.m_wndShellTree.m_dwAttributeFilterAny |= SFGAO_HIDDEN;
	else
		dlgShellBrowseFor.m_wndShellTree.m_dwAttributeFilterAllAbsent |= SFGAO_HIDDEN;
	
	dlgShellBrowseFor.m_wndShellTree.ShowShellContextMenusSet( false );
	dlgShellBrowseFor.m_bShowMakeNewFolderButton = false;
	dlgShellBrowseFor.m_bShowFolderEdit = false;

	
	/*CString strInitialFolder;	
	strInitialFolder.TrimLeft( _T(" \r\n\t") );
	strInitialFolder.TrimRight( _T(" \r\n\t") );
	
	CExtPIDL pidlCustomRoot;
	pidlCustomRoot.Empty();
	pidlCustomRoot.FromFolder(strInitialFolder, NULL);
	
	if( pidlCustomRoot != NULL )
		dlgShellBrowseFor.m_pidlRoot = pidlCustomRoot;

	if( ! strInitialFolder.IsEmpty() )
		dlgShellBrowseFor.m_pidlResult.FromFolder( LPCTSTR(strInitialFolder), NULL );
	*/

	dlgShellBrowseFor.m_bCurrentDirectorySetInitially = true;
	dlgShellBrowseFor.m_bCurrentDirectorySetOnWalk = true;
	dlgShellBrowseFor.m_bSaveRestoreShellLocation = true;
	dlgShellBrowseFor.m_bSaveRestoreWindowPosition = true;

	if( dlgShellBrowseFor.DoModal() != IDOK )
		return FALSE;
	else if( ! dlgShellBrowseFor.m_pidlResult.IsEmpty() )
	{
		CExtSafeString str = dlgShellBrowseFor.m_pidlResult.GetPath();
		if( str.IsEmpty() )
			str = dlgShellBrowseFor.m_strDisplayNameResult;
		
		if ( !NTCCProject::Instance().Init( (LPCTSTR)str, const_cast<std::ofstream&>(log) ) )
			return FALSE; 
	}
	else
		return FALSE;

	return TRUE;
}

bool CNanoTubesControlCenterApp::GetCmdValue(std::string key, std::string& val) const
{
	std::map<std::string, std::string>::const_iterator itFind = cmdLineParams.find(key);
	if ( itFind != cmdLineParams.end() )
	{
		val = itFind->second;
		return true;
	}
	else {
		val = "";
		return false;
	}
}

void CNanoTubesControlCenterApp::ParseCommandLine( CCommandLineInfo& rCmdInfo  )
{
	cmdLineParams.clear();
	
	typedef  boost::char_separator<char> boost_char_sep;
	typedef  boost::tokenizer< boost::char_separator<char> > boost_char_tokenizer;
	typedef  boost::tokenizer< boost::char_separator<char> >::iterator boost_char_tokenizer_iterator;

	boost_char_sep sep(" \t\n");
	std::string cmdLine(m_lpCmdLine);

	boost_char_tokenizer tokens(cmdLine, sep);
	vector<std::string>  parts;

	for ( boost_char_tokenizer_iterator it = tokens.begin(); it != tokens.end(); ++it)
		parts.push_back(boost::lexical_cast<string>(*it));

	
	boost_char_sep sep1("-:");
	for (vector<std::string>::iterator it = parts.begin(); it != parts.end(); ++it)
	{	
		boost_char_tokenizer details( (*it), sep1 );
		boost_char_tokenizer_iterator it1 = details.begin();

		std::string key = boost::lexical_cast<string>(*it1);
		std::string par = "";
		if (++it1 != details.end() )
			par = boost::lexical_cast<string>(*it1);

		cmdLineParams.insert( std::make_pair(key, par) );
	}
	
}

BOOL CNanoTubesControlCenterApp::InitInstance()
{
/////////////////////////////////

	//ofstream log;
	//log.open("ntcc.log");
	/*
	string s = "^S007POP1";	
	int crc0 = CRC_CCITT((unsigned char*)s.data(), s.length());

	int crc1 = CRC_CCITT1((unsigned char*)s.data(), s.length());
	*/
	log << "Set locale..." << endl;
	//log << setlocale( LC_ALL, "C" ) << endl;
	//log << setlocale( LC_ALL, "" ) << endl;
	//log << setlocale(LC_NUMERIC, "" ) << endl;
	
	/*TRACE("\n %f %f %f", 3.14 , 31.4, 314.0 );
	log << 3.14 << " " << 31.4 << " " << 314.0 << endl;*/
	log << "Init common  controls..." << endl;
	InitCommonControls();

	log << "Enable control container..." << endl;
	AfxEnableControlContainer();

	log << "Rich edit init..." << endl;
	AfxInitRichEdit();


	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}


	WSADATA ws;
	if (::WSAStartup(MAKEWORD( 2, 2 ), &ws) != 0)
	{
		cerr << "Unable to initialize sockets" << endl;
		return FALSE;
	}

//	CWinApp::InitInstance();

#ifdef _DEBUG
	/*_CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_DEBUG ); // enable file output
	_CrtSetReportFile( _CRT_WARN, _CRTDBG_FILE_STDOUT ); // set file to stdout
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_CRT_DF);
	_CrtMemCheckpoint(&_ms); // now forget about objects created before
	*/
#endif
 	
//////////////////////////////////////////////////////////////////////////
	
	log << "Init gdi plus..." << endl;
	//инициализация GDI+  
	Gdiplus::GdiplusStartupInput gsi;
	Gdiplus::GdiplusStartup(&guiPlusToken, &gsi, NULL);

	
	log << "ProfUIS advanced options..." << endl;
	//Prof-UIS advanced options
	SetupUiAdvancedOptions();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization

	SetRegistryKey(_T("ART Pte"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views

	/////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////

	log << "Prepare documents template..." << endl;
	CMultiDocTemplate* pDocOutShemaTemplate;
	pDocOutShemaTemplate = new CMultiDocTemplate(IDR_NANOTUBESCC_TYPE,
		RUNTIME_CLASS(CFSDoc),
		RUNTIME_CLASS(CModuleFrame), // custom MDI child frame
		RUNTIME_CLASS(CFSView));

	if (!pDocOutShemaTemplate)
		return FALSE;

	CMultiDocTemplate* pDocProjectTemplate;
	pDocProjectTemplate = new CMultiDocTemplate(IDR_NANOTUBESPROJECT_TYPE,
		RUNTIME_CLASS (CProjectDoc),
		RUNTIME_CLASS (CProjectFrame),
		RUNTIME_CLASS (CProjectView));

	if (!pDocProjectTemplate)
		return FALSE;
		
	AddDocTemplate(pDocProjectTemplate);
	AddDocTemplate(pDocOutShemaTemplate);

	///////////////////////////////////////////////////////////
	///
	/*
	log << "Prepare rich document images..." << endl;
	CExtRichDocObjectBase & _GC = CExtRichContentLayout::stat_GetObjectCountainerGlobal();	
	CExtRichDocObjectImage * pObjPic = NULL;

	pObjPic = new CExtRichDocObjectImage( _T("on.bmp") );
	VERIFY( pObjPic->ImageGet().LoadBMP_Resource( MAKEINTRESOURCE(IDB_ON_16x16_V3) ) );
	VERIFY( pObjPic->ImageGet().Make32() );
	VERIFY( _GC.ObjectAdd( pObjPic ) );
	
	//delete pObjPic;

	pObjPic = new CExtRichDocObjectImage( _T("off.bmp") );
	VERIFY( pObjPic->ImageGet().LoadBMP_Resource( MAKEINTRESOURCE(IDB_OFF_16x16_V3) ) );
	VERIFY( pObjPic->ImageGet().Make32() );
	VERIFY( _GC.ObjectAdd( pObjPic ) );

	//delete pObjPic;
	*/
	///////////////////////////////////////////////////////////

	const char* path = {"E:\\ART-MONBAT\\NTCC\\CONFIG\\M1"};
	if (!NTCCProject::Instance().Init(path, const_cast<std::ofstream&>(log)))
		return FALSE;
	
	///////////////////////////////////////////////////////////
	log << "Initialize XML Objects Factory..." << endl;
	XMLObjectFactory::Init();

	char szPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szPath);

	///////////////////////////////////////////////////////////
	CString strPath(szPath); 
	
	strPath += "\\xyplot.dll"; 
	log << "Initilize XYPLot..." << (LPCTSTR)strPath << endl;

	if (!XYPlotManager::Instance().Initialize((LPCTSTR)strPath, GetDesktopWindow()))
		return FALSE;

	//////////////////////////////////////////////////////////////////////////
	NTCCProject& prj = NTCCProject::Instance();

	log << "Search last loaded project ..." <<  endl;

	bool useProjectSelDialog =  (GetKeyState(VK_LSHIFT) & 0x8000) != 0; 


	char szCurDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szCurDir);

	if ( useProjectSelDialog )
	{
		if ( !SelectProject() )
			return FALSE;
	}
	else {
		CString strLastProject = GetProfileString("Settings", "LastProject", "");
		log << "Load last project ..." << (LPCTSTR)strLastProject << endl;

		if ( !prj.Init((LPCTSTR)strLastProject, log)  ) 
		{
			if ( AfxMessageBox("Application cannot find last project configuration.\nDo you want to select another one?", MB_YESNO|MB_ICONASTERISK) == IDYES)
			{
				if ( !SelectProject() )
					return FALSE;
			}
			else
				return FALSE;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	SetCurrentDirectory(szCurDir);

	log << "Create application main window ..." << endl;
	// create main MDI Frame window	
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME) )
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;
	
	pMainFrame->ModifyStyle(WS_CAPTION, WS_POPUPWINDOW);

	::SetClassLong(theApp.m_pMainWnd->m_hWnd, GCL_HCURSOR, (long)::LoadCursor(theApp.m_hInstance, MAKEINTRESOURCE(IDC_CUR_ARROW)));


	log << "Initialize application users ..." << endl;

	//////////////////////////////////////////////////////////////////////////
	AppUsersHelper::Instance().Initialize( "data.usr" );
	

// 	const NTCC_APP_USER* lpU = AppUsersHelper::Instance().GetUser("Johnny A. Matveichik");
// 	lpU = AppUsersHelper::Instance().GetUser("Alex N. Migoun");
// 	lpU = AppUsersHelper::Instance().GetUser("Alexandr M. Ivanovski");

	NTCC_APP_USER guest;

	guest.login = "Guest";
	guest.screenName = "Наблюдатель";
	guest.psw = "0000";
	guest.ug = ONLOOKER;

	AppUsersHelper::Instance().AddUser(&guest);

	guest.login = "Administrator";
	guest.screenName = "Администратор";
	guest.psw = "951";
	guest.ug = ADMINISTRATORS;

	AppUsersHelper::Instance().AddUser(&guest);

	
//////////////////////////////////////////////////////////////////////////
	log << "Initialize message provider helper ..." << endl;
	AppMessagesProviderHelper::Instance().RegisterOutputMessageDestination(m_pMainWnd);
	

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);


	/*
	if (cmdInfo.m_strFileName.IsEmpty() )
		m_strConfigPath = "config\\config.xml";
		//configPath = "config\\config.xml";
	else
		m_strConfigPath = cmdInfo.m_strFileName;
	*/
	
	//load extra fonts
	
	std::string strFontsPath = prj.GetPath(NTCCProject::PATH_FONTS);
	AppFontsHelper::Instance().Init(strFontsPath.c_str());

	
	std::string strConfigPath = prj.GetPath(NTCCProject::PATH_CONFIG, "config.xml", false);

	//////////////////////////////////////////////////////////////////
	
	AppLoadingHelper& appLH = AppLoadingHelper::Instance();
	//appLH.Initialize( (LPCTSTR)strConfigPath );
	
	log << "Load project configuration ..." << endl;
	
	CProjectDoc* pDoc = NULL;

	try {
		 pDoc = dynamic_cast<CProjectDoc*>(pDocProjectTemplate->OpenDocumentFile(strConfigPath.c_str(), FALSE));		
	}
	catch(std::exception const& e)
	{
		log << e.what() << std::endl;
		// If you are feeling mad (not in main) you could rethrow! 
	}
	catch(...)
	{
		log << "UNKNOWN EXCEPTION" << std::endl;
		// If you are feeling mad (not in main) you could rethrow! 
	}

	if ( pDoc == NULL )
		return FALSE;

	POSITION pos = pDoc->GetFirstViewPosition();
	CView* pView = pDoc->GetNextView(pos);
	ASSERT (pView != NULL);
	pView->OnInitialUpdate();

	pMainFrame->pProjectFrm = pView->GetParentFrame();

// 
// 	m_strConfigPath = pDoc->GetPathName();

	AddDocument( pDoc );

	//////////////////////////////////////////////////////////////////////////
	/// СЕРВЕР ДАННЫХ
	//////////////////////////////////////////////////////////////////////////
	CString cmds(m_lpCmdLine);
	
	//запускать сервер данных?
	
	std::string srvname;
	if ( GetCmdValue("server", srvname) )
	{
		pDataSrv = new NTCCDataSrv();

		//максимальное число клиентов
		int nMaxClients = PIPE_UNLIMITED_INSTANCES;
		std::string cc;
		if ( GetCmdValue("maxclients", cc) )
			nMaxClients = atoi( cc.c_str() );

		if ( !pDataSrv->Start( srvname.c_str() ) )
			delete pDataSrv;

	}

	//////////////////////////////////////////////////////////////////////////
	
	log << "Prepare loading report ..." << endl;

	std::string htmlTemlate = prj.GetPath(NTCCProject::PATH_HTML, "loading.dwt", false);
	std::string htmlOut		= prj.GetPath(NTCCProject::PATH_HTML, "loading.html", false);
	appLH.MakeReport(
		htmlTemlate.c_str(), 
		htmlOut.c_str(),
		pDoc->LoadingResult() );
	
	int res = appLH.LoadingDlgResult( pDoc->LoadingResult() );
	if (res == 0 || pDoc == NULL)
	{	
		HINSTANCE inst = ShellExecute(NULL, "open", htmlOut.c_str(), NULL, NULL, SW_SHOWNORMAL);
		appLH.Release();
		return FALSE;
	}
	
	appLH.Release();

// 
// 	if ( !AppXYPlotWndHelper::Init(m_pMainWnd) )
// 		return FALSE;

	pDoc->SetTitle("Summary");
	pDocProjectTemplate->InitialUpdateFrame((CFrameWnd*)m_pMainWnd, pDoc, FALSE);
	
#ifdef USE_SCREEN_RESOLUTION_1280X1024
 	
	::SetWindowPos(m_pMainWnd->m_hWnd, CWnd::wndTop.m_hWnd, 0, 0, 1280, 1024, SWP_NOACTIVATE|SWP_HIDEWINDOW);	
 	((CFrameWnd*)m_pMainWnd)->ActivateFrame( SW_SHOWNORMAL );
	((CFrameWnd*)m_pMainWnd)->UpdateWindow();

#else
	
	::SetWindowPos(m_pMainWnd->m_hWnd, CWnd::wndTop.m_hWnd, 0, 0, 0, 0, SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOSIZE|SWP_HIDEWINDOW);	
	((CFrameWnd*)m_pMainWnd)->ActivateFrame( SW_SHOWMAXIMIZED );	
	((CFrameWnd*)m_pMainWnd)->UpdateWindow();

#endif

	pDoc->UpdateAllViews(NULL, CProjectDoc::UPDATE_LOAD_COMPLETE,  NULL);

#ifdef USE_ADMIN_LOGIN
	AppUsersHelper::Instance().UserLogin("Administrator", "951");
#else
	AppUsersHelper::Instance().UserLogin("Guest", "0000");
#endif
	
	log << "Update main window ..." << endl;

 	((CFrameWnd*)m_pMainWnd)->RecalcLayout(TRUE);
	((CMainFrame*)m_pMainWnd)->InvalidateRect(NULL);
 	((CMainFrame*)m_pMainWnd)->UpdateWindow();

	return TRUE;
}


void  CNanoTubesControlCenterApp::OnAddUser()
{
	NTCC_APP_USER* pUser = AppUsersHelper::Instance().GetCurrentUser();
	if ( pUser->ug == ADMINISTRATORS )
	{
		СUserDataEditDlg dlg;
		dlg.DoModal();
	}
	else{
		AfxMessageBox("Недостаточно прав", MB_OK|MB_ICONEXCLAMATION );
	}
}

void  CNanoTubesControlCenterApp::OnLockApp()
{
	NTCC_APP_USER* pUser = AppUsersHelper::Instance().GetCurrentUser();
	if ( pUser == NULL || pUser->ug == ONLOOKER )
		AppUsersHelper::Instance().UserLogin();
	else
		AppUsersHelper::Instance().UserLogin("Guest", "0000");
}


// CAboutDlg dialog used for App About
class CAboutDlg : public CExtNCW < CExtResizableDialog >
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	CExtButton	m_BtnOK;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};


CAboutDlg::CAboutDlg() : CExtNCW < CExtResizableDialog >(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDOK, m_BtnOK);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CExtResizableDialog)
	ON_BN_CLICKED(IDOK, &CAboutDlg::OnBnClickedOk)
END_MESSAGE_MAP()


BOOL CAboutDlg::OnInitDialog()
{
	VERIFY( CExtNCW < CExtResizableDialog >::OnInitDialog() );

	//
	// Show size gripper
	//
	CExtNCW < CExtResizableDialog >::ShowSizeGrip( FALSE );

	//
	// Enable dialog position saving/restoring in registry
	//
	CExtNCW < CExtResizableDialog >::EnableSaveRestore(
		__PROF_UIS_PROJECT_DLG_PERSIST_REG_KEY,
		_T("CAboutDlg-Saved-Position")
		);

	// TODO: Add extra initialization here
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// App command to run the dialog
void CNanoTubesControlCenterApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CNanoTubesControlCenterApp message handlers


void CNanoTubesControlCenterApp::OnAppExit()
{

	
	int i = 0;
	CWinApp::OnAppExit();
}

BOOL CNanoTubesControlCenterApp::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	
	if (nID == ID_APP_EXIT && nCode == CN_COMMAND)
	{
		/*
		LPNTCC_APP_USER pUser = AppUsersHelper::Instance().GetCurrentUser();		
		
		if ( pUser->ug < ONLOOKER ) 
		{
			AppOBAScriptsHelper &sh = AppOBAScriptsHelper::Instance();
			if ( sh.HasActive() )
			{
				int res = AfxMessageBox(IDS_APP_CLOSE_WITH_ACTIVE_SCRIPTS, MB_ICONWARNING|MB_YESNO);
				
				if (res == IDNO) 
					return TRUE;
				
				sh.TerminateAllActive();				
			}
			
			return CWinApp::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
		}
		else
		{
			AfxMessageBox(IDS_APP_CLOSE_PERMISIONS, MB_ICONWARNING);
			return TRUE;
		}
		
		/*DiscreteDataSrc* pDSAppCanBeClosed = dynamic_cast<DiscreteDataSrc*>(DataSrc::GetDataSource("ST_APP_CAN_CLOSE"));
		
		if ( pDSAppCanBeClosed != NULL && !pDSAppCanBeClosed->IsEnabled() )
		{
			AfxMessageBox(IDS_APP_CLOSE_FAILURE, MB_ICONWARNING);
			return TRUE;
		}
		*/
	}
	
	return CWinApp::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CNanoTubesControlCenterApp::AddDocument(CDocument* pDoc)
{
	m_vSubDocs.push_back(pDoc);
}

CString CNanoTubesControlCenterApp::GetAppInfo()
{
	//Получение информации о версии приложения
	CString appPath = ::AfxGetApp()->m_pszExeName;
	appPath  += ".exe";
	CString out;
	const CString strGet[10] = 
	{ 
		"CompanyName",  "FileDescription",  "FileVersion", "InternalName",  "LegalCopyright",  
		"LegalTrademarks", "OriginalFilename", "ProductName",  "ProductVersion",  "Comments" 
	};

	DWORD dwHandle = 0;
	DWORD dwSize = ::GetFileVersionInfoSize(appPath, &dwHandle);

	char* pData = (char*)::LocalAlloc(LMEM_FIXED, dwSize);	

	if ( ::GetFileVersionInfo(appPath, 0, dwSize, pData))
	{
		struct LANGANDCODEPAGE 
		{
			WORD wLanguage;
			WORD wCodePage;
		} *lpTranslate;


		UINT len; //
		VerQueryValue(pData, "\\VarFileInfo\\Translation", (LPVOID*)&lpTranslate, &len);

		CString str;
		LPCSTR pVal;

		str.Format("\\StringFileInfo\\%04X%04X\\ProductName",lpTranslate->wLanguage, lpTranslate->wCodePage );
		VerQueryValue(pData, str.GetBuffer(MAX_PATH), (LPVOID*)&pVal, &len);
		str.ReleaseBuffer();
		str = pVal;
		out += str;
		out += "\n\r";

		str.Format("\\StringFileInfo\\%04X%04X\\ProductVersion",lpTranslate->wLanguage, lpTranslate->wCodePage );	
		VerQueryValue(pData, str.GetBuffer(MAX_PATH), (LPVOID*)&pVal, &len);
		str.ReleaseBuffer();
		str = pVal;
		
		int verinf[4];
		sscanf_s(pVal, "%d,%d,%d,%d", &verinf[0], &verinf[1], &verinf[2], &verinf[3]);

		str.Format("v.%d.%d.%d Build %d", verinf[0], verinf[1],  verinf[2], verinf[3]);		
		out += str;
		out += "\n\r";

		str.Format("\\StringFileInfo\\%04X%04X\\CompanyName",lpTranslate->wLanguage, lpTranslate->wCodePage );
		VerQueryValue(pData, str.GetBuffer(MAX_PATH), (LPVOID*)&pVal, &len);
		str.ReleaseBuffer();
		str = pVal;		
		out += str;
		out += "\n\r";

		str.Format("\\StringFileInfo\\%04X%04X\\LegalCopyright",lpTranslate->wLanguage, lpTranslate->wCodePage );
		VerQueryValue(pData, str.GetBuffer(MAX_PATH), (LPVOID*)&pVal, &len);
		str.ReleaseBuffer();
		str = pVal;
		out += str;
		
	}

	::LocalFree(pData);
	return out;
}

CPlotDlg* CNanoTubesControlCenterApp::GetPlotDlg()
{
	if (pPlotDlgWnd == NULL)
	{
		pPlotDlgWnd = new CPlotDlg(m_pMainWnd);

		if ( !pPlotDlgWnd->Create(IDD_PLOT, m_pMainWnd) )
		{
			delete pPlotDlgWnd;
			pPlotDlgWnd = NULL;
		}			
	}

	return pPlotDlgWnd;
}

int CNanoTubesControlCenterApp::ExitInstance()
{
	AppOBAScriptsHelper::Instance().TerminateAllActive();

	log.close();

	if (pDataSrv != NULL)
	{
		pDataSrv->Stop();
		delete pDataSrv;
	}

	//останавливаем сбор данных
	DataCollector::Instance().Stop();
	
	//останавливаем архивацию данных
	DataLogger::ClearAll();

	//останавливаем измерительные устройства
	IdentifiedPhysDevice::StopAll();
 	
	//останавливаем сторожевые объекты
	BaseWatchDog::ClearAll();

	//удаляем источники данных
	DataSrc::ClearAll();

	//удаляем фильтры
	BaseFilter::ClearAll();

	//удаляем сбор данных
	BaseConverter::ClearAll();
 
	//удаляем измерительные устройства
 	IdentifiedPhysDevice::ClearAll();

	//останавливаем менеджеры комманд
	CommandManager::ClearAll();

	//удаляем таймеры событий
	BaseTimer::ClearAll();

	//удаляем все документы
	for ( std::vector<CDocument*>::iterator it = m_vSubDocs.begin(); it != m_vSubDocs.end(); ++it )
		delete (*it);
	
	//чистим ключ реестра для записи состояния источников данных
	HKEY hKey = GetAppRegistryKey();
	DelRegTree( hKey, _T("Data sources") );

	std::string path = NTCCProject::Instance().GetPath(NTCCProject::PATH_BASE);
	WriteProfileString("Settings", "LastProject",  path.c_str());
	
	if (pPlotDlgWnd != NULL )
	{
		pPlotDlgWnd->DestroyWindow();
		delete pPlotDlgWnd;
	}

#ifdef _DEBUG
	//_CrtMemDumpAllObjectsSince(&_ms); // dump leaks
	//_CrtDumpMemoryLeaks();
#endif

	return CWinApp::ExitInstance();
}


void CNanoTubesControlCenterApp::OnViewLoadResults()
{
	std::string path = NTCCProject::Instance().GetPath(NTCCProject::PATH_HTML, "loading.html");
	HINSTANCE inst = ShellExecute(NULL, "open", path.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

void CNanoTubesControlCenterApp::OnViewActualConfig()
{
	std::string path = NTCCProject::Instance().GetPath(NTCCProject::PATH_CONFIG, "config.xml");
	HINSTANCE inst = ShellExecute(NULL, "open", path.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

void CAboutDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
}


