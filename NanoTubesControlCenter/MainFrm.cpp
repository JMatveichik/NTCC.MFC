// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "NanoTubesControlCenter.h"
#include "MainFrm.h"

#include "winmessages.h"
#include "ah_project.h"
#include "ah_msgprovider.h"


#include "Automation/DataCollector.h"
#include "PopupMessageWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame
LPCTSTR CMainFrame::lpszMainClassName = NULL;

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

IMPLEMENT_CExtPmBridge_MEMBERS( CMainFrame )

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	ON_WM_CREATE()
	ON_WM_MOVING()
	ON_WM_SIZING()
	
	ON_COMMAND_EX(ID_VIEW_LOG_BAR, OnBarCheck)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LOG_BAR, OnUpdateControlBarMenu)

	ON_COMMAND_EX(ID_VIEW_UI_LOOK_BAR, OnBarCheck)
	ON_UPDATE_COMMAND_UI(ID_VIEW_UI_LOOK_BAR, OnUpdateControlBarMenu)

	ON_COMMAND(ID_VIEW_DEBUG_WND, OnViewDebugWnd)
	
	ON_MESSAGE( RM_SHOW_USER_MESSAGE, OnMessageComming )
	
END_MESSAGE_MAP()

/*
static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};
*/

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
 	::memset( &m_dataFrameWP, 0, sizeof(WINDOWPLACEMENT) );
 	m_dataFrameWP.length = sizeof(WINDOWPLACEMENT);
	m_dataFrameWP.showCmd = SW_HIDE;
	m_dataFrameWP.ptMaxPosition = CPoint(0,0);
	m_dataFrameWP.ptMinPosition = CPoint(0,0);
	m_dataFrameWP.flags = WPF_RESTORETOMAXIMIZED|WPF_SETMINPOSITION;

	SystemParametersInfo(SPI_GETWORKAREA,0, (LPVOID)&m_dataFrameWP.rcNormalPosition, 0);
	

	CWinApp * pApp = ::AfxGetApp();
	ASSERT( pApp != NULL );
	ASSERT( pApp->m_pszRegistryKey != NULL );
	ASSERT( pApp->m_pszRegistryKey[0] != _T('\0') );
	ASSERT( pApp->m_pszProfileName != NULL );
	ASSERT( pApp->m_pszProfileName[0] != _T('\0') );


	PmBridge_Install(); // Subscribe

	if( ! g_PaintManager.PaintManagerStateLoad( pApp->m_pszRegistryKey, pApp->m_pszProfileName, pApp->m_pszProfileName )	)
		VERIFY(g_PaintManager.InstallPaintManager(RUNTIME_CLASS( SELECTED_PAINT_MANAGER ) ) );


	HCURSOR hCur = ::LoadCursor(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_CUR_ARROW));
	m_strPopupMsgWndClass = ::AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, hCur);

}

