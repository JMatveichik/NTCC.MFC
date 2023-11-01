// AxisPropertyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AxisPropertyDlg.h"
#include "LineProperiesDlg.h"


// CAxisPropertyDlg dialog
using namespace xyplot;

IMPLEMENT_DYNAMIC(CAxisPropertyDlg, CPlotDlgPage )

CAxisPropertyDlg::CAxisPropertyDlg(CWnd* parent, HPLOT& hActivePlot, long lAxisID)
: CPlotDlgPage(parent, hActivePlot, CAxisPropertyDlg::IDD),
	lActiveAxisID(lAxisID)
{
	/*
	IsPlotItemEnabled(lActiveAxisID, 0);
	GetPlotItemLong(PI_LEGEND, PLP_ALIGN) + 1;
	GetPlotItemColor(PI_BACKGROUND, BGP_COLOR_START);
	GetPlotItemString(PI_TITLE, 0);
	*/
		

	
	//lineAxis	=	axis->GetLine();

	//lineGridMaj =   axis->GetGridLine(TRUE);
	//lineGridMin =   axis->GetGridLine(FALSE);

	//lineTicksMaj = axis->GetTicksLine(TRUE);
	//lineTicksMin = axis->GetTicksLine(FALSE);
	
}

CAxisPropertyDlg::~CAxisPropertyDlg()
{
}

void CAxisPropertyDlg::DoDataExchange(CDataExchange* pDX)
{
	CExtResizableDialog ::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHK_ENABLE_TITLE, showTitle);
	DDX_Check(pDX, IDC_CHK_AUTOSCALED, autoScaled);
	DDX_Check(pDX, IDC_CHK_LOG_SCALE, logScaled);
	DDX_Text(pDX, IDC_AXIS_LABEL, strTitle);
	DDX_Text(pDX, IDC_ED_AXIS_RANGE_MIN, rangeMin);
	DDX_Text(pDX, IDC_ED_AXIS_RANGE_MAX, rangeMax);
	DDX_Check(pDX, IDC_CHK_MAJ_TICKS, ticksMajEnabled);
	DDX_Check(pDX, IDC_CHK_MIN_TICKS, ticksMinEnabled);
	DDX_Text(pDX, IDC_MAJ_TICKS_COUNT, ticksMajCount);
	DDX_Text(pDX, IDC_MIN_TICKS_COUNT, ticksMinCount);
	DDX_Text(pDX, IDC_MAJ_TICKS_LEN, ticksMajLen);
	DDX_Text(pDX, IDC_MIN_TICKS_LEN, ticksMinLen);
	DDX_Check(pDX, IDC_CHK_MAJ_DIGITS, digitsMajEnabled);
	DDX_Check(pDX, IDC_CHK_MIN_DIGITS, digitsMinEnabled);
	DDX_Check(pDX, IDC_CHK_MAJ_GRID, gridMajEnabled);
	DDX_Check(pDX, IDC_CHK_MIN_GRID, gridMinEnabled);
	DDX_Text(pDX, IDC_ED_DIGITS_MAJ_FORMAT, digitsMajFormat);
	DDX_Text(pDX, IDC_ED_DIGITS_MIN_FORMAT, digitsMinFormat);	
	DDX_CBIndex(pDX, IDC_CMB_TICKS_STYLE, ticksStyle);
}


BEGIN_MESSAGE_MAP(CAxisPropertyDlg, CExtNCW < CExtResizableDialog >)		
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BTN_LINE_TICKS_MAJ, IDC_BTN_LINE_AXIS, OnChangeLineType)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CHK_ENABLE_TITLE, IDC_CHK_MIN_GRID, OnChangeCheckState)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BTN_TITLE_FONT, IDC_BTN_DIG_MIN_FONT, OnChangeFont)
END_MESSAGE_MAP()

void CAxisPropertyDlg::OnChangeCheckState(UINT id)
{
	UpdateData(TRUE);
	UpdateControlsState();
}
// CAxisPropertyDlg message handlers
void CAxisPropertyDlg::OnChangeLineType(UINT id)
{
/*	LineData* pLineSelected = NULL;

	switch(id)
	{
	case IDC_BTN_LINE_TICKS_MAJ:
		pLineSelected = &lineTicksMaj;
		break;
	case IDC_BTN_LINE_TICKS_MIN:
		pLineSelected = &lineTicksMin;
		break;
	case IDC_BTN_LINE_GRID_MAJ:
		pLineSelected = &lineGridMaj;
	    break;
	case IDC_BTN_LINE_GRID_MIN:
		pLineSelected = &lineGridMin;
	    break;
	case IDC_BTN_LINE_AXIS:
		pLineSelected = &lineAxis;
	    break;
	}

	LineData lineEdited = *pLineSelected;

	CLineProperiesDlg dlg(&lineEdited, this);
	
	if (dlg.DoModal() == IDOK)
	{
		*pLineSelected = lineEdited;
	}
	*/
}

