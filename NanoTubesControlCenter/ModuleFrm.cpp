// ModuleFrm.cpp : implementation of the CModuleFrame class
//
#include "stdafx.h"
#include "NanoTubesControlCenter.h"
#include "ModuleFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CModuleFrame

IMPLEMENT_DYNCREATE(CModuleFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CModuleFrame, CMDIChildWnd)
	ON_WM_CREATE()
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()


// CModuleFrame construction/destruction

CModuleFrame::CModuleFrame() 
{
	
	
}

CModuleFrame::~CModuleFrame()
{
	
	static int i =0;
	////TRACE("\nCLOSE %d FRAME\n", ++i);
	
	for (std::vector<CExtResizableDialog*>::iterator it = m_pOutDlgs.begin(); it != m_pOutDlgs.end(); ++it)
		(*it)->DestroyWindow();

	for (std::vector<CNoCloseControlBar*>::iterator it = m_pOutBars.begin(); it != m_pOutBars.end(); ++it)
		(*it)->DestroyWindow();
	
	
	std::for_each(m_pOutBars.begin(), m_pOutBars.end(), delete_object<CExtControlBar>());		
	std::for_each(m_pOutDlgs.begin(), m_pOutDlgs.end(), delete_object<CExtResizableDialog>());
	
	
}

BOOL CModuleFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying the CREATESTRUCT cs
// 	cs.style = WS_CHILD |  FWS_ADDTOTITLE | WS_OVERLAPPED | WS_CAPTION | WS_MAXIMIZE;
// 	cs.dwExStyle &= ~(WS_EX_CLIENTEDGE|WS_EX_STATICEDGE|WS_EX_DLGMODALFRAME);
// 	
// 	cs.style &= ~WS_VISIBLE;
	
	if(cs.hMenu != NULL)
	{
		::DestroyMenu(cs.hMenu);      // удаляем меню, если загружено
		cs.hMenu = NULL;              // для этого окна меню нет
	}

	return CMDIChildWnd::PreCreateWindow(cs);
}



// CModuleFrame diagnostics

#ifdef _DEBUG
void CModuleFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CModuleFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

// CModuleFrame message handlers

int CModuleFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if( !CExtControlBar::FrameEnableDocking(this) )
	{
		ASSERT( FALSE );
		return -1;
	}

	if( !CExtControlBar::FrameInjectAutoHideAreas(this) )
	{
		ASSERT( FALSE );
		return -1;
	}



	CExtDynamicBarSite::Install( this );	
	return 0;
}