CMainFrame::~CMainFrame()
{
	PmBridge_Uninstall();
	m_wndMdiTabs.DestroyWindow();	
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CWinApp * pApp = ::AfxGetApp();
	ASSERT( pApp != NULL );
	ASSERT( pApp->m_pszRegistryKey != NULL );
	ASSERT( pApp->m_pszRegistryKey[0] != _T('\0') );
	ASSERT( pApp->m_pszProfileName != NULL );
	ASSERT( pApp->m_pszProfileName[0] != _T('\0') );
	pApp;

	if (CExtNCW < CMDIFrameWnd >::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Prof-UIS command manager profile
	VERIFY(g_CmdManager->ProfileSetup( __PROF_UIS_PROJECT_CMD_PROFILE_NAME, GetSafeHwnd() ) );	
	VERIFY(g_CmdManager->UpdateFromToolBar( __PROF_UIS_PROJECT_CMD_PROFILE_NAME, IDR_DS_VIEW_TOOL_BAR ) );	
	
/*
	static struct
	{
		UINT m_nCmdID;
		LPCTSTR m_sToolbarText;
		LPCTSTR m_sToolTipText;

	} arrToolbarCmdTexts[] =
	{
		{ ID_DS_USER_LAYOUT, _T("Пользовательский"),  _T("Просмотр списка источников данных в виде задданном пользователем")},
		{ ID_DS_BY_DEVICE_LAYOUT, _T("По устройствам"), _T("Просмотр списка источников данных по устройствам сети") },
		{ ID_DS_BY_TYPE_LAYOUT, _T("По типам"), _T("Просмотр списка источников данных по типу")},
		{ ID_DS_BY_SCRIPT_LAYOUT, _T("По скриптам"), _T("Просмотр списка источников данных по управляющим алгоритмам")}		
	};
*/
	static struct
	{
		UINT m_nCmdID;
		LPCTSTR m_sToolbarText;
		LPCTSTR m_sToolTipText;

	} arrToolbarCmdTexts[] =
	{
		{ ID_DS_USER_LAYOUT, _T("User defined"),  _T("View inner veriables list defined by user")},
		{ ID_DS_BY_DEVICE_LAYOUT, _T("By devices"), _T("View inner veriables list defined by devices") },
		{ ID_DS_BY_TYPE_LAYOUT, _T("By type"), _T("View inner veriables list defined by type")},
		{ ID_DS_BY_SCRIPT_LAYOUT, _T("By scripts"), _T("View inner veriables list defined by OBA Scripts")}		
	};

	for( int i = 0; i < sizeof(arrToolbarCmdTexts)/(sizeof(arrToolbarCmdTexts[0])); i++ )
	{
		CExtCmdItem * pCmdItem = g_CmdManager->CmdGetPtr( __PROF_UIS_PROJECT_CMD_PROFILE_NAME, arrToolbarCmdTexts[i].m_nCmdID );
		ASSERT( pCmdItem != NULL );
		
		pCmdItem->m_sToolbarText =  arrToolbarCmdTexts[i].m_sToolbarText;
		pCmdItem->m_sTipTool = arrToolbarCmdTexts[i].m_sToolTipText;
	}

	//////////////////////////////////////////////////////////////////////////
	/// CREATE MDI TAB WINDOW
	if( !m_wndMdiTabs.Create( this ) )
	{
		ASSERT( FALSE );
		return -1;
	}

	m_wndMdiTabs.ModifyTabWndStyle(
		__ETWS_SHOW_BTN_CLOSE,
		0 /*__ETWS_FULL_WIDTH|__ETWS_EQUAL_WIDTHS|__ETWS_CENTERED_TEXT*/,
		true
		);
	m_wndMdiTabs.ModifyTabWndStyleEx(
		0,
		__ETWS_EX_NO_PREFIX|__ETWS_EX_EXTENDED_ITEM_DRAGGING|__ETWS_EX_TAB_SHAPE_ITEM_DRAGGING
		);

	//////////////////////////////////////////////////////////////////////////

#ifdef DEBUG_OUT_WND

	if(		!m_wndLogBar.Create( _T("Messages"), this, ID_VIEW_LOG_BAR )
		||	!m_wndLogCtrl.Create( &m_wndLogBar) )
	{
		ASSERT( FALSE );
		return -1;
	}
	
	RecalcLayout();

	static CSize
		_sizeInitH( 800, 600 ),
		_sizeInitV( 600, 100),
		_sizeInitF( 600, 250);

 	m_wndLogBar.SetInitDesiredSizeHorizontal( _sizeInitH );
 	m_wndLogBar.SetInitDesiredSizeVertical( _sizeInitV );
 	m_wndLogBar.SetInitDesiredSizeFloating( _sizeInitF );

	CString info;
	info.Format("%s version %s", NTCCProject::Instance().Name().c_str(), NTCCProject::Instance().Version().c_str());
	m_wndLogCtrl.WriteText(true, 0x808000, info);
	m_wndLogBar.EnableDocking(CBRS_ALIGN_ANY);
	

	
#endif

	// Enable control bars in the frame window to be redocable
	//VERBOSE
	if( !CExtControlBar::FrameEnableDocking(this) )
	{
		ASSERT( FALSE );
		return -1;
	}

	// Enable autohide feature for resizable control bars
	//VERBOSE
	if( !CExtControlBar::FrameInjectAutoHideAreas(this) )
	{
		ASSERT( FALSE );
		return -1;
	}

#ifdef DEBUG_OUT_WND

	
	m_wndLogBar.DockControlBar(AFX_IDW_DOCKBAR_LEFT,false, false );	
	m_wndLogBar.DockControlBarInnerOuter(AFX_IDW_DOCKBAR_LEFT, false, this );

	//m_wndLogBar.FloatControlBar( CPoint(20,600) );
	m_wndLogBar.GetParentFrame()->RecalcLayout();
		
	m_wndLogBar.AutoHideModeSet(true, false, true, false);

#endif

#ifdef USE_ITEMS_PROPERTY_STORE
	//создание панели свойств только если в проект в режиме отладки 

	if ( NTCCProject::Instance().IsDebugMode() )
	{
		if ( !initPropertyesBar() )
			return FALSE;

	}

#endif

#ifndef __USE_PROFUIS_TAB_MDI_ONE_NOTE_WND

	m_wndMdiTabs.SetFont(16, "Segoe UI", false);
	m_wndMdiTabs.SetFont(16, "Segoe UI", true);

	m_wndMdiTabs.SetShift(2);
	m_wndMdiTabs.SetSize(36);	

#endif



	RecalcLayout();

	if(		(! m_wndToolBarUiLook.Create( NULL, this, ID_VIEW_UI_LOOK_BAR ) )
		||	(! m_wndToolBarUiLook.ThemeSwitcherInit() )
		)
	{
		//TRACE0("Failed to create m_wndToolBarUiLook toolbar\n");
		return -1;      // fail to create
	}

	m_wndToolBarUiLook.EnableDocking( CBRS_ALIGN_LEFT );
	DockControlBar( &m_wndToolBarUiLook, AFX_IDW_DOCKBAR_LEFT );
	ShowControlBar(&m_wndToolBarUiLook,  FALSE, FALSE );


	//CExtControlBar::ProfileBarStateLoad( this, pApp->m_pszRegistryKey, pApp->m_pszProfileName, pApp->m_pszProfileName, &m_dataFrameWP );
	//g_CmdManager->SerializeState( __PROF_UIS_PROJECT_CMD_PROFILE_NAME, pApp->m_pszRegistryKey, pApp->m_pszProfileName, false );

	PmBridge_OnPaintManagerChanged( g_PaintManager.GetPM( ) );

	return 0;
}

#ifdef USE_ITEMS_PROPERTY_STORE
BOOL CMainFrame::initPropertyesBar()
{
	
	if(	!m_wndPropBar.Create( _T("Свойства"), this, ID_VIEW_PROP_WND ) )
	{
		//TRACE0("Failed to create m_wndPropBar\n");
		return FALSE;		// fail to create
	}

	if( !m_PGC.Create(&m_wndPropBar, m_wndPropBar.GetDlgCtrlID(), CRect(0,0,0,0), WS_CHILD|WS_VISIBLE  ) )
	{
		//TRACE0("Failed to create m_PGC\n");
		return FALSE;		// fail to create
	}


	static CSize
		_sizeInitH( 250, 400 ),
		_sizeInitV( 250, 100),
		_sizeInitF( 600, 250);

	m_wndPropBar.SetInitDesiredSizeHorizontal( _sizeInitH );
	m_wndPropBar.SetInitDesiredSizeVertical( _sizeInitV );
	m_wndPropBar.SetInitDesiredSizeFloating( _sizeInitF );

	m_wndPropBar.EnableDocking(CBRS_ALIGN_ANY);

	m_wndPropBar.DockControlBarInnerOuter(AFX_IDW_DOCKBAR_BOTTOM, false, this );

	m_wndPropBar.AutoHideModeSet(true, false, true, true);


	m_PropertyStoreCompoundSelection.NameSet( _T("Выделенные элементы") );
	m_PropertyStoreAll.NameSet( _T("Все элементы") );


	CExtPropertyGridComboBoxBar * pCombo =
		STATIC_DOWNCAST( CExtPropertyGridComboBoxBar, m_PGC.GetChildByRTC( RUNTIME_CLASS(CExtPropertyGridComboBoxBar) ) );

	ASSERT_VALID( pCombo );
	pCombo->PropertyStoreInsert( &m_PropertyStoreCompoundSelection );
	pCombo->PropertyStoreInsert( &m_PropertyStoreAll );



	////////////////////////////////////////////////////////////////////
	CTypedPtrArray < CPtrArray, CExtPropertyGridWnd * > arrGrids;
	m_PGC.OnPgcQueryGrids( arrGrids );

	for(INT nGridIdx = 0; nGridIdx < arrGrids.GetSize(); nGridIdx ++ )
	{
		//XP Tree Boxes in grid
		CExtPropertyGridWnd * pGrid = arrGrids[ nGridIdx ];		
		g_PaintManager->LoadWinXpTreeBox( pGrid->m_iconTreeBoxExpanded, true );
		g_PaintManager->LoadWinXpTreeBox( pGrid->m_iconTreeBoxCollapsed, false );	

		//Sorted categories
		pGrid->m_bSortedCategories = true;

		//Sorted values
		pGrid->m_bSortedValues = false;

		//Highlight ReadOnly Values
		pGrid->m_bHighlightReadOnlyValues = true;

		//Make Bold Modified Names
		pGrid->m_bMakeBoldModifiedNames = true;

		//Make Bold Modified Names
		pGrid->m_bMakeBoldModifiedValues = true;

		pGrid->m_bDrawFocusRect = false;

		pGrid->m_bFullRowSelection = true;

		pGrid->m_bThemeBackground = true;

	} // for( ; nGridIdx < arrGrids.GetSize(); nGridIdx ++ )

	//////////////////////////////////////////////////////
	///Ballon toolbar in property grid
	CExtPropertyGridToolBar * pWnd = STATIC_DOWNCAST( CExtPropertyGridToolBar, m_PGC.GetChildByRTC( RUNTIME_CLASS(CExtPropertyGridToolBar) ) );
	if( pWnd != NULL )
		pWnd->m_bForceBalloonGradientInDialogs = true;

	::SetClassLong(m_hWnd, GCL_HCURSOR, (LONG)AfxGetApp()->LoadCursor(IDC_CUR_ARROW));

	return TRUE;
}
#endif

void CMainFrame::PmBridge_OnPaintManagerChanged( CExtPaintManager * pGlobalPM )
{
	if ( !m_hWnd )
		return;

	LOGFONT lf;	

	g_PaintManager->m_FontNormal.GetLogFont(&lf);
	CDC* pDC = GetDC();

	lf.lfHeight = -MulDiv(12, GetDeviceCaps(pDC->m_hDC, LOGPIXELSY), 72);
	strcpy_s(lf.lfFaceName, "Segoe UI");

	ReleaseDC(pDC);

	//////////////////////////////////////////////////
	//Normal font

	g_PaintManager->m_FontNormal.DeleteObject();
	g_PaintManager->m_FontNormal.CreateFontIndirect(&lf);

	//////////////////////////////////////////////////
	//Bold font
	lf.lfWeight = FW_SEMIBOLD;

	g_PaintManager->m_FontBold.DeleteObject();
	g_PaintManager->m_FontBold.CreateFontIndirect(&lf);

}

BOOL CMainFrame::DestroyWindow() 
{
CWinApp * pApp = ::AfxGetApp();
	ASSERT( pApp != NULL );
	ASSERT( pApp->m_pszRegistryKey != NULL );
	ASSERT( pApp->m_pszRegistryKey[0] != _T('\0') );
	ASSERT( pApp->m_pszProfileName != NULL );
	ASSERT( pApp->m_pszProfileName[0] != _T('\0') );
	pApp;

	//VERIFY( CExtControlBar::ProfileBarStateSave( this, pApp->m_pszRegistryKey, pApp->m_pszProfileName, pApp->m_pszProfileName, &m_dataFrameWP ) );
	VERIFY( g_CmdManager->SerializeState( __PROF_UIS_PROJECT_CMD_PROFILE_NAME, pApp->m_pszRegistryKey, pApp->m_pszProfileName, true ) );

	VERIFY( g_PaintManager.PaintManagerStateSave( pApp->m_pszRegistryKey, pApp->m_pszProfileName, pApp->m_pszProfileName ) );

	g_CmdManager->ProfileWndRemove( GetSafeHwnd() );
	
	return CExtNCW < CMDIFrameWnd >::DestroyWindow();
}

void CMainFrame::ActivateFrame(int nCmdShow) 
{
//  	if( m_dataFrameWP.showCmd != SW_HIDE )
//  	{
// 		SetWindowPlacement( &m_dataFrameWP );
// 		CExtNCW < CMDIFrameWnd >::ActivateFrame( m_dataFrameWP.showCmd );
// 		m_dataFrameWP.showCmd = SW_HIDE;
// 		return;
// 	}
// 	nCmdShow = SW_MAXIMIZE;
// 
// 	m_wndMdiTabs.UpdateTabWnd(true);

	CExtNCW < CMDIFrameWnd >::ActivateFrame(nCmdShow);
}

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext)
{
    return CFrameWnd::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext);
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	return CreateClient(lpcs, NULL);
} 


BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if (lpszMainClassName == NULL) 
	{
		
		if( !CExtNCW < CMDIFrameWnd >::PreCreateWindow(cs) )
			return FALSE;

		WNDCLASS wc;
		
		//пытаемся получить информацию о классе
		//если не удается, выходим с ошибкой
		if (!::GetClassInfo(AfxGetInstanceHandle(), cs.lpszClass, &wc)) 
			return FALSE;

		// теперь изменяем в wc все, что нужно
		// например, стиль класса
		wc.style |= CS_OWNDC;

		wc.hCursor = ::LoadCursor(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_CUR_ARROW));
		// регистрируем класс
		lpszMainClassName = AfxRegisterWndClass(wc.style, wc.hCursor, wc.hbrBackground, wc.hIcon);
	}

	if(cs.hMenu != NULL)
    {
         ::DestroyMenu(cs.hMenu);      // удаляем меню, если загружено
         cs.hMenu = NULL;              // для этого окна меню нет
     }

	CRect rc;
	SystemParametersInfo(SPI_GETWORKAREA,0, (LPVOID)&rc, 0);

	cs.lpszClass = lpszMainClassName;

	cs.cx = rc.Width();
	cs.cy = rc.Height();

	cs.style = WS_OVERLAPPED | WS_THICKFRAME  /*| FWS_ADDTOTITLE  | WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU*/;		
	cs.style &= ~WS_VISIBLE;

	return CExtNCW < CMDIFrameWnd >::PreCreateWindow(cs);
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	 CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	 CExtNCW<CMDIFrameWnd>::Dump(dc);
}

