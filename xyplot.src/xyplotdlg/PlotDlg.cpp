// PlotDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PlotDlg.h"

#include "../../xyplot/src/XYPlotWrapper.h"

#include "dlgs/GeneralPropDlg.h"
#include "dlgs/AxisPropertyDlg.h"
#include "dlgs/ProfilesPropDlg.h"
#include "dlgs/LevelsPropDlg.h"

// CXYPlotDlg dialog

using namespace xyplot;

IMPLEMENT_DYNAMIC(CXYPlotDlg, CExtResizableDialog)

CXYPlotDlg::CXYPlotDlg(CWnd* pParent, long plotID)
	: CExtResizableDialog(CXYPlotDlg::IDD, pParent),
	plHandle(plotID), hPlot((HPLOT&)plHandle)
{

}

CXYPlotDlg::~CXYPlotDlg()
{
}

void CXYPlotDlg::DoDataExchange(CDataExchange* pDX)
{
	CExtResizableDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PAGES_LIST, m_wndList);
	DDX_Control(pDX, IDC_PAGE_TITLE, m_pageTitle);
}


BEGIN_MESSAGE_MAP(CXYPlotDlg, CExtResizableDialog)
	ON_LBN_SELCHANGE(IDC_PAGES_LIST, OnSelchangeList)
	ON_BN_CLICKED(IDC_BTN_APPLY, &CXYPlotDlg::OnBnClickedBtnApply)
	//ON_BN_CLICKED(IDOK, &CXYPlotDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CXYPlotDlg message handlers


BOOL CXYPlotDlg::OnInitDialog()
{
	CExtResizableDialog::OnInitDialog();

	ShowSizeGrip(TRUE);

	SubclassChildControls();
	
	AddAnchor(IDCANCEL, __RDA_RB, __RDA_RB);
	AddAnchor(IDOK, __RDA_RB, __RDA_RB);

	CGeneralPropDlg* pGPP  = new CGeneralPropDlg(this, hPlot);
	if ( !pGPP->Create(CGeneralPropDlg::IDD, this) )
		return FALSE;
	InitPage("General", RGB(153,204,255), pGPP, IDI_GENERAL  );


	CAxisPropertyDlg* pAXB = new CAxisPropertyDlg(this, hPlot, PI_AXIS_BOTTOM);
	if ( !pAXB->Create(CAxisPropertyDlg::IDD, this) )
		return FALSE;

	InitPage("Bottom axis X ", RGB(153,204,255), pAXB, IDI_AXIS_BOTTOM  );

	CAxisPropertyDlg* pAYL = new CAxisPropertyDlg(this, hPlot, PI_AXIS_LEFT);
	if ( !pAYL->Create(CAxisPropertyDlg::IDD, this) )
		return FALSE;

	InitPage("Left axis Y ", RGB(153,204,255), pAYL, IDI_AXIS_LEFT  );

	CAxisPropertyDlg* pAXT = new CAxisPropertyDlg(this, hPlot, PI_AXIS_TOP);
	if ( !pAXT->Create(CAxisPropertyDlg::IDD, this) )
		return FALSE;

	InitPage("Top axis X ", RGB(153,204,255), pAXT, IDI_AXIS_TOP  );

	CAxisPropertyDlg* pAYR = new CAxisPropertyDlg(this, hPlot, PI_AXIS_RIGHT);
	if ( !pAYR->Create(CAxisPropertyDlg::IDD, this) )
		return FALSE;

	InitPage("Right axis Y ", RGB(153,204,255), pAYR, IDI_AXIS_RIGHT  );

	CProfilesPropDlg* pPPP = new CProfilesPropDlg(this, hPlot);
	if ( !pPPP->Create(CProfilesPropDlg::IDD, this) )
		return FALSE;

	InitPage("Profiles", RGB(153,204,255), pPPP, IDI_PROFILES  );


	CLevelsPropDlg* pLPP = new CLevelsPropDlg(this, hPlot);
	if ( !pLPP->Create(CLevelsPropDlg::IDD, this) )
		return FALSE;

	InitPage("Levels", RGB(153,204,255), pLPP, IDI_LEVELS  );

	
	m_wndList.SetCurSel(0);
	OnSelchangeList();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CXYPlotDlg::InitPage(LPCTSTR strTitle,  COLORREF color, CPlotDlgPage* page,  UINT nIconResID, INT nIconSize/* = 32*/, UINT nLoadImageFlags/* = 0*/)
{
	CRect rcPages;
	::GetWindowRect( ::GetDlgItem( m_hWnd, IDC_PAGES_PLACEHOLDER ), &rcPages);
	ScreenToClient( &rcPages );
	page->MoveWindow( &rcPages );

	AddAnchor( page->GetSafeHwnd(), __RDA_LT, __RDA_RB );

	HINSTANCE hInstResource = ::AfxFindResourceHandle( MAKEINTRESOURCE( IDD_DLG_PLOT_PROPERTIES ), RT_ICON);
	ASSERT( hInstResource != NULL );

	HICON hIcon = (HICON) ::LoadImage( hInstResource, MAKEINTRESOURCE(nIconResID), IMAGE_ICON, nIconSize, nIconSize, nLoadImageFlags );

	ASSERT( hIcon != NULL );
	
	XYPROPPAGE pp;
	pp.color = color;
	pp.icon.AssignFromHICON( hIcon, false );
	pp.page = page;
	pp.title = strTitle;

	m_wndList.AddIcon( pp.icon, strTitle, page->GetSafeHwnd() );

	m_pages.Add(pp);

	return TRUE;
}

void CXYPlotDlg::OnSelchangeList() 
{
	int nIndex = m_wndList.GetCurSel();
	CIconListBox::ITEM_DATA *pItemData = m_wndList.m_arrItems[nIndex];

	for( int i=0; i < m_wndList.m_arrItems.GetSize(); i++ )
	{
		if( nIndex != i )
		{
			::ShowWindow(m_wndList.m_arrItems[i]->m_hWndPage, SW_HIDE);
		}
	}

	LPXYPROPPAGE lppp = &m_pages.GetAt(nIndex);

	{
		::ShowWindow(pItemData->m_hWndPage, SW_SHOW);
		CString str;
		str.Format("<html><h3><b>%s properties</b></h3></html>", lppp->title);

		m_pageTitle.SetIcon(lppp->icon.ExtractHICON());
 		//m_pageTitle.SetBkColor(lppp->color);
		m_pageTitle.SetWindowText(str);
	}
}

void CXYPlotDlg::OnBnClickedBtnApply()
{
	// TODO: Add your control notification handler code here
	for (int i = 0; i < m_pages.GetSize(); i++)
	{
		CPlotDlgPage* page = m_pages.GetAt(i).page;		
		page->OnApply();
	}
}


void CXYPlotDlg::OnOK()
{
	OnBnClickedBtnApply();
	CExtResizableDialog::OnOK();
}
