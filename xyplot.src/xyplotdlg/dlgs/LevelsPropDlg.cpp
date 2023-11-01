// LevelsPropDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LevelsPropDlg.h"
#include "LineProperiesDlg.h"


using namespace xyplot;

// CLevelsPropDlg dialog

IMPLEMENT_DYNAMIC(CLevelsPropDlg, CPlotDlgPage)
CLevelsPropDlg::CLevelsPropDlg(CWnd* parent, HPLOT& hActivePlot)
	: CPlotDlgPage(parent, hActivePlot, CLevelsPropDlg::IDD)
	, axisToAttach(0)
	, levelValue(0.0)
	, levelCaption(_T(""))
	, useLevelCaption(FALSE)
{
	
}

CLevelsPropDlg::~CLevelsPropDlg()
{
}



void CLevelsPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CPlotDlgPage::DoDataExchange(pDX);
	DDX_CBIndex(pDX, IDC_CMB_AXIS_TO_ATTACH, axisToAttach);
	DDX_Text(pDX, IDC_ED_VALUE, levelValue);
	DDX_Text(pDX, IDC_ED_CAPTION, levelCaption);
	DDX_Control(pDX, IDC_LEVELS_LIST, levelsList);
	DDX_Check(pDX, IDC_CHK_USE_CAPTION, useLevelCaption);
	DDX_Control(pDX, IDC_CMB_CAPTION_ALIGN, captionAlignCmb);
}


BEGIN_MESSAGE_MAP(CLevelsPropDlg, CPlotDlgPage)
	ON_BN_CLICKED(IDC_BTN_CAPTION_FONT, OnBnClickedBtnCaptionFont)
	ON_LBN_SELCHANGE(IDC_LEVELS_LIST, OnLbnSelchangeLevelsList)
	ON_CBN_SELCHANGE(IDC_CMB_CAPTION_ALIGN, OnCbnSelchangeCmbCaptionAlign)
	ON_BN_CLICKED(IDC_CHK_USE_CAPTION, OnBnClickedChkUseCaption)
	ON_BN_CLICKED(IDC_BTN_LEVEL_LINE_TYPE, OnBnClickedBtnLevelLineType)
END_MESSAGE_MAP()


// CLevelsPropDlg message handlers

void CLevelsPropDlg::OnInitialRequest()
{

}

void CLevelsPropDlg::OnFinalRequest()
{

}

BOOL CLevelsPropDlg::OnInitDialog()
{
	CPlotDlgPage::OnInitDialog();

/*	LevelsMap levels = plot->GetLevelsMap();

	for (LevelsMapIterator it = levels.begin(); it != levels.end(); ++it)
	{
		LevelLine* pLevel = (*it).second;

		int nCount = levelsList.AddString(pLevel->GetLabel().c_str());

		levelsList.SetItemData(nCount, (*it).first);
	}

	levelsList.SetCurSel(0);
	OnLbnSelchangeLevelsList();
*/
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CLevelsPropDlg::OnLbnSelchangeLevelsList()
{
/*	const long levelID = (const long)levelsList.GetItemData(levelsList.GetCurSel());

	if (levelID == LB_ERR)
	{
		pActiveLevel = NULL;
		return;
	}

	LevelsMap::const_iterator it = plot->GetLevelsMap().find(levelID);
	if (it == plot->GetLevelsMap().end())
		return;

	LevelLine* pLevel = it->second;
	//LevelLine* pLevel = (LevelLine*)ptr;

	if(pLevel == NULL)
		return;

	axisToAttach    = pLevel->GetAxisToAttach();
	levelValue      = pLevel->GetValue();
	useLevelCaption = pLevel->IsLabelEnabled();
	levelCaption    = pLevel->GetLabel().c_str();
	captionAlign	= pLevel->GetLabelPosition();

	PrepareCaptionAlignControl();

	EnableCaptionsControls();

	UpdateData(FALSE);

	pActiveLevel =  pLevel;
	plot->Update();
	*/
}
void CLevelsPropDlg::EnableCaptionsControls ()
{
	GetDlgItem(IDC_CMB_CAPTION_ALIGN)->EnableWindow(useLevelCaption);
	GetDlgItem(IDC_ED_CAPTION)->EnableWindow(useLevelCaption);
	GetDlgItem(IDC_BTN_CAPTION_FONT)->EnableWindow(useLevelCaption);
}

void CLevelsPropDlg::PrepareCaptionAlignControl()
{
	captionAlignCmb.ResetContent();
	BOOL vertical = ( axisToAttach == PI_AXIS_BOTTOM || axisToAttach == PI_AXIS_TOP);	

	captionAlignCmb.AddString(vertical ? "Left   - Above" : "Bottom - Left");	 
	captionAlignCmb.AddString(vertical ? "Right  - Above" : "Top    - Left");   
	captionAlignCmb.AddString(vertical ? "Center - Above" : "Center - Left");   
	captionAlignCmb.AddString(vertical ? "Left   - Under" : "Bottom - Right");	 
	captionAlignCmb.AddString(vertical ? "Right  - Under" : "Top    - Right");	 
	captionAlignCmb.AddString(vertical ? "Center - Under" : "Center - Right");	 
	captionAlignCmb.AddString(vertical ? "Left   - At line" : "Bottom - At line"); 
	captionAlignCmb.AddString(vertical ? "Right  - At line" : "Top    - At line"); 
	captionAlignCmb.AddString(vertical ? "Center - At line" : "Center - At line"); 
	
	unsigned long aligns[] = {
		LLP_ABOVELEFT, 
		LLP_ABOVERIGHT,
		LLP_ABOVECENTER,
		LLP_UNDERLEFT,
		LLP_UNDERRIGHT,
		LLP_UNDERCENTER,
		LLP_ATLINELEFT,
		LLP_ATLINERIGHT,
		LLP_ATLINECENTER
	};

	for (int i=0; i < sizeof(aligns)/sizeof(unsigned long); i++)
	{
		captionAlignCmb.SetItemData(i, aligns[i]);
		if (captionAlignCmb.GetItemData(i) == captionAlign)
			captionAlignCmb.SetCurSel(i);
	}

}


void CLevelsPropDlg::OnBnClickedBtnCaptionFont()
{
	/*
	LOGFONT lf;
	memset(&lf, 0, sizeof (LOGFONT));

	EnhancedString* pES =  &pActiveLevel->GetLabel();

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
		pES->SetFs( 2 * dlg.GetSize() / 10 );
	}
	*/
}



void CLevelsPropDlg::OnCbnSelchangeCmbCaptionAlign()
{
// 	captionAlign = aligns[captionAlignCmb.GetCurSel()];
// 	pActiveLevel->SetLabelPosition(captionAlign);	
}

void CLevelsPropDlg::OnBnClickedChkUseCaption()
{
	UpdateData(TRUE);
	EnableCaptionsControls();
}

void CLevelsPropDlg::OnBnClickedBtnLevelLineType()
{
// 	LineData lineEdited = pActiveLevel->GetLine();
// 
// 	CLineProperiesDlg dlg(&lineEdited, this);
// 
// 	if (dlg.DoModal() == IDOK)
// 	{
// 		pActiveLevel->GetLine() = lineEdited;
// 	}
}