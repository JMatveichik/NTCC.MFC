// PlotDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NanoTubesControlCenter.h"
#include "PlotDlg.h"
#include "Automation/DataSrc.h"
#include "Automation/DataCollector.h"


using namespace xyplot;

// CPlotDlg dialog

IMPLEMENT_DYNAMIC(CPlotDlg, CExtResizableDialog)

CPlotDlg::CPlotDlg(CWnd* pParent /*=NULL*/)
	: CExtResizableDialog(CPlotDlg::IDD, pParent),
	axisLeftRange(0.0, 0.0), 
	axisRightRange(0.0, 0.0),
	axisBottomRange(0.0, 0.0),
	axisTopRange(0.0, 0.0),
	m_hUpdateThread(NULL),
	m_hStopUpdate(NULL),
	showAxisSliders(TRUE)
{

}



CPlotDlg::~CPlotDlg()
{
	if (m_hUpdateThread)
		::CloseHandle(m_hUpdateThread);

	if (m_hStopUpdate)
		::CloseHandle(m_hStopUpdate);

	XYPlotManager::Instance().DestroyPlot(hPlot);
}

void CPlotDlg::DoDataExchange(CDataExchange* pDX)
{
	CExtResizableDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_PLOT_TOOLBAR, m_wndPlotToolBar);

	DDX_Control(pDX, IDC_XAXIS_MIN, xMinSlider);
	DDX_Control(pDX, IDC_XAXIS_MAX, xMaxSlider);

	DDX_Control(pDX, IDC_YAXIS_MIN, yMinSlider);
	DDX_Control(pDX, IDC_YAXIS_MAX, yMaxSlider);

	DDX_Control(pDX, IDC_YAXIS_MIN2, yMinSlider2);
	DDX_Control(pDX, IDC_YAXIS_MAX2, yMaxSlider2);

	DDX_Control(pDX, IDC_PLOT_PLACEHOLDER, wndPlot);
	
}


BEGIN_MESSAGE_MAP(CPlotDlg, CExtResizableDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_COMMAND(ID_SHOW_LEGEND,  OnShowLegend)
	ON_COMMAND(ID_SHOW_REGIONS, OnShowRegions)
	ON_COMMAND(ID_SHOW_CHEKPOINTS,  OnShowCheckPoints)
	ON_COMMAND(ID_USEAXIS_SLIDER,  OnShowAxisSliders)

	ON_UPDATE_COMMAND_UI(ID_SHOW_LEGEND,  OnShowLegendUI)
	ON_UPDATE_COMMAND_UI(ID_SHOW_REGIONS, OnShowRegionsUI)
	ON_UPDATE_COMMAND_UI(ID_SHOW_CHEKPOINTS,  OnShowCheckPointsUI)
	ON_UPDATE_COMMAND_UI(ID_USEAXIS_SLIDER,  OnShowAxisSlidersUI)
	ON_WM_SIZE()	
END_MESSAGE_MAP()

void CPlotDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	SetTimer(TIMER_UPDATE, DataCollector::Instance().GetInterval(), NULL);
	CExtResizableDialog::OnShowWindow(bShow, nStatus);	
}

void CPlotDlg::AutoHideSet(bool bAutoHide/* = true*/)
{
	if ( bAutoHide )
		SetTimer(TIMER_HIDE, 3000, NULL);
	else
		KillTimer(TIMER_HIDE);
}


void CPlotDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{

	case TIMER_HIDE:

		KillTimer(TIMER_HIDE);
		KillTimer(TIMER_UPDATE);

		Clear();
		ShowWindow(SW_HIDE);		

		break;

	case TIMER_UPDATE:

		if ( this->IsWindowVisible() )
		{
			UpdatePlot();
		}
		break;
	}	

	CExtResizableDialog::OnTimer(nIDEvent);
}


// CPlotDlg message handlers

