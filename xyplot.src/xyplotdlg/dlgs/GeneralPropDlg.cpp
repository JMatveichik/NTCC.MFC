// GeneralPropDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GeneralPropDlg.h"
#include "LineProperiesDlg.h"
#include "../../../xyplot/src/rtfstring.h"

// CGeneralPropDlg dialog
using namespace xyplot;

IMPLEMENT_DYNAMIC(CGeneralPropDlg, CPlotDlgPage)
	CGeneralPropDlg::CGeneralPropDlg(CWnd* parent, HPLOT& hActivePlot)
	: CPlotDlgPage(parent, hActivePlot, CGeneralPropDlg::IDD)
	
{
	
	

}

CGeneralPropDlg::~CGeneralPropDlg()
{

}

void CGeneralPropDlg::OnInitialRequest()
{
	showTitle    = IsPlotItemEnabled(PI_TITLE, 0);
	showBorder   = IsPlotItemEnabled(PI_BORDER, 0);
	showLegend   = IsPlotItemEnabled(PI_LEGEND, 0);

	legendPos	 = GetPlotItemLong(PI_LEGEND, PLP_ALIGN) + 1;

	clrGBk1 = GetPlotItemColor(PI_BACKGROUND, BGP_COLOR_START);
	clrGBk2 = GetPlotItemColor(PI_BACKGROUND, BGP_COLOR_END);

	clrCBk1 = GetPlotItemColor(PI_BACKGROUND_CLIENT, BGP_COLOR_START);
	clrCBk2 = GetPlotItemColor(PI_BACKGROUND_CLIENT, BGP_COLOR_END);

	strPlotTitle = GetPlotItemString(PI_TITLE, 0);

	plotGradientDir = GetPlotItemLong(PI_BACKGROUND, BGP_DIRECTION);
	clientGradientDir  = GetPlotItemLong(PI_BACKGROUND_CLIENT, BGP_DIRECTION);

	legendLineWidth = GetPlotItemLong(PI_LEGEND, PLP_LINELENGTH);
	
}

void CGeneralPropDlg::OnFinalRequest()
{
	EnablePlotItem(PI_TITLE, 0, showTitle);
	EnablePlotItem(PI_BORDER, 0, showBorder);
	EnablePlotItem(PI_LEGEND, 0, showLegend);

	SetPlotItemLong(PI_LEGEND, PLP_ALIGN, legendPos - 1); 

	SetPlotItemColor(PI_BACKGROUND, BGP_COLOR_START, clrGBk1);
	SetPlotItemColor(PI_BACKGROUND, BGP_COLOR_END, clrGBk2);

	SetPlotItemColor(PI_BACKGROUND_CLIENT, BGP_COLOR_START, clrCBk1);
	SetPlotItemColor(PI_BACKGROUND_CLIENT, BGP_COLOR_END, clrCBk2);

	SetPlotItemString(PI_TITLE, 0, (LPCTSTR)strPlotTitle);

	SetPlotItemLong(PI_BACKGROUND, BGP_DIRECTION, plotGradientDir);
	SetPlotItemLong(PI_BACKGROUND_CLIENT, BGP_DIRECTION, clientGradientDir);

	SetPlotItemLong(PI_LEGEND, PLP_LINELENGTH, legendLineWidth);
}

void CGeneralPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CPlotDlgPage::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_CHK_SHOW_TITLE, showTitle);
	DDX_Check(pDX, IDC_CHK_SHOW_BORDER, showBorder);
	DDX_Check(pDX, IDC_CHK_SHOW_LEGEND, showLegend);
	
	DDX_CBIndex(pDX, IDC_BTN_CBG_DIRECT, clientGradientDir);
	DDX_CBIndex(pDX, IDC_BTN_GBG_DIRECT, plotGradientDir);

	DDX_CBIndex(pDX, IDC_COMBO_LEG_ALIGN, legendPos);

	DDX_Text(pDX, IDC_LEGEND_LINES_WIDTH, legendLineWidth);
	DDX_Text(pDX, IDC_TITLE_EDIT, strPlotTitle);
	
	DDX_Control(pDX, IDC_TITLE_EDIT, editTitle);

	DDX_Control(pDX, IDC_BTN_CBG_CLR1, btnCBk1);
	DDX_Control(pDX, IDC_BTN_CBG_CLR2, btnCBk2);
	DDX_Control(pDX, IDC_BTN_GBG_CLR1, btnGBk1);
	DDX_Control(pDX, IDC_BTN_GBG_CLR2, btnGBk2);
 	
	__EXT_MFC_DDX_ColorButton(pDX, IDC_BTN_CBG_CLR1, clrCBk1);
	__EXT_MFC_DDX_ColorButton(pDX, IDC_BTN_CBG_CLR2, clrCBk2);
	__EXT_MFC_DDX_ColorButton(pDX, IDC_BTN_GBG_CLR1, clrGBk1);
	__EXT_MFC_DDX_ColorButton(pDX, IDC_BTN_GBG_CLR2, clrGBk2);
}

BEGIN_MESSAGE_MAP(CGeneralPropDlg, CPlotDlgPage)
	ON_BN_CLICKED(IDC_BTN_BORDER_LINE, OnBnClickedBtnBorderLine)	
END_MESSAGE_MAP()

// CGeneralPropDlg message handlers
BOOL CGeneralPropDlg::OnInitDialog()
{
	CPlotDlgPage::OnInitDialog();	

	btnCBk1.m_clrSelected = clrCBk1;
 	btnCBk2.m_clrSelected = clrCBk2;
 	btnGBk1.m_clrSelected = clrGBk1;
 	btnGBk2.m_clrSelected = clrGBk2;

	CRect rc;
	editTitle.GetWindowRect(&rc);

	HMODULE hm = LoadLibrary("riched20.dll");
	
	RTFString rtf( hm );
	CSize sz;

	rtf.SetText((LPCTSTR)strPlotTitle);
	rtf.GetDimensions(sz);
	
	editTitle.SendMessage(EM_SETZOOM, int(rc.Width()*0.95), sz.cx);
	
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_LEGEND_LINES_WIDTH))->SetRange(0, 100);

	FreeLibrary(hm);

	return TRUE;
}

void CGeneralPropDlg::OnBnClickedBtnBorderLine()
{
// 	CLineProperiesDlg dlg(&borderLine, this);
// 	
// 	dlg.DoModal();
}



