// DataSrcViewDlg.cpp : implementation file
//

#include "stdafx.h"
#include "..\NanoTubesControlCenter.h"
#include "DataSrcViewDlg.h"

#include "..\ah_images.h"

#ifdef _DEBUG
#define new DEBUG_NEW	
#endif
// CDataSrcViewDlg dialog

#define ROW_HEIGHT 30

IMPLEMENT_DYNAMIC(CDataSrcViewDlg, CExtResizableDialog)

CDataSrcViewDlg::CDataSrcViewDlg(CWnd* pParent /*=NULL*/)
	: CExtResizableDialog(CDataSrcViewDlg::IDD, pParent)
{
	m_nLayoutType = CProjectDoc::VIEW_DS_USER_LAYOUT;
}

CDataSrcViewDlg::~CDataSrcViewDlg()
{
}

void CDataSrcViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CExtResizableDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_DS_VIEW_TOOL_BAR, m_wndToolBar);
	DDX_Control(pDX, IDC_DS_TREE_GRID, m_wndDSList);
}


BEGIN_MESSAGE_MAP(CDataSrcViewDlg, CExtResizableDialog)
	ON_WM_SIZE()
	
	ON_COMMAND(ID_DS_USER_LAYOUT, OnDataSrcViewUserLayout)
	ON_COMMAND(ID_DS_BY_TYPE_LAYOUT, OnDataSrcViewByTypeLayout)
	ON_COMMAND(ID_DS_BY_DEVICE_LAYOUT, OnDataSrcViewByDeviceLayout)
	ON_COMMAND(ID_DS_BY_SCRIPT_LAYOUT, OnDataSrcViewByScriptLayout)

	ON_UPDATE_COMMAND_UI(ID_DS_USER_LAYOUT, OnUpdateDataSrcViewUserLayout)
	ON_UPDATE_COMMAND_UI(ID_DS_BY_TYPE_LAYOUT, OnUpdateDataSrcViewByTypeLayout)
	ON_UPDATE_COMMAND_UI(ID_DS_BY_DEVICE_LAYOUT, OnUpdateDataSrcViewByDeviceLayout)
	ON_UPDATE_COMMAND_UI(ID_DS_BY_SCRIPT_LAYOUT, OnUpdateDataSrcViewByScriptLayout)

	ON_COMMAND(ID_VIEW_DYNAMIC, OnDataSrcViewDynamic)
	
	ON_WM_CONTEXTMENU()

	ON_WM_TIMER()
END_MESSAGE_MAP()

void CDataSrcViewDlg::OnDSGridDblClick( NMHDR * pNotifyStruct, LRESULT * result )
{
	*result = 0L;
}


BOOL CDataSrcViewDlg::Create(UINT nIDTemplate, CWnd* pParent, CProjectDoc* pDoc)
{
	if ( pDoc == NULL )
		return FALSE;
	
	m_pDoc = pDoc;
	return CExtResizableDialog::Create(nIDTemplate, pParent);
}

// CDataSrcViewDlg message handlers

