//#pragma warning( disable : 4786 )
#include "stdafx.h"
#include "ticks.h"
#include "axis.h"
#include "linedata.h"

static const char BEGINMARK[] = "[ticks]";	//маркер начала записи в файл информации о метках 
static const char ENDMARK[] = "[/ticks]";		//маркер окончания записи в файл информации о метках

using namespace xyplot;

double FirstLogTick(double val, double& step)
{
	int order = 0;

	if( val <= 1.0)
	{
		while ( val <= 1.0 ) 
		{
			val *= 10;			
			order--;
		}		
		step = pow (10.0, order);
		return ceil(val) * step; 
	}
	else if( val > 1.0)
	{
		while ( val > 10.0  ) 
		{
			val /= 10;
			order++;
		}
		step = pow (10.0, order); 
		return floor(val) * step; 
	}
	return 0.0;
}

Ticks::Ticks(Axis* parentAxis)
:
majorLength (8),	//
minorLength (5),
majorCount  (5),
minorCount  (1),
enabledMajor (TRUE),
enabledMinor (TRUE)

{
	lineMajor = new LineData;
	lineMinor = new LineData;
	
	lineMajor->SetWidth(3);
	lineMinor->SetWidth(3);	

	style = TICKS_INSIDE;
	
	this->parentAxis = parentAxis;	
}	

Ticks::~Ticks()
{	
	delete lineMajor;
	delete lineMinor;
}

int Ticks::GetMaxLength () const
{
	if ((style & TICKS_OUTSIDE) != TICKS_OUTSIDE)
		return 0;

	return (majorLength > minorLength) ? majorLength : minorLength;
}

void Ticks::DrawTick(HDC hdc, int x, int y, int len)
{
	assert(parentAxis);

	//long align = parentAxis->GetAlign();		

	if ( parentAxis->GetID() == PI_AXIS_LEFT || parentAxis->GetID() == PI_AXIS_RIGHT ) 
	{
		if ((style & TICKS_OUTSIDE) == TICKS_OUTSIDE) 
		{
			if (parentAxis->GetID() == PI_AXIS_LEFT)
			{
				MoveToEx( hdc, x - len, y, NULL);
				LineTo( hdc, x,  y);
			}
			else
			{
				MoveToEx( hdc, x , y, NULL);
				LineTo( hdc, x + len,  y);
			}
			
		}
		if ((style & TICKS_INSIDE) == TICKS_INSIDE) 
		{
			if (parentAxis->GetID() == PI_AXIS_LEFT)
			{
				MoveToEx( hdc, x, y, NULL);
				LineTo( hdc, x + len , y);
			}
			else
			{
				MoveToEx( hdc, x - len, y, NULL);
				LineTo( hdc, x, y);
			}
		}
		if ((style & TICKS_BOTH) == TICKS_BOTH)
		{
			MoveToEx( hdc, x - len, y, NULL);
			LineTo( hdc, x + len , y);
		}
	}
	else
	{
		if ((style & TICKS_OUTSIDE) == TICKS_OUTSIDE) 
		{
			if (parentAxis->GetID() == PI_AXIS_TOP)
			{
				MoveToEx( hdc, x, y, NULL);
				LineTo( hdc, x, y - len);
			}
			else
			{
				MoveToEx( hdc, x, y, NULL);
				LineTo( hdc, x, y + len);
			}			
		}
		if ((style & TICKS_INSIDE) == TICKS_INSIDE) 
		{
			if (parentAxis->GetID() == PI_AXIS_TOP)
			{
				MoveToEx( hdc, x, y, NULL);
				LineTo( hdc, x, y + len);
			}
			else
			{
				MoveToEx( hdc, x, y, NULL);
				LineTo( hdc, x, y - len);
			}
		}
		if ((style & TICKS_BOTH) == TICKS_BOTH)
		{
			MoveToEx( hdc, x, y - len, NULL);
			LineTo( hdc, x, y + len);
		}
	}	
}
void Ticks::Clear()
{
	majorPoints.clear();
	minorPoints.clear();

	majorValues.clear();
	minorValues.clear();
}


