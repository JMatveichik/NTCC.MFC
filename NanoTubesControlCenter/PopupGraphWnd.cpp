// PopupGraphWnd.cpp : implementation file
//

#include "stdafx.h"
#include "PopupGraphWnd.h"
#include "FSView.h"
#include "Automation/DataSrc.h"
#include "Automation/DataCollector.h"
#include "Resource.h"


#ifdef _DEBUG
#define new DEBUG_NEW	
#endif

// CPopupGraphWnd
const UINT TIMER_HIDE	= 1;
const UINT TIMER_UPDATE = 2;

IMPLEMENT_DYNAMIC(CPopupGraphWnd, CWnd)

IMPLEMENT_CExtPmBridge_MEMBERS( CPopupGraphWnd )

using namespace xyplot;

CPopupGraphWnd::CPopupGraphWnd() : 
m_nBufferSize(25), 
bLButtonDown(false),
m_dYAxisMin(0.0), 
m_dYAxisMax(1.0)
{
	
}

CPopupGraphWnd::~CPopupGraphWnd()
{
}


BEGIN_MESSAGE_MAP(CPopupGraphWnd, CWnd)
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_SETCURSOR()
	ON_WM_NCHITTEST()
	ON_WM_RBUTTONDBLCLK()
END_MESSAGE_MAP()



// CPopupGraphWnd message handlers


void CPopupGraphWnd::OnShowWindow(BOOL bShow, UINT nStatus)
{
	
	SetTimer(TIMER_UPDATE, DataCollector::Instance().GetInterval(), NULL);

	CWnd::OnShowWindow(bShow, nStatus);	
}

void CPopupGraphWnd::AutoHideSet(bool bAutoHide/* = true*/)
{
	if ( bAutoHide )
		SetTimer(TIMER_HIDE, 3000, NULL);
	else
		KillTimer(TIMER_HIDE);
}

void CPopupGraphWnd::Clear()
{
	if (m_hPlot == NULL)
		return;

	XYPlotManager& plot = XYPlotManager::Instance();

	//Удаляем все профили
	for(PROFILESMAP::iterator it = m_profiles.begin(); it != m_profiles.end(); ++it)
		plot.DeleteProfile(m_hPlot, (*it).second);			

	m_profiles.clear();

	m_dYAxisMin = 0.0; 
	m_dYAxisMax = 1.0;

}
void CPopupGraphWnd::SelectLastAddedProfile()
{
	if (m_hPlot == NULL)
		return;

	XYPlotManager& plot = XYPlotManager::Instance();
	/*
	for(PROFILESMAP::iterator it = m_profiles.begin(); it != m_profiles.end(); ++it)
		plot.EnableProfileSymbols(m_hPlot, (*it).second, FALSE);	

	plot.EnableProfileSymbols(m_hPlot, m_hLastAddedProfile, TRUE);	
	*/
}

void CPopupGraphWnd::OnTimer(UINT_PTR nIDEvent)
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

	CWnd::OnTimer(nIDEvent);
}

void CPopupGraphWnd::SetTitle(std::string title)
{
	XYPlotManager& plot = XYPlotManager::Instance();
	std::string out = "{\\rtf1\\ansicpg1251{\\fonttbl{\\f0\\fcharset204 Segue UI;}}{\\colortbl \\red0\\green0\\blue0;}\\cf0\\f0\\fs32";
	out += title;
	out += "\\par}";

	plot.SetString(m_hPlot, PI_TITLE, 0, out.c_str() );	
}

