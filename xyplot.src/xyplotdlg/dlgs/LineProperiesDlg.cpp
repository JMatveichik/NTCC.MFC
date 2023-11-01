// LineProperiesDlg.cpp : implementation file
//
#include "stdafx.h"
#include "LineProperiesDlg.h"

// CLineProperiesDlg dialog

IMPLEMENT_DYNAMIC(CLineProperiesDlg,  CExtResizableDialog )
CLineProperiesDlg::CLineProperiesDlg(CWnd* pParent /*=NULL*/)
	: CExtNCW < CExtResizableDialog >(CLineProperiesDlg::IDD, pParent)	
{

// 	lineType     = pLine->GetType();
// 	lineTemplate = pLine->GetTemplate().c_str();
// 	lineWidth	 = pLine->GetWidth();
// 	lineColor	 = pLine->GetColor();	
}

CLineProperiesDlg::~CLineProperiesDlg()
{
}

void CLineProperiesDlg::DoDataExchange(CDataExchange* pDX)
{
	CExtNCW < CExtResizableDialog >::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_LINE_COLOR, btnLineColor);
	DDX_CBIndex(pDX, IDC_CMB_LINE_TYPE, lineType);
	DDX_Text(pDX, IDC_ED_CUSTOM_LINE_TEMPLATE, lineTemplate);
	DDX_Text(pDX, IDC_ED_LINE_WIDTH, lineWidth);
	__EXT_MFC_DDX_ColorButton(pDX, IDC_BTN_LINE_COLOR, lineColor);
}


BEGIN_MESSAGE_MAP(CLineProperiesDlg, CExtNCW < CExtResizableDialog >)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_CBN_SELENDOK(IDC_CMB_LINE_TYPE, OnCbnSelendokCmbLineType)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LINE_WIDTH, OnDeltaposSpinLineWidth)
	//ON_MESSAGE(CPN_SELENDOK,     OnSelEndOK)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CLineProperiesDlg message handlers

BOOL CLineProperiesDlg::OnInitDialog()
{
	CExtNCW < CExtResizableDialog >::OnInitDialog();

	SubclassChildControls();

// 	btnLineColor.SetBkColor();
// 	btnLineColor.m_clrSelected = lineColor;
// 		
// 	CSpinButtonCtrl* pSpin = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_LINE_WIDTH);
// 	pSpin->SetRange(1, 10);
// 
// 	GetDlgItem(IDC_ED_CUSTOM_LINE_TEMPLATE)->EnableWindow(lineType == PLS_CUSTOM);
// 
// 	DrawExampleLine();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
void CLineProperiesDlg::DrawExampleLine()
{
// 	CStatic* pExample = (CStatic*)GetDlgItem( IDC_EXAMPLE_LINE );
// 	CDC* pDC = pExample->GetDC();
// 
// 	RECT rc;
// 	pExample->GetWindowRect(&rc);
// 
// 	int width  = rc.right - rc.left;
// 	int height = rc.bottom - rc.top;
// 
// 	memDC.CreateCompatibleDC(pDC);
// 	exampBmp.CreateCompatibleBitmap(pDC, width, height);
// 
// 	CBitmap* pOldBmp = memDC.SelectObject( &exampBmp );
// 	pExample->ScreenToClient(&rc);
// 
// 	memDC.FillSolidRect(&rc, GetSysColor(COLOR_BTNFACE));
// 
// 	HPEN pOldPen = (HPEN)memDC.SelectObject( lineCurrent );
// 
// 	memDC.MoveTo(rc.left, rc.top + height / 2);
// 	memDC.LineTo(rc.right, rc.top + height / 2);
// 
// 	memDC.SelectObject(pOldPen);
// 	memDC.SelectObject(pOldBmp);
// 	memDC.DeleteDC();
// 
// 	pExample->SetBitmap(exampBmp);
// 	exampBmp.DeleteObject(); 

}

void CLineProperiesDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

// 	lineCurrent.SetColor(lineColor);
// 	lineCurrent.SetType(lineType);
// 	lineCurrent.SetTemplate((LPCTSTR)lineTemplate);
// 	lineCurrent.SetWidth(lineWidth);	
// 
// 	*pLine = lineCurrent;	

	OnOK();
}

void CLineProperiesDlg::OnCbnSelendokCmbLineType()
{
	UpdateData();
// 	lineCurrent.SetType(lineType);
// 	lineTemplate = lineCurrent.GetTemplate().c_str();
// 
// 	GetDlgItem(IDC_ED_CUSTOM_LINE_TEMPLATE)->EnableWindow(lineType == PLS_CUSTOM);
// 	UpdateData(FALSE);
// 
// 	DrawExampleLine();
}

void CLineProperiesDlg::OnDeltaposSpinLineWidth(NMHDR *pNMHDR, LRESULT *pResult)
{
// 	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
// 		
// 	lineCurrent.SetWidth(pNMUpDown->iPos);
// 	DrawExampleLine();

	*pResult = 0;
}


LONG CLineProperiesDlg::OnSelEndOK(UINT lParam, LONG wParam)
{
// 	lineCurrent.SetColor(COLORREF(lParam));
// 	DrawExampleLine();
	return 0;
}
void CLineProperiesDlg::OnPaint()
{
// 	DrawExampleLine();
 	CPaintDC dc(this); // device context for painting	
}
