// ProfilesPropDllg.cpp : implementation file
//

#include "stdafx.h"
#include "ProfilesPropDlg.h"
#include "LineProperiesDlg.h"


// CProfilesPropDllg dialog

using namespace xyplot;

IMPLEMENT_DYNAMIC(CProfilesPropDlg, CPlotDlgPage)

CProfilesPropDlg::CProfilesPropDlg(CWnd* parent, HPLOT& hActivePlot)
	: CPlotDlgPage(parent, hActivePlot, CProfilesPropDlg::IDD)
	, drawProfileLine(FALSE)
	, markType(0)
	, markSize(0)
	, markFreq(0)
	, markLineColor(0x000000)
	, markFillColor(0x000000)	

{
	
}

CProfilesPropDlg::~CProfilesPropDlg()
{
}

void CProfilesPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CPlotDlgPage::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_PROFILE_VISIBLE, drawProfileLine);
	DDX_Check(pDX, IDC_USE_MARKS, drawProfileMarks);
	DDX_Control(pDX, IDC_PROFILE_LIST, profileList);
	DDX_CBIndex(pDX, IDC_CMB_MARK_TYPE, markType);
	DDX_Text(pDX, IDC_ED_MARK_SIZE, markSize);
	DDX_Text(pDX, IDC_ED_MARK_FREQ, markFreq);
	DDX_Control(pDX, IDC_MARK_LINE_COLOR, btnMarkLineColor);
	DDX_Control(pDX, IDC_MARK_FILL_COLOR, btnMarkFillColor);
	__EXT_MFC_DDX_ColorButton(pDX, IDC_MARK_LINE_COLOR, markLineColor);
	__EXT_MFC_DDX_ColorButton(pDX, IDC_MARK_FILL_COLOR, markFillColor);


}


BEGIN_MESSAGE_MAP(CProfilesPropDlg, CPlotDlgPage)
	ON_LBN_SELCHANGE(IDC_PROFILE_LIST, OnLbnSelchangeProfileList)
	ON_BN_CLICKED(IDC_USE_MARKS, OnBnClickedUseMarks)
	ON_BN_CLICKED(IDC_BTN_LINE_PROFILE, OnBnClickedBtnLineProfile)
END_MESSAGE_MAP()


// CProfilesPropDllg message handlers

void CProfilesPropDlg::OnInitialRequest()
{
	XYPlotManager& pm = XYPlotManager::Instance();

	pm.GetProfileList(hPlot, profiles);

	for (std::vector<HPROFILE>::const_iterator it = profiles.begin(); it != profiles.end(); ++it)
	{
		CString str = GetPlotItemString(long(*it), PRP_NAME);
		int nCount = profileList.AddString(str);

		profileList.SetItemData(nCount, DWORD_PTR(long(*it)));
	}

}

void CProfilesPropDlg::OnFinalRequest()
{
	// 	if ( pActiveProf == NULL )
	// 		return FALSE;
	// 
	// 	UpdateData(TRUE);
	// 
	// 	pActiveProf->SetVisible(drawProfileLine);
	// 	pActiveProf->UseSymblos(drawProfileMarks);
	// 	pActiveProf->GetMarks().SetMarkType(markType);
	// 	pActiveProf->GetMarks().SetSize(markSize);
	// 	pActiveProf->GetMarks().SetFrequency(markFreq);
	// 	pActiveProf->GetMarks().SetLineColor(markLineColor);
	// 	pActiveProf->GetMarks().SetFillColor(markFillColor);
	// 

}

BOOL CProfilesPropDlg::OnInitDialog()
{
	CPlotDlgPage::OnInitDialog();

	profileList.SetCurSel(0);
	OnLbnSelchangeProfileList();

	return TRUE;
}

void CProfilesPropDlg::OnLbnSelchangeProfileList()
{
 	const long profID = (const long)profileList.GetItemData(profileList.GetCurSel());
 
	if (profID == LB_ERR)
 	{
 		activeProf = -1L;
 		return;
 	}

 	UpdateData(TRUE);
 
 	if ( activeProf != -1L && activeProf != profID)
 	{
/*		
		EnablePlotItem(activeProf, )
 		pActiveProf->SetVisible(drawProfileLine);
 		pActiveProf->UseSymblos(drawProfileMarks);
 		pActiveProf->GetMarks().SetMarkType(markType);
 		pActiveProf->GetMarks().SetSize(markSize);
 		pActiveProf->GetMarks().SetFrequency(markFreq);
 		pActiveProf->GetMarks().SetLineColor(markLineColor);
 		pActiveProf->GetMarks().SetFillColor(markFillColor);
*/
 	}	
// 
// 	drawProfileLine = pProf->IsVisible();
// 	drawProfileMarks = pProf->IsUseSymblos();
// 	markType = pProf->GetMarks().GetMarkType();
// 	markSize  = pProf->GetMarks().GetSize();
// 	markFreq = pProf->GetMarks().GetFrequency();
// 	markLineColor = pProf->GetMarks().GetLineColor();
// 	markFillColor = pProf->GetMarks().GetFillColor();
// 	
// 	UpdateData(FALSE);	
// 	
// 	OnMarksEnable();
// 
// 	pActiveProf =  pProf;
// 	plot->Update();
}

void CProfilesPropDlg::OnMarksEnable()
{
	GetDlgItem(IDC_CMB_MARK_TYPE)->EnableWindow(drawProfileMarks);
	GetDlgItem(IDC_ED_MARK_SIZE)->EnableWindow(drawProfileMarks);
	GetDlgItem(IDC_ED_MARK_FREQ)->EnableWindow(drawProfileMarks);
	GetDlgItem(IDC_MARK_LINE_COLOR)->EnableWindow(drawProfileMarks);
	GetDlgItem(IDC_MARK_FILL_COLOR)->EnableWindow(drawProfileMarks);
	
}

void CProfilesPropDlg::OnBnClickedUseMarks()
{
	UpdateData(TRUE);
	OnMarksEnable();
}


void CProfilesPropDlg::OnBnClickedBtnLineProfile()
{
// 	LineData lineEdited = pActiveProf->GetLineData();
// 
// 	CLineProperiesDlg dlg(&lineEdited, this);
// 
// 	if (dlg.DoModal() == IDOK)
// 	{
// 		pActiveProf->GetLineData() = lineEdited;
	//}
}