BOOL CPopupGraphWnd::AddCheckPoint(std::string name, double val)
{
	if (m_hPlot == NULL)
		return FALSE;

	XYPlotManager& plot = XYPlotManager::Instance();
	//KillTimer(TIMER_HIDE);

	double xmin, xmax;
	plot.GetAxisRange(m_hPlot, PI_AXIS_BOTTOM, xmin, xmax);

	LEVELSMAP::iterator fnd =  m_checkPoints.find(name);
	if ( fnd != m_checkPoints.end() )
		plot.SetLevel(m_hPlot,  fnd->second, val);
	else {
		
		DataSrc* pDS = DataSrc::GetDataSource(name);

		COLORREF clr = ( pDS != NULL ) ? pDS->Color() : RGB(128, 0, 0);

		HLEVEL hlvl = plot.CreateLevel(m_hPlot, PI_AXIS_BOTTOM, val, name.c_str(), clr, 2);		
		m_checkPoints.insert( std::make_pair(name, hlvl) );
	}
	return TRUE;
	
}

CPopupGraphWnd::tagAxisScaleType CPopupGraphWnd::GetAxisScaleType(const std::pair<double, double>& r)
{
	if ( (r.first == r.second ) && (r.first == 0.0) )
		return AXISSCALEAUTO;

	else if ((r.first == r.second ) && (r.first != 0.0))
		return AXISSCALECALCULATE;

	return AXISSCALEFIXED;
}


void CPopupGraphWnd::SetupAxis(UINT axisID, std::string title, const std::pair<double, double>& r)
{
	if (m_hPlot == NULL)
		return;

	XYPlotManager& pm = XYPlotManager::Instance();

	pm.SetString(m_hPlot, axisID, PAI_TITLE, title.c_str() );
	pm.EnableItem(m_hPlot, axisID, PAI_TITLE, title.empty() ? FALSE : TRUE );

	pm.EnableItem(m_hPlot, axisID, PAI_MAJOR_TICKS, TRUE );
	pm.EnableItem(m_hPlot, axisID, PAI_MINOR_TICKS, TRUE );

	pm.EnableItem(m_hPlot, axisID, PAI_MAJOR_DIGITS, TRUE );
	pm.EnableItem(m_hPlot, axisID, PAI_MINOR_DIGITS, TRUE );

	tagAxisScaleType ast = GetAxisScaleType( r );

	if ( ast != AXISSCALEAUTO )
	{
		pm.EnableItem(m_hPlot, axisID, PAI_AUTOSCALE, FALSE);

		pm.SetDouble(m_hPlot, axisID, PAI_LOWER_LIMIT, r.first);
		pm.SetDouble(m_hPlot, axisID, PAI_UPPER_LIMIT, r.second);
	}
	else 
		pm.EnableItem(m_hPlot, axisID, PAI_AUTOSCALE, TRUE);

}

BOOL CPopupGraphWnd::AddDataSrc(GRAPHIC_GROUP& gg)
{
	if (m_hPlot == NULL)
		return FALSE;

	XYPlotManager& pm = XYPlotManager::Instance();

	KillTimer(TIMER_HIDE);

	Clear();
	
	int idx = 0;

	for(std::vector<std::string>::const_iterator dsn = gg.dslya.begin(); dsn != gg.dslya.end(); ++dsn, idx++)
	{
		DataSrc* pDS = DataSrc::GetDataSource((*dsn));

		if( pDS == NULL )
			continue;

		HPROFILE hProf = pm.CreateProfile(m_hPlot, 
			dsn->c_str(), 
			pDS->Color(), 
			gg.lw[idx], 
			gg.lt[idx], 
			gg.lp[idx].c_str(), 
			true, 
			false, 
			PI_AXIS_BOTTOM, PI_AXIS_LEFT );	
			m_profiles.insert(std::make_pair((*dsn), hProf));
	}

	if ( !gg.dslya.empty() )
	{	
		SetupAxis(PI_AXIS_LEFT, gg.yLeftAxisTitle, gg.yLeftAxisRange);
		leftYAxisScale = GetAxisScaleType(gg.yLeftAxisRange);
	}


	for(std::vector<std::string>::const_iterator dsn = gg.dsrya.begin(); dsn != gg.dsrya.end(); ++dsn, idx++)
	{
		DataSrc* pDS = DataSrc::GetDataSource((*dsn));

		if( pDS == NULL )
			continue;

		HPROFILE hProf = pm.CreateProfile(m_hPlot, 
			dsn->c_str(), 
			pDS->Color(), 
			gg.lw[idx], 
			gg.lt[idx], 
			gg.lp[idx].c_str(), 
			true, 
			false, 
			PI_AXIS_BOTTOM, PI_AXIS_RIGHT );	

		m_profiles.insert(std::make_pair((*dsn), hProf));
	}

	if ( !gg.dsrya.empty() )
	{	
		SetupAxis(PI_AXIS_RIGHT, gg.yRightAxisTitle, gg.yRightAxisRange);
		rightYAxisScale = GetAxisScaleType(gg.yRightAxisRange);
	}

	pm.SetString(m_hPlot, PI_AXIS_BOTTOM, PAI_TITLE, gg.xAxisTitle.c_str() );
	pm.SetString(m_hPlot, PI_TITLE, 0, gg.title.c_str() );

	return TRUE;
}

