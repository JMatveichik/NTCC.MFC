// PlotPropertiesDialog.cpp : implementation file
//

#include "stdafx.h"
#include "PlotPropertiesDialog.h"


// CPlotPropertiesDialog dialog

IMPLEMENT_DYNAMIC(CPlotPropertiesDialog,  CExtResizableDialog )
CPlotPropertiesDialog::CPlotPropertiesDialog(XYPlot* plot, CWnd* pParent /*=NULL*/)
	: CExtNCW < CExtResizableDialog >(CPlotPropertiesDialog::IDD, pParent)
{
	ASSERT(plot != NULL);

	this->plot = plot;

	Axis* pAxis = plot->GetAxis(PI_AXIS_TOP);
	pDlgAxisPropXT = new CAxisPropertyDlg(pAxis);

	pAxis = plot->GetAxis(PI_AXIS_BOTTOM);
	pDlgAxisPropXB = new CAxisPropertyDlg(pAxis);

	pAxis = plot->GetAxis(PI_AXIS_LEFT);
	pDlgAxisPropYL = new CAxisPropertyDlg(pAxis);

	pAxis = plot->GetAxis(PI_AXIS_RIGHT);
	pDlgAxisPropYR = new CAxisPropertyDlg(pAxis);

	pDlgGenProp	   = new CGeneralPropDlg(plot);

	pDlgProfProp   = new CProfilesPropDlg(plot);

	pDlgLevelsProp = new CLevelsPropDlg(plot);

	m_pActivePage =  pDlgGenProp;
}

CPlotPropertiesDialog::~CPlotPropertiesDialog()
{
	delete pDlgAxisPropXT;	
	delete pDlgAxisPropXB;	
	delete pDlgAxisPropYL;	
	delete pDlgAxisPropYR;	
	delete pDlgProfProp;
	delete pDlgGenProp;
	delete pDlgLevelsProp;
}

void CPlotPropertiesDialog::DoDataExchange(CDataExchange* pDX)
{
	CExtNCW < CExtResizableDialog >::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_PAGES_LIST, m_ctrlPagesList);

	DDX_Control(pDX, IDC_PAGES_LIST, m_wndList);
	DDX_Control(pDX, IDC_PAGE_TITLE, m_pageTitle);
	DDX_Control(pDX, IDC_BTN_LOAD, btnLoad);
	DDX_Control(pDX, IDC_BTN_SAVE, btnSave);
}


BEGIN_MESSAGE_MAP(CPlotPropertiesDialog, CExtNCW < CExtResizableDialog >)
	//ON_NOTIFY(LVN_ITEMCHANGING, IDC_PAGES_LIST, OnLvnItemchangingPagesList)
	ON_LBN_SELCHANGE(IDC_PAGES_LIST, OnSelchangeList)
	ON_BN_CLICKED(IDC_BTN_APPLY, OnBnClickedBtnApply)
	ON_BN_CLICKED(IDC_BTN_SAVE, OnBnClickedBtnSave)

	ON_COMMAND(ID_LOAD_FROM_FILE, OnLoadFromFile)
	ON_COMMAND(ID_LOAD_DEFAULT, OnLoadDefault)
	ON_COMMAND(ID_SAVE_TOFILE, OnSaveToFile)
	ON_COMMAND(ID_SAVE_AS_DEFAULT, OnSaveAsDefault)

	ON_BN_CLICKED(IDOK, &CPlotPropertiesDialog::OnBnClickedOk)
END_MESSAGE_MAP()


// CPlotPropertiesDialog message handlers