BOOL CDataSrcViewDlg::OnInitDialog()
{
	CExtResizableDialog::OnInitDialog();

	if ( !m_wndToolBar.LoadToolBar(IDR_DS_VIEW_TOOL_BAR) )
		return FALSE;

	/*
	static LPCTSTR arrHdrNames[] =  
	{	
		_T("Идентификатор ИД"), 
		_T("Сигнал"), 
		_T("Значение"), 
		_T("Единицы"), 
		_T("Описание") 
	};
	*/
	static LPCTSTR arrHdrNames[] =  
	{	
		_T("Inner variable"), 
		_T("Signal"), 
		_T("Value"), 
		_T("Units"), 
		_T("Description") 
	};

	static INT arrHdrExtents[] = 	{ 350, 100, 100, 100, 600 };

	/*
	m_wndDSList.SiwModifyStyle( __ESIS_STH_PIXEL|__ESIS_STV_ITEM |__EGBS_SFB_FULL_ROWS
		|__EGBS_GRIDLINES | __EGBS_RESIZING_CELLS_OUTER |__EGBS_RESIZING_CELLS_INNER|__EGBS_DYNAMIC_RESIZING,
		0, true);
	
	m_wndDSList.SiwModifyStyleEx( __EGBS_BSE_EX_PROPORTIONAL_COLUMN_WIDTHS|__EGBS_BSE_EX_PROPORTIONAL_ROW_HEIGHTS|__EGBS_EX_CELL_EXPANDING_INNER|__EGBS_FIXED_SIZE_ROWS, 0, true );
	*/
	

	m_wndDSList.SiwModifyStyle(
		__ESIS_STH_PIXEL
		|__ESIS_STV_ITEM
		| __EGBS_SFB_FULL_ROWS // | __EGBS_SFM_CELLS_HV
		|__EGBS_RESIZING_CELLS_OUTER_V|__EGBS_DYNAMIC_RESIZING
		|__EGBS_MULTI_AREA_SELECTION|__EGBS_NO_HIDE_SELECTION
		|__EGBS_GRIDLINES_V|__EGBS_LBEXT_SELECTION
		|__EGBS_SUBTRACT_SEL_AREAS
		//|__EGBS_RESIZING_CELLS_INNER_V|__EGBS_RESIZING_CELLS_OUTER_V
		,
		__EGBS_FIXED_SIZE_ROWS,
		false
		);
	m_wndDSList.SiwModifyStyleEx(
		__EGBS_EX_CELL_TOOLTIPS_OUTER|__EGBS_EX_CELL_EXPANDING_INNER
		|__EGWS_EX_PM_COLORS,
		0,
		false
		);
	m_wndDSList.BseModifyStyle(
		__EGWS_BSE_SORT_COLUMNS|__EGWS_BSE_SORT_COLUMNS_ALLOW_MULTIPLE
		//|__EGWS_BSE_SORT_ROWS|__EGWS_BSE_SORT_ROWS_ALLOW_MULTIPLE - un-supported & un-reasonable
		|__EGWS_BSE_BUTTONS_IN_FOCUSED_ROW,
		0,
		false
		);

	m_wndDSList.BseModifyStyle(
		0,
		__EGWS_BSE_EDIT_SINGLE_LCLICK
		|__EGWS_BSE_EDIT_SINGLE_FOCUSED_ONLY
		|__EGWS_BSE_EDIT_DOUBLE_LCLICK
		|__EGWS_BSE_EDIT_RETURN_CLICK
		|__EGWS_BSE_EDIT_F2_CLICK
		|__EGWS_BSE_EDIT_CELLS_INNER
		|__EGWS_BSE_EDIT_AUTO
		,
		false
		);


	LONG nColCount = sizeof(arrHdrNames) / sizeof(arrHdrNames[0]);
	
	m_wndDSList.ColumnInsert(0, nColCount);
	m_wndDSList.OuterColumnCountLeftSet( 1L );
	m_wndDSList.OuterColumnCountRightSet( 0 );
	m_wndDSList.FixedSizeColumnsSet(false);
	m_wndDSList.DefaultColumnWidthSet(24);
	
	m_wndDSList.OuterRowCountTopSet( 1L );
	m_wndDSList.DefaultRowHeightSet(40, true);
	m_wndDSList.OuterColumnWidthSet(true, 0L, 10);
	
	for( LONG nColNo = 0L; nColNo < nColCount; nColNo++ )
	{
		CExtGridCellHeaderFilter * pHeaderFilterCell = STATIC_DOWNCAST( CExtGridCellHeaderFilter, 
		m_wndDSList.GridCellGetOuterAtTop( nColNo, 0L, RUNTIME_CLASS(CExtGridCellHeaderFilter)));
	
		pHeaderFilterCell->ExtentSet( g_PaintManager->UiScalingDo( arrHdrExtents[nColNo], CExtPaintManager::__EUIST_X ) );
		pHeaderFilterCell->TextSet( arrHdrNames[nColNo] );
		pHeaderFilterCell->ModifyStyle( __EGCS_BUTTON_DROPDOWN, 0 );
	}

	RepositionBars(0, 0xFFFF, IDC_DS_TREE_GRID); 

	AddGridIcon("enabled.ico");
	AddGridIcon("disabled.ico");
	

	SetTimer(1, 1000, NULL);

	HCURSOR hCur = ::LoadCursor(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_CUR_ARROW));

	CRect rc;
	::GetWindowRect( GetDesktopWindow()->m_hWnd, &rc );

	int xOffset = rc.Width() / 5;
	int yOffset = rc.Height() / 4;

	rc.DeflateRect(xOffset, yOffset);