BOOL CPopupGraphWnd::AddDataSrc(std::string name, bool bClearOld/* = true*/, int lt/* = PLS_SOLID*/, int lw/* = 2*/, std::string lp/* = "2 3"*/, bool toLeftAxis/* = true*/)
{
	if (m_hPlot == NULL)
		return FALSE;

	XYPlotManager& plot = XYPlotManager::Instance();

	KillTimer(TIMER_HIDE);

	if ( bClearOld )
		Clear();
	
	DataSrc* pDS = DataSrc::GetDataSource(name);
	
	if( pDS == NULL )
		return FALSE;

	HPROFILE hProf = plot.CreateProfile(m_hPlot, name.c_str(), pDS->Color(), lw, lt, lp.c_str(), true, false, PI_AXIS_BOTTOM, toLeftAxis ? PI_AXIS_LEFT : PI_AXIS_RIGHT);	
	m_profiles.insert(std::make_pair(name, hProf));

	m_hLastAddedProfile = hProf;
	
	AnalogDataSrc* pADS = dynamic_cast<AnalogDataSrc*>(pDS);
	if ( pADS != NULL )
	{	
		//Устанавливаем диапазон по оси Y
		m_dYAxisMin =	min(m_dYAxisMin, pADS->GetMin());
		m_dYAxisMax =	max(m_dYAxisMax, pADS->GetMax());
	} 
	else
	{
		m_dYAxisMin =	min(m_dYAxisMin, -0.5 );
		m_dYAxisMax =	max(m_dYAxisMax, 1.5  );
	}

	SetupAxis(PI_AXIS_LEFT, "", pair<double, double>(m_dYAxisMin, m_dYAxisMax));

	//plot.SetAxisRange(m_hPlot, PI_AXIS_LEFT, m_dYAxisMin, m_dYAxisMax);

	//SelectLastAddedProfile();

	UpdatePlot();
	
	SetTimer(TIMER_HIDE, 3000, NULL);

	return TRUE;
}