BOOL CPlotDlg::OnInitDialog()
{
	if (!CExtResizableDialog::OnInitDialog() )
		return FALSE;

	SubclassChildControls();

	if ( !m_wndPlotToolBar.LoadToolBar(IDR_PLOT_TOOLBAR) )
		return FALSE;

	// TODO:  Add extra initialization here
	AddAnchor( IDC_PLOT_PLACEHOLDER, CPoint(0, 0), CPoint(100, 100));
	AddAnchor( IDC_YAXIS_MAX2, CPoint(100, 0), CPoint(100, 100));
	AddAnchor( IDC_YAXIS_MIN2, CPoint(100, 0), CPoint(100, 100));

	AddAnchor( IDC_XAXIS_MIN, CPoint(0, 100), CPoint(100, 100));
	AddAnchor( IDC_XAXIS_MAX, CPoint(0, 100), CPoint(100, 100));

	
	//101 0000 0000 0001 0000 0010 0000 0011
	//101 0000 0000 0001 0000 0110 0000 0011

	XYPlotManager& pm = XYPlotManager::Instance();
	
	xMinSlider.SetStyle(CExtSliderWnd::ES_PROFUIS);
	xMaxSlider.SetStyle(CExtSliderWnd::ES_PROFUIS);
	yMinSlider.SetStyle(CExtSliderWnd::ES_PROFUIS);
	yMaxSlider.SetStyle(CExtSliderWnd::ES_PROFUIS);	
	yMinSlider2.SetStyle(CExtSliderWnd::ES_PROFUIS);
	yMaxSlider2.SetStyle(CExtSliderWnd::ES_PROFUIS);	
	
	try 
	{
		wndPlot.ModifyStyle(0, SS_NOTIFY);
		hPlot = pm.CreatePlot(wndPlot.m_hWnd);
	}
	catch (std::exception& e)
	{
		TRACE("\n%s", e.what() );
	}	

	if (!hPlot)
		return -1;

	
	RepositionBars(0, 0xFFFF, 0); 

	wndPlot.GetClientRect( &rcPlotRect );

	pm.SetRedraw(hPlot, FALSE);
	
	if ( !initPlot() )
		initPlotDefautlts();

	pm.SetRedraw(hPlot, TRUE);

// 	m_hUpdateThread = ::CreateThread(NULL, 0, UpdateThread, this, 0, NULL);
// 	if (!m_hUpdateThread)
// 		return FALSE;
// 
// 	m_hStopUpdate = ::CreateEvent(NULL, TRUE, FALSE, NULL);
// 	if (!m_hStopUpdate)
// 		return FALSE;

	//EnableSaveRestore( _T("Dialog Positions"), _T("PlotDlg") );
	
	return TRUE;
}

BOOL CPlotDlg::initPlot()
{
	XYPlotManager& pm = XYPlotManager::Instance();
	/*if (!pm.LoadProperties(hPlot, "plot.bin") )
		return FALSE;	*/
	
	return FALSE;
}

