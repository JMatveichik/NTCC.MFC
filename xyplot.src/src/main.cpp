//#pragma warning( disable : 4786 )

#include "stdafx.h"
#include "main.h"
#include "exceptions.h"
#include "RTFString.h"
#include "global.h"
#include "xyplotconst.h"
#include "..\resource.h"

using namespace std;
using namespace xyplot;

const char XYPLOTHEADER[] = "[xyplot.v3.7]";
const char XYPLOTFOOTER[] = "[/xyplot]";

#pragma warning(disable:4355)

XYPlot::XYPlot(HMODULE hm) : hmod(hm), title(this), selection(this), legend(this) 
{	
	redraw = TRUE;

	bkColor = RGB( 0xff, 0xff, 0xff );

	//Фон окна
	bkTotal.SetFillColors(RGB(255, 255, 255), RGB(54, 50, 144));
    	
	//Фон графика
	bkClient.SetFillColors(RGB(54, 50, 144), RGB(255, 255, 255));    
	

	legend.Enable( FALSE  );
	legend.SetAlign(PLP_LEFT);

	border.Enable(FALSE);

	InitializeCriticalSection( &mainCritical );
	InitializeCriticalSection( &streamerCritical );
	
	disabled = FALSE;

	m_dGrowthFactorX = 1.2;
	m_dGrowthFactorY = 1.2;

	///	Стили осей 
	unsigned long lStyleX1 = AF_AXIS_TITLED | AF_AXIS_MINOR_TICKS | AF_AXIS_MINOR_GRIDLINES | AF_AXIS_MINOR_LABELS | \
		AF_AXIS_MAJOR_TICKS | AF_AXIS_MAJOR_GRIDLINES | AF_AXIS_MAJOR_LABELS;
	unsigned long lStyleY1 = lStyleX1;
	unsigned long lStyleX2 = 0; 
	unsigned long lStyleY2 = 0;

	xAxisBottom = new Axis(this);
	xAxisTop	= new Axis(this);
	yAxisLeft	= new Axis(this);
	yAxisRight	= new Axis(this);

	xAxisBottom->Create(this, PI_AXIS_BOTTOM, lStyleX1, "Заголовок оси X 1", 0.0, 10.0 );
	xAxisBottom->SetDigitsFormat("%5.1f", TRUE);
	xAxisBottom->SetDigitsFormat("%5.2f", FALSE);
	xAxisBottom->SetMinorTicksCount(4);
	xAxisBottom->SetMajorTicksCount(6);
	
	yAxisLeft->Create(this, PI_AXIS_LEFT, lStyleY1, "Заголовок оси Y 1", 0, 8.0);	
	yAxisLeft->SetDigitsFormat("%5.1f", TRUE);
	yAxisLeft->SetDigitsFormat("%5.2f", FALSE);
	yAxisLeft->SetMinorTicksCount(4);
	yAxisLeft->SetMajorTicksCount(5);

	xAxisTop->Create(this, PI_AXIS_TOP, lStyleX2, "Заголовок оси X 2", 0.0, 10.0);
	xAxisTop->SetDigitsFormat("%5.2f", TRUE);
	xAxisTop->SetDigitsFormat("%5.3f", FALSE);
	xAxisTop->SetMinorTicksCount(4);
	xAxisTop->SetMajorTicksCount(6);

	yAxisRight->Create(this, PI_AXIS_RIGHT, lStyleY2, "Заголовок оси Y 2", 0.0, 8.0);
	yAxisRight->SetDigitsFormat("%3.1f", TRUE);
	yAxisRight->SetDigitsFormat("%3.1f", FALSE);
	yAxisRight->SetMinorTicksCount(4);
	yAxisRight->SetMajorTicksCount(5);

	selectionStart = false;
	enableQuickScaleMode = false;

	//RegSetDefaults();
}

#pragma warning(default:4355)

XYPlot::~XYPlot()
{
	DeleteProfile( PROFILE_KEY(0L) );
	DeleteLevelLine( NULL );
	DeleteRegion( NULL );

	delete xAxisBottom;
	delete xAxisTop;
	delete yAxisLeft;
	delete yAxisRight;

	DeleteCriticalSection( &mainCritical );
	DeleteCriticalSection( &streamerCritical );
}

long XYPlot::Create( HWND hwndParent )
{
	if( !hmod || !hwndParent || !IsWindow( hwndParent ) )
		return PE_INVALID_WINDOW;

	// Already hooked?
#pragma warning(disable:4312)
	WNDPROC lpfnWndProc = (WNDPROC)GetWindowLongPtr(hwndParent, GWLP_WNDPROC);
	HWND res = (HWND)CallWindowProc(lpfnWndProc, hwndParent, 0xBFF0, 0x02, 0xAAFF);
#pragma warning(default:4312)

	if (res == hwndParent)
		return PE_ALREADY_HOOKED;

	this->hwndParent = hwndParent;
	SetRect( &totalRect, 0, 0, 0, 0 );

	if (HookChain())
		return PE_INVALID_WINDOW;

	Update();

	return PE_NOERROR;
}

void XYPlot::Destroy()
{
	UnhookChain();
}


Axis* XYPlot::GetAxis(long axisPos)
{
	Axis* pAxis = NULL;
	switch(axisPos)
	{
	case PI_AXIS_BOTTOM:
		pAxis = xAxisBottom;
		break;

	case PI_AXIS_TOP:
		pAxis = xAxisTop;
		break;

	case PI_AXIS_LEFT:
		pAxis = yAxisLeft;
		break;

	case PI_AXIS_RIGHT:
		pAxis = yAxisRight;
		break;
	}
	return pAxis;
}

void XYPlot::GetClientBkColors(COLORREF& clr1, COLORREF& clr2) const
{
	bkTotal.GetFillColors(clr1, clr2);
}
void XYPlot::GetPlotBkColors(COLORREF& clr1, COLORREF& clr2) const
{
	bkClient.GetFillColors(clr1, clr2);
	
}

void XYPlot::SetClientBkColors(COLORREF clr1, COLORREF clr2)
{
	bkTotal.SetFillColors(clr1, clr2);
	Update();
}
void XYPlot::SetPlotBkColors(COLORREF clr1, COLORREF clr2)
{
	bkClient.SetFillColors(clr1, clr2);
	Update();
}