// 	if ( !pGraphWnd.CreateEx(WS_EX_TOPMOST, ::AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, hCur) , "Graph popup", WS_POPUPWINDOW, rc, this, IDC_STATIC ) )
// 	{
// 		////TRACE( "\nPopup graphic window creation failed" );		
// 		return -1;
// 	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDataSrcViewDlg::OnSize(UINT nType, int cx, int cy)
{
	CExtResizableDialog::OnSize(nType, cx, cy);
	RepositionBars(0, 0xFFFF, IDC_DS_TREE_GRID); 	
}

void CDataSrcViewDlg::SetViewLayout(UINT nType)
{
	m_nLayoutType = nType;

	const DATA_SOURCES_VIEW_GROUPS& layout = m_pDoc->GetLayout(nType);

	m_wndDSList.ItemRemove(m_wndDSList.ItemGetRoot(), false);
	groupNodes.clear();
	for( DATA_SOURCES_VIEW_GROUPS::const_iterator it = layout.begin(); it != layout.end(); ++it)
	{
		LPDS_VIEW_GROUP group = (*it);		
		HTREEITEM hGroup = InsertGroup( group );

		m_wndDSList.ItemExpandAll(hGroup, TVE_EXPAND, TRUE);

		for (int i = 0; i < (int)group->dsID.size(); i++)
		{
			HTREEITEM hItem = InsertDataSource(hGroup, group->dsID.at(i), group);			
		}

		m_wndDSList.ItemExpandAll(hGroup, TVE_EXPAND, TRUE);		
	}
	
}
 
void  CDataSrcViewDlg::OnDataSrcViewUserLayout()
{
	SetViewLayout(CProjectDoc::VIEW_DS_USER_LAYOUT);
}

void  CDataSrcViewDlg::OnDataSrcViewByTypeLayout()
{
	SetViewLayout(CProjectDoc::VIEW_DS_BY_TYPE_LAYOUT);
}

void  CDataSrcViewDlg::OnDataSrcViewByDeviceLayout()
{
	SetViewLayout(CProjectDoc::VIEW_DS_BY_DEVICE_LAYOUT);
}

void  CDataSrcViewDlg::OnDataSrcViewByScriptLayout()
{
	SetViewLayout(CProjectDoc::VIEW_DS_BY_SCRIPT_LAYOUT);
}

void  CDataSrcViewDlg::OnUpdateDataSrcViewUserLayout(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio( (m_nLayoutType == CProjectDoc::VIEW_DS_USER_LAYOUT) ? TRUE : FALSE);
	pCmdUI->Enable( !m_pDoc->GetLayout(CProjectDoc::VIEW_DS_USER_LAYOUT).empty() );
}

void  CDataSrcViewDlg::OnUpdateDataSrcViewByTypeLayout(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio( (m_nLayoutType == CProjectDoc::VIEW_DS_BY_TYPE_LAYOUT) ? TRUE : FALSE);	
	pCmdUI->Enable( !m_pDoc->GetLayout(CProjectDoc::VIEW_DS_BY_TYPE_LAYOUT).empty() );
}

void  CDataSrcViewDlg::OnUpdateDataSrcViewByDeviceLayout(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio( (m_nLayoutType == CProjectDoc::VIEW_DS_BY_DEVICE_LAYOUT) ? TRUE : FALSE);	
	pCmdUI->Enable( !m_pDoc->GetLayout(CProjectDoc::VIEW_DS_BY_DEVICE_LAYOUT).empty() );
}
void  CDataSrcViewDlg::OnUpdateDataSrcViewByScriptLayout(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio( (m_nLayoutType == CProjectDoc::VIEW_DS_BY_SCRIPT_LAYOUT) ? TRUE : FALSE);	
	pCmdUI->Enable( !m_pDoc->GetLayout(CProjectDoc::VIEW_DS_BY_SCRIPT_LAYOUT).empty() );
}

