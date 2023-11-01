#pragma once

#include "..\resource.h"
#include "PlotDlgPage.h"

// CGeneralPropDlg dialog

class CGeneralPropDlg : public CPlotDlgPage
{
	DECLARE_DYNAMIC(CGeneralPropDlg)

public:
	CGeneralPropDlg(CWnd* pParent, xyplot::HPLOT& hActivePlot);   // standard constructor
	virtual ~CGeneralPropDlg();

// Dialog Data
	enum { IDD = IDD_DLG_GENERAL_PROPERTIES };

protected:
	DECLARE_MESSAGE_MAP()

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	virtual void OnInitialRequest();
	virtual void OnFinalRequest();
	virtual BOOL OnInitDialog();
	
	afx_msg void OnBnClickedBtnBorderLine();	
	

protected:

	BOOL showTitle;
	BOOL showBorder;
	BOOL showLegend;

	int legendPos;
	int legendLineWidth;

	CString		   strPlotTitle;
	CRichEditCtrl  editTitle;


	COLORREF	   clrCBk1;
	COLORREF	   clrCBk2;
	COLORREF	   clrGBk1;
	COLORREF	   clrGBk2;
	
	int plotGradientDir;
	int clientGradientDir;

	CExtColorButton btnCBk1;
	CExtColorButton btnCBk2;
	CExtColorButton btnGBk1;
	CExtColorButton btnGBk2;

	
};