long XYPlot::AddProfile( const char* name, COLORREF color, int width, int linetype, const char* line_template, BOOL bVisible, BOOL bMarked, BOOL bottom_axis, BOOL left_axis, long* lProfileID )
{
	MainCriticalSectionHandler handler( &mainCritical );

	*lProfileID = 0L;

	if (!name || !strlen(name))
		return PE_INVALID_ARG;

	if (linetype == PLS_CUSTOM)
	{
		if (!line_template || strlen(line_template) < 2)
			return PE_INVALID_ARG;
	}

	if (width < 1)
		return PE_INVALID_ARG;

	if (!lProfileID)
		return PE_INVALID_ARG;

	Profile *profile = NULL;
	try
	{
		Axis* pAxisX = NULL;
		Axis* pAxisY = NULL;

		if (bottom_axis)
			pAxisX = xAxisBottom;
		else
			pAxisX = xAxisTop;

		if (left_axis)
			pAxisY = yAxisLeft;
		else
			pAxisY = yAxisRight;

		profile = new Profile( name, color, width, bVisible, bMarked, *pAxisX, *pAxisY);
		
		pAxisX->AttachProfile(profile);
		pAxisY->AttachProfile(profile);

		profile->UseSymblos(bMarked); 
		profile->GetLineData().SetType(linetype);
		if (linetype == PLS_CUSTOM)
			profile->GetLineData().SetTemplate(line_template);
	}
	catch (...)
	{
		return PE_INSUFFICIENT_MEMORY;
	}

	*lProfileID = Global::Pointer2ID(profile);
	
	pair<ProfileMapIterator,bool> result = profileMap.insert(make_pair(*lProfileID, profile) );
	if (!result.second)
		return PE_UNKNOWN;

	return PE_NOERROR;
}


//////////////////////////////////////////////////////////////////////////
long XYPlot::AddLevelLine(std::string name,	COLORREF color, long* lLevelID,  int width/* = 1*/, LONG axispos/* = AXIS_LEFT*/, double value)
{

	MainCriticalSectionHandler handler( &mainCritical );

	*lLevelID = 0L;

	LevelLine *level = NULL;
	try
	{
		Axis* axis = NULL;

		switch(axispos) 
		{
		case PI_AXIS_BOTTOM:
			axis = xAxisBottom;
			break;

		case  PI_AXIS_TOP:
			axis = xAxisTop;
			break;

		case  PI_AXIS_LEFT:
			axis = yAxisLeft;
			break;

		case  PI_AXIS_RIGHT:
			axis = yAxisRight;
			break;
		}

		if (axis == NULL)
			return PE_INVALID_ARG;

		level = new LevelLine(this, axis, name, color, width, PLS_SOLID, value); 

	}
	catch (...)
	{
		return PE_INSUFFICIENT_MEMORY;
	}

	*lLevelID = Global::Pointer2ID(level);

	pair<LevelsMapIterator, bool> result = levelsMap.insert( make_pair(*lLevelID, level) );
	
	if (!result.second)
		return PE_UNKNOWN;

	return PE_NOERROR;
}

long XYPlot::DeleteLevelLine( long key )
{
	EnterCriticalSection( &mainCritical );

	if( !key )
	{
		for( LevelsMapIterator it = levelsMap.begin(); it != levelsMap.end(); ++it )
			delete (*it).second;

		levelsMap.clear();		
	}
	else
	{		
		LevelLine* level = FindLevelLine( key );
		if (level == NULL)
			return PE_INVALID_PROFILE;

		delete level;
		levelsMap.erase( key );		
	}

	LeaveCriticalSection( &mainCritical );
	return PE_NOERROR;
}

LevelLine* XYPlot::FindLevelLine( long key )
{
	LevelsMapIterator it = levelsMap.find( key );

	if( it == levelsMap.end() )
		return NULL;

	return (*it).second;
	
}

long XYPlot::SetLevelValue( long key, double value )
{
	LevelsMapIterator it = levelsMap.find( key );

	if( it == levelsMap.end() )
		return PE_INVALID_PROFILE;

	LevelLine* lvl = (*it).second;
	lvl->SetValue(value);

	return PE_NOERROR;
}

long XYPlot::DeleteProfile( PROFILE_KEY key )
{
	EnterCriticalSection( &mainCritical );

	if( !key )
	{
		selection.Select(0, 0);

		for( ProfileMapIterator it = profileMap.begin(); it != profileMap.end(); ++it )
		{
			Profile* prof = (*it).second;
			
			prof->AxisX().DetachProfile(prof);
			prof->AxisY().DetachProfile(prof);

			delete prof;
		}
		
		profileMap.clear();
	}
	else
	{	
		
		Profile* profile = FindProfile( key );
		if (profile == NULL)
			return PE_INVALID_PROFILE;

		if (key == selection.GetSelection() )
			selection.Select(0, 0);

		delete profile;

		profileMap.erase( key );
	}

	LeaveCriticalSection( &mainCritical );
	return PE_NOERROR;
}

Profile* XYPlot::FindProfile( PROFILE_KEY key )
{
	if (profileMap.empty())
		return NULL;

	ProfileMapIterator it = profileMap.find( PROFILE_KEY(key) );

	if( it == profileMap.end() )
		return NULL;

	return (*it).second;
}

PROFILE_KEY XYPlot::FindProfile( const Profile* profile ) const
{
	PROFILE_KEY key(0);
	ProfileMapConstIterator it;
	for (it=profileMap.begin(); it!=profileMap.end(); ++it)
	{
		if (it->second == profile)
		{
			key = it->first;
			break;
		}
	}
	return key;
}

long XYPlot::SetData( PROFILE_KEY key, const double *px, const double *py, int count )
{
	MainCriticalSectionHandler handler( &mainCritical );

	Profile* profile = FindProfile( key );
	if (profile == NULL)
		return PE_INVALID_PROFILE;
	
	profile->SetData( px, py, count );

	Update();

	return PE_NOERROR;
}

long XYPlot::AppendData( PROFILE_KEY key, double x, double y )
{
	return AppendData(key, &x, &y, 1);
}

long XYPlot::AppendData( PROFILE_KEY key, const double *px, const double *py, int count )
{
	//TODO: Optimize drawing of data
	//TODO: Implement data (double and POINT) caching
	MainCriticalSectionHandler handler( &mainCritical );
			
	Profile* profile = FindProfile( key );
	if (profile == NULL)
		return PE_INVALID_PROFILE;

	profile->Append( px, py, count );

	if (redraw)
	{
		double dMinX, dMaxX, dMinY, dMaxY;
		double dAxisMinX, dAxisMaxX, dAxisMinY, dAxisMaxY;

		const Axis& axisX = profile->AxisX();
		axisX.GetRange(dAxisMinX, dAxisMaxX);
	
		const Axis& axisY = profile->AxisY();	
		axisY.GetRange(dAxisMinY, dAxisMaxY);

		profile->Range(dMinX, dMaxX, dMinY, dMaxY);

		if (dAxisMinX > dMinX)
			dAxisMinX = dMinX - (dMaxX - dMinX) * (m_dGrowthFactorX - 1.0);

		if (dAxisMaxX < dMaxX)
			dAxisMaxX = dMaxX + (dMaxX - dMinX) * (m_dGrowthFactorX - 1.0);

		profile->AxisX().ModifyStyle(AF_AXIS_AUTOSCALED, FALSE, FALSE);
		profile->AxisX().SetRange(dAxisMinX, dAxisMaxX);

		if (dAxisMinY > dMinY)
			dAxisMinY = dMinY - (dMaxY - dMinY) * (m_dGrowthFactorY - 1.0);

		if (dAxisMaxY < dMaxY)
			dAxisMaxY = dMaxY + (dMaxY - dMinY) * (m_dGrowthFactorY - 1.0);

		profile->AxisY().ModifyStyle(AF_AXIS_AUTOSCALED, FALSE, FALSE);
		profile->AxisY().SetRange(dAxisMinY, dAxisMaxY);

		Update();
	}
	
	return PE_NOERROR;
}

