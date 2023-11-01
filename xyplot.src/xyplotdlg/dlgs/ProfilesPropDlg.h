#pragma once

#include "..\resource.h"
#include "plotdlgpage.h"

// CProfilesPropDllg dialog

class CProfilesPropDlg : public CPlotDlgPage
{
	DECLARE_DYNAMIC(CProfilesPropDlg)

public:
	CProfilesPropDlg(CWnd* parent, xyplot::HPLOT& hActivePlot);   // standard constructor
	virtual ~CProfilesPropDlg();

// Dialog Data
	enum { IDD = IDD_DLG_PROFILE_PROPERTIES };

protected:

	DECLARE_MESSAGE_MAP()

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	virtual void OnInitialRequest();
	virtual void OnFinalRequest();
	virtual BOOL OnInitDialog();

	afx_msg void OnLbnSelchangeProfileList();
	afx_msg void OnBnClickedUseMarks();
	afx_msg void OnBnClickedBtnLineProfile();
	void OnMarksEnable();

private:

	BOOL drawProfileLine;
	BOOL drawProfileMarks;
	
	int markType;
	int markSize;
	int markFreq;

	CExtColorButton btnMarkLineColor;
	CExtColorButton btnMarkFillColor;
	CListBox profileList;
	
	COLORREF markLineColor;
	COLORREF markFillColor;

	std::vector<xyplot::HPROFILE> profiles;
	long activeProf;

public:
	
};
