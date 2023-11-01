// ProjectView.cpp : implementation file
//

#include "stdafx.h"
#include "NanoTubesControlCenter.h"
#include "ProjectView.h"
#include "ProjectDoc.h"
#include "OutMessage.h"

#include "ah_images.h"
#include "ah_project.h"

#ifdef _DEBUG
#define new DEBUG_NEW	
#endif
// CProjectView

IMPLEMENT_DYNCREATE(CProjectView, CView)

CProjectView::CProjectView()
{

}

CProjectView::~CProjectView()
{
	CFrameWnd* pFrame = (CFrameWnd*)AfxGetMainWnd();

	if (pFrame->GetActiveView() == this)
		pFrame->SetActiveView(0, FALSE);
}

BEGIN_MESSAGE_MAP(CProjectView, CView)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CProjectView diagnostics

#ifdef _DEBUG
void CProjectView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CProjectView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CProjectView message handlers

BOOL CProjectView::InitScriptsPage()
{
// 	if ( !m_dlgScripts.Create(IDD_SCRIPTS_DLG, &m_container) )
// 	{
// 		////TRACE("\tScripts dialog page creation error!\n");
// 		return FALSE;
// 	}
	
	return TRUE;
}

BOOL CProjectView::InitLoadingListPage()
{
/*
	if ( !m_wndLoadList.Create(WS_CHILD|WS_VISIBLE|LVS_REPORT, CRect(0,0,0,0), &m_container, (UINT)IDC_STATIC))
	{
		////TRACE("\tInit loading list page creation error!\n");
		return FALSE;
	}
	
	m_wndLoadList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_FLATSB);

	LV_COLUMN lvc;
	memset(&lvc, 0, sizeof(LV_COLUMN));

	lvc.mask = LVCF_TEXT|LVCF_WIDTH|LVCF_FMT;
	
	lvc.iSubItem = 0; lvc.pszText = "#"; lvc.fmt = LVCFMT_LEFT; lvc.cx = 40;
	m_wndLoadList.InsertColumn(0, &lvc);

	lvc.iSubItem = 1; lvc.pszText = "Время"; lvc.fmt = LVCFMT_LEFT; lvc.cx = 80;
	m_wndLoadList.InsertColumn(1, &lvc);

	lvc.iSubItem = 2; lvc.pszText = "Сообщение"; lvc.fmt = LVCFMT_LEFT; lvc.cx = 800;
	m_wndLoadList.InsertColumn(2, &lvc);
	*/
	return TRUE;
}

