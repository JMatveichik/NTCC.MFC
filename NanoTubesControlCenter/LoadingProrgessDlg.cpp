// LoadingProrgessDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LoadingProrgessDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW	
#endif
// CLoadingProrgessDlg dialog

//IMPLEMENT_DYNAMIC(CLoadingProrgessDlg, CExtResizableDialog )

#define __COLUMN_SECTION_NAME             0L
#define __COLUMN_SECTION_PROGRESS         1L

CLoadingProrgessDlg::CLoadingProrgessDlg(CWnd* pParent /*=NULL*/)
	: CExtNCW < CExtResizableDialog >(CLoadingProrgessDlg::IDD, pParent)
{

}

CLoadingProrgessDlg::~CLoadingProrgessDlg()
{
}

void CLoadingProrgessDlg::DoDataExchange(CDataExchange* pDX)
{
	CExtNCW < CExtResizableDialog >::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRID, m_wndGrid);
	DDX_Control(pDX, IDC_COMMON_PROGRESS, m_wndCommonProgress);
	DDX_Control(pDX, IDC_STEP_INFO, m_lblCurStep);
	
	
}


BEGIN_MESSAGE_MAP(CLoadingProrgessDlg, CExtResizableDialog)
	ON_MESSAGE( RM_LOADING_INIT, OnLoadingInit )
	ON_MESSAGE( RM_ADD_LOADING_SECTION, OnAddLoadingSection)
	ON_MESSAGE( RM_ADD_INFO_SECTION, OnAddInfoSection)
	ON_MESSAGE( RM_STEP_SECTION, OnStepSection)
	ON_MESSAGE( RM_LOADING_COMPLETE, OnLoadComplete)

	ON_WM_SIZE()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDCANCEL, &CLoadingProrgessDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CLoadingProrgessDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHK_IGNORE_ERROR, &CLoadingProrgessDlg::OnBnClickedChkIgnoreError)
END_MESSAGE_MAP()


// CLoadingProrgessDlg message handlers


