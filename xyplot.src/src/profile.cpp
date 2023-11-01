#include "stdafx.h"
#include "profile.h"
#include "exceptions.h"
#include "linedata.h"
#include "digits.h"
#include "xyplotconst.h"
#include "CoordConverter.h"
#include "main.h"

using namespace std;
using namespace xyplot;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const int Profile::DEFAULTWIDTH = 2;

// Profile::Profile() :
// name(""),		// Имя кривой для отображения в легенде
// showMarks(FALSE),// Отображать или нет метки на кривой
// showLine(TRUE),	// Отображать или нет линию кривой	
// axisX(NULL),	// ось X к которой привязан профиль
// axisY(NULL),	// ось Y к которой привязан профиль
// newData(TRUE),	// флаг обновления данных 
// minX(DBL_MAX),	// минимальное значение физической координаты X
// maxX(DBL_MIN),	// максимальное значение физической координаты X
// minY(DBL_MAX),	// минимальное значение физической координаты Y
// maxY(DBL_MIN)	// максимальное значение физической координаты Y
// {
// 	SetRect(&lastRenderedRect, 0,0,0,0);
// }

Profile::Profile( std::string name, long color, int width, BOOL showLine, BOOL showMarks, const Axis& xAxis, const Axis& yAxis ) :
axisX(const_cast<Axis&>(xAxis)), axisY(const_cast<Axis&>(yAxis))
{
	this->name = name;
	this->showLine = showLine;
	this->showMarks = showMarks;
	

	line.SetColor(color);
	line.SetWidth( width );
	line.SetType( PLS_SOLID );

	marks.Enable(showMarks);
	marks.SetLineColor(color);
	marks.SetFillColor(color);

	newData = TRUE;
	minX = DBL_MAX;	// минимальное значение физической координаты X
	maxX = DBL_MIN;	// максимальное значение физической координаты X
	minY = DBL_MAX;	// минимальное значение физической координаты Y
	maxY = DBL_MIN;	// максимальное значение физической координаты Y

	showInLegend = TRUE;

	firstVisible = lastVisible = 0;

	SetRect(&lastRenderedRect, 0,0,0,0);
	InitializeCriticalSection(&cs);
}

Profile::~Profile()
{
	DeleteCriticalSection(&cs);
}

void Profile::Show (BOOL bShow)
{
	EnterCriticalSection(&cs);
	
	showLine  = bShow; 
	showMarks = bShow;
	
	LeaveCriticalSection(&cs);
}

void Profile::SetName( std::string name )
{
	EnterCriticalSection(&cs);
	this->name = name;	
	LeaveCriticalSection(&cs);
}

const POINT* Profile::GetRenderPointArray( unsigned& count ) const
{
	count = unsigned(dataDevice.size());
	return count > 0 ? &dataDevice[0] : NULL;
}

long Profile::GetUnit(unsigned index, double &x, double &y) const
{
	if (dataPhysX.empty() || index >= dataPhysX.size())
		return PE_OUT_OF_RANGE;

	x = dataPhysX.at( index );
	y = dataPhysY.at( index );

	return PE_NOERROR;
}

long Profile::GetData( unsigned* count, double *px, double* py ) const
{
	EnterCriticalSection(&cs);

	if (*count < dataPhysX.size())
	{
		*count = unsigned(dataPhysX.size());
		LeaveCriticalSection(&cs);
		return PE_INSUFFICIENT_STORAGE;
	}
	memcpy(px, &dataPhysX[0], dataPhysX.size() * sizeof(double));
	memcpy(py, &dataPhysY[0], dataPhysY.size() * sizeof(double));
	*count = unsigned(dataPhysX.size());
	
	LeaveCriticalSection(&cs);
	return PE_NOERROR;
}

void Profile::Reset() 
{
	EnterCriticalSection(&cs);
	dataPhysX.clear();
	dataPhysY.clear();
	dataDevice.clear(); 
	LeaveCriticalSection(&cs);
}

void Profile::SetData( const double *pfx, const double* pfy, int count )
{
	assert( pfx && pfy);

	EnterCriticalSection(&cs);

	dataPhysX.resize(count);
	dataPhysY.resize(count);
	dataDevice.resize(count);

	memcpy(&dataPhysX[0], pfx, count * sizeof(double));
	memcpy(&dataPhysY[0], pfy, count * sizeof(double));
	memset(&dataDevice[0], 0, count * sizeof(POINT));
	
	newData = TRUE;

	LeaveCriticalSection(&cs);
}

void Profile::Append( double fx, double fy )
{
	EnterCriticalSection(&cs);

	dataPhysX.push_back(fx);
	dataPhysY.push_back(fy);
	
	//Caching min max
	if (minX > fx)		minX = fx;
	if (maxX < fx)		maxX = fx;
	if (minY > fy)		minY = fy;
    if (maxY < fy)		maxY = fy;

	newData = TRUE;

	LeaveCriticalSection(&cs);
}