#endif //_DEBUG

// CMainFrame message handlers

 //CONTAINER || CONTAINER_SERVER
BOOL CMainFrame::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_NCLBUTTONDBLCLK)
		return TRUE;

	if ( pMsg->message == WM_SYSKEYDOWN )
	{
		if ( pMsg->wParam == VK_MENU )
			return TRUE;		
	}
	
	if( pMsg->message == WM_MDIDESTROY || pMsg->message == WM_NCDESTROY )
		m_wndMdiTabs.PreTranslateMessage(pMsg);

	//m_wndMdiTabs.PreTranslateMessage(pMsg);
	return CExtNCW < CMDIFrameWnd >::PreTranslateMessage(pMsg);
}

void CMainFrame::OnViewDebugWnd()
{
	if (pProjectFrm != NULL)
	{
		if ( pProjectFrm->IsWindowVisible() ) {
			pProjectFrm->ActivateFrame(SW_HIDE);
			pProjectFrm->ShowWindow(SW_HIDE);
		}
		else
			pProjectFrm->ActivateFrame(SW_NORMAL);
		
	}
}

void CMainFrame::OnUpdateControlBarMenu(CCmdUI* pCmdUI)
{
	CExtControlBar::DoFrameBarCheckUpdate( this, pCmdUI, false );
}

