#include "StdAfx.h"
#include "gridlines.h"
#include "Axis.h"

static const char BEGINMARK[] = "[gridlines]";
static const char ENDMARK[] = "[/gridlines]";

using namespace xyplot;

GridLines::GridLines(Axis* parentAxis)
{
	this->parentAxis = parentAxis;

	//линия для отображения основных делений
	lineMajor = new LineData(RGB(80,80,80), 1, PLS_SOLID);	
	 //линия для отображения дополнителных делений
	lineMinor = new LineData(RGB(180,180,180), 1, PLS_CUSTOM, LineData::ROUND, "0 3");
}

GridLines::~GridLines(void)
{
	delete lineMajor;
	delete lineMinor;
}

LineData& GridLines::GetLine(BOOL bMajor)
{
	if (bMajor)
		return *lineMajor;
	else
		return *lineMinor;
}
void GridLines::SetLine(const LineData& line, BOOL bMajor)
{
	if (bMajor)
		*lineMajor = line;
	else
		*lineMinor = line;
}


void GridLines::OnDraw( HDC hdc )
{
	if( !enabledMajor && !enabledMinor )
		return;

	int id = parentAxis->GetID();
	//int pos    = parentAxis->GetAlign();
//	int offset = parentAxis->GetTicksOffset();
	RECT rcEdge; 
	
	parentAxis->GetClientRect(rcEdge);
	
	int ClientWidth  = rcEdge.right - rcEdge.left; 
	int ClientHeight = rcEdge.bottom - rcEdge.top; 

	HPEN oldPen = NULL;

	if ( enabledMajor )
	{
		oldPen = (HPEN) SelectObject( hdc, (HPEN)*lineMajor);

		const std::vector<POINT>& majorTicksPos = parentAxis->GetMajorTicksPos();
		
		for (unsigned i = 1; i < majorTicksPos.size()-1; i++)
		{
			POINT  ptFrom  = majorTicksPos.at(i);			
			POINT  ptTo = ptFrom;

			if( id == PI_AXIS_TOP || id == PI_AXIS_BOTTOM )
			{
				if (id == PI_AXIS_BOTTOM)
					ptTo.y = ptFrom.y - ClientHeight; 
				else
					ptTo.y = ptFrom.y + ClientHeight;								
			}
			else
			{
				if (id == PI_AXIS_LEFT)					
					ptTo.x = ptFrom.x + ClientWidth; 
				else
					ptTo.x = ptFrom.x - ClientWidth; 
			}

			MoveToEx(hdc, ptFrom.x, ptFrom.y, NULL);
			LineTo(hdc, ptTo.x, ptTo.y);
			
		}
	}
	SelectObject(hdc, oldPen);

	if ( enabledMinor )
	{
		oldPen = (HPEN) SelectObject( hdc, (HPEN)*lineMinor);

		const std::vector<POINT>& minorTicksPos = parentAxis->GetMinorTicksPos();
		for (unsigned i = 0; i < minorTicksPos.size(); i++)
		{
			POINT  ptFrom  = minorTicksPos.at(i);			
			POINT  ptTo = ptFrom;

			

			if( id == PI_AXIS_TOP || id == PI_AXIS_BOTTOM )
			{				
				if (id == PI_AXIS_BOTTOM)
					ptTo.y = ptFrom.y - ClientHeight; 
				else
					ptTo.y = ptFrom.y + ClientHeight;
			}
			else
			{
				if (id == PI_AXIS_LEFT)					
					ptTo.x = ptFrom.x + ClientWidth; 
				else
					ptTo.x = ptFrom.x - ClientWidth; 					
				
			}

			MoveToEx(hdc, ptFrom.x, ptFrom.y, NULL);
			LineTo(hdc, ptTo.x, ptTo.y);			
		}
		SelectObject(hdc, oldPen);
	}
}
BOOL GridLines::Write(HANDLE hFile) const
{
	DWORD dwRes;

	if (!WriteString(hFile, BEGINMARK))
		return FALSE;	

	if (!WriteFile( hFile, &enabledMajor, sizeof(enabledMajor), &dwRes, NULL ) || dwRes != sizeof(enabledMajor))
		return FALSE;

	if (!WriteFile( hFile, &enabledMinor, sizeof(enabledMinor), &dwRes, NULL ) || dwRes != sizeof(enabledMinor))
		return FALSE;

	if (!lineMajor->Write(hFile))
		return FALSE;

	if (!lineMinor->Write(hFile))
		return FALSE;

	if (!WriteString(hFile, ENDMARK))
		return FALSE;

	return TRUE;
}

BOOL GridLines::Read(HANDLE hFile)
{
	try {
		if (ReadString(hFile) != BEGINMARK)
			return FALSE;

		DWORD dwRes;
		if (!ReadFile( hFile, &enabledMajor, sizeof(enabledMajor), &dwRes, NULL ) || dwRes != sizeof(enabledMajor))
			return FALSE;

		if (!ReadFile( hFile, &enabledMinor, sizeof(enabledMinor), &dwRes, NULL ) || dwRes != sizeof(enabledMinor))
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