void XYPlot::Update( RECT *pUpdateRect )
{	
	if( !redraw )
		return;

	InvalidateRect( hwndParent, pUpdateRect, FALSE );
	UpdateWindow( hwndParent );
}

long XYPlot::GetAxisRange(long axis, double &dmin, double &dmax ) const 
{
	const Axis* pAxis = NULL;

	switch(axis)
	{
		case PI_AXIS_LEFT:
			pAxis = yAxisLeft;
			break;

		case PI_AXIS_BOTTOM:
			pAxis = xAxisBottom;
			break;

		case PI_AXIS_RIGHT:
			pAxis = yAxisRight;
			break;

		case PI_AXIS_TOP:
			pAxis = xAxisTop;
			break;

		default:
			pAxis = NULL;
			break;
	}
	
	if(pAxis == NULL)
		return PE_INVALID_ARG;

	pAxis->GetRange(dmin, dmax);
	return PE_NOERROR;	
}

long XYPlot::SetAxisRange(long axis, double dmin, double dmax )
{
	if( dmin > dmax )
		return PE_INVALID_RANGE;

	Axis* pAxis = NULL;

	switch(axis)
	{
	case PI_AXIS_LEFT:
		pAxis = yAxisLeft;
		break;

	case PI_AXIS_BOTTOM:
		pAxis = xAxisBottom;
		break;

	case PI_AXIS_RIGHT:
		pAxis = yAxisRight;
		break;

	case PI_AXIS_TOP:
		pAxis = xAxisTop;
		break;

	default:
		pAxis = NULL;
		break;
	}

	if(pAxis == NULL)
		return PE_INVALID_ARG;

	pAxis->SetRange(dmin, dmax);
	return PE_NOERROR;
}

PLOTHITTESTRESULT XYPlot::HitTest(const POINT ptHit, LONG flags)
{
	PLOTHITTESTRESULT phtr;
	phtr.ptHit = ptHit;
	phtr.itemID = 0;
	phtr.keyControl = (flags & MK_CONTROL) != 0;
	phtr.pointIndex = 0;

	if (title.PointInRect(ptHit))
	{
		phtr.itemID = PI_TITLE;
	}
	else if ( legend.PointInRect(ptHit) )
	{	
		phtr.itemID = PI_LEGEND;
		
		ProfileMapIterator it = profileMap.find( legend.HitTest( ptHit ) );		 
		if (it != profileMap.end())
		{
			phtr.itemID	= long((*it).first);
			phtr.pointIndex = 0;
		}
	}
	else if ( PtInRect(&clientEdge, ptHit ) && profileMap.size() != 0 )	
	{
		for(ProfileMapIterator it = profileMap.begin(); it != profileMap.end(); it++)
		{
			Profile* profile = (*it).second;
			
			//Проверяем наличие данных профиля
			if ( profile->DataSize() == 0 )
				continue;

			unsigned count;
			const POINT* pptLine = profile->GetRenderPointArray( count );

			//попала ли точка в диапазон профиля
			if (ptHit.x < pptLine[0].x || ptHit.x > pptLine[count - 1].x)
				continue;

			//Находим индекс
			unsigned ind;
			for( ind = 0; ind < count && ptHit.x > pptLine[ind].x; ind++ );
			
			//Уточняем индекс ближайшей точки
			if (abs(ptHit.x - pptLine[ind - 1].x) < abs(ptHit.x - pptLine[ind].x))
				ind -= 1;

			//
			if( ind == -1 || ind == count )
				continue;

			double y = pptLine[ind].y - double( pptLine[ind].y - pptLine[ind-1].y ) *
										double( pptLine[ind].x - ptHit.x ) /
										double( pptLine[ind].x - pptLine[ind-1].x );

			int yOffset = 3 + profile->GetLineData().GetWidth();

			if( abs( ptHit.y - y ) < yOffset )
			{
				phtr.ptHit = pptLine[ind];
				phtr.pointIndex = ind;
				phtr.itemID = long((*it).first);
				break;
			}
		}		
	}	
	else if ( xAxisBottom->PointInRect(ptHit) )
	{
		phtr.itemID = PI_AXIS_BOTTOM;
	}
	else if ( xAxisTop->PointInRect(ptHit) )
	{
		phtr.itemID = PI_AXIS_TOP;
	}
	else if ( yAxisLeft->PointInRect(ptHit) )
	{
		phtr.itemID = PI_AXIS_LEFT;
	}
	else if ( yAxisRight->PointInRect(ptHit) )
	{
		phtr.itemID = PI_AXIS_RIGHT;
	}
	return phtr;
}

void XYPlot::NormalRect(POINT p1, POINT p2)
{
	if (p1.x <  p2.x)  
	{
		sel.left = p1.x;
		sel.right = p2.x;
	} 
	else 
	{
		sel.left = p2.x;
		sel.right = p1.x;
	}
	if (p1.y <  p2.y)
	{
		sel.top = p1.y;
		sel.bottom = p2.y;
	}
	else
	{
		sel.top = p2.y;
		sel.bottom = p1.y;
	}
}

void XYPlot::DrawSelectingRect(HDC hdc)
{
	SetROP2(hdc, R2_XORPEN);
	HPEN pen = CreatePen(PS_DOT, 2, GetSysColor(COLOR_GRAYTEXT));
	HPEN old = (HPEN)SelectObject(hdc, pen);

	DrawFocusRect(hdc , &sel);

	SelectObject(hdc, old);
	DeleteObject(pen);
}

LRESULT CALLBACK XYPlot::fnXYPlotHandlerProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
#pragma warning(disable:4312)
	XYPlot *pPlot = reinterpret_cast<XYPlot*>( GetWindowLongPtr( hwnd, GWLP_USERDATA ) );	