BOOL CMainFrame::OnBarCheck(UINT nID)
{
	return CExtControlBar::DoFrameBarCheckCmd( this, nID, false );
}

void CMainFrame::OnMoving(UINT fwSide, LPRECT pRect)
{
// 	RECT rc;
// 	GetWindowRect(&rc);
// 	*pRect = rc;

	CExtNCW<CMDIFrameWnd>::OnMoving(fwSide, pRect);	
}

void CMainFrame::OnSizing(UINT fwSide, LPRECT pRect)
{
// 	RECT rc;
// 	GetWindowRect(&rc);
// 	*pRect = rc;
	
	CExtNCW<CMDIFrameWnd>::OnSizing(fwSide, pRect);	
}

LRESULT CMainFrame::OnMessageComming( WPARAM wParam, LPARAM lParam)
{
	COutMessage* pMsg = (COutMessage*)lParam;

	if ( wParam  == 1)
	{
		CRect rc;
		GetWindowRect(&rc);

		int xOffset = 20;

		CPoint ptTopLeft	 = rc.BottomRight();
		ptTopLeft.Offset(-400, -150);

		CPoint ptBottomRight = rc.BottomRight();	
		rc.SetRect(ptTopLeft, ptBottomRight);
		rc.OffsetRect(-20, -20);

		CPopupMessageWnd* msgWnd = new CPopupMessageWnd();

		if ( msgWnd->CreateEx(WS_EX_TOPMOST|WS_EX_NOPARENTNOTIFY, m_strPopupMsgWndClass, "Message popup", WS_POPUPWINDOW, rc, this, IDC_STATIC ) )	
		{
			msgWnd->ShowMessage(pMsg);					
		}			
	}

#ifdef DEBUG_OUT_WND

	CString msg;
	CTime tm = CTime::GetCurrentTime();

	if (pMsg->UseTimePreFix() )
	{
		msg = tm.Format("%H:%M:%S > ");
		if ( strlen(pMsg->MessageTitle()) != 0)
		{
			msg += pMsg->MessageTitle();
			msg += " > ";
		}	
	}
	msg += pMsg->MessageText();

	COLORREF clrs[] = 
	{
		0x000000,
		0x800080,
		0x000080,
		0x008000, 
		0x000000
	};

	m_wndLogCtrl.WriteText(false, clrs[pMsg->MessageType()], msg, pMsg->BreakLine());

#endif

	delete pMsg;
	
	return 0L;
}

