#ifndef __WARNIINGREGIONS_H_INCLUDED__
#define __WARNIINGREGIONS_H_INCLUDED__

#include "border.h"
#include "BackGroundFill.h"


class Axis;
class PlotRegion;
class RTFString;
class XYPlot;

typedef std::map<long, PlotRegion*> RegionsMap;
typedef RegionsMap::iterator RegionsMapIterator;
typedef RegionsMap::const_iterator RegionsMapConstIterator;


class PlotRegion
{

public:
	PlotRegion(XYPlot* parent,
				  Axis* axisX, 
				  Axis* axisY, 
				  double ptFromX, double ptFromY,
				  double ptToX, double ptToY,
				  std::string labelText = "", 
				  COLORREF color = RGB(128,80,80),
				  double opaque = 100,
				  COLORREF BorderColor = RGB(0,0,0),
				  int BorderWidth = 1, 
				  int BorderLine  = xyplot::PLS_SOLID);

	~PlotRegion(void);

	BOOL PreDraw();

	/*Процедура отисовки региона */
	void OnDraw(HDC hdc);

	void SetXRange(double x1, double x2); 
	void SetYRange(double y1, double y2);


	//Get region border 
	LineData& GetLineData()
	{
		return border.GetLineData();
	}
	
	//Get region background
	BackGround& GetBG()
	{
		return bk;
	}

	std::string GetTitle() const;
	
	void SetTitle(std::string title) const;
	

protected:
	
	const Axis& parentAxisX;		//Ось X которой принадлежит регион 
	const Axis& parentAxisY;		//Ось Y которой принадлежит регион
	
	Border border;				
	BackGround bk;

	RECT rgnRect;

	double xFrom, xTo;
	double yFrom, yTo;

	RTFString* regionText;
};

#endif //__WARNIINGREGIONS_H_INCLUDED__