#pragma warning(default:4312)

	switch( msg )
	{
	case WM_SHOWWINDOW:
		
		pPlot->SetRedraw(wParam > 0);
		break;

	case WM_ERASEBKGND:
		return 0;

	case WM_PAINT:
		{
			MainCriticalSectionHandler handler( &pPlot->mainCritical );
			GetClientRect( hwnd, &pPlot->totalRect );

			HDC hdc;
			PAINTSTRUCT ps;
			hdc = BeginPaint(hwnd, &ps);
		
			try {
				pPlot->OnDraw(hdc);
			}
			catch (exception& e)
			{ 
				pPlot->VisualException(hdc, string("XYPlot: ") + e.what(), "OnDraw function exception");
			}
			catch (XYPlotException& e)
			{ 
				pPlot->VisualException(hdc, e.Message().c_str(), "OnDraw function exception");
			}
			catch (...)
			{ 
				pPlot->VisualException(hdc, "XYPlot: OnDraw has invoked unexpected exception!", "OnDraw function exception");
			}

			EndPaint(hwnd, &ps);
			
			return 0;
		}

	case WM_LBUTTONDOWN:
		{
			POINT pt;

			pt.x = LOWORD(lParam);
			pt.y = HIWORD(lParam);

			if (pPlot->enableQuickScaleMode)
			{
				if (!pPlot->selectionStart)
				{
					pPlot->fromPt.x = LOWORD(lParam);
					pPlot->fromPt.y = HIWORD(lParam);

					pPlot->selectionStart = true;
					SetCapture(pPlot->hwndParent);
				}
			}
			else
			{
				PLOTHITTESTRESULT phtr = pPlot->HitTest( pt, (LONG)wParam );
				pPlot->selection.ResolvePlotHitTest(&phtr);
				pPlot->Update();
			}
		}
		break;

	case WM_LBUTTONUP:
		{	
			if (pPlot->enableQuickScaleMode)
			{
				if (pPlot->selectionStart)
				{
					pPlot->toPt.x = LOWORD(lParam);
					pPlot->toPt.y = HIWORD(lParam);

					HDC hdc = GetDC(pPlot->hwndParent);

					pPlot->DrawSelectingRect(hdc);

					pPlot->NormalRect(pPlot->fromPt, pPlot->toPt);
					ReleaseCapture();

					double minX, maxX;

					CoordConverter::ConvertToPhysic(*pPlot->xAxisBottom, pPlot->sel.left, minX);
					CoordConverter::ConvertToPhysic(*pPlot->xAxisBottom, pPlot->sel.right, maxX);

					pPlot->xAxisBottom->SetRange(minX, maxX);

					CoordConverter::ConvertToPhysic(*pPlot->xAxisTop, pPlot->sel.left, minX);
					CoordConverter::ConvertToPhysic(*pPlot->xAxisTop, pPlot->sel.right, maxX);

					pPlot->xAxisTop->SetRange(minX, maxX);


					double minY, maxY;

					CoordConverter::ConvertToPhysic(*pPlot->yAxisLeft, pPlot->sel.bottom, minY);
					CoordConverter::ConvertToPhysic(*pPlot->yAxisLeft, pPlot->sel.top, maxY);

					pPlot->yAxisLeft->SetRange(minY, maxY);

					CoordConverter::ConvertToPhysic(*pPlot->yAxisRight, pPlot->sel.bottom, minY);
					CoordConverter::ConvertToPhysic(*pPlot->yAxisRight, pPlot->sel.top, maxY);

					pPlot->yAxisRight->SetRange(minY, maxY);

					SetRect(&pPlot->sel, 0, 0, 0, 0);

					pPlot->selectionStart = false;
					pPlot->Update(NULL);
				}
			}
		}
		break;

	case WM_MOUSEMOVE:
		{
			if (pPlot->enableQuickScaleMode)
			{
				if (pPlot->selectionStart )
				{
					pPlot->toPt.x = LOWORD(lParam);
					pPlot->toPt.y = HIWORD(lParam);

					HDC hdc = GetDC(pPlot->hwndParent);

					pPlot->DrawSelectingRect(hdc);
					pPlot->NormalRect(pPlot->fromPt, pPlot->toPt);
					pPlot->DrawSelectingRect(hdc);
				}
			}
		}
		break;

	case WM_RBUTTONDOWN:
		{
			
		}
		break;
	
	case WM_PRINT:
		{
			HDC hDC = HDC(lParam);
			pPlot->OnDraw(hDC);
		}
		

		break;

	case WM_KEYDOWN:

		pPlot->selection.ResolveKeyPress( LONG(wParam) );
		pPlot->Update();
		break;

	case 0xBFF0:
		if( wParam == 0x02 && lParam == 0xAAFF )
			return LRESULT(hwnd);
		else
			break;

	case WM_SIZE:
		pPlot->recalcAxisSize = TRUE;
		break;

//	case WM_CLOSE:
	case WM_DESTROY:
		pPlot->Destroy();
		break;

	default:
		break;
	}

	return CallWindowProc( pPlot->lpfnOriginalWndProc, hwnd, msg, wParam, lParam );
}