BOOL CPlotDlg::initPlotDefautlts()
{
	XYPlotManager& pm = XYPlotManager::Instance();

	SetTitle("Values dynamic");
	
	pm.EnableItem(hPlot, PI_AXIS_BOTTOM, PAI_TIME_HISTORY, TRUE );

	pm.SetLong(hPlot, PI_AXIS_BOTTOM, PAI_LINE_WIDTH, 2);	
	pm.SetLong(hPlot, PI_AXIS_LEFT, PAI_LINE_WIDTH, 2);	
	pm.SetLong(hPlot, PI_AXIS_TOP, PAI_LINE_WIDTH, 2);	
	pm.SetLong(hPlot, PI_AXIS_RIGHT, PAI_LINE_WIDTH, 2);	

	pm.SetLong(hPlot, PI_AXIS_BOTTOM, PAI_MINOR_TICKS_COUNT, 3);	
	pm.SetLong(hPlot, PI_AXIS_BOTTOM, PAI_MAJOR_TICKS_COUNT, 7);	

	pm.SetLong(hPlot, PI_AXIS_LEFT, PAI_MINOR_TICKS_COUNT, 1);
	pm.SetLong(hPlot, PI_AXIS_LEFT, PAI_MAJOR_TICKS_COUNT, 11);	

	pm.EnableItem(hPlot, PI_AXIS_LEFT, PAI_TITLE, FALSE);
	pm.EnableItem(hPlot, PI_AXIS_LEFT, PAI_MINOR_DIGITS, FALSE);
	
	pm.SetLong(hPlot, PI_SELECTION, SP_MODE, SM_TRACE);
	
	pm.EnableItem(hPlot, PI_AXIS_LEFT, PAI_AUTOSCALE, FALSE);		
	pm.EnableItem(hPlot, PI_AXIS_RIGHT, PAI_AUTOSCALE, FALSE);		
	pm.EnableItem(hPlot, PI_AXIS_BOTTOM, PAI_AUTOSCALE, FALSE);
	pm.EnableItem(hPlot, PI_AXIS_TOP, PAI_AUTOSCALE, FALSE );

	pm.EnableItem(hPlot, PI_AXIS_RIGHT, PAI_TITLE, FALSE);
	
// 	pm.EnableItem(hPlot, PI_LEGEND, 0, true);
// 	pm.SetLong(hPlot, PI_LEGEND, PLP_ALIGN, PLP_LEFT);
// 	pm.SetSelectionMode(hPlot, SM_TRACE);

	double xInterval = DataCollector::Instance().GetTimeInterval();
	std::string xTitle;
	buildPlotString("Time", "PF DinText Pro", 16, COLORREF(0x000000), xTitle);	

	double x1 = (double)CTime::GetCurrentTime().GetTime();
	x1 = x1 - int(x1) % 600;
	double x2 = x1 + xInterval;

	axisBottomRange.first  = axisTopRange.first  = x1;
	axisBottomRange.second = axisTopRange.second = x2;

	pm.SetAxisRange(hPlot, PI_AXIS_BOTTOM, x1, x2);

	pm.SetString(hPlot, PI_AXIS_BOTTOM, PAI_TITLE, xTitle.c_str() );

	pm.SetString(hPlot, PI_AXIS_TOP, PAI_TITLE, "" );
	pm.SetString(hPlot, PI_AXIS_RIGHT, PAI_TITLE, "" );

	int ticks = pm.GetLong(hPlot, PI_AXIS_BOTTOM, PAI_MAJOR_TICKS_COUNT) - 1;
	dAxisShift = 600;

	COLORREF clrStart = g_PaintManager->GetColor(CExtPaintManagerOffice2003::_2003CLR_GRADIENT_DARK);
	COLORREF clrEnd   = g_PaintManager->GetColor(CExtPaintManagerOffice2003::_2003CLR_GRADIENT_LIGHT);

	pm.SetColor(hPlot, PI_BACKGROUND, BGP_COLOR_START, clrStart);
	pm.SetColor(hPlot, PI_BACKGROUND, BGP_COLOR_END,	clrEnd);

	pm.SetColor(hPlot, PI_BACKGROUND_CLIENT, BGP_COLOR_END,	clrStart);
	pm.SetColor(hPlot, PI_BACKGROUND_CLIENT, BGP_COLOR_START,	clrEnd);

	
	return TRUE; 
}

void CPlotDlg::Clear()
{
	if (hPlot == NULL)
		return;

	XYPlotManager& plot = XYPlotManager::Instance();

	//Удаляем все профили
	for(PROFILESMAP::iterator it = profiles.begin(); it != profiles.end(); ++it)
		plot.DeleteProfile(hPlot, (*it).second);			

	profiles.clear();
	axisRangeFromDS(NULL, false);
}