BOOL CPopupGraphWnd::AddRegion(std::string name, const REGIONINFO& ri)
{
	if (m_hPlot == NULL)
		return FALSE;

	XYPlotManager& plot = XYPlotManager::Instance();
	//KillTimer(TIMER_HIDE);

	double xmin, xmax;
	plot.GetAxisRange(m_hPlot, PI_AXIS_BOTTOM, xmin, xmax);

	REGIONSMAP::iterator fnd =  m_regions.find(name);
	if ( fnd != m_regions.end() )
	{
		UpdateRegions();
	}
	else {
		
		double xFrom = (double)ri.pTimer->StartTime();
		double xTo   = (ri.pTimer->Duration() == 0) ? (double)CTime::GetCurrentTime().GetTime() : xFrom + (double)ri.pTimer->Duration();
		
		TRACE("\n\tTIMER %20s FROM %d  TO %d", ri.pTimer->Name().c_str(), (long)xFrom, (long)xTo);

		std::strstream str;
		str << "{\\rtf1\\ansicpg1251{\\fonttbl{\\f0\\fcharset204 ";
		str << ri.title.fontname.c_str() << ";}}{\\colortbl ";
		str << "\\red"	 << (int)GetRValue(ri.title.color);
		str << "\\green" << (int)GetGValue(ri.title.color);
		str << "\\blue"  << (int)GetBValue(ri.title.color);
		str << ";}\\cf0\\f0\\fs" << ri.title.fontsize;
		str << ri.title.text.c_str();
		str << "\\par}" << ends;

		HPLOTREGION hrgn = plot.CreateRegion(m_hPlot, PI_AXIS_BOTTOM, PI_AXIS_LEFT, xFrom, xTo, DBL_MIN, DBL_MAX, str.str() );		
		
		plot.SetLong(m_hPlot, hrgn, BGP_DIRECTION,  ri.vertical ? BGP_GRADIENT_VERT : BGP_GRADIENT_HORZ );

		plot.SetColor(m_hPlot, hrgn, BGP_COLOR_START, ri.color.from);
		plot.SetColor(m_hPlot, hrgn, BGP_COLOR_END, ri.color.to);
		plot.SetDouble(m_hPlot, hrgn, BGP_OPACITY_START, ri.opacity.from);
		plot.SetDouble(m_hPlot, hrgn, BGP_OPACITY_END, ri.opacity.to);
		
		plot.SetColor(m_hPlot, hrgn, BGP_BORDER_COLOR, ri.border.color);
		plot.SetLong(m_hPlot, hrgn, BGP_BORDER_STYLE, ri.border.type);
		plot.SetLong(m_hPlot, hrgn, BGP_BORDER_WIDTH, ri.border.width);
		plot.SetString(m_hPlot, hrgn, BGP_BORDER_TEMPL, ri.border.pat.c_str());

		plot.SetColor(m_hPlot, hrgn, BGP_HATCH_COLOR, ri.hatch.color);
		plot.SetLong(m_hPlot, hrgn, BGP_HATCH_STYLE, ri.hatch.type);
		

		m_regions.insert( std::make_pair(name, hrgn) );
		m_regionsInfo.insert( std::make_pair(hrgn, ri) );
	}
	return TRUE;
}

void CPopupGraphWnd::UpdateRegions()
{
	if (m_hPlot == NULL)
		return;

	XYPlotManager& plot = XYPlotManager::Instance();

	for ( REGIONSMAP::iterator it =  m_regions.begin(); it != m_regions.end(); ++it)
	{
		REGIONSINFOMAP::iterator fnd = 	m_regionsInfo.find(it->second); 
		if ( fnd != m_regionsInfo.end() )  
		{
			HPLOTREGION hrgn = fnd->first;
			REGIONINFO& ri = fnd->second;

			if ( ri.pTimer != NULL && ri.pTimer->IsStarted() )
			{
				double xFrom = (double)ri.pTimer->StartTime();
				double xTo   = (ri.pTimer->Duration() == 0) ? (double)CTime::GetCurrentTime().GetTime() : xFrom + (double)ri.pTimer->Duration();

				plot.SetRegion(m_hPlot, hrgn, xFrom, xTo, DBL_MIN, DBL_MAX);
			}
		}
	}
}

//#define _USE_XYPLOT_DATA

void CPopupGraphWnd::OptimizeAxisScale(std::pair<double, double>& ar )
{
	double aMin = ar.first;
	double aMax = ar.second;

	if ( aMax - aMin < 0.0001 )
	{
		aMin = 0.0;
		aMax = 1.0;
	}

	int a1 = (int) pow(10.0, (int) (floor (log10(aMax) ) ) ); 
	int a2 = (int) pow(10.0, (int) (floor (log10(aMin) ) ) ); 

	if (a1 == 0 && a2 == 0)
	{
		a1 = 1;
		a2 = 1;
	}

	double d1 = ( (int)(aMax / a1)*a1 + a1);
	double d2 = ( (int)(aMin / a2)*a2 - a2);

	ar.first  = max(d2, 0.0);
	ar.second = d1;	
}