void XYPlot::VisualException(HDC hdc, string strMessage, string strTitle)
{
	COLORREF bk = RGB(100, 20, 20);

	HBRUSH hBrush = CreateSolidBrush(bk);
	FillRect(hdc, &totalRect, hBrush);
	DeleteObject(hBrush);
	
	LOGFONT lf;
	strcpy_s(lf.lfFaceName, 32, "Tahoma");
	lf.lfHeight = -MulDiv(int(0.05*(totalRect.bottom - totalRect.top)), GetDeviceCaps(hdc, LOGPIXELSY), 72);
	lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = FW_NORMAL;
	lf.lfItalic = 0;
	lf.lfUnderline = 0;
	lf.lfStrikeOut = 0;
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfQuality = DEFAULT_QUALITY;
	lf.lfPitchAndFamily = DEFAULT_PITCH|FF_DONTCARE;

	HFONT font = CreateFontIndirect(&lf);
	HFONT hFontOld = (HFONT)SelectObject(hdc, font);
	COLORREF old = ::SetBkColor(hdc, bk);
	COLORREF otc = ::SetTextColor(hdc, RGB(255, 0, 0));
	DrawText(hdc, strMessage.c_str(), int(strMessage.length()), &totalRect, DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	::SetBkColor(hdc, old);
	::SelectObject(hdc, hFontOld);
	::DeleteObject(font);
}

bool XYPlot::HookChain()
{
	#pragma warning(disable:4312 4244)

	SetWindowLongPtr( hwndParent, GWLP_USERDATA, (LONG_PTR) this );

	lpfnOriginalWndProc = (WNDPROC) GetWindowLongPtr( hwndParent, GWLP_WNDPROC );
	if( !lpfnOriginalWndProc )
		return false;

	lpfnOriginalWndProc = (WNDPROC) SetWindowLongPtr( hwndParent, GWLP_WNDPROC, (LONG_PTR)fnXYPlotHandlerProc );
	if( !lpfnOriginalWndProc )
		return false;

	return true;

	#pragma warning(default:4312 4244)
}

void XYPlot::UnhookChain()
{
	#pragma warning(disable:4244)
	if( lpfnOriginalWndProc )
		SetWindowLongPtr( hwndParent, GWLP_WNDPROC, (LONG_PTR) lpfnOriginalWndProc );
	#pragma warning(default:4244)
}

void XYPlot::DeflateVisible( RECT &visible )
{
	RECT r;
	legend.GetRect( r );

	if( legend.GetAlign() == PLP_BOTTOM )
	{
		visible.bottom = r.top;
	}
	else if( legend.GetAlign() == PLP_LEFT )
	{
		visible.left = r.right;
	}
	else if( legend.GetAlign() == PLP_RIGHT )
	{
		visible.right = r.left;
	}
}

inline void TraceRect( HDC hdc, const RECT& r )
{
	Rectangle( hdc, r.left, r.top, r.right, r.bottom );
}

void XYPlot::PreDraw( HDC hdc, LPRECT lpRect/* = NULL*/ )
{
	RECT visible;
	RECT titleRect;
	title.CalcRect(titleRect);

	// border
	CopyRect( &visible, lpRect == NULL? &totalRect : lpRect);
	if( border.IsEnabled() )
	{
		int borderWidth = border.GetLineData().GetWidth();
		InflateRect(&visible, -borderWidth, -borderWidth);
		border.SetRect( totalRect );
	}

	// legend
	RECT legendRect;
	if (legend.IsEnabled())
	{
		CopyRect(&legendRect, &visible);
		legend.PreDraw( hdc, &legendRect );
		legend.GetRect( legendRect );

		if( legend.GetAlign() == PLP_LEFT )
			visible.left += legendRect.right - legendRect.left;
		else if( legend.GetAlign() == PLP_RIGHT )
			visible.right -= legendRect.right - legendRect.left;
		else // legend.GetAlign() = PLP_BOTTOM
			visible.bottom -= legendRect.bottom - legendRect.top;
	}
	

	/*AXIS*/
	//////////////////////////////////////////////////////////////////////
	///	

	// clientEdge
	CopyRect( &clientEdge, &visible );

	if( title.IsEnabled() )
		clientEdge.top += abs( titleRect.bottom - titleRect.top );
		
	RECT rcTop	= xAxisTop->PreDraw(hdc, clientEdge);
	SIZE szTop	= xAxisTop->GetSize();

	RECT rcRight  = yAxisRight->PreDraw(hdc, clientEdge);
	SIZE szRight  = yAxisRight->GetSize();

	RECT rcLeft   = yAxisLeft->PreDraw(hdc, clientEdge);
	SIZE szLeft	  = yAxisLeft->GetSize();

	RECT rcBottom = xAxisBottom->PreDraw(hdc, clientEdge);
	SIZE szBottom = xAxisBottom->GetSize();

	rcTop.left  += szLeft.cx;
	rcTop.right -= szRight.cx;

	rcRight.top    += szTop.cy;
	rcRight.bottom -= szBottom.cy;

	rcBottom.left  += szLeft.cx;
	rcBottom.right -= szRight.cx;

	rcLeft.top    += szTop.cy;
	rcLeft.bottom -= szBottom.cy;

	xAxisTop->SetRect(hdc, rcTop);
	yAxisRight->SetRect(hdc, rcRight);
	yAxisLeft->SetRect(hdc, rcLeft);
	xAxisBottom->SetRect(hdc, rcBottom);

	::SetRect(&clientEdge, rcLeft.right, rcTop.bottom, rcRight.left, rcBottom.top);

	recalcAxisSize = FALSE;

	for (RegionsMapConstIterator itRgn = regionsMap.begin(); itRgn != regionsMap.end(); ++itRgn)
	{
		PlotRegion* pRgn = (*itRgn).second;
		pRgn->PreDraw();
	}

	///
	//////////////////////////////////////////////////////////////////////////////////

	// title
	if( title.IsEnabled() )
	{
		title.SetRect( clientEdge.left, visible.top, 
		clientEdge.right, visible.top + abs(titleRect.bottom - titleRect.top) );

		title.PreDraw( hdc );
	}
}

void XYPlot::TraceMessage(HDC hdc)
{
	static const char* msg = "window size is too small";
	static COLORREF bk = RGB(200, 200, 200);

	HBRUSH hBrush = CreateSolidBrush(bk);
	FillRect(hdc, &totalRect, hBrush);
	DeleteObject(hBrush);

	HFONT hFontOld = (HFONT)SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));
	COLORREF old = ::SetBkColor(hdc, bk);
	DrawText(hdc, msg, int(strlen(msg)), &totalRect, DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	::SetBkColor(hdc, old);
	SelectObject(hdc, hFontOld);
}