BOOL CPlotPropertiesDialog::OnInitDialog()
{
	CExtNCW < CExtResizableDialog >::OnInitDialog();

	SubclassChildControls();


	pDlgGenProp->Create(CGeneralPropDlg::IDD, this);	
	pDlgAxisPropXB->Create(CAxisPropertyDlg::IDD, this );
	pDlgAxisPropXT->Create(CAxisPropertyDlg::IDD, this );
	pDlgAxisPropYL->Create(CAxisPropertyDlg::IDD, this );
	pDlgAxisPropYR->Create(CAxisPropertyDlg::IDD, this );
	pDlgProfProp->Create(CProfilesPropDlg::IDD,  this );
	pDlgLevelsProp->Create(CLevelsPropDlg::IDD,   this );

	CRect rcPages;
	::GetWindowRect( ::GetDlgItem( m_hWnd, IDC_PAGES_PLACEHOLDER ), &rcPages);
	ScreenToClient( &rcPages );

	HINSTANCE hInstResource = ::AfxFindResourceHandle( MAKEINTRESOURCE( IDD_DLG_PLOT_PROPERTIES ), RT_ICON);
	ASSERT( hInstResource != NULL );

	HICON hIcon = (HICON) ::LoadImage( hInstResource, MAKEINTRESOURCE( IDI_MAIN_DLG ),IMAGE_ICON, 24, 24, 0 );
	SetIcon(hIcon, TRUE);

	//////////////////////////////////////////////////////////////////////////
	struct
	{
		LPCTSTR m_strResource;
		INT m_nWidth, m_nHeight;
		UINT m_nLoadImageFlags;
		CExtResizableDialog *m_pWndPage;
		LPCTSTR m_strText;
		COLORREF staticColor;
	} _iconInit[] =
	{
		{	MAKEINTRESOURCE( IDI_ICON1 ), 32, 32, 0, pDlgGenProp, _T("General"), RGB(153,204,255)},
		{	MAKEINTRESOURCE( IDI_ICON2 ), 32, 32, 0, pDlgAxisPropXB, _T("Bottom Axis"), RGB(255,153,204)},
		{	MAKEINTRESOURCE( IDI_ICON2 ), 32, 32, 0, pDlgAxisPropXT, _T("Top Axis"), RGB(153,153,255)},
		{	MAKEINTRESOURCE( IDI_ICON2 ), 32, 32, 0, pDlgAxisPropYL, _T("Left Axis"), RGB(204,204,255)},
		{	MAKEINTRESOURCE( IDI_ICON2 ), 32, 32, 0, pDlgAxisPropYR, _T("Right Axis"), RGB(254,204,255)},
		{	MAKEINTRESOURCE( IDI_ICON_AXIS ), 32, 32, 0, pDlgProfProp, _T("Profiles"), RGB(204,255,204)},
		{	MAKEINTRESOURCE( IDI_ICON3 ), 32, 32, 0, pDlgLevelsProp, _T("Levels && Regions"), RGB(170,50,110)}		
	};

	for( int nInit = 0; nInit < (sizeof(_iconInit)/sizeof(_iconInit[0])) ; nInit++ )
	{		
		_iconInit[nInit].m_pWndPage->MoveWindow( &rcPages );
		AddAnchor( _iconInit[nInit].m_pWndPage->GetSafeHwnd(), __RDA_LT, __RDA_RB );

		hIcon = (HICON) ::LoadImage( hInstResource, _iconInit[nInit].m_strResource,
			IMAGE_ICON, _iconInit[nInit].m_nWidth, _iconInit[nInit].m_nHeight, _iconInit[nInit].m_nLoadImageFlags );

		ASSERT( hIcon != NULL );
		CExtCmdIcon icon;

		icon.AssignFromHICON( hIcon, false );
		ASSERT( ! icon.IsEmpty() );

		m_wndList.AddIcon( icon, _iconInit[nInit].m_strText, _iconInit[nInit].m_pWndPage->GetSafeHwnd() );

		pagesColor.push_back( _iconInit[nInit].staticColor );
	} // for( nInit = 0; nInit < (sizeof(_iconInit)/sizeof(_iconInit[0])) ; nInit++ )


	_iconInit[0].m_pWndPage->ShowWindow(SW_SHOW);
	m_wndList.SetCurSel( 0 );

	/*Создание шрифта для заголовка страниц*/
	if (fontPagesTitle.GetSafeHandle() == NULL) 
	{
		CFont* pFont = m_pageTitle.GetFont();
		LOGFONT lf;
		pFont->GetLogFont(&lf);

		lf.lfWeight = FW_BLACK;
		lf.lfHeight = 20; 

		fontPagesTitle.CreateFontIndirect(&lf);

		m_pageTitle.SetFont(&fontPagesTitle);
	}	
	
	btnLoad.m_dwMenuOpt |= TPMX_NO_HIDE_RARELY;
	CMenu _menuLoad;
	VERIFY( _menuLoad.LoadMenu(IDR_MENU_LOAD) );
	btnLoad.m_menu.Attach( _menuLoad.Detach() );

	CMenu _menuSave;
	btnSave.m_dwMenuOpt |= TPMX_NO_HIDE_RARELY;
	VERIFY( _menuSave.LoadMenu(IDR_MENU_SAVE) );
	btnSave.m_menu.Attach( _menuSave.Detach() );
	
	OnSelchangeList();
    return TRUE;
}

void CPlotPropertiesDialog::OnLoadFromFile()
{
	static char BASED_CODE szFilter[] = "Plot Config Files (*.pcfg)|*.pcfg|All Files (*.*)|*.*||";
	CFileDialog dlg(TRUE, "pcfg", NULL, OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST, szFilter, this) ;
	if (dlg.DoModal() == IDOK)
	{
		plot->LoadProperties( dlg.GetPathName() );
	}
}

void CPlotPropertiesDialog::OnSaveToFile()
{

	static char BASED_CODE szFilter[] = "Plot Config Files (*.pcfg)|*.pcfg|All Files (*.*)|*.*||";
	CFileDialog dlg(FALSE, "pcfg", "init", OFN_PATHMUSTEXIST, szFilter, this) ;
	if (dlg.DoModal() == IDOK)
	{
		plot->SaveProperties( dlg.GetPathName() );
	}
}

void CPlotPropertiesDialog::OnLoadDefault()
{

}

void CPlotPropertiesDialog::OnSaveAsDefault()
{

}

void CPlotPropertiesDialog::OnSelchangeList() 
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

	{
		::ShowWindow(pItemData->m_hWndPage, SW_SHOW);
		CString str = "    ";
		str += 	pItemData->m_strText;
		str += " properties";
		
		CExtCmdIcon icon = pItemData->m_icon;
		m_pageTitle.SetIcon(icon.ExtractHICON());
		m_pageTitle.SetBkColor(pagesColor.at(nIndex));
		m_pageTitle.SetWindowText(str);

	}
}

void CPlotPropertiesDialog::OnBnClickedBtnApply()
{
	pDlgAxisPropXT->OnApply();
	pDlgAxisPropXB->OnApply();
	pDlgAxisPropYL->OnApply();
	pDlgAxisPropYR->OnApply();
	pDlgGenProp->OnApply();
	pDlgProfProp->OnApply();

	plot->Update();
}

void CPlotPropertiesDialog::OnBnClickedBtnSave()
{
	plot->RegSetDefaults();
}

void CPlotPropertiesDialog::OnBnClickedOk()
{
	OnBnClickedBtnApply();
	OnOK();
}