void CMainFrame::PostNcDestroy()
{
	CExtNCW<CMDIFrameWnd>::PostNcDestroy();
}

void CMainFrame::_CombineMixedStoreAll()
{
	/*
	m_PropertyStoreAll.ItemRemove();
	CFSDoc* pDoc = (CFSDoc*)CMDIChildWnd::GetActiveDocument();

	CExtPropertyGridComboBoxBar * pCombo =
		STATIC_DOWNCAST( CExtPropertyGridComboBoxBar, m_PGC.GetChildByRTC( RUNTIME_CLASS(CExtPropertyGridComboBoxBar) ) );

	if ( pDoc != NULL )
	{
		CDrawObjList* pSel = pDoc->GetItems();
		POSITION pos = pSel->GetHeadPosition();

		while(pos != NULL)
		{
			CDrawObj* pDO = pSel->GetNext(pos);
			CRect rc;
			pDO->GetPosition(rc);

			////TRACE("\n\t--> %s at [%04d;%04d] ", pDO->GetThisClass()->m_lpszClassName, rc.left, rc.top);
			CExtPropertyStore& ps = pDO->GetPropertyStore();
			m_PropertyStoreAll.Combine( &ps);
			pCombo->PropertyStoreInsert( &ps);
		}

	}



	if( pCombo == NULL )
		return;

	ASSERT_VALID( pCombo );
	if( pCombo->GetCurSel() == 1 )
		m_PGC.PropertyStoreSynchronize();
	*/
}

void CMainFrame::_CombineMixedStoreSelection()
{
	/*
	m_PropertyStoreCompoundSelection.ItemRemove();

	CFSDoc* pDoc = (CFSDoc*)CMDIChildWnd::GetActiveDocument();

	if ( pDoc != NULL )
	{
		CDrawObjList* pSel = pDoc->GetSelectedItems();
		POSITION pos = pSel->GetHeadPosition();

		while(pos != NULL)
		{
			CDrawObj* pDO = pSel->GetNext(pos);
			m_PropertyStoreCompoundSelection.Combine( &pDO->GetPropertyStore() );
		}

	}

	if( m_PGC.IsPrintPreviewMode() )
	{
		m_PGC.PropertyStoreSynchronize();
		m_PGC.DoReGenerateReport();
	} // if( m_PGC.IsPrintPreviewMode() )
	else
	{
		CExtPropertyGridComboBoxBar * pCombo =
			STATIC_DOWNCAST( CExtPropertyGridComboBoxBar, m_PGC.GetChildByRTC( RUNTIME_CLASS(CExtPropertyGridComboBoxBar) ) );

		if( pCombo != NULL )
		{
			ASSERT_VALID( pCombo );
			if( pCombo->GetCurSel() == 0 )
			{
				m_PGC.SetRedraw( FALSE );
				m_PGC.PropertyStoreSynchronize();
				m_PGC.SetRedraw( TRUE );
				m_PGC.RedrawWindow( NULL, NULL, RDW_INVALIDATE|RDW_ERASE|RDW_ALLCHILDREN );
			} // if( pCombo->GetCurSel() == 0 )
		} // if( pCombo != NULL )
	} // else from if( m_PGC.IsPrintPreviewMode() )

	CExtPropertyGridWnd * pPGW = m_PGC.GetActiveGrid();
	ASSERT_VALID( pPGW );

	HTREEITEM hti = pPGW->ItemGetRoot();
	ASSERT( hti != NULL );
	if( pPGW->ItemGetChildCount( hti ) > 0L )
	{
		hti = pPGW->ItemGetChildAt( hti, 0L );
		ASSERT( hti != NULL );
		pPGW->ItemFocusSet( hti );
	}
	*/
}