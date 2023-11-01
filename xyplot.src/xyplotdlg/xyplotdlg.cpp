// xyplotdlg.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "xyplotdlg.h"
#include "plotdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "../../xyplot/src/XYPlotWrapper.h"

using namespace xyplot;

extern "C" __declspec (dllexport) long _stdcall Run(HWND hWndParent, long plotID)
{
	__PROF_UIS_MANAGE_STATE (AfxGetStaticModuleState());
	CXYPlotDlg dlg( CWnd::FromHandle(hWndParent), plotID );

	if ( !XYPlotManager::Instance().Initialize("xyplot.dll", hWndParent) )
		return PE_INVALID_DIALOG_DLL;

	dlg.DoModal();
	return PE_NOERROR;
}

//
//TODO: If this DLL is dynamically linked against the MFC DLLs,
//		any functions exported from this DLL which call into
//		MFC must have the AFX_MANAGE_STATE macro added at the
//		very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

// CxyplotdlgApp

BEGIN_MESSAGE_MAP(CXYPlotDlgApp, CWinApp)
END_MESSAGE_MAP()


// CxyplotdlgApp construction

CXYPlotDlgApp::CXYPlotDlgApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CxyplotdlgApp object

CXYPlotDlgApp theApp;



void CXYPlotDlgApp::SetupUiAdvancedOptions() 
{

	g_PaintManager.InstallPaintManager(RUNTIME_CLASS( SELECTED_PAINT_MANAGER ) );

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

// CxyplotdlgApp initialization

BOOL CXYPlotDlgApp::InitInstance()
{
	CWinApp::InitInstance();	
	
	AfxInitRichEdit();

	SetupUiAdvancedOptions();

	return TRUE;
}