void CAxisPropertyDlg::OnInitialRequest()
{
	showTitle  = IsPlotItemEnabled(lActiveAxisID, PAI_TITLE);
	logScaled  = IsPlotItemEnabled(lActiveAxisID, PAI_LOG10);
	autoScaled = IsPlotItemEnabled(lActiveAxisID, PAI_AUTOSCALE);

	strTitle   = GetPlotItemString(lActiveAxisID, PAI_TITLE);

	ticksMajEnabled = IsPlotItemEnabled(lActiveAxisID, PAI_MAJOR_TICKS);
	ticksMinEnabled = IsPlotItemEnabled(lActiveAxisID, PAI_MINOR_TICKS);

	ticksMajCount = GetPlotItemLong(lActiveAxisID, PAI_MAJOR_TICKS_COUNT);
	ticksMinCount = GetPlotItemLong(lActiveAxisID, PAI_MINOR_TICKS_COUNT);
	ticksMajLen   = GetPlotItemLong(lActiveAxisID, PAI_MAJOR_TICKS_LENGTH);
	ticksMinLen   = GetPlotItemLong(lActiveAxisID, PAI_MINOR_TICKS_LENGTH);

	digitsMajEnabled = IsPlotItemEnabled(lActiveAxisID, PAI_MAJOR_DIGITS);
	digitsMinEnabled = IsPlotItemEnabled(lActiveAxisID, PAI_MINOR_DIGITS);

	digitsMajFormat = GetPlotItemString(lActiveAxisID, PAI_MAJOR_DIGITS_FORMAT);
	digitsMinFormat = GetPlotItemString(lActiveAxisID, PAI_MINOR_DIGITS_FORMAT);

	gridMajEnabled = IsPlotItemEnabled(lActiveAxisID, PAI_MAJOR_GRID_LINES);
	gridMinEnabled = IsPlotItemEnabled(lActiveAxisID, PAI_MAJOR_GRID_LINES);
	
	rangeMin = GetPlotItemDouble(lActiveAxisID, PAI_LOWER_LIMIT);
	rangeMax = GetPlotItemDouble(lActiveAxisID, PAI_UPPER_LIMIT);

	//ticksStyle = axis->GetTicksStyle() - 1;
}

void CAxisPropertyDlg::OnFinalRequest()
{
	EnablePlotItem(lActiveAxisID, PAI_TITLE, showTitle);
	EnablePlotItem(lActiveAxisID, PAI_LOG10, logScaled);
	EnablePlotItem(lActiveAxisID, PAI_AUTOSCALE, autoScaled);

	SetPlotItemString(lActiveAxisID, PI_TITLE, strTitle);

	EnablePlotItem(lActiveAxisID, PAI_MAJOR_TICKS, ticksMajEnabled);
	EnablePlotItem(lActiveAxisID, PAI_MINOR_TICKS, ticksMinEnabled);

	SetPlotItemLong(lActiveAxisID, PAI_MAJOR_TICKS_COUNT, ticksMajCount );
	SetPlotItemLong(lActiveAxisID, PAI_MINOR_TICKS_COUNT, ticksMinCount);
	SetPlotItemLong(lActiveAxisID, PAI_MAJOR_TICKS_LENGTH, ticksMajLen);
	SetPlotItemLong(lActiveAxisID, PAI_MINOR_TICKS_LENGTH, ticksMinLen);

	EnablePlotItem(lActiveAxisID, PAI_MAJOR_DIGITS, digitsMajEnabled);
	EnablePlotItem(lActiveAxisID, PAI_MINOR_DIGITS, digitsMinEnabled);

	SetPlotItemString(lActiveAxisID, PAI_MAJOR_DIGITS_FORMAT, digitsMajFormat);
	SetPlotItemString(lActiveAxisID, PAI_MINOR_DIGITS_FORMAT, digitsMinFormat);

	EnablePlotItem(lActiveAxisID, PAI_MAJOR_GRID_LINES, gridMajEnabled);
	EnablePlotItem(lActiveAxisID, PAI_MAJOR_GRID_LINES, gridMinEnabled);

	SetPlotItemDouble(lActiveAxisID, PAI_LOWER_LIMIT, rangeMin);
	SetPlotItemDouble(lActiveAxisID, PAI_UPPER_LIMIT, rangeMax);
}

