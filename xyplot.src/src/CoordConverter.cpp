#include "StdAfx.h"
#include "CoordConverter.h"
#include "Axis.h"

using namespace xyplot;

CoordConverter::CoordConverter(void)
{
}

CoordConverter::~CoordConverter(void)
{
}

BOOL CoordConverter::ConvertToDevice(const Axis& axisX, const Axis& axisY, double x, double y, POINT& pt)
{
	//Получаем клиентский прямоугольник 
// 	RECT rc;	
// 	axisX.GetClientRect(rc);
// 	
// 	//Получаем физические пределы
// 	double fxMin, fxMax, fyMin, fyMax;
// 	axisX.GetRange(fxMin, fxMax);
// 	axisY.GetRange(fyMin, fyMax);	
// 
// 	ConvertToDevice()
// 	//Готовим структуру 
// 	GRAPHLIMITS gr;
// 	gr.SetRect(rc);
// 	gr.SetPhisicalLimits(fxMin, fxMax, fyMin, fyMax);
	if ( !ConvertToDevice(axisX, x, pt.x) )
		return FALSE;

	if ( !ConvertToDevice(axisY, y, pt.y) )
		return FALSE;

	return TRUE;
}

// BOOL CoordConverter::ConvertToDevice(const AXISLIMITS& al, double val, int& res, BOOL logMode/* = FALSE*/)
// {
// 	double kx = 1.0;	
// 
// 	if ( !logMode )
// 	{
// 		kx  = (al.maxPos - al.minVal) / ( al.maxVal - al.minVal);
// 		res = al.minPos + int( kx * (val - al.minVal) );
// 	}
// 	else
// 	{
// 		if ( val <= 0 || al.minVal <= 0 || al.maxVal <= 0)
// 			return FALSE;
// 
// 		kx = (al.maxPos - al.minVal) / ( log10(al.maxVal) - log10( al.minVal ) );
// 		res = al.minPos + int( kx * ( log10(val) - log10(al.minVal) ));
// 	}
// 
// 	return TRUE;
// }

BOOL CoordConverter::ConvertToDevice(const Axis& axis, double val, LONG& res)
{
	double kx = 1.0;
	AXISLIMITS al;
	axis.GetLimits(&al);

	bool isXAxis = axis.GetID() == PI_AXIS_TOP || axis.GetID() == PI_AXIS_BOTTOM;

	if ( !axis.IsLogarithmic() )
	{
		kx  = (al.maxPos - al.minPos) / ( al.maxVal - al.minVal);
		if ( isXAxis )
			res = al.minPos + int( kx * (val - al.minVal) );
		else
			res = al.maxPos - int( kx * (val - al.minVal) );

	}
	else
	{
		if ( val <= 0 || al.minVal <= 0 || al.maxVal <= 0)
			return FALSE;

		kx = (al.maxPos - al.minPos) / ( log10(al.maxVal) - log10( al.minVal ) );
		if ( isXAxis )
			res = al.minPos + int( kx * ( log10(val) - log10(al.minVal) ));
		else
			res = al.maxPos - int( kx * ( log10(val) - log10(al.minVal) ));
	}

	return TRUE;
}

BOOL CoordConverter::ConvertToDevice(int minDevice, 
									 int maxDevice, 
									 double minPhys, 
									 double maxPhys, 
									 double val, 
									 int& res, 
									 BOOL rev/* = FALSE*/,
									 BOOL logMode/* = FALSE*/)
{
	
	res = rev ? maxDevice : minDevice;
	
	double k = 1.0;
	int pos = 0;

	if ( !logMode )
	{
		k   = (maxDevice - minDevice) / ( maxPhys - minPhys);
		pos = int( k * (val - minPhys) );
		
	}
	else
	{
		if ( val <= 0 || minPhys <= 0 || maxPhys <= 0)
			return FALSE;

		k   = (maxDevice - minDevice) / ( log10(maxPhys) - log10( minPhys ) );
		pos = int( k * ( log10(val) - log10(minPhys) ));		
	}

	if (rev)
		res -=  pos;
	else
		res +=  pos;

	return TRUE;
}