void CPlotDlg::axisRangeFromDS(const DataSrc* pDS, bool toLeftAxis)
{
	if ( pDS == NULL )
	{
		axisLeftRange.first = 0.0;
		axisLeftRange.second = 100.0;
		axisRightRange.first = 0.0;
		axisRightRange.second = 100.0;
		return;
	}

	const AnalogDataSrc* pADS = dynamic_cast<const AnalogDataSrc*>( pDS );
	//Устанавливаем диапазон по оси Y

	if ( pADS != NULL )
	{	
		//Устанавливаем диапазон по оси Y
		if (toLeftAxis) 
		{
			axisLeftRange.first  =	min(axisLeftRange.first,  pADS->GetMin());
			axisLeftRange.second =	max(axisLeftRange.second, pADS->GetMax());
		}
		else
		{
			axisRightRange.first  =	min(axisLeftRange.first,  pADS->GetMin());
			axisRightRange.second =	max(axisLeftRange.second, pADS->GetMax());
		}
	} 
	else
	{		
		if (toLeftAxis) {
			axisLeftRange.first  =	min(axisLeftRange.first,  -0.5);
			axisLeftRange.second =	max(axisLeftRange.second, 1.5);
		}
		else
		{
			axisRightRange.first  =	min(axisLeftRange.first,  -0.5);
			axisRightRange.second =	max(axisLeftRange.second, 1.5);
		}
	}
}

void CPlotDlg::updateAxisSliders()
{
	setSlider(IDC_XAXIS_MIN, PI_AXIS_BOTTOM);
	setSlider(IDC_XAXIS_MAX, PI_AXIS_BOTTOM);

	setSlider(IDC_YAXIS_MIN, PI_AXIS_LEFT);
	setSlider(IDC_YAXIS_MAX, PI_AXIS_LEFT);
}

void CPlotDlg::setSlider(UINT id, UINT axis)
{
	XYPlotManager& plot = XYPlotManager::Instance();

	CExtSliderWnd* pSlider = NULL;
	pSlider = (CExtSliderWnd*)GetDlgItem(id);

	int ticks = plot.GetLong(hPlot, axis, PAI_MAJOR_TICKS_COUNT) - 2;
	
	pSlider->SetRange(0, ticks, TRUE);

	pSlider->SetTicFreq(1);
	pSlider->SetPageSize(1);
	pSlider->SetLineSize(1);
	
	if ( id == IDC_XAXIS_MIN )
		pSlider->SetPos(0);
	else if (id == IDC_XAXIS_MAX)
		pSlider->SetPos(ticks);
	else if (id == IDC_YAXIS_MIN)
		pSlider->SetPos(ticks);
	else if (id == IDC_YAXIS_MAX)
		pSlider->SetPos(0);
}

BOOL CPlotDlg::AddDataSrc(std::string name, bool bClearOld, AXISRANGE rng/* = true */, int lt/* = xyplot::PLS_SOLID*/, int lw/* = 2*/, std::string lp/* = "2 3"*/, bool toLeftAxis/* = true*/)
{
	if (hPlot == NULL)
		return FALSE;

	XYPlotManager& plot = XYPlotManager::Instance();

	//KillTimer(TIMER_HIDE);

	if ( bClearOld )
		Clear();

	const DataSrc* pDS = DataSrc::GetDataSource(name);

	if( pDS == NULL )
		return FALSE;

	HPROFILE hProf = plot.CreateProfile(hPlot, name.c_str(), pDS->Color(), lw, lt, lp.c_str(), true, false, PI_AXIS_BOTTOM, toLeftAxis ? PI_AXIS_LEFT : PI_AXIS_RIGHT);	
	profiles.insert(std::make_pair(name, hProf));

	bool useDSScaling = fabs( rng.first - rng.second ) == 0.0;

	if (useDSScaling)
		axisRangeFromDS(pDS, toLeftAxis);
	else  
		(toLeftAxis ? axisLeftRange : axisRightRange) = rng;

	if (toLeftAxis)
		plot.SetAxisRange(hPlot, PI_AXIS_LEFT, axisLeftRange.first, axisLeftRange.second);
	else
		plot.SetAxisRange(hPlot, PI_AXIS_RIGHT, axisRightRange.first, axisRightRange.second);

	updateAxisSliders();
	initProfilesData();

	return TRUE;
}