void DrawTransparentBitmap(HDC hdc, HBITMAP hBitmap, short xStart,
	short yStart, COLORREF cTransparentColor)
{
	BITMAP     bm;
	COLORREF   cColor;
	HBITMAP    bmAndBack, bmAndObject, bmAndMem, bmSave;
	HBITMAP    bmBackOld, bmObjectOld, bmMemOld, bmSaveOld;
	HDC        hdcMem, hdcBack, hdcObject, hdcTemp, hdcSave;
	POINT      ptSize;

	hdcTemp = CreateCompatibleDC(hdc);
	SelectObject(hdcTemp, hBitmap);   // Выбираем битмап

	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bm);
	ptSize.x = bm.bmWidth;            // Получаем ширину битмапа
	ptSize.y = bm.bmHeight;           // Получаем высоту битмапа
	DPtoLP(hdcTemp, &ptSize, 1);      // Конвертируем из координат
	// устройства в логические
	// точки

	// Создаём несколько DC для хранения временных данных.
	hdcBack   = CreateCompatibleDC(hdc);
	hdcObject = CreateCompatibleDC(hdc);
	hdcMem    = CreateCompatibleDC(hdc);
	hdcSave   = CreateCompatibleDC(hdc);

	// Создаём битмап для каждого DC.

	// Монохромный DC
	bmAndBack   = CreateBitmap(ptSize.x, ptSize.y, 1, 1, NULL);

	// Монохромный DC
	bmAndObject = CreateBitmap(ptSize.x, ptSize.y, 1, 1, NULL);

	bmAndMem    = CreateCompatibleBitmap(hdc, ptSize.x, ptSize.y);
	bmSave      = CreateCompatibleBitmap(hdc, ptSize.x, ptSize.y);

	// В каждом DC должен быть выбран объект битмапа для хранения
	// пикселей.
	bmBackOld   = (HBITMAP)SelectObject(hdcBack, bmAndBack);
	bmObjectOld = (HBITMAP)SelectObject(hdcObject, bmAndObject);
	bmMemOld    = (HBITMAP)SelectObject(hdcMem, bmAndMem);
	bmSaveOld   = (HBITMAP)SelectObject(hdcSave, bmSave);

	// Устанавливаем режим маппинга.
	SetMapMode(hdcTemp, GetMapMode(hdc));

	// Сохраняем битмап, переданный в параметре функции, так как
	// он будет изменён.
	BitBlt(hdcSave, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0, SRCCOPY);

	// Устанавливаем фоновый цвет (в исходном DC) тех частей,
	// которые будут прозрачными.
	cColor = SetBkColor(hdcTemp, cTransparentColor);

	// Создаём маску для битмапа путём вызова BitBlt из исходного
	// битмапа на монохромный битмап.
	BitBlt(hdcObject, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0,
		SRCCOPY);

	// Устанавливаем фоновый цвет исходного DC обратно в
	// оригинальный цвет.
	SetBkColor(hdcTemp, cColor);

	// Создаём инверсию маски.
	BitBlt(hdcBack, 0, 0, ptSize.x, ptSize.y, hdcObject, 0, 0,
		NOTSRCCOPY);

	// Копируем фон главного DC в конечный.
	BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdc, xStart, yStart,
		SRCCOPY);

	// Накладываем маску на те места, где будет помещён битмап.
	BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdcObject, 0, 0, SRCAND);

	// Накладываем маску на прозрачные пиксели битмапа.
	BitBlt(hdcTemp, 0, 0, ptSize.x, ptSize.y, hdcBack, 0, 0, SRCAND);

	// XOR-им битмап с фоном на конечном DC.
	BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0, SRCPAINT);

	// Копируем на экран.
	BitBlt(hdc, xStart, yStart, ptSize.x, ptSize.y, hdcMem, 0, 0,
		SRCCOPY);

	// Помещаем оригинальный битмап обратно в битмап, переданный в
	// параметре функции.
	BitBlt(hdcTemp, 0, 0, ptSize.x, ptSize.y, hdcSave, 0, 0, SRCCOPY);

	// Удаляем битмапы из памяти.
	DeleteObject(SelectObject(hdcBack, bmBackOld));
	DeleteObject(SelectObject(hdcObject, bmObjectOld));
	DeleteObject(SelectObject(hdcMem, bmMemOld));
	DeleteObject(SelectObject(hdcSave, bmSaveOld));

	// Удаляем DC из памяти.
	DeleteDC(hdcMem);
	DeleteDC(hdcBack);
	DeleteDC(hdcObject);
	DeleteDC(hdcSave);
	DeleteDC(hdcTemp);
}

void XYPlot::DrawToBitmap(HBITMAP hBmp, bool bDrawWindowBG, bool bDrawClientBG)
{
	MainCriticalSectionHandler handler( &mainCritical );

	BITMAP bmp;
	GetObject(hBmp, sizeof(BITMAP), &bmp);
	
	SetRect(&totalRect, 0, 0, bmp.bmWidth, bmp.bmHeight);

	HDC hdc = CreateCompatibleDC(NULL);
	HBITMAP hOldBmp = (HBITMAP)SelectObject(hdc, hBmp);

	BITMAPINFOHEADER bminfoheader;
	::ZeroMemory(&bminfoheader, sizeof(BITMAPINFOHEADER));
	bminfoheader.biSize        = sizeof(BITMAPINFOHEADER);
	bminfoheader.biWidth       = bmp.bmWidth;
	bminfoheader.biHeight      = bmp.bmHeight;
	bminfoheader.biPlanes      = 1;
	bminfoheader.biBitCount    = 32;
	bminfoheader.biCompression = BI_RGB;

	unsigned char* pPixels = new unsigned char[bmp.bmWidth * bmp.bmHeight * 4];

	OnDraw(hdc, NULL, bDrawWindowBG, bDrawClientBG);

	GetDIBits(hdc, hBmp, 0, bmp.bmHeight, pPixels, (BITMAPINFO*) &bminfoheader, DIB_RGB_COLORS); // load pixel info

	// add alpha channel values of 255 for every pixel if bmp
	for (int count = 0; count < bmp.bmWidth * bmp.bmHeight; count++)
	{
		pPixels[count * 4 + 3] = 255; //<----  here i've tried to change the value to test different transparency, but it doesn't change anything
	}
	SetDIBits(hdc, hBmp, 0, bmp.bmHeight, pPixels, (BITMAPINFO*) &bminfoheader, DIB_RGB_COLORS); // save the pixel info for later manipulation


	
	//DrawTransparentBitmap(hdc, hBmp, 0, 0, 0x00FFFFFF);

	SelectObject(hdc, hOldBmp);	
	DeleteDC(hdc);
}

void XYPlot::OnDraw(HDC hdc, LPRECT lpRect/* = NULL*/, bool bDrawWindowBG/* = true*/, bool bDrawClientBG/* = true*/)
{
	if (!redraw)
		return;

	PreDraw( hdc, lpRect );

	if (disabled)
	{
		TraceMessage( hdc );
		return;	
	}

	HDC hdc_mem = CreateCompatibleDC( hdc );
	int nDCWidth  = GetDeviceCaps( hdc, HORZRES );
	int nDCHeight = GetDeviceCaps( hdc, VERTRES );
	bitmap = CreateCompatibleBitmap( hdc, nDCWidth, nDCHeight );
	HBITMAP holdbmp = (HBITMAP) SelectObject( hdc_mem, bitmap );
	
	
	bkTotal.SetRect(totalRect);	
	if (bDrawWindowBG)
		bkTotal.OnDraw(hdc_mem);

	bkClient.SetRect(clientEdge);
	if (bDrawClientBG)
		bkClient.OnDraw(hdc_mem);

	//рисуем профили
	HRGN clipRgn = CreateRectRgnIndirect(&clientEdge);
	SelectClipRgn(hdc_mem, clipRgn);

	//рисуем области
	for (RegionsMapConstIterator itRgn = regionsMap.begin(); itRgn != regionsMap.end(); ++itRgn)
		(*itRgn).second->OnDraw(hdc_mem);

	//рисуем линии уровней
	for( LevelsMapIterator it = levelsMap.begin(); it != levelsMap.end(); ++it )
		(*it).second->OnDraw(hdc_mem);

	
	for( ProfileMapIterator it = profileMap.begin(); it != profileMap.end(); ++it )
		(*it).second->Render(hdc_mem, clientEdge);
	SelectClipRgn(hdc_mem, NULL);
	DeleteObject(clipRgn);

	title.OnDraw( hdc_mem );
	legend.OnDraw( hdc_mem );

 	xAxisBottom->OnDraw(hdc_mem);
 	xAxisTop->OnDraw(hdc_mem);
 	yAxisLeft->OnDraw(hdc_mem);
 	yAxisRight->OnDraw(hdc_mem);

	border.OnDraw( hdc_mem );

	selection.OnDraw(hdc_mem, clientEdge);

	// flip memory device context to screen
	BitBlt(hdc, totalRect.left, totalRect.top, totalRect.right-totalRect.left, totalRect.bottom-totalRect.top, 
		hdc_mem, totalRect.left, totalRect.top, SRCCOPY);
	
	SelectObject(hdc_mem, holdbmp);

	DeleteObject(bitmap);
	DeleteDC(hdc_mem);
}

