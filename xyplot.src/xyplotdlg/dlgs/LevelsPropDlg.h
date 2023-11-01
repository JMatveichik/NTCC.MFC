#pragma once

#include "..\resource.h"
#include "PlotDlgPage.h"

// CLevelsPropDlg dialog

class CLevelsPropDlg : public CPlotDlgPage
{
	DECLARE_DYNAMIC(CLevelsPropDlg)

public:
	CLevelsPropDlg(CWnd* pParent, xyplot::HPLOT& hActivePlot);   // standard constructor
	virtual ~CLevelsPropDlg();

// Dialog Data
	enum { IDD = IDD_DLG_LEVELS_PROPERTIES };

protected:
	DECLARE_MESSAGE_MAP()

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	
	virtual void OnInitialRequest();
	virtual void OnFinalRequest();
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedBtnCaptionFont();	
	afx_msg void OnLbnSelchangeLevelsList();
	afx_msg void OnCbnSelchangeCmbCaptionAlign();
	afx_msg void OnBnClickedChkUseCaption();
	afx_msg void OnBnClickedBtnLevelLineType();


	void PrepareCaptionAlignControl();
	void EnableCaptionsControls ();

private:

	int		 axisToAttach;
	double	 levelValue;
	CString  levelCaption;
	CListBox levelsList;
	
	BOOL useLevelCaption;
	CExtComboBox captionAlignCmb;
	long	  captionAlign;

	
};
