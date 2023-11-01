// PlotTesterView.cpp : implementation of the CPlotTesterView class
//

#include "stdafx.h"
#include "PlotTester.h"

#include "PlotTesterDoc.h"
#include "PlotTesterView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace xyplot;

// CPlotTesterView

IMPLEMENT_DYNCREATE(CPlotTesterView, CView)

BEGIN_MESSAGE_MAP(CPlotTesterView, CView)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CPlotTesterView construction/destruction

CPlotTesterView::CPlotTesterView()
{
	DWORD dwID = 0L;
	flashThread = CreateThread( NULL, 0, fnDataProc, this, 0L, &dwID );

	evStart	= ::CreateEvent( NULL, FALSE, FALSE, NULL );
	evQuit  = ::CreateEvent( NULL, FALSE, FALSE, NULL );
}

CPlotTesterView::~CPlotTesterView()
{
	SetEvent(evStart);
	SetEvent(evQuit);
	WaitForSingleObject( flashThread, 10000 );
}

DWORD WINAPI CPlotTesterView::fnDataProc( LPVOID lpParam )
{
	XYPlotManager& pm = XYPlotManager::Instance();
	CPlotTesterView* pSelf = (CPlotTesterView*)lpParam;

	double x = 0.0;
	double y = 0.0;

	WaitForSingleObject( pSelf->evStart, INFINITE );

	while( WaitForSingleObject( pSelf->evQuit, 100L ) != WAIT_OBJECT_0 )
	{	
		x += 10;

		pm.SetRedraw(pSelf->m_hPlot, FALSE);

		y = 10.0*cos(0.5*x+1);
		pm.AppendData(pSelf->m_hPlot, pSelf->m_hProf1, &x, &y, 1);

		y = 10.0*sin(0.8*x);
		pm.AppendData(pSelf->m_hPlot, pSelf->m_hProf2, &x, &y, 1);

		y = 5.0*sin(2.0*x);
		pm.AppendData(pSelf->m_hPlot, pSelf->m_hProf3, &x, &y, 1);

		pm.SetRedraw(pSelf->m_hPlot, TRUE);

		Sleep(100);
	}		

	return 0L;
}

BOOL CPlotTesterView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= WS_CLIPSIBLINGS|WS_MAXIMIZE;

	BOOL bPreCreated = CView::PreCreateWindow(cs);

	if( !bPreCreated )
		return FALSE;

	return bPreCreated;
}

// CPlotTesterView diagnostics
#ifdef _DEBUG
void CPlotTesterView::AssertValid() const
{
	CView::AssertValid();
}

void CPlotTesterView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CPlotTesterDoc* CPlotTesterView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPlotTesterDoc)));
	return (CPlotTesterDoc*)m_pDocument;
}
#endif //_DEBUG


// CPlotTesterView message handlers