BOOL CPlotDlg::AddDataSrc(GRAPHIC_GROUP& gg)
{
	if (hPlot == NULL)
		return FALSE;

	XYPlotManager& pm = XYPlotManager::Instance();
	Clear();

	int idx = 0;

	for(std::vector<std::string>::const_iterator dsn = gg.dslya.begin(); dsn != gg.dslya.end(); ++dsn, idx++)
		AddDataSrc((*dsn), false,  gg.yLeftAxisRange, gg.lt[idx], gg.lw[idx],  gg.lp[idx].c_str(), true);

	idx = 0;
	for(std::vector<std::string>::const_iterator dsn = gg.dsrya.begin(); dsn != gg.dsrya.end(); ++dsn, idx++)
		AddDataSrc((*dsn), false,  gg.yRightAxisRange, gg.lt[idx], gg.lw[idx],  gg.lp[idx].c_str(), false);	

	
	pm.SetString(hPlot, PI_AXIS_LEFT,  PAI_TITLE, gg.yLeftAxisTitle.c_str() );
	pm.SetString(hPlot, PI_AXIS_RIGHT, PAI_TITLE, gg.yRightAxisTitle.c_str() );

	pm.SetString(hPlot, PI_AXIS_LEFT,  PAI_MAJOR_DIGITS_FORMAT, gg.yLeftAxisLblFormat.c_str() );
	pm.SetString(hPlot, PI_AXIS_LEFT,  PAI_MINOR_DIGITS_FORMAT, gg.yLeftAxisLblFormat.c_str() );

	pm.SetString(hPlot, PI_AXIS_RIGHT, PAI_TITLE, gg.yRightAxisTitle.c_str() );

	pm.SetString(hPlot, PI_AXIS_RIGHT,  PAI_MAJOR_DIGITS_FORMAT, gg.yRightAxisLblFormat.c_str() );
	pm.SetString(hPlot, PI_AXIS_RIGHT,  PAI_MINOR_DIGITS_FORMAT, gg.yRightAxisLblFormat.c_str() );

	pm.SetString(hPlot, PI_AXIS_BOTTOM, PAI_TITLE, gg.xAxisTitle.c_str() );
	
	SetTitle(gg.title);	

	return TRUE;
}

BOOL CPlotDlg::AddCheckPoint(std::string name, double val)
{

	return TRUE;
}

void CPlotDlg::buildPlotString(std::string text, std::string fontname, int fontsize, COLORREF color, std::string& out)
{
	std::strstream str;

	str << "{\\rtf1\\ansicpg1251{\\fonttbl{\\f0\\fcharset204 ";
	str << fontname.c_str() << ";}}{\\colortbl ";
	str << "\\red"	 << (int)GetRValue(color);
	str << "\\green" << (int)GetGValue(color);
	str << "\\blue"  << (int)GetBValue(color);
	str << ";}\\cf0\\f0\\fs" << fontsize;
	str << text.c_str();
	str << "\\par}" << ends;

	out = str.str();
}