BOOL CProjectView::InitTerminalPage()
{
/*	if ( !m_dlgTerminal.Create(IDD_TERMINAL_DLG, &m_container) )
	{
		//TRACE("\tTerminal page creation error!\n");
		return FALSE;
	}
	*/
	return TRUE;
}
BOOL CProjectView::InitIndustrialNetListPage()
{
	
	if (!m_wndNetList.Create(&m_container, CRect(0, 0, 0, 0), (UINT)IDC_STATIC))
	{
		//TRACE("\tCExtTreeGridWnd creation error!\n");
		return FALSE;
	}
	
	m_wndNetList.SiwModifyStyle( 
		__ESIS_STH_PIXEL|
		__ESIS_STV_ITEM |
		__EGBS_SFB_FULL_ROWS|
		
		__EGBS_GRIDLINES_V|
		__EGBS_RESIZING_CELLS_MASK|
		__EGBS_RESIZING_CELLS_OUTER_H |
		//__EGBS_RESIZING_CELLS_INNER_H |
		__EGBS_DYNAMIC_RESIZING
		//__EGBS_RESIZING_CELLS_OUTER_V
		, 0, true);

	m_wndNetList.SiwModifyStyleEx( 
		__EGBS_EX_HVI_HIGHLIGHT_ROWS|
		__EGBS_EX_CELL_TOOLTIPS|
		__EGBS_EX_CORNER_AREAS_3D|
		__EGBS_EX_CORNER_AREAS_CURVE|
		__EGBS_EX_CELL_EXPANDING_INNER
		, 0, true);
	
	m_wndNetList.BseModifyStyleEx(
		__EGBS_BSE_EX_DBLCLK_BEST_FIT_VISIBLE_ROWS|
		//__EGBS_BSE_EX_PROPORTIONAL_COLUMN_WIDTHS|		
		//__EGBS_BSE_EX_PROPORTIONAL_ROW_HEIGHTS|		
		__EGBS_BSE_EX_AUTO_BEST_FIT_ROWS
		, 
		0, 
		true );

	/*static LPCTSTR arrHdrNames[] = 
	{	_T("Идентификатор устройства"),
		_T("Производитель"),
		_T("Модель"),
		_T("Описание"),
	};*/

	static LPCTSTR arrHdrNames[] = 
	{	
	_T("Device ID"),
	_T("Manufacturer"),
	_T("Model"),
	_T("Description"),
	};

	static double arrHdrExtentsPercent[] = { 0.5, 0.25, 0.25};
	static int    arrHdrExtents[] = { 350, 250, 250, 450};

	LONG nColCount = sizeof(arrHdrNames) / sizeof(arrHdrNames[0]);

	m_wndNetList.ColumnInsert(0, nColCount);
	m_wndNetList.OuterColumnCountLeftSet( 1 );
	m_wndNetList.OuterColumnCountRightSet( 0 );
	m_wndNetList.OuterRowCountTopSet( 1 );
	m_wndNetList.FixedSizeColumnsSet(false);

	m_wndNetList.DefaultRowHeightSet(32);

	for( LONG nColNo = 0L; nColNo < nColCount; nColNo++ )
	{
		CExtGridCellHeaderFilter * pHeaderFilterCell = STATIC_DOWNCAST( CExtGridCellHeaderFilter, 
			m_wndNetList.GridCellGetOuterAtTop( nColNo, 0L, RUNTIME_CLASS(CExtGridCellHeaderFilter)));		
		pHeaderFilterCell->ExtentPercentSet(arrHdrExtentsPercent[nColNo]);
		pHeaderFilterCell->ExtentSet(arrHdrExtents[nColNo]);

		pHeaderFilterCell->TextSet( arrHdrNames[nColNo] );
		pHeaderFilterCell->ModifyStyle( __EGCS_BUTTON_DROPDOWN, 0 );

		//pHeaderFilterCell->FontHeightSet(20);
		pHeaderFilterCell->FontWeightSet(FW_BOLD);
		pHeaderFilterCell->FontFaceNameSet("Segoe UI");

	}

	CExtGridDataProvider& data = m_wndNetList.GetTreeData();
	CExtCmdIcon* pCmdIcon = AppImagesHelper::Instance().GetIcon("disabled.ico", ICON_SIZE_24);
	data.IconInsert(pCmdIcon);

	pCmdIcon = AppImagesHelper::Instance().GetIcon("enabled.ico", ICON_SIZE_24);
	data.IconInsert(pCmdIcon);

	std::vector<const IdentifiedPhysDevice*> devs;
	IdentifiedPhysDevice::EnumDevices(devs);

	HTREEITEM hRoot = NULL;
	DWORD dwBaseStyle = __EGCS_EX_WRAP_TEXT|
						//__EGCS_READ_ONLY|
						__EGCS_NO_INPLACE_CONTROL|
						__EGCS_TA_VERT_CENTER 
						//__EGCS_ICA_HORZ_LEFT|
						//__EGCS_ICA_VERT_CENTER
	;

	INT rowNo = 0;
	for(std::vector<const IdentifiedPhysDevice*>::const_iterator it = devs.begin();it != devs.end(); ++it)
	{
		HTREEITEM hItem = m_wndNetList.ItemInsert(hRoot, ULONG(-1), 1);
		CExtSafeString txt; 

		const IdentifiedPhysDevice* pDev = (*it);
		
		//////////////////////////////////////////////////////////////////////////
		//Колонка Имя 	
		CExtGridCell* pCell = m_wndNetList.ItemGetCell(hItem, 0, -1, RUNTIME_CLASS(CExtGridCellHeader) );
		pCell->ModifyStyle(__EGCS_EX_AUTO_BEST_FIT);
		pCell->IconIndexSet(pDev->IsAlive() ? 1 : 0);

		pCell = m_wndNetList.ItemGetCell(hItem, 0, 0, RUNTIME_CLASS(CExtGridCellString) );
		
		txt = pDev->Name().c_str();
		pCell->TextSet( txt );
		pCell->LParamSet( (LPARAM)pDev );

		
		pCell->ModifyStyle( dwBaseStyle , 0);

		pCell = m_wndNetList.ItemGetCell(hItem, 1, 0, RUNTIME_CLASS(CExtGridCellString) );
		txt = pDev->Manufacturer().c_str();
		pCell->TextSet( txt );
		pCell->ModifyStyle( dwBaseStyle , 0);

		pCell = m_wndNetList.ItemGetCell(hItem, 2, 0, RUNTIME_CLASS(CExtGridCellString) );
		txt = pDev->Model().c_str();
		pCell->TextSet( txt );
		pCell->ModifyStyle( dwBaseStyle , 0);

		pCell = m_wndNetList.ItemGetCell(hItem, 3, 0, RUNTIME_CLASS(CExtGridCellString) );
		pCell->ModifyStyle( dwBaseStyle , 0);
		pCell->ModifyStyleEx( __EGCS_EX_WRAP_TEXT|__EGCS_EX_MULTILINE_TEXT|__EGCS_EX_AUTO_BEST_FIT );
		
		pCell->ExtentSet( 24, -1 ); 
		pCell->ExtentSet( 24, 0 ); 
		pCell->ExtentSet( 32, 1 ); 

		
		txt = pDev->Description().c_str();
		pCell->TextSet( txt );
		
		m_wndNetList.BestFitRow(rowNo++, 0, false, true, false, true);
	}

	
	return TRUE;
}