void CPlotTesterView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	const int POINTS = 200;
	const int SIZE = POINTS+1;
	double X[SIZE];
	double Y[SIZE];

	XYPlotManager& pm = XYPlotManager::Instance();

	m_hPlot = pm.CreatePlot(m_hWnd);
	if (!m_hPlot)
		std::runtime_error("Unable to create plot");

	pm.SetRedraw(m_hPlot, FALSE);

	char szTitle[] = "{\\rtf1\\ansicpg1251{\\fonttbl{\\f0\\fcharset204 Comic Sans MS;}}\
{\\colortbl \\red0\\green112\\blue192;\\red255\\green0\\blue0;}\\cf0\\b\\f0\\fs72Длинное название\\cf1 plotplot\\par}";

	char szXTitle[] = "{\\rtf1\\ansicpg1251{\\fonttbl{\\f0\\fcharset204 Arial;}}\
{\\colortbl \\red0\\green112\\blue192;\\red255\\green0\\blue0;}\\cf0\\b\\f0\\fs42X axis\\cf1\\par}";

	char szYTitle[] = "{\\rtf1\\ansicpg1251{\\fonttbl{\\f0\\fcharset204 Tahoma;}}\
{\\colortbl \\red0\\green112\\blue192;\\red255\\green0\\blue0;}\\cf0\\b\\f0\\fs42Y axis\\par}";

	pm.SetString(m_hPlot, PI_TITLE, 0, szTitle);
	pm.SetString(m_hPlot, PI_AXIS_TOP, PAI_TITLE, szXTitle);
	pm.SetString(m_hPlot, PI_AXIS_BOTTOM, PAI_TITLE, szXTitle);
	pm.SetString(m_hPlot, PI_AXIS_LEFT, PAI_TITLE, szYTitle);
	pm.SetString(m_hPlot, PI_AXIS_RIGHT, PAI_TITLE, szYTitle);

	pm.SetAxisRange(m_hPlot, PI_AXIS_LEFT, -10, 10);

	pm.EnableItem(m_hPlot, PI_BORDER, 0, TRUE);
	pm.EnableItem(m_hPlot, PI_LEGEND, 0, TRUE);
	pm.EnableItem(m_hPlot, PI_AXIS_LEFT, PAI_MAJOR_TICKS, TRUE);
	pm.EnableItem(m_hPlot, PI_AXIS_LEFT, PAI_MINOR_TICKS, TRUE);
	pm.EnableItem(m_hPlot, PI_AXIS_LEFT, PAI_MAJOR_DIGITS, TRUE);
	//pm.EnableItem(m_hPlot, PI_AXIS_LEFT, PAI_AUTOSCALE, TRUE);
	//pm.EnableItem(m_hPlot, PI_AXIS_BOTTOM, PAI_TIME_HISTORY, TRUE);
	//pm.EnableItem(m_hPlot, PI_AXIS_BOTTOM, PAI_AUTOSCALE, TRUE);
	//pm.SetAxisRange(m_hPlot, PI_AXIS_BOTTOM, -200, 300);
	//pm.SetAxisRange(m_hPlot, PI_AXIS_LEFT, -200, DBL_MAX);

	m_hProf1 = pm.CreateProfile(m_hPlot, "Profile 0", 0x00ff00, 1); 
	pm.SetLong(m_hPlot, m_hProf1, PRP_WIDTH, 2);

	pm.SetColor(m_hPlot, PI_BACKGROUND, BGP_COLOR_START, 0xFFFFFF);
	pm.SetColor(m_hPlot, PI_BACKGROUND, BGP_COLOR_END, 255);
	//pm.SetColor(m_hPlot, PI_BACKGROUND_CLIENT, BGP_COLOR_START, 255);
	//pm.SetColor(m_hPlot, PI_BACKGROUND_CLIENT, BGP_COLOR_END, 0xFFFFFF);

	time_t t = time(0);
	for (int i=0; i < SIZE; i++ )
	{
		X[i] = i*0.1;
		//X[i] = t + i;// = i*0.1;
		Y[i] = fabs(10.0*sin(double(0.5*X[i])));		
	}
	pm.SetData(m_hPlot, m_hProf1, X, Y, SIZE);

	m_hProf2 = pm.CreateProfile(m_hPlot, "Profile 1", 0x400080, 1);
 	for (int i=0; i<SIZE; i++ )
 	{		
		X[i] = i*0.1;
		Y[i] = fabs(10.0*cos(double(0.5*X[i])));
 		//Y[i] = 10.0*sin(0.5*X[i]);
 	}
 	pm.SetData(m_hPlot, m_hProf2, X, Y, SIZE);
	
	m_hProf3 = pm.CreateProfile(m_hPlot, "Profile 2", 0x80ff80, 1, PLS_SOLID, NULL, TRUE, TRUE); 
	for (int i=0; i<SIZE; i++ )
	{
		X[i] = i*0.1;
		Y[i] = 3.0*cos(X[i])*exp(-0.2*X[i]);
	}
	pm.SetData(m_hPlot, m_hProf3, X, Y, SIZE);

	char szLevel1[] = "{\\rtf1\\ansicpg1251{\\fonttbl{\\f0\\fcharset204 Tahoma;}}\
{\\colortbl \\red128\\green255\\blue128;}\\cf0\\f0\\fs22Уровень 1}";
	lvl1 = pm.CreateLevel(m_hPlot, PI_AXIS_BOTTOM, 4.5, szLevel1, RGB(128, 255, 128), 3);

	char szLevel2[] = "{\\rtf1\\ansicpg1251{\\fonttbl{\\f0\\fcharset204 Tahoma;}}\
{\\colortbl \\red128\\green255\\blue255;}\\cf0\\f0\\fs22Уровень 2}";
	lvl2 = pm.CreateLevel(m_hPlot, PI_AXIS_LEFT, -5.0, szLevel2, RGB(128, 0, 255), 3);
	pm.SetColor(m_hPlot, lvl2, 0, RGB(0,128,128));

	//pm.SetValue(m_hPlot, PLP_LINELENGTH, 50);
	pm.SetSelectionMode(m_hPlot, SM_FLASH);
	//pm.SetSelectionMode(m_hPlot, SM_TRACE);
	//pm.Select(m_hPlot, m_hProf3, 0);
	pm.SetLong(m_hPlot, PI_LEGEND, PLP_ALIGN, PLP_LEFT);


	HGROUP hGrp = pm.CreateGroup(m_hPlot);
	pm.BindProfileToGroup(m_hPlot, hGrp, m_hProf2);
	pm.BindProfileToGroup(m_hPlot, hGrp, m_hProf3);
	pm.Select(m_hPlot, hGrp, 2000);

	pm.SetRedraw(m_hPlot);

	char szRegion[] = "{\\rtf1\\ansicpg1251{\\fonttbl{\\f0\\fcharset204 Tahoma;}}\
					  {\\colortbl \\red128\\green255\\blue255;}\\cf0\\f0\\fs14Region 1}";
	HPLOTREGION hR = pm.CreateRegion(m_hPlot, PI_AXIS_BOTTOM, PI_AXIS_LEFT, 2, 4, 0, 5, szRegion);
	pm.SetColor(m_hPlot, hR, BGP_COLOR_START, RGB(128,0,128) );
	pm.SetColor(m_hPlot, hR, BGP_COLOR_END, RGB(255,255,255) );
	pm.SetColor(m_hPlot, hR, 0, RGB(0,255,0) );
	
	long l = pm.GetLong(m_hPlot, PI_AXIS_LEFT, PAI_LINE_WIDTH );
	
	((CMDIChildWnd*)GetParent())->MDIMaximize();
}

void CPlotTesterView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	XYPlotManager::Instance().RunDialog(m_hPlot);
}


void CPlotTesterView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_F1)
	{
		static bool started = false;
		if (!started)
			SetEvent(evStart);
		else
			ResetEvent(evStart);
		started = !started;
	}
	else if (nChar == VK_F3)
		XYPlotManager::Instance().RunDialog(m_hPlot);
	else if (nChar == VK_F8)
	{
		double min, max;
		XYPlotManager::Instance().GetAxisRange(m_hPlot, PI_AXIS_BOTTOM, min, max);
	}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