void XYPlot::RegSetDefaults()
{
	HKEY hkey = NULL;
	unsigned long disp = 0L;
	//unsigned long valsize = 0L;

	// always create default setting key
	long res = RegCreateKeyEx( HKEY_CURRENT_USER, Global::defaultRegLocation, 0L, NULL, 
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkey, &disp );

	if( res != ERROR_SUCCESS )
		throw XYPlotException( "RegSetDefaults(): failed to create default setting key!" );

	// key creation succeeded
	// obtain data and write it to default registry value
	pair< void*, long> retPair; WriteProperties( this, NULL, retPair );
	res = RegSetValueEx( hkey, "Data", 0L, REG_BINARY, (const unsigned char*) retPair.first, retPair.second );
	RegCloseKey( hkey );
	delete [] retPair.first;

	if( res != ERROR_SUCCESS )
		throw XYPlotException( "RegSetDefaults(): failed to save defaults!" );
	
	// copy defaults to preffered configuration key if not exists
	res = RegOpenKeyEx( HKEY_CURRENT_USER, Global::currentRegLocation, 0L, KEY_ALL_ACCESS, &hkey );
	if( res == ERROR_SUCCESS )
	{
		res = RegQueryValueEx( hkey, "Data", NULL, NULL, NULL, NULL );
		if( res == ERROR_SUCCESS )
		{			
			// preffered key already exists --> load it
			RegCloseKey( hkey );
			RegLoadSettings(Global::currentRegLocation, "Data");
			return;
		}
	}
	
	res = RegCreateKeyEx( HKEY_CURRENT_USER, Global::currentRegLocation, 0L, NULL, REG_OPTION_NON_VOLATILE, 
		KEY_ALL_ACCESS, NULL, &hkey, &disp );
	WriteProperties( this, NULL, retPair );
	res = RegSetValueEx( hkey, "Data", 0L, REG_BINARY, (const unsigned char*) retPair.first, retPair.second );
	RegCloseKey( hkey );
	delete [] retPair.first;
	if( res != ERROR_SUCCESS )
		throw XYPlotException( "RegSetDefaults(): failed to save preffered settings!" );
}

BOOL XYPlot::RegSaveSettings( const char *keyString, const char* pszValue  )
{
	HKEY hkey = NULL;
	unsigned long disp = 0L;
	long res = RegCreateKeyEx( HKEY_CURRENT_USER, keyString, 0L, NULL, 
	REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkey, &disp );

	if( res != ERROR_SUCCESS )
		return FALSE;

	pair< void*, long> retPair;
	WriteProperties( this, NULL, retPair );
	
	res = RegSetValueEx( hkey, pszValue, 0L, REG_BINARY, 
		(const unsigned char*) retPair.first, retPair.second );
	RegCloseKey( hkey );

	delete [] retPair.first;

	if( res != ERROR_SUCCESS )
		return FALSE;
		
	return TRUE;
}

BOOL XYPlot::RegLoadSettings( const char *keyString, const char* pszValue )
{
	HKEY hkey = NULL;
	unsigned long valsize = 0L;
	char *data = NULL;
	long res = 0L;

	if( !keyString )
		return FALSE;
	else
		res = RegOpenKeyEx( HKEY_CURRENT_USER, keyString, 0L, KEY_ALL_ACCESS, &hkey );

	if( res != ERROR_SUCCESS )
	{
		// current settings not found --> try to load defaults
		res = RegOpenKeyEx( HKEY_CURRENT_USER, Global::defaultRegLocation, 0L, KEY_READ, &hkey );
		if( res != ERROR_SUCCESS )
			return FALSE;

		res = RegQueryValueEx( hkey, pszValue, NULL, NULL, NULL, NULL );
		if( res != ERROR_SUCCESS )
			return FALSE;
	}
	
		
	res = RegQueryValueEx( hkey, pszValue, NULL, NULL, NULL, &valsize );
	if( res != ERROR_SUCCESS )
		return FALSE;
	
	// alloc neccessary space for data
	if( valsize < 1 )
		throw XYPlotException( "RegSaveSettings(): invalid value size retrieved!" );

	data = new char[ valsize ];
	
	res = RegQueryValueEx( hkey, pszValue, NULL, NULL, (unsigned char*)data, &valsize );
	if( res != ERROR_SUCCESS )
	{
		delete [] data;
		RegCloseKey( hkey );
		return FALSE;
	}

	try{ 
		ReadProperties( this, NULL, std::make_pair( data, valsize ) ); 
	}
	catch( ... )
	{
		delete [] data;
		RegCloseKey( hkey );
		return FALSE;
	}
	
	RegCloseKey( hkey );
	delete [] data;
	
	return TRUE;
}

void XYPlot::SaveProperties( const char *lpszFileName )
{
	try
	{
		MainCriticalSectionHandler lockSaveProperties( &streamerCritical );
		pair< void*, long> retPair;
		WriteProperties( this, lpszFileName, retPair );
		
		delete [] retPair.first;
	}
	catch( ... )
	{
		MessageBox( hwndParent, "Failed to save properties.", "XYPlot", MB_OK|MB_ICONHAND );
	}
}

void XYPlot::LoadProperties( const char *lpszFileName )
{
	try
	{
		MainCriticalSectionHandler lockSaveProperties( &streamerCritical );

		ReadProperties( this, lpszFileName, std::make_pair((void*)0,long(0)) );
	}
	catch( ... )
	{
		MessageBox( hwndParent, "Failed to load properties.", "XYPlot", MB_OK|MB_ICONHAND );
	}
	
}