HTREEITEM CDataSrcViewDlg::InsertGroup( LPDS_VIEW_GROUP group )
{
	HTREEITEM hRoot = NULL;
	HTREEITEM hGroup = NULL;

	std::map<LPDS_VIEW_GROUP, HTREEITEM>:: const_iterator itFind = groupNodes.find( group->pParentGroup );

	if ( itFind != groupNodes.end() )
	{
		hRoot = (*itFind).second;
		hGroup = m_wndDSList.ItemInsert(hRoot, ULONG(-1), 1);			
	}
	else
		hGroup = m_wndDSList.ItemInsert(hRoot, ULONG(-1), 1);		


	groupNodes.insert(std::make_pair(group, hGroup));

	CExtGridCell * pCellHdr;
	pCellHdr = m_wndDSList.ItemGetCell( hGroup, 0L, -1, RUNTIME_CLASS( CExtGridCellHeader ) );
	ASSERT_VALID( pCellHdr );
	pCellHdr->ModifyStyle(
		__EGCS_HDR_FOCUS_ARROW_DISPLAY
		|__EGCS_HDR_FOCUS_ARROW_RESERVE_SPACE
		|__EGCS_HDR_ROW_COLUMN_NUMBER
		|__EGCS_TA_HORZ_RIGHT
		);
	pCellHdr->ExtentSet(   g_PaintManager->UiScalingDo(ROW_HEIGHT, CExtPaintManager::__EUIST_Y));

	CExtGridCell* pCell = m_wndDSList.ItemGetCell(hGroup, COL_IND_NAME, 0, RUNTIME_CLASS(CExtGridCellString));
	DWORD dwBaseStyle = __EGCS_EX_WRAP_TEXT/*|__EGCS_READ_ONLY*/|__EGCS_NO_INPLACE_CONTROL|__EGCS_TA_VERT_CENTER; 

	PrepareCell(pCell, dwBaseStyle |__EGCS_ICA_HORZ_LEFT|__EGCS_ICA_HORZ_LEFT, group->bkColor, 12, FW_SEMIBOLD);

	pCell->TextSet(group->strTitle);
	

	SetItemColor(hGroup, group->bkColor);

	//////////////////////////////////////////////////////////////////////////
	//Колонка сигнал	
	pCell = m_wndDSList.ItemGetCell(hGroup, COL_IND_SIGNAL, 0, RUNTIME_CLASS(CExtGridCellString));
	PrepareCell(pCell, dwBaseStyle |__EGCS_ICA_HORZ_LEFT|__EGCS_ICA_HORZ_LEFT, group->bkColor, 12);

	//////////////////////////////////////////////////////////////////////////
	//Колонка значение
	pCell = m_wndDSList.ItemGetCell(hGroup, COL_IND_VALUE, 0, RUNTIME_CLASS(CExtGridCellString));
	PrepareCell(pCell, dwBaseStyle |__EGCS_ICA_HORZ_LEFT|__EGCS_ICA_HORZ_LEFT, group->bkColor, 12, FW_BOLD);

	//////////////////////////////////////////////////////////////////////////
	//Колонка единицы	
	pCell = m_wndDSList.ItemGetCell(hGroup, COL_IND_UNITS, 0, RUNTIME_CLASS(CExtGridCellString));
	PrepareCell(pCell, dwBaseStyle |__EGCS_ICA_HORZ_LEFT|__EGCS_ICA_HORZ_LEFT, group->bkColor, 12);

	return hGroup;
}