std::pair<double, double> CPopupGraphWnd::CalcAxisRange(UINT axisID)
{
	if (m_hPlot == NULL)
		return std::pair<double, double>(0.0, 1.0);

	const DataCollector& dc = DataCollector::Instance(); 	
	XYPlotManager& pm = XYPlotManager::Instance();

	dc.LockData();
	
	double aMin = 0.0;
	double aMax = 1.0;

	switch (axisID)
	{
		case PI_AXIS_LEFT:
		case PI_AXIS_RIGHT:
			{
				for (PROFILESMAP::const_iterator it = m_profiles.begin(); it != m_profiles.end(); ++it)
				{
					HPROFILE hProfile = (*it).second;
					if ( pm.GetLong(m_hPlot, hProfile, PRP_YPARENTAXIS) != axisID )
						continue;

#ifdef _USE_XYPLOT_DATA

					double val = 0.0;
					val = pm.GetDouble(m_hPlot, hProfile, PRP_MIN_VALUE);
					if(val <= aMin)
						aMin = val;

					val = pm.GetDouble(m_hPlot, hProfile, PRP_MAX_VALUE);
					if(val > aMax)
						aMax = val;

#else
					std::vector<double> yVal;
					if (!dc.GetDataSourceBuffer((*it).first, yVal) )
						continue;
					
					std::pair< vector<double>::iterator, vector<double>::iterator > eMinMax;
					eMinMax = minmax_element(yVal.begin(), yVal.end());

					if((*eMinMax.first) < aMin)
						aMin = (*eMinMax.first);

					if((*eMinMax.second) > aMax)
						aMax = (*eMinMax.second);
#endif

				}

			}
			break;

		case PI_AXIS_BOTTOM:
		case PI_AXIS_TOP:
			{
				std::vector<double> xVal;
				if ( !dc.GetTimeBuffer( xVal ) )
					return std::pair<double, double>(aMin, aMax);

				std::pair< vector<double>::iterator, vector<double>::iterator > eMinMax;
				eMinMax = minmax_element(xVal.begin(), xVal.end());

				aMin = (*eMinMax.first);
				aMin = (*eMinMax.second);
			}
		break;

		default:
// 			aMin = 0.0;
// 			aMax = 1.0;
			break;

	}

	dc.UnlockData();

	return std::pair<double, double>(aMin, aMax);
}