void XYPlot::WriteProperties( XYPlot* pPlot, const char* fileName, pair<void*, long>& retPair)
{
	retPair.first = 0;
	retPair.second = 0;

	char name[MAX_PATH];

	// Prepare file
	if( !fileName )
	{
		SYSTEMTIME st;
		GetLocalTime( &st );
		GetTempPath( sizeof( name ), name );
		string tempFileName = name;
		if( tempFileName.length() < 1 )
			tempFileName = ".";
		GetTempFileName( tempFileName.c_str(), "grt", st.wMinute + st.wHour + st.wSecond + st.wMilliseconds, name );
	}
	else
		strcpy_s( name, fileName );

	HANDLE file = CreateFile( name, FILE_ALL_ACCESS, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	if( file == INVALID_HANDLE_VALUE )
		return;
	
	// Write header
	if (!RTFString::WriteString(file, XYPLOTHEADER))
		return;

	list<SavableItem*> to_be_saved;
	to_be_saved.push_back(&pPlot->border);
	to_be_saved.push_back(&pPlot->title);
	to_be_saved.push_back(&pPlot->legend);
	to_be_saved.push_back(&pPlot->selection);
	to_be_saved.push_back(pPlot->xAxisBottom);
	to_be_saved.push_back(pPlot->xAxisTop);
	to_be_saved.push_back(pPlot->yAxisLeft);
	to_be_saved.push_back(pPlot->yAxisRight);
	

	for(list<SavableItem*>::iterator it=to_be_saved.begin(); it!=to_be_saved.end(); ++it)
	{
		try
		{
			if (!(*it)->Write(file))
				return;
		}
		catch (...)
		{
			return;
		}
	}

	
	DWORD dwRes;


	// background color
	if (!WriteFile( file, &pPlot->bkColor, sizeof(pPlot->bkColor), &dwRes, NULL) || dwRes != sizeof(pPlot->bkColor))
		return;
	
	// write footer
	if (!RTFString::WriteString(file, XYPLOTFOOTER))
		return;

	long size = GetFileSize( file, NULL );
	
	if( fileName )
	{
		CloseHandle( file );
		retPair.second = size;
		return;
	}
	else
	{
		char *data = new char[ size ];

		SetFilePointer( file, 0, NULL, FILE_BEGIN );
		ReadFile( file, data, size, &dwRes, NULL );
		CloseHandle( file );
		DeleteFile( name );
		retPair.first = (void*) data;
		retPair.second = size;
		return;
	}
}

void XYPlot::ReadProperties( XYPlot *pPlot, const char *fileName,  pair<void*,long> inPair )
{
	assert(pPlot != NULL);

	// Prepare file
	char name[MAX_PATH];
	if( !fileName )
	{
		SYSTEMTIME st;
		GetLocalTime( &st );
		GetTempPath( sizeof( name ), name );
		string tempFileName = name;
		if( tempFileName.length() < 1 )
			tempFileName = ".";
		GetTempFileName( tempFileName.c_str(), "grt", st.wMinute + st.wHour + st.wSecond + st.wMilliseconds, name );
		HANDLE file = CreateFile( name, FILE_ALL_ACCESS, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL );
		DWORD dwRes;
		WriteFile( file, inPair.first, inPair.second, &dwRes, NULL );
		CloseHandle( file );

		if( long(dwRes) != inPair.second )
			throw XYPlotException( "ReadProperties(): completely unexpected" );
	}
	else
		strcpy_s( name, fileName );
	
	HANDLE file = CreateFile( name, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

	list<SavableItem*> to_be_saved;
	to_be_saved.push_back(&pPlot->border);
	to_be_saved.push_back(&pPlot->title);
	to_be_saved.push_back(&pPlot->legend);
	to_be_saved.push_back(&pPlot->selection);
	to_be_saved.push_back(pPlot->xAxisBottom);
	to_be_saved.push_back(pPlot->xAxisTop);
	to_be_saved.push_back(pPlot->yAxisLeft);
	to_be_saved.push_back(pPlot->yAxisRight);


	// check header
	if (RTFString::ReadString(file) != XYPLOTHEADER)
		throw XYPlotException("ReadProperties");

	for(list<SavableItem*>::iterator it=to_be_saved.begin(); it!=to_be_saved.end(); ++it)
	{
		if (!(*it)->Read(file))
			throw XYPlotException("ReadProperties");
	}

	DWORD dwRes;
	// background color
	if (!ReadFile( file, &pPlot->bkColor, sizeof(pPlot->bkColor), &dwRes, NULL) || dwRes != sizeof(pPlot->bkColor))
		throw XYPlotException("ReadProperties");
	
	// check footer
	if (RTFString::ReadString(file) != XYPLOTFOOTER)
		throw XYPlotException("ReadProperties");

	pPlot->Update();

	CloseHandle( file );

	if( !fileName )
		DeleteFile( name );
}

//////////////////////////////////////////////////////////////////////////
// Regions functions

long XYPlot::AddRegion( long xAxisID, long yAxisID, long* lRegionID, double x1, double x2, double y1, double y2, const char* pszName )
{
	MainCriticalSectionHandler handler( &mainCritical );

	*lRegionID = 0L;

	PlotRegion *rgn = NULL;
	try
	{
		Axis* pAxisX = NULL;
		Axis* pAxisY = NULL;

		if (xAxisID == PI_AXIS_BOTTOM)
			pAxisX = xAxisBottom;
		else if (xAxisID == PI_AXIS_TOP)
			pAxisX = xAxisTop;
		else
			return PE_INVALID_ARG; 

		if (yAxisID == PI_AXIS_LEFT)
			pAxisY = yAxisLeft;
		else if (yAxisID == PI_AXIS_RIGHT)
			pAxisY = yAxisRight;
		else
			return PE_INVALID_ARG; 

		rgn = new PlotRegion(this, pAxisX, pAxisY, x1, y1, x2, y2, pszName);
	}
	catch (...)
	{
		return PE_INSUFFICIENT_MEMORY;
	}

	*lRegionID = Global::Pointer2ID(rgn);

	pair<RegionsMapIterator, bool> result = regionsMap.insert(make_pair(*lRegionID, rgn) );

	if (!result.second)
		return PE_UNKNOWN;

	return PE_NOERROR;
}

PlotRegion* XYPlot::FindRegion(long lRegionID)
{
	RegionsMapIterator it = regionsMap.find( lRegionID );

	if( it == regionsMap.end() )
		return NULL;

	return (*it).second;
}

//Удалить регион
long XYPlot::DeleteRegion(long lRegionID)
{
	MainCriticalSectionHandler handler( &mainCritical );

	if( !lRegionID )
	{
		for( RegionsMapIterator it = regionsMap.begin(); it != regionsMap.end(); ++it )
		{
			PlotRegion* rgn = (*it).second;
			delete rgn;
		}

		regionsMap.clear();		
	}
	else
	{		
		PlotRegion* rgn = FindRegion( lRegionID );
		if (rgn == NULL)
			return PE_INVALID_PROFILE;

		delete rgn;
		regionsMap.erase( lRegionID );
	}

	return PE_NOERROR;
}

long XYPlot::SetRegionLimits( long lRegionID, double x1, double x2, double y1, double y2 )
{
	MainCriticalSectionHandler handler( &mainCritical );

	PlotRegion* rgn = FindRegion( lRegionID );
	if (rgn == NULL)
		return PE_INVALID_PROFILE;

	rgn->SetXRange(x1, x2);
	rgn->SetYRange(y1, y2);

	return PE_NOERROR;
}

