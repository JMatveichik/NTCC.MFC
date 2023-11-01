// dlgs\PlotDlgPage.cpp : implementation file
//

#include "stdafx.h"
#include "plotdlgpage.h"

//#include "../../../xyplot/src/XYPlotWrapper.h"
// CPlotDlgPage

using namespace xyplot;

IMPLEMENT_DYNAMIC(CPlotDlgPage, CExtResizableDialog)

CPlotDlgPage::CPlotDlgPage(CWnd* parent, HPLOT& hActivePlot, UINT IDD) 
: CExtResizableDialog  (IDD, parent), hPlot( hActivePlot ) 
{

}

CPlotDlgPage::~CPlotDlgPage()
{
}


BEGIN_MESSAGE_MAP(CPlotDlgPage, CExtResizableDialog)
END_MESSAGE_MAP()

// CPlotDlgPage message handlers

BOOL CPlotDlgPage::OnInitDialog()
{
	CExtResizableDialog::OnInitDialog();
	
	SubclassChildControls();
	ShowSizeGrip( FALSE );

	try
	{
		OnInitialRequest();
	}
	catch (XYPlotRequestFailure& e)
	{
		CString msg;
		msg.Format("%s", e.what());

		AfxMessageBox(msg, MB_ICONHAND);
		return FALSE;
	}
	catch (...)
	{
		CString msg("Unknown exception");
		AfxMessageBox(msg, MB_ICONHAND);
		return FALSE;
	}

	UpdateData(FALSE);
	return TRUE;
}

BOOL CPlotDlgPage::OnApply()
{
	UpdateData(TRUE);
	
	XYPlotManager& pm = XYPlotManager::Instance();
	pm.SetRedraw(hPlot, FALSE);

	try
	{
		OnFinalRequest();
	}
	catch (XYPlotRequestFailure& e)
	{
		CString msg;
		msg.Format("%s", e.what());

		AfxMessageBox(msg, MB_ICONHAND);
		pm.SetRedraw(hPlot, TRUE);
		return FALSE;
	}
	catch (...)
	{
		CString msg("Unknown exception");
		AfxMessageBox(msg, MB_ICONHAND);
		pm.SetRedraw(hPlot, TRUE);
		return FALSE;
	}
	pm.SetRedraw(hPlot, TRUE);
	return TRUE;
}

long CPlotDlgPage::GetPlotItemLong(long lItem, long lSubItem) const 
{	
	return XYPlotManager::Instance().GetLong(hPlot, lItem, lSubItem);
}

BOOL CPlotDlgPage::SetPlotItemLong(long lItem, long lSubItem, long val)
{
	return XYPlotManager::Instance().SetLong(hPlot, lItem, lSubItem, val);
}
 
double CPlotDlgPage::GetPlotItemDouble(long lItem, long lSubItem) const
{
	
	return XYPlotManager::Instance().GetDouble(hPlot, lItem, lSubItem);
}

BOOL CPlotDlgPage::SetPlotItemDouble(long lItem, long lSubItem, double val)
{	
	return XYPlotManager::Instance().SetDouble(hPlot, lItem, lSubItem, val);
}

BOOL CPlotDlgPage::IsPlotItemEnabled(long lItem, long lSubItem) const
{	
	return XYPlotManager::Instance().IsEnabled(hPlot, lItem, lSubItem);
}

BOOL CPlotDlgPage::EnablePlotItem(long lItem, long lSubItem, BOOL enabled)
{	
	return XYPlotManager::Instance().EnableItem(hPlot, lItem, lSubItem, enabled);
}

COLORREF CPlotDlgPage::GetPlotItemColor(long lItem, long lSubItem) const
{	
	return XYPlotManager::Instance().GetColor(hPlot, lItem, lSubItem);
}

BOOL CPlotDlgPage::SetPlotItemColor(long lItem, long lSubItem, COLORREF color)
{	
	return XYPlotManager::Instance().SetColor(hPlot, lItem, lSubItem, color);
}

CString CPlotDlgPage::GetPlotItemString(long lItem, long lSubItem) const
{	
	return XYPlotManager::Instance().GetString(hPlot, lItem, lSubItem).c_str();
}

BOOL CPlotDlgPage::SetPlotItemString(long lItem, long lSubItem, CString str)
{
	return XYPlotManager::Instance().SetString(hPlot, lItem, lSubItem, (LPCTSTR)str);
}