void Ticks::PreDraw( )
{
	assert(parentAxis);

	BOOL logMode = parentAxis->IsLogarithmic();

	AXISLIMITS al;
	parentAxis->GetLimits(&al);	

	//Очищаем массивы 
	majorValues.clear();
	minorValues.clear();

	if ( !logMode )
	{
		BOOL bIsMajor = TRUE;
		int nMinor = 0;

		int nTickCount  = majorCount + (minorCount * (majorCount - 1));
		double dStep =  (al.maxVal - al.minVal) / ( nTickCount - 1);

		for (int i = 0; i < nTickCount; i++ )
		{			
			double val = al.minVal + i * dStep;
			
			if ( bIsMajor )
				majorValues.push_back(val);
			else
				minorValues.push_back(val);
			
			if (nMinor == minorCount)
			{
				nMinor   = 0;
				bIsMajor = TRUE;
			}
			else
			{
				bIsMajor = FALSE;
				nMinor++;
			}
		}
	}
	else
	{
		double minStep, maxStep;

		double posMin = FirstLogTick(al.minVal, minStep);
		double posMax = FirstLogTick(al.maxVal, maxStep);

		int nOrderMin = int(log10(minStep));
		int nOrderMax = int(log10(maxStep));

		// Значение шага текущего логарифмического диапазона
		double curStep   = pow(10.0, nOrderMin);

		for(int i = nOrderMin; i <= nOrderMax; i++)
		{
			// Максимальное значение текущего логарифмического диапазона
			double curMaxVal = pow(10.0, i + 1.0);		
			
			// Значение первой дополнительной метки 
			double firstMinor = max(2 * curStep, posMin);

			// Значение последней дополнительной метки 
			double lastMinor  = min(curMaxVal - curStep, posMax);

			double curVal = firstMinor;

			while ( curVal <= lastMinor )
			{	
				minorValues.push_back(curVal);				
				curVal += curStep;				
			}

			if (curVal > al.maxVal)
				break;

			majorValues.push_back(curMaxVal);
			curStep = curMaxVal;
		}
	}
}


void Ticks::CalcTicksPos()
{
	assert (parentAxis);

	int id	= parentAxis->GetID();
	//ASSERT(axisDir != CUSTOM);

	BOOL logMode = parentAxis->IsLogarithmic();

	int x = origin.x;
	int y = origin.y;

	AXISLIMITS al;
	parentAxis->GetLimits(&al);	

	//Очищаем массивы 
	majorPoints.clear();
	minorPoints.clear();

	for (unsigned int i = 0; i < majorValues.size(); i++ )
	{			
		double val = majorValues[i];
		int res = 0;

		CoordConverter::ConvertToDevice(al.minPos, al.maxPos, al.minVal, al.maxVal, val, res, 
			(id == PI_AXIS_LEFT || id == PI_AXIS_RIGHT), logMode);
		

		if (id == PI_AXIS_LEFT || id == PI_AXIS_RIGHT)
			y = res;
		else
			x = res;

		POINT pt = {x, y};

		majorPoints.push_back(pt);
	}
	
	for (unsigned int i = 0; i < minorValues.size(); i++ )
	{			
		double val = minorValues[i];
		int res = 0;

		CoordConverter::ConvertToDevice(al.minPos, al.maxPos, al.minVal, al.maxVal, val, res, 
			(id == PI_AXIS_LEFT || id == PI_AXIS_RIGHT), logMode);
		
		if (id == PI_AXIS_LEFT || id == PI_AXIS_RIGHT)
			y = res;
		else
			x = res;

		POINT pt = {x, y};

		minorPoints.push_back(pt);
	}
}

void Ticks::OnDraw( HDC hdc )
{
    HPEN oldpen = NULL;	

	if ( enabledMajor)
	{
		oldpen = (HPEN) SelectObject( hdc, *lineMajor);

		for (unsigned int i = 1; i < majorPoints.size()-1; i++ )
		{
			POINT pt = majorPoints[i];
			DrawTick (hdc, pt.x, pt.y, majorLength);		
		}

		SelectObject( hdc, oldpen );
	}
	

	if ( enabledMinor)
	{
		oldpen = (HPEN) SelectObject( hdc, *lineMinor);

		for (unsigned int i = 0; i < minorPoints.size(); i++ )
		{			
			POINT pt = minorPoints[i];		
			DrawTick (hdc, pt.x, pt.y, minorLength);			
		}

		SelectObject( hdc, oldpen );
	}	
}