BOOL CProjectView::InitDataSourcesListPage()
{
	if ( !m_dlgDsView.Create(IDD_DS_VIEW_DLG, &m_container, (CProjectDoc*)GetDocument()) )
	{
		//TRACE("\tTerminal page creation error!\n");
		return FALSE;
	}

	return TRUE;
}

void CProjectView::OnInitialUpdate()
{
	CExtPageContainerWnd::g_ctAnimationStepClocks = 50;
	CExtPageContainerWnd::g_nAnimationStepCount = 0;

	if ( !m_container.Create( this, (UINT)IDC_STATIC, CRect(0,0,0,0), __EPCWS_STYLES_DEFAULT|__EPCWS_HORIZONTAL, WS_CHILD))
		return;

	std::string iconPath = NTCCProject::Instance().GetPath(NTCCProject::PATH_IMAGES, "Factory.ico");
	HICON hIcon1 = (HICON)LoadImage(NULL, iconPath.c_str(), IMAGE_ICON, 32, 32, LR_LOADFROMFILE);

	iconPath = NTCCProject::Instance().GetPath(NTCCProject::PATH_IMAGES, "gauge.ico");
	HICON hIcon2 = (HICON)LoadImage(NULL, iconPath.c_str(), IMAGE_ICON, 32, 32, LR_LOADFROMFILE);

	iconPath = NTCCProject::Instance().GetPath(NTCCProject::PATH_IMAGES, "WorkModule.ico");
	HICON hIcon3 = (HICON)LoadImage(NULL, iconPath.c_str(), IMAGE_ICON, 32, 32, LR_LOADFROMFILE);

	if ( InitDataSourcesListPage() )
		m_container.PageInsert(m_dlgDsView.GetSafeHwnd(), -1L, _T("Inner vasriables"), hIcon3, true);

	if ( InitIndustrialNetListPage() )
		m_container.PageInsert(m_wndNetList.GetSafeHwnd(), -1L, _T("Industrial net"), hIcon1, true);

	/*
	if ( InitScriptsPage() )
		m_container.PageInsert(m_dlgScripts.GetSafeHwnd(), -1L, _T("OBA скрипты"), hIcon1, true);
	
	
	if ( InitTerminalPage() )
		m_container.PageInsert(m_dlgTerminal.GetSafeHwnd(), -1L, _T("Терминал"), hIcon2, true);
		*/
	
	m_container.PageExpand(0, true, true, true);
	m_container.ShowWindow(SW_SHOWNORMAL);

	CView::OnInitialUpdate();
	// TODO: Add your specialized code here and/or call the base class
}

void CProjectView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	switch (lHint)
	{
		case CProjectDoc::UPDATE_ADD_MESSAGE:
		{
			/*COutMessage* pOutMsg = dynamic_cast<COutMessage*>(pHint);

			if ( pOutMsg != NULL)
			{
				int ind = m_wndLoadList.GetItemCount();

				m_wndLoadList.InsertItem( ind, "" );
				m_wndLoadList.SetItemText(ind , 2, pOutMsg->MessageText() );

				m_wndLoadList.UpdateWindow();

				delete pOutMsg;
			}
			*/
		}
		break;

		case CProjectDoc::UPDATE_LOAD_COMPLETE:
		
			//m_dlgTerminal.UpdateCommandManagersList();
			m_dlgDsView.SetViewLayout(CProjectDoc::VIEW_DS_BY_TYPE_LAYOUT);
			//m_dlgScripts.UpdateScriptsList();

			break;
	}
	CView::OnUpdate(pSender, lHint, pHint);
}

void CProjectView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if ( m_container.GetSafeHwnd() != NULL )
	{	
		CRect rcClient;
		GetClientRect(&rcClient);
		m_container.MoveWindow(&rcClient, true);
	}
}