HTREEITEM CDataSrcViewDlg::InsertDataSource(HTREEITEM hGroup, std::string  dsID, LPDS_VIEW_GROUP pGroup)
{

	DataSrc* pDS = DataSrc::GetDataSource( dsID );

	bool dsFound = ( pDS != NULL );

	//Пытаемся привести к аналоговому источнику данных
	AnalogDataSrc* pADS = dynamic_cast<AnalogDataSrc*>(pDS); 
	bool isAnalogDS = ( pADS != NULL );

	//Пытаемся привести к дискретному источнику данных
	DiscreteDataSrc* pDDS = dynamic_cast<DiscreteDataSrc*>(pDS);
	bool isDiscreteDS = ( pDDS != NULL );

	IComposite* pComposed = dynamic_cast<IComposite*>(pDS);
	bool isComposedDS = ( pComposed != NULL );

	HTREEITEM hItem = m_wndDSList.ItemInsert(hGroup, ULONG(-1), 1);

	DWORD dwBaseStyle = __EGCS_EX_WRAP_TEXT|/*__EGCS_READ_ONLY|*/__EGCS_NO_INPLACE_CONTROL|__EGCS_TA_VERT_CENTER; 
	//__EGCS_ICA_HORZ_LEFT|__EGCS_ICA_VERT_CENTER
	CExtSafeString txt; 

	CExtGridCell * pCellHdr;
	pCellHdr = m_wndDSList.ItemGetCell( hItem, 0L, -1, RUNTIME_CLASS( CExtGridCellHeader ) );
	ASSERT_VALID( pCellHdr );
	pCellHdr->ModifyStyle(
		__EGCS_HDR_FOCUS_ARROW_DISPLAY
		|__EGCS_HDR_FOCUS_ARROW_RESERVE_SPACE
		|__EGCS_HDR_ROW_COLUMN_NUMBER
		|__EGCS_TA_HORZ_RIGHT
		);
	pCellHdr->ExtentSet(   g_PaintManager->UiScalingDo(ROW_HEIGHT, CExtPaintManager::__EUIST_Y));

	//////////////////////////////////////////////////////////////////////////
	//Колонка Имя 	
	CExtGridCell* pCell = m_wndDSList.ItemGetCell(hItem, COL_IND_NAME, 0, RUNTIME_CLASS(CExtGridCellString) );
	PrepareCell(pCell, dwBaseStyle |__EGCS_ICA_HORZ_LEFT|__EGCS_ICA_HORZ_LEFT, pGroup->bkColor, 12);

	txt = dsFound ? pDS->Name().c_str() : dsID.c_str(); 
	pCell->TextSet( txt );
	

	//////////////////////////////////////////////////////////////////////////
	//Колонка сигнал	
	pCell = m_wndDSList.ItemGetCell(hItem, COL_IND_SIGNAL, 0, RUNTIME_CLASS(CExtGridCellString));
	PrepareCell(pCell, dwBaseStyle |__EGCS_ICA_HORZ_LEFT|__EGCS_ICA_HORZ_LEFT, pGroup->bkColor, 12);

	if ( !dsFound )
		txt = "-";
	else if ( isAnalogDS )
		txt.Format(pADS->OutFormat().c_str(), pADS->GetSignal());		
	else if ( isDiscreteDS )
		txt = pDDS->IsEnabled() ? "-" : "-";

	pCell->TextSet( txt );
	pCell->LParamSet((LPARAM)pDS);

	//////////////////////////////////////////////////////////////////////////
	//Колонка значение
	pCell = m_wndDSList.ItemGetCell(hItem, COL_IND_VALUE, 0, RUNTIME_CLASS(CExtGridCellString));
	PrepareCell(pCell, dwBaseStyle |__EGCS_ICA_HORZ_LEFT|__EGCS_ICA_HORZ_LEFT, pGroup->bkColor, 12, FW_BOLD);


	if ( !dsFound )
		txt = "-";
	else if ( isAnalogDS )
		txt.Format(pADS->OutFormat().c_str(), pADS->GetValue());		
	else if ( isDiscreteDS )
		txt = pDDS->ToString().c_str();// pDDS->IsEnabled() ? "Enabled" : "Disabled";

	pCell->TextSet( txt );
	pCell->LParamSet((LPARAM)pDS);


	//////////////////////////////////////////////////////////////////////////
	//Колонка единицы	
	pCell = m_wndDSList.ItemGetCell(hItem, COL_IND_UNITS, 0, RUNTIME_CLASS(CExtGridCellString));
	PrepareCell(pCell, dwBaseStyle |__EGCS_ICA_HORZ_LEFT|__EGCS_ICA_HORZ_LEFT, pGroup->bkColor, 12);

	txt = "";
	if ( !dsFound )
		txt = "-";
	else if ( isAnalogDS )
		txt = pADS->Units().c_str();		

	pCell->TextSet( txt );


	//////////////////////////////////////////////////////////////////////////
	//Колонка описание
	pCell = m_wndDSList.ItemGetCell(hItem, COL_IND_DESCRIPTION, 0, RUNTIME_CLASS(CExtGridCellString));
	PrepareCell(pCell, dwBaseStyle |__EGCS_ICA_HORZ_LEFT|__EGCS_ICA_HORZ_LEFT, pGroup->bkColor, 12);

	txt = "";
	if ( !dsFound )
		txt = "Источник данных не найден";
	else
		txt = pDS->Description().c_str();

	pCell->TextSet( txt );

	// Если составной источник данных добавляем все 
	// входящие в его состав подисточники
	if ( isComposedDS )
	{
		for (int i = 0; i < pComposed->Size(); i++)
		{
			const DataSrc* pSubDS = pComposed->GetSubDataSrc(i);
			if (pSubDS != NULL)
				InsertDataSource(hItem, pSubDS->Name(), pGroup);
			else
				InsertDataSource(hItem, "Источник не найден", pGroup);
		}
	}

	return hItem;
}