void Ticks::SetLine(const LineData& line, BOOL bMajor/* = TRUE*/) 
{ 
	if (bMajor)
		*lineMajor = line; 
	else
		*lineMinor = line;
}

LineData& Ticks::GetLine(BOOL bMajor/* = TRUE*/) 
{ 
	if (bMajor)
		return *lineMajor; 
	else
		return *lineMinor;	
}

BOOL Ticks::Write(HANDLE hFile) const
{
	if (!WriteString(hFile, BEGINMARK))
		return FALSE;

	DWORD dwRes;
	if (!WriteFile( hFile, &enabledMajor, sizeof(enabledMajor), &dwRes, NULL ) || dwRes != sizeof(enabledMajor))
		return FALSE;

	if (!WriteFile( hFile, &enabledMinor, sizeof(enabledMinor), &dwRes, NULL ) || dwRes != sizeof(enabledMinor))
		return FALSE;

	if (!WriteFile( hFile, &majorCount, sizeof(majorCount), &dwRes, NULL ) || dwRes != sizeof(majorCount))
		return FALSE;

	if (!WriteFile( hFile, &minorCount, sizeof(minorCount), &dwRes, NULL ) || dwRes != sizeof(minorCount))
		return FALSE;	

	if (!WriteFile( hFile, &majorLength, sizeof(majorLength), &dwRes, NULL ) || dwRes != sizeof(majorLength))
		return FALSE;

	if (!WriteFile( hFile, &minorLength, sizeof(minorLength), &dwRes, NULL ) || dwRes != sizeof(minorLength))
		return FALSE;
	
	if (!WriteFile( hFile, &style, sizeof(style), &dwRes, NULL ) || dwRes != sizeof(style))
		return FALSE;

	if (!lineMajor->Write(hFile))
		return FALSE;

	if (!lineMinor->Write(hFile))
		return FALSE;

	if (!WriteString(hFile, ENDMARK))
		return FALSE;

	return TRUE;
}

BOOL Ticks::Read(HANDLE hFile)
{
	try
	{
		if (ReadString(hFile) != BEGINMARK)
			return FALSE;

		DWORD dwRes;
		if (!ReadFile( hFile, &enabledMajor, sizeof(enabledMajor), &dwRes, NULL ) || dwRes != sizeof(enabledMajor))
			return FALSE;

		if (!ReadFile( hFile, &enabledMinor, sizeof(enabledMinor), &dwRes, NULL ) || dwRes != sizeof(enabledMinor))
			return FALSE;

		if (!ReadFile( hFile, &majorCount, sizeof(majorCount), &dwRes, NULL ) || dwRes != sizeof(majorCount))
			return FALSE;

		if (!ReadFile( hFile, &minorCount, sizeof(minorCount), &dwRes, NULL ) || dwRes != sizeof(minorCount))
			return FALSE;	

		if (!ReadFile( hFile, &majorLength, sizeof(majorLength), &dwRes, NULL ) || dwRes != sizeof(majorLength))
			return FALSE;

		if (!ReadFile( hFile, &minorLength, sizeof(minorLength), &dwRes, NULL ) || dwRes != sizeof(minorLength))
			return FALSE;

		if (!ReadFile( hFile, &style, sizeof(style), &dwRes, NULL ) || dwRes != sizeof(style))
			return FALSE;

		if (!lineMajor->Read(hFile))
			return FALSE;
		
		if (!lineMinor->Read(hFile))
			return FALSE;
		
		if (ReadString(hFile) != ENDMARK)
			return FALSE;
	}
	catch (...)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL Ticks::GetNearestTick(double& val, POINT& pt)
{
//	double findVal = val;
//	double diffMaj = DBL_MAX;
	int majInd = 0;

//	double diffMin = DBL_MAX;
	int minInd = 0;

	while ( majorValues[majInd++] < val );
	if ( abs(majorValues[majInd] - val) > abs(majorValues[majInd - 1] - val) )
		majInd--;

	while ( minorValues[minInd++] < val );
	if ( abs(minorValues[minInd] - val) > abs(minorValues[minInd - 1] - val) )
		minInd--;

	if (abs(majorValues[majInd] - val)  < abs(minorValues[minInd] - val) )
	{
		val = majorValues[majInd];
		pt = majorPoints[majInd];
		return TRUE;
	}
	else 
	{
		val = minorValues[minInd];
		pt = minorPoints[minInd];
		return FALSE;
	}
}