#pragma once

#include "resource.h"
#include "dlgs/IconListBox.h"

#include "../../xyplot/src/XYPlotWrapper.h"

// CXYPlotDlg dialog


class CPlotDlgPage;
class CXYPlotDlg : public CExtResizableDialog
{
	DECLARE_DYNAMIC(CXYPlotDlg)


public:
	CXYPlotDlg(CWnd* pParent, long plotID);   // standard constructor
	virtual ~CXYPlotDlg();

// Dialog Data
	enum { IDD = IDD_DLG_PLOT_PROPERTIES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:

	typedef struct tagXYPROPPAGE{
		LPCTSTR title;
		COLORREF color;
		CExtCmdIcon icon;
		CPlotDlgPage* page;
	} XYPROPPAGE, *LPXYPROPPAGE;
	
	CArray<XYPROPPAGE> m_pages;
	
	xyplot::PlotHandle plHandle;	
	xyplot::HPLOT& hPlot;

	CIconListBox m_wndList;
	CExtLabel	 m_pageTitle;	

private:
	BOOL InitPage(LPCTSTR strTitle,  COLORREF color, CPlotDlgPage* page,  UINT nIconResID, INT nIconSize = 32, UINT nLoadImageFlags = 0);

public:
	virtual BOOL OnInitDialog();

	afx_msg void OnSelchangeList();
	afx_msg void OnBnClickedBtnApply();
	afx_msg void OnOK();
};

