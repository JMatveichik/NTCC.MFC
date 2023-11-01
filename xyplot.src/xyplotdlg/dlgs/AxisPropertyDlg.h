#pragma once

#include "..\resource.h"

#include "plotdlgpage.h"
// CAxisPropertyDlg dialog

class CAxisPropertyDlg : public CPlotDlgPage
{
	DECLARE_DYNAMIC(CAxisPropertyDlg)

public:

	CAxisPropertyDlg(CWnd* parent, xyplot::HPLOT& hActivePlot, long lAxisID);   // standard constructor
	virtual ~CAxisPropertyDlg();

// Dialog Data
	enum { IDD = IDD_DLG_AXIS_PROPERTIES };

	
protected:

	DECLARE_MESSAGE_MAP()

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual void OnInitialRequest();
	virtual void OnFinalRequest();
	virtual BOOL OnInitDialog();

	afx_msg void OnChangeLineType(UINT id);
	afx_msg void OnChangeCheckState(UINT id);
	afx_msg void OnChangeFont(UINT id);

	void UpdateControlsState();

protected:

	BOOL showTitle;
	BOOL autoScaled;
	BOOL logScaled;
	
	CString strTitle;
	
	double rangeMin;
	double rangeMax;
	
	BOOL ticksMajEnabled;
	BOOL ticksMinEnabled;

	int ticksMajCount;
	int ticksMinCount;
	int ticksMajLen;
	int ticksMinLen;

	
	int ticksStyle;

	BOOL digitsMajEnabled;
	BOOL digitsMinEnabled;
	CString digitsMajFormat;
	CString digitsMinFormat;

	BOOL gridMajEnabled;
	BOOL gridMinEnabled;
	long lActiveAxisID;

	
};