BOOL CPlotDlg::AddRegion(std::string name, const REGIONINFO& ri)
{
	if (hPlot == NULL)
		return FALSE;

	XYPlotManager& plot = XYPlotManager::Instance();
	//KillTimer(TIMER_HIDE);

	double xmin, xmax;
	plot.GetAxisRange(hPlot, PI_AXIS_BOTTOM, xmin, xmax);

	REGIONSMAP::iterator fnd =  rmap.find(name);
	if ( fnd != rmap.end() )
	{
		UpdateRegions();
	}
	else {

		double xFrom = (double)ri.pTimer->StartTime();
		double xTo   = (ri.pTimer->Duration() == 0) ? (double)CTime::GetCurrentTime().GetTime() : xFrom + (double)ri.pTimer->Duration();

		std::string rTitle;
		buildPlotString(ri.title.text, ri.title.fontname, ri.title.fontsize, ri.title.color, rTitle);

		HPLOTREGION hrgn = plot.CreateRegion(hPlot, PI_AXIS_BOTTOM, PI_AXIS_LEFT, xFrom, xTo, DBL_MIN, DBL_MAX, rTitle.c_str() );		

		plot.SetLong(hPlot, hrgn, BGP_DIRECTION,  ri.vertical ? BGP_GRADIENT_VERT : BGP_GRADIENT_HORZ );

		plot.SetColor(hPlot, hrgn, BGP_COLOR_START, ri.color.from);
		plot.SetColor(hPlot, hrgn, BGP_COLOR_END, ri.color.to);
		plot.SetDouble(hPlot, hrgn, BGP_OPACITY_START, ri.opacity.from);
		plot.SetDouble(hPlot, hrgn, BGP_OPACITY_END, ri.opacity.to);

		plot.SetColor(hPlot, hrgn, BGP_BORDER_COLOR, ri.border.color);
		plot.SetLong(hPlot, hrgn, BGP_BORDER_STYLE, ri.border.type);
		plot.SetLong(hPlot, hrgn, BGP_BORDER_WIDTH, ri.border.width);
		plot.SetString(hPlot, hrgn, BGP_BORDER_TEMPL, ri.border.pat.c_str());

		plot.SetColor(hPlot, hrgn, BGP_HATCH_COLOR, ri.hatch.color);
		plot.SetLong(hPlot, hrgn, BGP_HATCH_STYLE, ri.hatch.type);


		rmap.insert( std::make_pair(name, hrgn) );
		rminfo.insert( std::make_pair(hrgn, ri) );
	}
	return TRUE;
}

void CPlotDlg::UpdateRegions()
{
	if (hPlot == NULL)
		return;

	XYPlotManager& plot = XYPlotManager::Instance();

	for ( REGIONSMAP::iterator it =  rmap.begin(); it != rmap.end(); ++it)
	{
		REGIONSINFOMAP::iterator fnd = 	rminfo.find(it->second); 
		if ( fnd != rminfo.end() )  
		{
			HPLOTREGION hrgn = fnd->first;
			REGIONINFO& ri = fnd->second;

			if ( ri.pTimer != NULL && ri.pTimer->IsStarted() )
			{
				double xFrom = (double)ri.pTimer->StartTime();
				double xTo   = (ri.pTimer->Duration() == 0) ? (double)CTime::GetCurrentTime().GetTime() : xFrom + (double)ri.pTimer->Duration();

				plot.SetRegion(hPlot, hrgn, xFrom, xTo, DBL_MIN, DBL_MAX);
			}
		}
	}
}

void CPlotDlg::SetTitle(std::string title)
{
	XYPlotManager& plot = XYPlotManager::Instance();
	std::string out;
	buildPlotString(title, "PF DinText Pro", 32, COLORREF(0x000000), out);	
	plot.SetString(hPlot, PI_TITLE, 0, out.c_str() );	
}

void CPlotDlg::initProfilesData()
{
	const DataCollector& dc = DataCollector::Instance(); 
	XYPlotManager& plot = XYPlotManager::Instance();

	std::vector<double> xVal;
	if ( !dc.GetTimeBuffer( xVal ) )
		return;
	
	const double* pdX = xVal.data(); 
	const double* pdY = NULL;

	for (PROFILESMAP::const_iterator it = profiles.begin(); it != profiles.end(); ++it)
	{
		std::vector<double> yVal;

		if (!dc.GetDataSourceBuffer((*it).first, yVal) )
			continue;

		pdY  = yVal.data();

		if ( yVal.size() < 2 )
			return;

		HPROFILE hProfile = (*it).second;
		plot.SetData(hPlot, hProfile, pdX, pdY, yVal.size() );		
	}
}