BOOL CLoadingProrgessDlg::OnInitDialog()
{
	VERIFY( CExtNCW < CExtResizableDialog >::OnInitDialog() );

	//
	// Show size gripper
	//

	CExtNCW < CExtResizableDialog >::ShowSizeGrip( FALSE );

	SubclassChildControls();

	AddAnchor(IDC_GRID, __RDA_LT, __RDA_RB);
	AddAnchor(IDC_COMMON_PROGRESS, __RDA_LT, __RDA_RT);
	AddAnchor(IDC_CHK_IGNORE_ERROR, __RDA_LB, __RDA_LB);
	AddAnchor(IDCANCEL, __RDA_RB, __RDA_RB);
	AddAnchor(IDOK, __RDA_RB, __RDA_RB);
	AddAnchor(IDC_BANER, __RDA_LB, __RDA_LB);
	AddAnchor(IDC_TEXT, __RDA_LB, __RDA_LB);
	

	// TODO:  Add extra initialization here
	

	m_wndGrid.SiwModifyStyle(
		(__ESIS_STH_PIXEL|__ESIS_STV_ITEM)	// item scroll window styles
		//| __EGBS_SFB_CELLS						// selection/focus type
		//| __EGBS_RESIZING_CELLS_OUTER			// enable row/column resizing
		//| __EGBS_RESIZING_CELLS_INNER			
		//| __EGBS_DYNAMIC_RESIZING				// resize rows/columns on-the-fly
		//|__ESIS_DISABLE_AUTOHIDE_SB_H|__ESIS_DISABLE_AUTOHIDE_SB_V
		,
		0,
		false
		);
	m_wndGrid.BseModifyStyleEx(
		//__EGBS_BSE_EX_DBLCLK_BEST_FIT_ROW		// do best fit sizing on double clicks over column divider
		//| __EGBS_BSE_EX_DBLCLK_BEST_FIT_COLUMN	// do best fit sizing on double clicks over row divider
		__EGBS_BSE_EX_PROPORTIONAL_COLUMN_WIDTHS
		,
		__EGWS_BSE_EX_DEFAULT,
		false
		);
	
	//m_wndGrid.BseModifyStyle( 
	//	__EGWS_BSE_SORT_COLUMNS, 
	//	0, 
	//	false 
	//	);

// 	m_wndGrid.SiwModifyStyleEx(
// 		__EGBS_EX_CELL_TOOLTIPS_INNER
// 		| __EGBS_EX_CELL_EXPANDING_INNER
// 		| __EGWS_EX_PM_COLORS
// 		| __EGBS_EX_CORNER_AREAS_3D	
// 		,
// 		0,
// 		false
// 		);

	m_wndGrid.OuterRowCountTopSet( 1L, false );
	m_wndGrid.OuterRowHeightSet(true, 0L, 24);
	m_wndGrid.DefaultRowHeightSet(24, true);
	
	
	//m_wndGrid.OuterRowCountBottomSet( 0L, false );
	//m_wndGrid.OuterColumnCountLeftSet( 0L, false );
	//m_wndGrid.OuterColumnCountRightSet( 0L, false );
	//m_wndGrid.OuterColumnWidthSet( true, 0L, 14 );
	//m_wndGrid.OuterColumnWidthSet( false, 0L, 14 );

	m_wndGrid.NoHideSelectionSet( true, false );
	m_wndGrid.SubtractSelectionAreasSet( true, false );
	m_wndGrid.LbExtSelectionSet( true );
	m_wndGrid.GridLinesHorzSet( false, false );
	m_wndGrid.GridLinesVertSet( false, false );
	m_wndGrid.HoverEventsSet( true, true );
	m_wndGrid.MultiAreaSelectionSet( true );
	m_wndGrid.HoverHighlightSet( true, false, true, true, true, true );


	CRect rcGrid;
	m_wndGrid.GetWindowRect(&rcGrid);

	int fullWidth = rcGrid.Width();

	static struct
	{
		LPCTSTR m_strText;
		INT m_nWidthMin;
		INT m_nWidthMax;
		INT m_nWidthInit;
		double m_nWidthPercent;
		DWORD m_dwStyle;
	}
	g_arrHeaderDefinitions[] =
	{
		{ _T("Секция"),     fullWidth/2, fullWidth/2, fullWidth/2,   0.0, __EGCS_TA_HORZ_LEFT },
		{ _T("Прогресс"),   fullWidth/2, fullWidth/2, fullWidth/2,   0.0, __EGCS_TA_HORZ_CENTER },
	};

	LONG nColNo, nColCount = sizeof(g_arrHeaderDefinitions) / sizeof(g_arrHeaderDefinitions[0]);
	m_wndGrid.ColumnAdd( nColCount, false );
	for( nColNo = 0L; nColNo < nColCount; nColNo++ )
	{
		CExtGridCellHeader * pCellHeader =
			STATIC_DOWNCAST( CExtGridCellHeader,
			m_wndGrid.GridCellGetOuterAtTop( nColNo, 0L, RUNTIME_CLASS(CExtGridCellHeader) ) );

		ASSERT_VALID( pCellHeader );
		
		int fntHeight = 12;
		pCellHeader	->FontFaceNameSet("Segoe UI");
		pCellHeader	->FontHeightSet(-MulDiv(fntHeight, GetDeviceCaps(GetDC()->m_hDC, LOGPIXELSY), 72) );

		pCellHeader	-> TextSet( g_arrHeaderDefinitions[nColNo].m_strText );

		pCellHeader	-> ExtentPercentSet( g_arrHeaderDefinitions[nColNo].m_nWidthPercent );

		pCellHeader	-> ExtentSet( g_arrHeaderDefinitions[nColNo].m_nWidthMin, -1);
		pCellHeader	-> ExtentSet( g_arrHeaderDefinitions[nColNo].m_nWidthInit, 0);
		pCellHeader	-> ExtentSet( g_arrHeaderDefinitions[nColNo].m_nWidthMax, 1);

		
		pCellHeader	-> ModifyStyle( g_arrHeaderDefinitions[nColNo].m_dwStyle|__EGCS_HDR_FOCUS_ARROW_DISPLAY );
		//pCellHeader	-> FontWeightSet( FW_BOLD );
	} // for( nColNo = 0L; nColNo < nColCount; nColNo++ )

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CLoadingProrgessDlg::OnSize(UINT nType, int cx, int cy)
{
	CExtNCW < CExtResizableDialog >::OnSize(nType, cx, cy);
}

void CLoadingProrgessDlg::OnMove(int x, int y)
{
	CExtNCW < CExtResizableDialog >::OnMove(x, y);
	ArrangeLayout();

	CRect rcGrid;
	if ( m_wndGrid.GetSafeHwnd() == NULL )
		return;

	m_wndGrid.GetWindowRect(&rcGrid);

	int fullWidth = rcGrid.Width();

	static struct
	{		
		INT m_nWidthMin;
		INT m_nWidthMax;
		INT m_nWidthInit;
	}
	g_arrHeaderDefinitions[] =
	{
		{fullWidth/2, fullWidth/2, fullWidth/2},
		{fullWidth/2, fullWidth/2, fullWidth/2},		
	};

	LONG nColNo, nColCount = sizeof(g_arrHeaderDefinitions) / sizeof(g_arrHeaderDefinitions[0]);
	for( nColNo = 0L; nColNo < nColCount; nColNo++ )
	{
		CExtGridCellHeader * pCellHeader =
			STATIC_DOWNCAST( CExtGridCellHeader,
			m_wndGrid.GridCellGetOuterAtTop( nColNo, 0L, RUNTIME_CLASS(CExtGridCellHeader) ) );

		ASSERT_VALID( pCellHeader );

		pCellHeader	-> ExtentSet( g_arrHeaderDefinitions[nColNo].m_nWidthMin, -1);
		pCellHeader	-> ExtentSet( g_arrHeaderDefinitions[nColNo].m_nWidthInit, 0);
		pCellHeader	-> ExtentSet( g_arrHeaderDefinitions[nColNo].m_nWidthMax, 1);			
	} 
}

long CLoadingProrgessDlg::AddLoadingSection(DOCUMENT_SECTION_INFO& sec)
{
	m_wndGrid.RowAdd();
	long row = m_wndGrid.RowCountGet() - 1;
	
	CExtGridCellString * pCellSectionTitle =
		STATIC_DOWNCAST(
			CExtGridCellString,
			m_wndGrid.GridCellGet(__COLUMN_SECTION_NAME, row, 0, 0, RUNTIME_CLASS(CExtGridCellString) )
		);
	
	int fntHeight = 12;
		pCellSectionTitle->FontFaceNameSet("Segoe UI");
	pCellSectionTitle->FontHeightSet(-MulDiv(fntHeight, GetDeviceCaps(GetDC()->m_hDC, LOGPIXELSY), 72) );
	pCellSectionTitle->TextSet( sec.title );

	CExtGridCellProgress * pCellProgress =
		STATIC_DOWNCAST(
			CExtGridCellProgress,
			m_wndGrid.GridCellGet( __COLUMN_SECTION_PROGRESS, row, 0, 0, RUNTIME_CLASS(CExtGridCellProgress) ) 
		);

	/*UNREFERENCED_PARAMETER(pCellProgress);*/


	pCellProgress->FontFaceNameSet("Segoe UI");
	pCellProgress->FontHeightSet(-MulDiv(fntHeight, GetDeviceCaps(GetDC()->m_hDC, LOGPIXELSY), 72) );
	pCellProgress->SetStep( 1 );
	pCellProgress->SetRange( 0, sec.subItemsCount );
	

	pCellProgress->ModifyStyle( __EGCS_TA_HORZ_RIGHT );
	pCellProgress->TextSet( _T("Обработано : ") );
	pCellProgress->TextModeSet( CExtGridCellProgress::eTextAndPosition );
	pCellProgress->TiedTextSet( false );
	pCellProgress->BarColorsSet( RGB(0,80,80), RGB(0,128,80) );
	pCellProgress->TextColorsSet( COLORREF( -1L ), RGB(255,255,0));

	pCellProgress->InvertBarGradientSet( true );
	
	m_wndGrid.InvalidateRect(NULL);
	
	return row;
}


LRESULT CLoadingProrgessDlg::OnLoadingInit(WPARAM wParam, LPARAM lParam)
{
	m_wndCommonProgress.SetStep(1);
	m_wndCommonProgress.SetRange(0, wParam);
	
	return 0L;
}

LRESULT CLoadingProrgessDlg::OnAddLoadingSection(WPARAM wParam, LPARAM lParam)
{

	LPDOCUMENT_SECTION_INFO lpsi = (LPDOCUMENT_SECTION_INFO)lParam;
	long id = AddLoadingSection(*lpsi);

	m_lblCurStep.SetWindowText(lpsi->title);
	return id;
}

LRESULT CLoadingProrgessDlg::OnAddInfoSection(WPARAM wParam, LPARAM lParam)
{
	return 0L;
}
LRESULT CLoadingProrgessDlg::OnStepSection(WPARAM wParam, LPARAM lParam)
{
	m_wndCommonProgress.StepIt();

	long lSecID = (long)wParam;
	bool bOk = *((bool*)lParam);

	if ( lSecID < 0 )
		lSecID = m_wndGrid.RowCountGet() - 1;

	CExtGridCellProgress * pCellProgress =
		STATIC_DOWNCAST(
		CExtGridCellProgress,
		m_wndGrid.GridCellGet( __COLUMN_SECTION_PROGRESS, lSecID, 0, 0, RUNTIME_CLASS(CExtGridCellProgress) ) 
		);


	if ( !bOk )
		pCellProgress->BarColorsSet( RGB(80,0,0), RGB(128, 0, 0) );
		
	pCellProgress->StepIt();
		
	CRect rcCellExtra;    
	if( m_wndGrid.GridCellRectsGet( __COLUMN_SECTION_PROGRESS, lSecID, 0, 0, NULL, &rcCellExtra ) )
		m_wndGrid.InvalidateRect( &rcCellExtra );
	
	m_wndGrid.UpdateWindow();
	m_wndGrid.InvalidateRect(NULL);
	
	return 0L;
}

LRESULT CLoadingProrgessDlg::OnLoadComplete(WPARAM wParam, LPARAM lParam)
{
	HWND hCheckIgnoreErrors =  ::GetDlgItem(m_hWnd, IDC_CHK_IGNORE_ERROR);
	::ShowWindow(hCheckIgnoreErrors, SW_SHOWNORMAL);
	
	HWND hAnalizeBtn		=  ::GetDlgItem(m_hWnd, IDCANCEL);
	::EnableWindow(hAnalizeBtn, TRUE);

	if (wParam == 1)
	{
		HWND hContinueBtn	=  ::GetDlgItem(m_hWnd, IDOK);
		::EnableWindow(hContinueBtn, TRUE);
	}

	return 0L;
}

void CLoadingProrgessDlg::OnBnClickedCancel()
{
	EndModalLoop(0);
}


void CLoadingProrgessDlg::OnBnClickedOk()
{
	EndModalLoop(1);
}


void CLoadingProrgessDlg::OnBnClickedChkIgnoreError()
{
	HWND hContinueBtn	=  ::GetDlgItem(m_hWnd, IDOK);
	::EnableWindow(hContinueBtn, IsDlgButtonChecked(IDC_CHK_IGNORE_ERROR));	
}