BOOL CoordConverter::ConvertToDevice(const GRAPHLIMITS& grLimits, double x, double y, POINT& ptRes, BOOL xLogMode/*= FASLE */, BOOL yLogMode/* = FALSE*/, BOOL calcX /*= TRUE*/, BOOL calcY /*= TRUE */)
{
	double kx = 1.0;
	double ky = 1.0;

	ptRes.x = grLimits.left;
	ptRes.y = grLimits.bottom;

	if (calcX)
	{
		if ( !xLogMode )
		{
			kx = (grLimits.right - grLimits.left) / (grLimits.xMax - grLimits.xMin);
			ptRes.x = grLimits.left + int( kx * (x - grLimits.xMin) );
		}
		else
		{
			if ( x <= 0 || grLimits.xMin <= 0 || grLimits.xMax <= 0)
				return FALSE;

			kx = (grLimits.right - grLimits.left) / ( log10(grLimits.xMax) - log10(grLimits.xMin) );
			ptRes.x = grLimits.left + int( kx * ( log10(x) - log10(grLimits.xMin) ));
		}
	}
	
	if (calcY)
	{	
		if ( !yLogMode )
		{
			ky = (grLimits.bottom - grLimits.top) / (grLimits.yMax - grLimits.yMin);
			ptRes.y = grLimits.bottom - int( ky * (y - grLimits.yMin) );
		}
		else
		{
			if ( y <= 0 || grLimits.yMin <= 0 || grLimits.yMax <= 0)
				return FALSE;

			ky = (grLimits.bottom - grLimits.top) / ( log10(grLimits.yMax) - log10(grLimits.yMin) );
			ptRes.y = grLimits.bottom - int( ky * ( log10(y) - log10(grLimits.yMin) ) );
		}
	}

	return TRUE;
}


BOOL CoordConverter::ConvertToPhysic(const Axis& axis, int val, double& res)
{
	double kx = 1.0;	

	AXISLIMITS al;
	axis.GetLimits(&al);

	bool isXAxis = axis.GetID() == PI_AXIS_TOP || axis.GetID() == PI_AXIS_BOTTOM;

	if ( !axis.IsLogarithmic() )
	{
		kx  =  ( al.maxVal - al.minVal) / (al.maxPos - al.minPos);
		if ( isXAxis )
			res = al.minVal + double( kx * (val - al.minPos) );
		else
			res = al.maxVal - double( kx * (val - al.minPos) );

	}
	else
	{
		if ( val <= 0 || al.minVal <= 0 || al.maxVal <= 0)
			return FALSE;

		kx =  ( al.maxPos  - al.minPos )   / ( log10(al.maxVal) - log10(al.minVal) ) ;
		double A;

		if ( isXAxis )
			A = ( val  - al.minPos ) / kx + log10(al.minVal);
		else
			A = ( al.maxPos - val ) / kx + log10(al.minVal);

		res = pow(10.0, A); 		
	}

	return TRUE;
}

BOOL CoordConverter::ConvertToPhysic(const AXISLIMITS& al, int val, double& res, BOOL logMode/* = FALSE*/)
{
	double kx = 1.0;	

	if ( !logMode )
	{
		kx  =  ( al.maxVal - al.minVal) / (al.maxPos - al.minPos);
		res = al.minVal + double( kx * (val - al.minPos) );
	}
	else
	{
		if ( val <= 0 || al.minVal <= 0 || al.maxVal <= 0)
			return FALSE;

 		kx =  ( al.maxPos  - al.minPos )   / ( log10(al.maxVal) - log10(al.minVal) ) ;
		double A = (val  - al.minPos ) / kx + log10(al.minVal);
		res = pow(10.0, A); 		
	}

	return TRUE;
}
// DPOINT CoordConverter::ConvertToPhysics(Axis& axisX, Axis& axisY, POINT pt)
// {
// 	DPOINT ppt;
// 
// 	return ppt;
// }