DWORD WINAPI CPlotDlg::UpdateThread(LPVOID lpParameter)
{
	CPlotDlg* pSelf = (CPlotDlg*)lpParameter;	
	UINT wait = DataCollector::Instance().GetInterval();
	
	while (true) 
	{
		if (::WaitForSingleObject(pSelf->m_hStopUpdate, 0) == WAIT_OBJECT_0) 
			break;

		if ( pSelf->IsWindowVisible() )
			pSelf->UpdatePlot();

		::Sleep(wait);
	}

	return 0L;
}

void CPlotDlg::UpdatePlot()
{
	if (hPlot == NULL)
		return;

	XYPlotManager& plot = XYPlotManager::Instance();
	plot.SetRedraw(hPlot, FALSE);

	const DataCollector& dc = DataCollector::Instance(); 
	UpdateRegions();

	std::vector<double> xVal;
	if ( !dc.GetTimeBuffer( xVal ) )
	{
		plot.SetRedraw(hPlot, TRUE);
		return;
	}
	
	if (xVal.size() >= dc.GetSubBufferCapacity())
	{
		double dMinXA;
		double dMaxXA;
		plot.GetAxisRange(hPlot, PI_AXIS_BOTTOM, dMinXA, dMaxXA);

		if (dMaxXA <= xVal.front())
		{
			
			dMinXA = xVal.front();
			dMaxXA = xVal.back();

			dMinXA = dMinXA - (int)dMinXA % 600;
			dMaxXA = dMinXA + DataCollector::Instance().GetTimeInterval();			

			//TRACE("> Scroll X range");
			
			plot.SetAxisRange(hPlot, PI_AXIS_BOTTOM, dMinXA, dMaxXA);
			axisBottomRange.first  = axisTopRange.first  = dMinXA;
			axisBottomRange.second = axisTopRange.second = dMaxXA;
		}

		if ( xVal.back() >= dMaxXA )
		{
			dMinXA += dAxisShift;
			dMaxXA += dAxisShift; 			
			//TRACE("> Shift X range");
			plot.SetAxisRange(hPlot, PI_AXIS_BOTTOM, dMinXA, dMaxXA);
			axisBottomRange.first  = axisTopRange.first  = dMinXA;
			axisBottomRange.second = axisTopRange.second = dMaxXA;
		}
	}

	if  ( !xVal.empty() )
	{
		const double x = xVal.back(); 		

		for (PROFILESMAP::const_iterator it = profiles.begin(); it != profiles.end(); ++it)
		{
			double y;

			if ( !dc.GetLast((*it).first, y) )
				continue;

			HPROFILE hProfile = (*it).second;
			plot.AppendData(hPlot, hProfile, &x, &y, 1 );		
		}
	}
	plot.SetRedraw(hPlot, TRUE);
}

void CPlotDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CSliderCtrl* pSlider = reinterpret_cast<CSliderCtrl*>(pScrollBar);  

	UINT id = pSlider->GetDlgCtrlID();

	double xInterval = fabs( axisBottomRange.second - axisBottomRange.first );
	double shift = xInterval / pSlider->GetNumTics();

	// Check which slider sent the notification  
	UINT axislim = ( id == IDC_XAXIS_MIN ) ? PAI_LOWER_LIMIT : PAI_UPPER_LIMIT;	
	double lim   = ( id == IDC_XAXIS_MIN ) ? axisBottomRange.first : axisBottomRange.second;

	double val = ( id == IDC_XAXIS_MIN ) ? 
		lim + pSlider->GetPos() * shift :
		lim - (pSlider->GetRangeMax() - pSlider->GetPos() ) * shift;

	XYPlotManager& pm = XYPlotManager::Instance();
	pm.SetDouble(hPlot, PI_AXIS_BOTTOM, axislim, val);

	// Check what happened 
	switch(nSBCode)
	{
	case TB_LINEUP:  
	case TB_LINEDOWN:  
	case TB_PAGEUP:  
	case TB_PAGEDOWN:  
		break;

	case TB_THUMBPOSITION:  
		{
			
		}
		break;

	case TB_TOP:  
	case TB_BOTTOM:  
	case TB_THUMBTRACK:  
	case TB_ENDTRACK:  
	default:  
		break;  
	}

	CExtResizableDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CPlotDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CSliderCtrl* pSlider = reinterpret_cast<CSliderCtrl*>(pScrollBar);  

	UINT id = pSlider->GetDlgCtrlID();

	double xInterval = fabs( axisLeftRange.second - axisLeftRange.first );
	double shift = xInterval / pSlider->GetNumTics();

	// Check which slider sent the notification  
	UINT axislim = ( id == IDC_YAXIS_MIN ) ? PAI_LOWER_LIMIT : PAI_UPPER_LIMIT;	
	double lim   = ( id == IDC_YAXIS_MIN ) ? axisLeftRange.first : axisLeftRange.second;

	double val = ( id == IDC_YAXIS_MIN ) ? 
		lim + (pSlider->GetRangeMax() - pSlider->GetPos() ) * shift :
		lim - pSlider->GetPos() * shift;

	XYPlotManager& pm = XYPlotManager::Instance();
	pm.SetDouble(hPlot, PI_AXIS_LEFT, axislim, val);

	// Check what happened  
	switch(nSBCode)
	{
	case TB_LINEUP:  
	case TB_LINEDOWN:  
	case TB_PAGEUP:  
	case TB_PAGEDOWN:  
	case TB_THUMBPOSITION:  
	case TB_TOP:  
	case TB_BOTTOM:  
	case TB_THUMBTRACK:  
	case TB_ENDTRACK:  
	default:  
		break;  
	}

	CExtResizableDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CPlotDlg::OnShowLegend()
{
	if ( hPlot == NULL)
		return;

	XYPlotManager& pm = XYPlotManager::Instance();	
	BOOL legend = pm.IsEnabled(hPlot, PI_LEGEND, 0);
	pm.EnableItem(hPlot, PI_LEGEND, 0, !legend);
	UpdatePlot();
}

void CPlotDlg::OnShowRegions()
{
}

void CPlotDlg::OnShowCheckPoints()
{

}

void CPlotDlg::OnShowAxisSliders()
{
	showAxisSliders = !showAxisSliders;
	
	xMinSlider.ShowWindow(showAxisSliders? SW_SHOW : SW_HIDE);
	xMaxSlider.ShowWindow(showAxisSliders? SW_SHOW : SW_HIDE);

	yMinSlider.ShowWindow(showAxisSliders? SW_SHOW : SW_HIDE);
	yMaxSlider.ShowWindow(showAxisSliders? SW_SHOW : SW_HIDE);

	yMinSlider2.ShowWindow(showAxisSliders? SW_SHOW : SW_HIDE);
	yMaxSlider2.ShowWindow(showAxisSliders? SW_SHOW : SW_HIDE);	

	if ( showAxisSliders ) {
		RepositionBars(0, 0xFFFF, 0); 
		wndPlot.MoveWindow( &rcPlotRect );		
		UpdateWindow();
	}
	else
	{
		wndPlot.GetClientRect(&rcPlotRect);
		RepositionBars(0, 0xFFFF, IDC_PLOT_PLACEHOLDER); 
		UpdateWindow();
	}
}

void CPlotDlg::OnShowLegendUI(CCmdUI *pCmdUI)
{
	if ( hPlot == NULL)
		return;

	XYPlotManager& pm = XYPlotManager::Instance();	
	pCmdUI->SetCheck( pm.IsEnabled(hPlot, PI_LEGEND, 0) );
	
}	

void CPlotDlg::OnShowRegionsUI(CCmdUI *pCmdUI)
{

}
void CPlotDlg::OnShowCheckPointsUI(CCmdUI *pCmdUI)
{
}
void CPlotDlg::OnShowAxisSlidersUI(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(showAxisSliders);
}



void CPlotDlg::OnSize(UINT nType, int cx, int cy)
{

	CExtResizableDialog::OnSize(nType, cx, cy);
	
	rcPlotRect.right += cx;
	rcPlotRect.bottom += cy;
}
