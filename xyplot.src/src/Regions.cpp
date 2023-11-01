#include "StdAfx.h"
#include "regions.h"
#include "rtfstring.h"
#include "CoordConverter.h"
#include "main.h"

using namespace xyplot;

PlotRegion::PlotRegion(XYPlot* parent,
							 Axis* axisX, 
							 Axis* axisY, 
							 double ptFromX, double ptFromY,
							 double ptToX, double ptToY,
							 std::string labelText/* = ""*/, 
							 COLORREF color/* = RGB(128,80,80)*/,
							 double opaque/* = 100*/,
							 COLORREF BorderColor,  
							 int BorderWidth/* = 0*/, 
							 int BorderLine/*  = PLS_SOLID*/) : 
	parentAxisX(*axisX), 
	parentAxisY(*axisY)	
{

	SetXRange(ptFromX, ptToX);
	SetYRange(ptFromY, ptToY);
	
	border.GetLineData().SetColor(BorderColor);
	border.GetLineData().SetType(BorderLine);
	border.GetLineData().SetWidth( BorderWidth );

	bk.SetFillColors(color, color);
	bk.SetHatchStyle(BGHS_NONE);

	regionText = new RTFString(parent->hmod);
	regionText->SetText(labelText);
		
}

PlotRegion::~PlotRegion(void)
{
	delete regionText;
}

std::string PlotRegion::GetTitle() const
{
	return regionText->GetText();
}

void PlotRegion::SetTitle(std::string title) const
{
	return regionText->SetText(title);
}

void PlotRegion::SetXRange(double x1, double x2)
{
	xFrom = x1;
	xTo = x2;
}

void PlotRegion::SetYRange(double y1, double y2)
{
	yFrom = y1;
	yTo = y2;
}

BOOL PlotRegion::PreDraw()
{
	double minX, maxX;
	double minY, maxY;

	parentAxisX.GetRange(minX, maxX);
	parentAxisY.GetRange(minY, maxY);

	double x = xFrom;
	double y = yFrom;
	
	if (x == DBL_MIN )
		x = minX;

	if (y == DBL_MIN )
		y = minY;

	POINT ptFrom;
	CoordConverter::ConvertToDevice(parentAxisX, parentAxisY, x, y, ptFrom);
	

	x = xTo;
	y = yTo;

	if (x == DBL_MAX )
		x = maxX;

	if (y == DBL_MAX )
		y = maxY;

	POINT ptTo;
	CoordConverter::ConvertToDevice(parentAxisX, parentAxisY, x, y, ptTo);

	rgnRect.left  = ptFrom.x;
	rgnRect.right = ptTo.x;

	rgnRect.bottom = ptFrom.y;
	rgnRect.top	= ptTo.y;

	border.SetRect(rgnRect);
	bk.SetRect(rgnRect);



	return TRUE;
}

/*Процедура отисовки региона */
void PlotRegion::OnDraw(HDC hdc)
{
 	//::SetBkColor(hdc, fillColor);

	//отрисовака фона заливки
	bk.OnDraw(hdc);

	//отрисовка границ
	border.OnDraw(hdc);

	//отрисовка тeкста
	SIZE size;
	regionText->GetDimensions(size);

	int x = rgnRect.left; //+ (rgnRect.right - rgnRect.left) / 2;
	int y = rgnRect.top;  //+ (rgnRect.bottom - rgnRect.top) / 2;

 	//x -= abs(size.cx) / 2;
 	//y -= abs(size.cy) / 2;

	regionText->Draw(hdc, x, y);
	
}