void Profile::Append( const double *pfx, const double *pfy, unsigned count )
{
	EnterCriticalSection(&cs);
	dataPhysX.insert(dataPhysX.end(), &pfx[0], &pfx[count]);
	dataPhysY.insert(dataPhysY.end(), &pfy[0], &pfy[count]);
	newData = TRUE;
	LeaveCriticalSection(&cs);
}

void Profile::SetLineData(const LineData& srcLine)
{
	EnterCriticalSection(&cs);
	line = srcLine;
	LeaveCriticalSection(&cs);
}

void Profile::Range(double &xmin, double &xmax, double &ymin, double &ymax) const
{
	EnterCriticalSection(&cs);

	if( dataPhysX.size() < 1 )
	{
		xmin = ymin = 0.0;
		xmax = ymax = 1.0;
		LeaveCriticalSection(&cs);
		return;
	}
	if (!newData)
	{
		xmin = minX;
		xmax = maxX;
		ymin = minY;
		ymax = maxY;
		LeaveCriticalSection(&cs);
		return;
	}

	minX = maxX = xmin = xmax = dataPhysX.at(0);
	minY = maxY = ymin = ymax = dataPhysY.at(0);

	for( unsigned i = 1; i < dataPhysX.size(); i++ )
	{
		double x = dataPhysX.at(i);
		double y = dataPhysY.at(i);

		if( xmin > x ) xmin = x;		
		if( xmax < x ) xmax = x;
		if( ymin > y ) ymin = y;		
		if( ymax < y ) ymax = y;
	}
	
	minX = xmin;	maxX = xmax;
	minY = ymin;	maxY = ymax;

	LeaveCriticalSection(&cs);
}

/*Получить физические пределы кривой по оси X*/
void Profile::RangeX(double &xmin, double &xmax) const
{
	MainCriticalSectionHandler csh(&cs);

	if( dataPhysX.size() < 1 )
	{
		xmin = 0.0;
		xmax = 1.0;
		return;
	}
	if (!newData)
	{
		xmin = minX;
		xmax = maxX;		
		return;
	}
	minX = maxX = xmin = xmax = dataPhysX.at(0);	

	for( unsigned i = 1; i < dataPhysX.size(); i++ )
	{
		double x = dataPhysX.at(i);	

		if( xmin > x ) xmin = x;		
		if( xmax < x ) xmax = x;		
	}

	minX = xmin;	maxX = xmax;	
}

/*Получить физические пределы кривой по оси Y*/
void Profile::RangeY(double &ymin, double &ymax) const
{
	MainCriticalSectionHandler csh(&cs);

	if( dataPhysY.size() < 1 )
	{
		ymin = 0.0;
		ymax = 1.0;
		return;
	}
	if (!newData)
	{	
		ymin = minY;
		ymax = maxY;
		return;
	}

	minY = maxY = ymin = ymax = dataPhysY.at(0);

	for( unsigned i = 1; i < dataPhysY.size(); i++ )
	{		
		double y = dataPhysY.at(i);
		
		if( ymin > y ) ymin = y;		
		if( ymax < y ) ymax = y;
	}

	minY = ymin;	maxY = ymax;
}


/*Рисование кривой*/
void Profile::Render(HDC hDC, RECT rect)
{
	MainCriticalSectionHandler csh(&cs);

	// Return if the rectangle and data are the same as before
	if (!showLine || (EqualRect(&lastRenderedRect, &rect) && !newData))
		return;

	//Очистка массива координат устройства 
	dataDevice.clear();

	//Если нет данных
	if ( dataPhysX.empty() )
		return;

	unsigned count = unsigned( dataPhysX.size() );

	firstVisible = count;
	lastVisible = 0;
	for ( unsigned i = 0; i < count; i++)
	{
		double fx = dataPhysX.at(i);	// Значение по оси x
		double fy = dataPhysY.at(i);   // Значение по оси y

		POINT pt;
		
		if ( !CoordConverter::ConvertToDevice(axisX, axisY, fx, fy, pt) )
			continue;

		if (pt.x <= rect.left)
			firstVisible = unsigned(dataDevice.size());
		else if (pt.x <= rect.right)
			lastVisible = unsigned(dataDevice.size());

		dataDevice.push_back(pt);
	}
	
	//Draw profile line
	if ( showLine )
	{
		HPEN oldpen = (HPEN)SelectObject( hDC, (HPEN)line);
		Polyline( hDC, &dataDevice[0], int( dataDevice.size()) );
		SelectObject( hDC, oldpen );
	}	

	if ( showMarks && marks.IsEnabled() )
		marks.OnDraw( hDC, &dataDevice[0], int(dataDevice.size()) );
	
	newData = FALSE;
}

const POINT Profile::GetRenderPoint(int index, double& physX, double& physY) const
{
	MainCriticalSectionHandler csh(&cs);

	physX = dataPhysX.at(index);
	physY = dataPhysY.at(index);
	return dataDevice.at( index ); 
}