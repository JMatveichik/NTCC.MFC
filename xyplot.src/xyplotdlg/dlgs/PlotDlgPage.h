#pragma once

// CPlotDlgPage
#include "../plotdlg.h"
#include "../../../xyplot/src/XYPlotWrapper.h"

class CPlotDlgPage : public CExtResizableDialog
{
	friend class CXYPlotDlg;

	DECLARE_DYNAMIC(CPlotDlgPage)

public:
	
	CPlotDlgPage(CWnd* parent, xyplot::HPLOT& hActivePlot, UINT IDD);
	virtual ~CPlotDlgPage();

	enum { IDD = 0 };

protected:

	DECLARE_MESSAGE_MAP()

protected:

	
	virtual BOOL OnInitDialog();
	BOOL OnApply();

	virtual void OnInitialRequest() = 0;
	virtual void OnFinalRequest() = 0;

	xyplot::HPLOT& hPlot;

	long GetPlotItemLong(long lItem, long lSubItem) const;
	BOOL SetPlotItemLong(long lItem, long lSubItem, long val);


 	double GetPlotItemDouble(long lItem, long lSubItem) const;
 	BOOL SetPlotItemDouble(long lItem, long lSubItem, double val);

	BOOL IsPlotItemEnabled(long lItem, long lSubItem) const;
	BOOL EnablePlotItem(long lItem, long lSubItem, BOOL enabled);

	COLORREF GetPlotItemColor(long lItem, long lSubItem) const;
	BOOL SetPlotItemColor(long lItem, long lSubItem, COLORREF color);

	CString GetPlotItemString(long lItem, long lSubItem) const;
	BOOL SetPlotItemString(long lItem, long lSubItem, CString str);

protected:

	virtual void OnOk() {};
	virtual void OnCancel() {};	
};