int CDataSrcViewDlg::AddGridIcon(HICON hIcon)
{

	ASSERT( hIcon != NULL );

	CExtCmdIcon icon;
	CExtGridDataProvider& data = m_wndDSList.GetTreeData();

	icon.AssignFromHICON(hIcon, false);

	return data.IconInsert(&icon);
}

int CDataSrcViewDlg::AddGridIcon(std::string name)
{
	CExtCmdIcon* pCmdIcon = AppImagesHelper::Instance().GetIcon(name, ICON_SIZE_24);
	if (pCmdIcon == NULL)
		return -1;

	CExtGridDataProvider& data = m_wndDSList.GetTreeData();		
	return data.IconInsert(pCmdIcon);
}

int CDataSrcViewDlg::AddGridIcon(UINT idIcon, int cx)
{	
	HICON hIcon;
	hIcon = (HICON) ::LoadImage( ::AfxGetInstanceHandle(), MAKEINTRESOURCE(idIcon), IMAGE_ICON, cx, cx, 0); 

	return AddGridIcon(hIcon);
}

bool CDataSrcViewDlg::SetInnerItem(HTREEITEM hItem, int col, LPCTSTR strText, int iconInd, COLORREF clr, bool bold, bool italic)
{
	CExtGridCell* pCell = m_wndDSList.ItemGetCell(hItem, col, 0, RUNTIME_CLASS(CExtGridCellString));

	//Текст ячейки
	pCell->TextSet(strText); 

	//иконка
	pCell->IconIndexSet(iconInd);  

	//Цвет текста
	pCell->TextColorSet(CExtGridCell::__ECS_NORMAL, clr);

	//жирный
	pCell->FontWeightSet( bold ? FW_BOLD : FW_NORMAL ); 

	//курсив
	pCell->FontItalicSet( italic ); 

	return true;
}


void CDataSrcViewDlg::SetItemColor(HTREEITEM hItem, COLORREF clr)
{
	LONG nColCount = m_wndDSList.ColumnCountGet();

	for( LONG nColNo = 0L; nColNo < nColCount; nColNo++ )
	{
		CExtGridCell* pCell = m_wndDSList.ItemGetCell(hItem, nColNo, 0, RUNTIME_CLASS(CExtGridCellString));
		pCell->BackColorSet(CExtGridCell::__ECS_NORMAL, clr);
	}
}

void CDataSrcViewDlg::PrepareCell(HTREEITEM hItem, INT nCol, DWORD dwStyle/* = -1L*/, COLORREF clr/* = -1L*/, 
								  LONG fntHeight/* = -1L*/, LONG fntWeight/* = -1L*/, __EXT_MFC_SAFE_LPCTSTR sFaseName/* = NULL*/)
{
	CExtGridCell* pCell = m_wndDSList.ItemGetCell(hItem, nCol, 0, RUNTIME_CLASS(CExtGridCellString));			
	PrepareCell(pCell, dwStyle, clr, fntHeight, fntWeight, sFaseName);
}

