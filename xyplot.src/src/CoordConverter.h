#pragma once

class Axis;

typedef struct tagAxisLimits
{
	tagAxisLimits() :  minPos(0), maxPos(0), 
		minVal(0.0), maxVal(0.0)
	{
	}

	tagAxisLimits(int minP, int maxP, double minV, double maxV ) :
	minPos(minP), maxPos(maxP),	minVal(minV), maxVal(maxV)
	{
	}

	int minPos;
	int maxPos;
	double minVal; 
	double maxVal; 

}AXISLIMITS, *LPAXISLIMITS;

typedef struct tagGraphLimits
{
	tagGraphLimits() :  left(0), bottom(0), right(1), top(1),
						xMin(0.0), yMin(0.0), xMax(1.0), yMax(1.0)
	{
	}

	tagGraphLimits(const RECT& rcClient, double xmin, double xmax, double ymin, double ymax ) :
	left(rcClient.left), bottom(rcClient.bottom), right(rcClient.right), top(rcClient.top),
		xMin(xmin), yMin(ymin), xMax(xmax), yMax(ymax)
	{
	}

	void SetRect(const RECT& rc)
	{
		left	= rc.left;
		bottom	= rc.bottom;
		right	= rc.right;
		top		= rc.top;
	}

	void SetPhisicalLimits(double xmin, double xmax, double ymin, double ymax)
	{
		xMin = xmin;
		yMin = ymin;
		xMax = xmax; 
		yMax = ymax;
	}

	int left;
	int bottom;
	int right;
	int top;

	double xMin;
	double yMin;
	double xMax;
	double yMax;

}GRAPHLIMITS, *LPGRAPHLIMITS;

class CoordConverter
{
public:

	CoordConverter(void);
	~CoordConverter(void);

	static BOOL	  ConvertToDevice(const Axis& axisX, const Axis& axisY, double x, double y, POINT& pt);
	static BOOL	  ConvertToDevice(const GRAPHLIMITS& grLimits, double x, double y, POINT& pt, BOOL xLogMode = FALSE, BOOL yLogMode = FALSE, BOOL calcX = TRUE, BOOL calcY = TRUE );
	//static BOOL	  ConvertToDevice(const AXISLIMITS& axisLimits, double val, int& res, BOOL logMode = FALSE);
	static BOOL ConvertToDevice(const Axis& axis, double val, LONG& res);
	
	static BOOL	ConvertToPhysic(const AXISLIMITS& axisLimits, int val, double& res, BOOL logMode = FALSE);
	static BOOL ConvertToPhysic(const Axis& axis, int val, double& res);

	static BOOL CoordConverter::ConvertToDevice(int minDevice, 
		int maxDevice, 
		double minPhys, 
		double maxPhys, 
		double val, 
		int& res, 
		BOOL rev/* = FALSE*/,
		BOOL logMode/* = FALSE*/);

	//static BOOL   ConvertToPhysics(Axis& axisX, Axis& axisY, POINT pt);
	
};