void CPopupGraphWnd::UpdatePlot()
{
	if (m_hPlot == NULL)
		return;


	XYPlotManager& plot = XYPlotManager::Instance();

	plot.SetRedraw(m_hPlot, FALSE);

	const DataCollector& dc = DataCollector::Instance(); 

	dc.LockData();

	UpdateRegions();

	std::vector<double> xVal;
	if ( !dc.GetTimeBuffer( xVal ) )
		return;

	const double* pdX = xVal.data(); 
	const double* pdY = NULL;

	for (PROFILESMAP::const_iterator it = m_profiles.begin(); it != m_profiles.end(); ++it)
	{
		std::vector<double> yVal;

		if (!dc.GetDataSourceBuffer((*it).first, yVal) )
			continue;

		pdY  = yVal.data();
		
		if ( yVal.size() < 2 )
			return;

		HPROFILE hProfile = (*it).second;
		plot.SetData(m_hPlot, hProfile, pdX, pdY, yVal.size() );		
	}

	if ( leftYAxisScale == AXISSCALECALCULATE )
	{
		std::pair<double, double> ar = CalcAxisRange(PI_AXIS_LEFT);
		OptimizeAxisScale(ar);
		plot.SetAxisRange(m_hPlot, PI_AXIS_LEFT, ar.first, ar.second);
	}

	if ( rightYAxisScale == AXISSCALECALCULATE )
	{
		std::pair<double, double> ar = CalcAxisRange(PI_AXIS_RIGHT);
		OptimizeAxisScale(ar);
		plot.SetAxisRange(m_hPlot, PI_AXIS_RIGHT, ar.first, ar.second);
	}

	if (xVal.size() >= dc.GetSubBufferCapacity())
	{
		double dMinXA;
 		double dMaxXA;
 		plot.GetAxisRange(m_hPlot, PI_AXIS_BOTTOM, dMinXA, dMaxXA);
 		
		if (dMaxXA <= xVal.front())
		{
			dMinXA = xVal.front();
			dMaxXA = xVal.back();
			//TRACE("> Scroll X range");
		}

 		if ( xVal.back() >= dMaxXA )
		{
			dMinXA += m_dAxisShift;
 			dMaxXA += m_dAxisShift; 			
			//TRACE("> Shift X range");
 		}

		plot.SetAxisRange(m_hPlot, PI_AXIS_BOTTOM, dMinXA, dMaxXA);
	}

	dc.UnlockData();
	
	plot.SetRedraw(m_hPlot, TRUE);
}


BOOL CPopupGraphWnd::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= WS_BORDER|WS_THICKFRAME;
	cs.hMenu = (HMENU)NULL;

	return CWnd::PreCreateWindow(cs);
}

int CPopupGraphWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	PmBridge_Install(); // Subscribe

	::SetWindowLongPtr(m_hWnd, GWL_EXSTYLE, GetWindowLongPtr(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	::SetLayeredWindowAttributes(m_hWnd,RGB(0,0,0), 128, LWA_ALPHA);

	HINSTANCE hInst = AfxGetInstanceHandle();
	
	hCursorArrow = ::LoadCursor(hInst, MAKEINTRESOURCE(IDC_CUR_ARROW) ); 
	hCursorMove  = ::LoadCursor(hInst, MAKEINTRESOURCE(IDC_CUR_MOVE) );
	hCursorNS	 = ::LoadCursor(hInst, MAKEINTRESOURCE(IDC_CUR_NS) );
	hCursorWE	 = ::LoadCursor(hInst, MAKEINTRESOURCE(IDC_CUR_WE) );
	hCursorNWSE  = ::LoadCursor(hInst, MAKEINTRESOURCE(IDC_CUR_NWSE) );
	hCursorNESW  = ::LoadCursor(hInst, MAKEINTRESOURCE(IDC_CUR_NESW) );
	
	tme.cbSize = sizeof(tme);
	tme.hwndTrack = m_hWnd;
	tme.dwFlags = TME_LEAVE;

	XYPlotManager& pm = XYPlotManager::Instance();

	try 
	{
		m_hPlot = pm.CreatePlot(m_hWnd);
	}
	catch (std::exception& e)
	{
		TRACE("\n%s", e.what() );
	}	
	
	if (!m_hPlot)
		return -1;

	

	LOGFONT lf;

	g_PaintManager->m_FontNormal.GetLogFont(&lf);
	lf.lfCharSet = DEFAULT_CHARSET;

	SetTitle("Динамика изменений");

	pm.EnableItem(m_hPlot, PI_AXIS_BOTTOM, PAI_TIME_HISTORY, TRUE );
	
	pm.SetLong(m_hPlot, PI_AXIS_BOTTOM, PAI_LINE_WIDTH, 2);	
	pm.SetLong(m_hPlot, PI_AXIS_LEFT, PAI_LINE_WIDTH, 2);	
	pm.SetLong(m_hPlot, PI_AXIS_TOP, PAI_LINE_WIDTH, 2);	
	pm.SetLong(m_hPlot, PI_AXIS_RIGHT, PAI_LINE_WIDTH, 2);	

	pm.SetLong(m_hPlot, PI_AXIS_BOTTOM, PAI_MINOR_TICKS_COUNT, 4);	
	pm.SetLong(m_hPlot, PI_AXIS_BOTTOM, PAI_MAJOR_TICKS_COUNT, 7);	
	
	
	pm.SetLong(m_hPlot, PI_AXIS_LEFT, PAI_MINOR_TICKS_COUNT, 4);
	pm.SetLong(m_hPlot, PI_AXIS_LEFT, PAI_MAJOR_TICKS_COUNT, 10);	
	
	pm.EnableItem(m_hPlot, PI_AXIS_LEFT, PAI_AUTOSCALE, FALSE);		
	pm.EnableItem(m_hPlot, PI_AXIS_LEFT, PAI_TITLE, FALSE);
	pm.EnableItem(m_hPlot, PI_AXIS_LEFT, PAI_MINOR_DIGITS, FALSE);
	

	pm.EnableItem(m_hPlot, PI_AXIS_RIGHT, PAI_AUTOSCALE, FALSE);		
	pm.EnableItem(m_hPlot, PI_AXIS_RIGHT, PAI_TITLE, FALSE);

	pm.EnableItem(m_hPlot, PI_AXIS_BOTTOM, PAI_AUTOSCALE, FALSE );

	pm.SetLong(m_hPlot, PI_AXIS_LEFT, PAI_LINE_WIDTH, 1);
	pm.SetLong(m_hPlot, PI_AXIS_TOP, PAI_LINE_WIDTH, 1);
	pm.SetLong(m_hPlot, PI_AXIS_RIGHT, PAI_LINE_WIDTH, 1);
	pm.SetLong(m_hPlot, PI_AXIS_BOTTOM, PAI_LINE_WIDTH, 1);
	

	pm.EnableItem(m_hPlot, PI_LEGEND, 0, true);
	pm.SetLong(m_hPlot, PI_LEGEND, PLP_ALIGN, PLP_LEFT);
	pm.SetSelectionMode(m_hPlot, SM_FLASH);

	double xInterval = DataCollector::Instance().GetTimeInterval();
	std::string xTitle = "{\\rtf1\\ansicpg1251{\\fonttbl{\\f0\\fcharset204 Segue UI;}}{\\colortbl \\red0\\green0\\blue0;}\\cf0\\f0\\fs24Time\\par}";
	
	double x1 = (double)CTime::GetCurrentTime().GetTime();
	double x2 = x1 + xInterval;

	pm.SetAxisRange(m_hPlot, PI_AXIS_BOTTOM, x1, x2);
	

	pm.SetString(m_hPlot, PI_AXIS_BOTTOM, PAI_TITLE, xTitle.c_str() );

	pm.SetString(m_hPlot, PI_AXIS_TOP, PAI_TITLE, "" );
	pm.SetString(m_hPlot, PI_AXIS_RIGHT, PAI_TITLE, "" );

	int ticks = pm.GetLong(m_hPlot, PI_AXIS_BOTTOM, PAI_MAJOR_TICKS_COUNT) - 1;
	m_dAxisShift = xInterval / ticks;


	COLORREF clrStart = g_PaintManager->GetColor(CExtPaintManagerOffice2003::_2003CLR_GRADIENT_DARK);
	COLORREF clrEnd   = g_PaintManager->GetColor(CExtPaintManagerOffice2003::_2003CLR_GRADIENT_LIGHT);

	pm.SetColor(m_hPlot, PI_BACKGROUND, BGP_COLOR_START, clrStart);
	pm.SetColor(m_hPlot, PI_BACKGROUND, BGP_COLOR_END,	clrEnd);

	pm.SetColor(m_hPlot, PI_BACKGROUND_CLIENT, BGP_COLOR_END,	clrStart);
	pm.SetColor(m_hPlot, PI_BACKGROUND_CLIENT, BGP_COLOR_START,	clrEnd);

	

	return 0;
}

void CPopupGraphWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	KillTimer(TIMER_HIDE);
	TrackMouseEvent(&tme);

	::SetLayeredWindowAttributes(m_hWnd,RGB(0,0,0),255,LWA_ALPHA);

	CWnd::OnMouseMove(nFlags, point);
}