void CDataSrcViewDlg::PrepareCell(CExtGridCell* pCell, DWORD dwStyle/* = -1L*/, COLORREF clr/* = -1L*/, 
								  LONG fntHeight/* = -1L*/, LONG fntWeight/* = -1L*/, __EXT_MFC_SAFE_LPCTSTR sFaseName/* = NULL*/)
{
	if (dwStyle != -1L)
		pCell->ModifyStyle( dwStyle , 0);

	if (clr != -1L)
		pCell->BackColorSet(CExtGridCell::__ECS_NORMAL, clr);

	if (fntHeight != -1)
		pCell->FontHeightSet(-MulDiv(fntHeight, GetDeviceCaps(GetDC()->m_hDC, LOGPIXELSY), 72));

	if (fntWeight != -1)
		pCell->FontWeightSet(fntWeight);

	if (sFaseName != NULL)
		pCell->FontFaceNameSet(sFaseName);
}
void CDataSrcViewDlg::OnContextMenu(CWnd* pWnd, CPoint pos)
{
	CExtPopupMenuWnd * pMenu = new CExtPopupMenuWnd;
	VERIFY( pMenu->CreatePopupMenu( GetSafeHwnd() ) );
	VERIFY( pMenu->LoadMenu( GetSafeHwnd(), IDR_DS_POPUP_MENU ) );

	pMenu->TrackPopupMenu( 0, pos.x, pos.y );
}


void CDataSrcViewDlg::OnDataSrcViewDynamic()
{
	CPoint pt;
	GetCursorPos(&pt);
	
	CExtGridHitTestInfo& hti = m_wndDSList.HoverInfoGet();	
	CExtGridCell* pCell =  m_wndDSList.GridCellGet( hti, RUNTIME_CLASS(CExtGridCellString) );

	DataSrc* pDS = (DataSrc*)pCell->LParamGet();
	if ( pDS != NULL )
	{
// 		pGraphWnd.AddDataSrc(pDS->Name(), false);
// 		pGraphWnd.AutoHideSet(true);
// 		pGraphWnd.ShowWindow(SW_SHOW);
	}	
}

void CDataSrcViewDlg::UpdateDataList()
{
	HTREEITEM hRoot = m_wndDSList.ItemGetRoot();

	long rootsCount = m_wndDSList.ItemGetChildCount(hRoot);
	for (long i = 0;  i < rootsCount; i++ )
	{
		HTREEITEM hTree = m_wndDSList.ItemGetChildAt(hRoot, i);
		HTREEITEM hItem = m_wndDSList.ItemGetFirstChild(hTree);

		if (hTree == hItem)
			continue;

		while (hItem != NULL)
		{
			CString str;

			//Колонка сигнал
			CExtGridCell* pCell = m_wndDSList.ItemGetCell(hItem, COL_IND_SIGNAL, 0, RUNTIME_CLASS(CExtGridCellString));

			DataSrc* pDS = (DataSrc*)pCell->LParamGet();

			const AnalogDataSrc* pADS = dynamic_cast<const AnalogDataSrc*>(pDS); 
			const DiscreteDataSrc* pDDS = dynamic_cast<const DiscreteDataSrc*>(pDS); 

			if ( pADS != NULL )
				str.Format(pADS->OutFormat().c_str(), pADS->GetSignal() );			

			else if (pDDS != NULL)
				str = pDDS->IsEnabled() ? "Hi" : "Low";

			pCell->TextSet(str);

			//Колонка значение
			pCell = m_wndDSList.ItemGetCell(hItem, COL_IND_VALUE, 0, RUNTIME_CLASS(CExtGridCellString));

			if ( pADS != NULL )
			{
				str.Format(pADS->OutFormat().c_str(), pADS->GetValue() );			

			}
			else if (pDDS != NULL)
			{
				str = pDDS->ToString().c_str();//pDDS->IsEnabled() ? "Enabled" : "Disabled";

				CExtGridCell* pCellIcon = m_wndDSList.ItemGetCell(hItem, 0, 0, RUNTIME_CLASS(CExtGridCellString));
				int indexIcon = pDDS->IsEnabled() ? 0 : 1;
				pCellIcon->IconIndexSet(indexIcon); 
			}

			pCell->TextSet(str);
			hItem = m_wndDSList.ItemGetNext(hItem, false, false, true);

		}
	}
	m_wndDSList.UpdateWindow();
	m_wndDSList.Invalidate(FALSE);

}
void CDataSrcViewDlg::OnTimer(UINT_PTR nIDEvent)
{
	UpdateDataList();
	CExtResizableDialog::OnTimer(nIDEvent);
}