BOOL CAxisPropertyDlg::OnInitDialog()
{
	CPlotDlgPage::OnInitDialog();
	

	
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_MAJ_TICKS_COUNT))->SetRange(0, 100);
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_MIN_TICKS_COUNT))->SetRange(0, 100);
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_MAJ_TICKS_LEN))->SetRange(0, 100);
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_MIN_TICKS_LEN))->SetRange(0, 100);

	UpdateControlsState();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CAxisPropertyDlg::UpdateControlsState()
{
	GetDlgItem(IDC_AXIS_LABEL)->EnableWindow(showTitle);
	GetDlgItem(IDC_BTN_TITLE_FONT)->EnableWindow(showTitle);

	GetDlgItem(IDC_ED_AXIS_RANGE_MIN)->EnableWindow(!autoScaled);
	GetDlgItem(IDC_ED_AXIS_RANGE_MAX)->EnableWindow(!autoScaled);

	GetDlgItem(IDC_MAJ_TICKS_COUNT)->EnableWindow(ticksMajEnabled);
	GetDlgItem(IDC_MAJ_TICKS_LEN)->EnableWindow(ticksMajEnabled);
	GetDlgItem(IDC_BTN_LINE_TICKS_MAJ)->EnableWindow(ticksMajEnabled);
	GetDlgItem(IDC_SPIN_MAJ_TICKS_COUNT)->EnableWindow(ticksMajEnabled);
	GetDlgItem(IDC_SPIN_MAJ_TICKS_LEN)->EnableWindow(ticksMajEnabled);

	GetDlgItem(IDC_MIN_TICKS_COUNT)->EnableWindow(ticksMinEnabled);
	GetDlgItem(IDC_MIN_TICKS_LEN)->EnableWindow(ticksMinEnabled);
	GetDlgItem(IDC_BTN_LINE_TICKS_MIN)->EnableWindow(ticksMinEnabled);
	GetDlgItem(IDC_SPIN_MIN_TICKS_COUNT)->EnableWindow(ticksMinEnabled);
	GetDlgItem(IDC_SPIN_MIN_TICKS_LEN)->EnableWindow(ticksMinEnabled);	

	GetDlgItem(IDC_CMB_TICKS_STYLE)->EnableWindow(ticksMajEnabled || ticksMinEnabled);

	GetDlgItem(IDC_BTN_DIG_MAJ_FONT)->EnableWindow(digitsMajEnabled);
	GetDlgItem(IDC_ED_DIGITS_MAJ_FORMAT)->EnableWindow(digitsMajEnabled);	

	GetDlgItem(IDC_BTN_DIG_MIN_FONT)->EnableWindow(digitsMinEnabled);
	GetDlgItem(IDC_ED_DIGITS_MIN_FORMAT)->EnableWindow(digitsMinEnabled);

	GetDlgItem(IDC_BTN_LINE_GRID_MAJ)->EnableWindow(gridMajEnabled);
	GetDlgItem(IDC_BTN_LINE_GRID_MIN)->EnableWindow(gridMinEnabled);	
}



void CAxisPropertyDlg::OnChangeFont(UINT id)
{
	/*
	EnhancedString* pES =  NULL;

	switch(id)
	{
	case IDC_BTN_TITLE_FONT:
		//pES = &axis->GetLabelEnhancedString();
		break;
	case IDC_BTN_DIG_MAJ_FONT:
		pES = &axis->GetDigitsEnhancedString(TRUE);
		break;
	case IDC_BTN_DIG_MIN_FONT:
		pES = &axis->GetDigitsEnhancedString(FALSE);
	    break;
	}

	LOGFONT lf;


	memset(&lf, 0, sizeof (LOGFONT));

	CClientDC dc(this);
	lf.lfHeight = -MulDiv(pES->GetFs() / 2, dc.GetDeviceCaps(LOGPIXELSY), 72);
	strcpy_s(lf.lfFaceName, pES->GetFaceName().c_str());

	lf.lfItalic = pES->IsPropertyEnabled(ESP_ITALIC);
	lf.lfUnderline = pES->IsPropertyEnabled(ESP_UNDERLINE);
	lf.lfWeight = pES->IsPropertyEnabled(ESP_BOLD) ? FW_BOLD : FW_NORMAL;
	
	CFontDialog dlg(&lf);
	
	if ( dlg.DoModal() == IDOK)
	{
		pES->EnableProperty(ESP_ITALIC, dlg.m_lf.lfItalic);
		pES->EnableProperty(ESP_UNDERLINE, dlg.m_lf.lfUnderline);
		pES->EnableProperty(ESP_BOLD, dlg.m_lf.lfWeight == FW_BOLD);
		pES->SetColor( dlg.GetColor() );
		pES->SetFaceName(dlg.m_lf.lfFaceName);
		int fs = dlg.GetSize() / 10 ; //-MulDiv(dlg.m_lf.lfHeight, 72, dc.GetDeviceCaps(LOGPIXELSY));
		
		pES->SetFs( 2 * fs );
		if (id == IDC_BTN_TITLE_FONT)
		{
			strTitle = pES->Body().c_str();
			UpdateData(FALSE);
		}
	}*/
}