BOOL CModuleFrame::AddControlBar(CONTROLBAR_INFO& bi)
{
	//ВЫДЕЛЯЕМ КОМАНДУ
	CExtCmdItem * pCmdItem = g_CmdManager->CmdAllocPtr( __PROF_UIS_PROJECT_CMD_PROFILE_NAME );
	ASSERT( pCmdItem != NULL );

	pCmdItem->m_sTipTool   = bi.title.c_str();
	pCmdItem->m_sTipStatus = bi.title.c_str();
	pCmdItem->m_sMenuText  = bi.title.c_str();
	pCmdItem->m_sToolbarText = _T("");		

	//СОЗДАЕМ ПАНЕЛЬ
	std::auto_ptr<CNoCloseControlBar> pOutDSBar( new CNoCloseControlBar() );	

	if(	!pOutDSBar->Create( bi.title.c_str(), this, pCmdItem->m_nCmdID, WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS
		|CBRS_TOP|CBRS_GRIPPER|CBRS_TOOLTIPS
		|CBRS_FLYBY|CBRS_SIZE_DYNAMIC
		|CBRS_HIDE_INPLACE) )
	{
		////TRACE("Ошибка создания панели управления %s\n", bi.title.c_str() );		
		return FALSE;		// fail to create
	}
	
	///////////////////////////////////////////////////
	

	const CExtCmdIcon* pIcon = AppImagesHelper::Instance().GetIcon( bi.iconPath, ICON_SIZE_32 );
	VERIFY(g_CmdManager->CmdSetIcon( __PROF_UIS_PROJECT_CMD_PROFILE_NAME, pCmdItem->m_nCmdID, pIcon, false ));

	
	if ( bi.cbitems.empty() )
		return FALSE;

	//создание блока диалога 
	CAutoCalcSizeDialog* pNewDlg = NULL;

	if(bi.type == "GASPANEL_BAR")	//ГАЗОВАЯ ПАНЕЛЬ
	{
		/*
		OmegaGasPanel* pPanel = OmegaGasPanel::FindGasPanel( bi.ids[0] );
		ASSERT ( pPanel != NULL );	
		

		std::auto_ptr<CGasPanelDialog> pGasPanelDlg (new CGasPanelDialog(pOutDSBar.get(),  pPanel) );	

		if( ! pGasPanelDlg->Create( IDD_GAS_PANEL_DIALOG, pOutDSBar.get()) )
		{
			//TRACE("Ошибка создания диалога газовой панели для модуля %s\n", bi.title.c_str());
			return FALSE;		// fail to create
		}
		pNewDlg = pGasPanelDlg.release();
		*/
	}
	else if ( bi.type == "ANALOG_OUTPUT_BAR" ) //УПРАВЛЕНИЕ ВЫХОДНЫМИ АНАЛОГОВЫМИ ИСТОЧНИКАМИ ДАННЫХ
	{
		std::auto_ptr<COutputDSControlDlg> pOutDSDlg (new COutputDSControlDlg(bi, pOutDSBar.get() ) );	

		if( ! pOutDSDlg->Create( IDD_DS_OUTPUT_DLG, pOutDSBar.get()) )
		{
			////TRACE("Ошибка создания диалога управления %s\n", bi.title.c_str());
			return FALSE;		// fail to create
		}
		pNewDlg = pOutDSDlg.release();
	}

	if ( pNewDlg == NULL )
		return FALSE;

	//pNewDlg->ShowWindow(SW_SHOW);

	
	//ПОЛУЧИТЬ МИНИМАЛЬНЫЕ РАЗМЕРЫ ДИАЛОГА
	CSize sz = pNewDlg->GetDlgMinSize();
	sz.cy += GetSystemMetrics(SM_CYSMCAPTION) + 0;

	pOutDSBar->SetInitDesiredSizeHorizontal( sz );
	pOutDSBar->SetInitDesiredSizeVertical(  sz );
	pOutDSBar->SetInitDesiredSizeFloating( sz );	

  	pOutDSBar->FrameEnableDocking(this);
  	pOutDSBar->EnableDocking( CBRS_ALIGN_TOP );
	

	pOutDSBar->DockControlBarInnerOuter(AFX_IDW_DOCKBAR_TOP,  true, this, true);		
 	pOutDSBar->AutoHideModeSet(true, false, true, true);

	pOutDSBar->AutoHideHeightSet(sz.cy);	
	

	RecalcLayout();

	m_pOutBars.push_back( pOutDSBar.release() );
	m_pOutDlgs.push_back( pNewDlg );
	
	return TRUE;
}

void CModuleFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	
	CFSDoc* pDoc = (CFSDoc*)CMDIChildWnd::GetActiveDocument();
	if ( pDoc != NULL )
	{
		m_strTitle = pDoc->GetTitle();
		HICON hIcon = pDoc->GetDocumentIcon();
		if (hIcon != NULL)
			SetIcon(hIcon, false);		
	}

	__super::OnUpdateFrameTitle(bAddToTitle);
}

LRESULT CModuleFrame::WindowProc(UINT message,  WPARAM wParam, LPARAM lParam) 
{
	if ( message == WM_SIZE || message == WM_SIZING )
	{
		SetRedraw(FALSE);
		LRESULT ret = CMDIChildWnd::WindowProc(message, wParam, lParam);
		SetRedraw();

		RedrawWindow( 0, 0, RDW_FRAME|RDW_INVALIDATE|RDW_UPDATENOW|RDW_ALLCHILDREN);

		return   ret;
	}
    return CMDIChildWnd::WindowProc(message, wParam, lParam);
}

void CModuleFrame::ActivateFrame(int nCmdShow)
{
 	if (nCmdShow != SW_HIDE)
	     nCmdShow = SW_SHOWMAXIMIZED;
	
	CMDIChildWnd::ActivateFrame(nCmdShow);	
}