BOOL CPopupGraphWnd::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if ( message == WM_MOUSELEAVE || message == WM_NCMOUSELEAVE)
	{
		::SetLayeredWindowAttributes(m_hWnd,RGB(0,0,0),128,LWA_ALPHA);
		SetTimer(TIMER_HIDE, 3000, NULL);
	}

	return CWnd::OnWndMsg(message, wParam, lParam, pResult);
}

void CPopupGraphWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCursor(hCursorMove);

	WPARAM SC_DRAGMOVE = 0xF012;
	ReleaseCapture();
	::SendMessage(m_hWnd, WM_SYSCOMMAND, SC_DRAGMOVE, 0);

	CWnd::OnLButtonDown(nFlags, point);
}

void CPopupGraphWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	SetCursor(hCursorArrow);
	CWnd::OnLButtonUp(nFlags, point);
}

void CPopupGraphWnd::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	CWnd::OnRButtonDblClk(nFlags, point);

	KillTimer(TIMER_HIDE);
	KillTimer(TIMER_UPDATE);

	Clear();
	ShowWindow(SW_HIDE);
}

BOOL CPopupGraphWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	HCURSOR hCur = NULL;

	switch (nHitTest)
	{
		case HTBOTTOM:		//In the lower horizontal border of the window.
		case HTTOP:			//In the upper horizontal border of the window.
			hCur = hCursorNS;
			break;

		case HTBOTTOMLEFT:	//In the lower-left corner of the window border.
		case HTTOPRIGHT:	//In the upper-right corner of the window border.
			hCur = hCursorNESW;
			break;

		case HTTOPLEFT:		//In the upper-left corner of the window border.
		case HTBOTTOMRIGHT: //In the lower-right corner of the window border.
			hCur = hCursorNWSE;
			break;
		
		case HTLEFT:		//In the left border of the window.
		case HTRIGHT:		//In the right border of the window.
			hCur = hCursorWE;
			break;

		case HTCLIENT:		//In a client area.
		case HTCAPTION:
			if ( bLButtonDown )
				hCur = hCursorMove;
			else
				hCur = hCursorArrow;
			break;
	}

	::SetCursor(hCur);
	return TRUE;

	//return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CPopupGraphWnd::PmBridge_OnPaintManagerChanged( CExtPaintManager * pGlobalPM )
{
	//TODO: Смена цветовой гаммы графика при изменении глобального менеджера отрисовки
	COLORREF clrStart = pGlobalPM->GetColor(CExtPaintManagerOffice2003::_2003CLR_GRADIENT_DARK);
	COLORREF clrEnd   = pGlobalPM->GetColor(CExtPaintManagerOffice2003::_2003CLR_GRADIENT_LIGHT);

	if ( m_hPlot == NULL )
		return;

	XYPlotManager& plot = XYPlotManager::Instance();

	plot.SetRedraw(m_hPlot, FALSE);
	plot.SetColor(m_hPlot, PI_BACKGROUND, BGP_COLOR_START, clrStart);
	plot.SetColor(m_hPlot, PI_BACKGROUND, BGP_COLOR_END,	clrEnd);

	plot.SetColor(m_hPlot, PI_BACKGROUND_CLIENT, BGP_COLOR_END,	clrStart);
	plot.SetColor(m_hPlot, PI_BACKGROUND_CLIENT, BGP_COLOR_START,	clrEnd);

	plot.SetRedraw(m_hPlot, TRUE);
}
