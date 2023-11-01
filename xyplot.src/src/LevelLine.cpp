#include "StdAfx.h"
#include "LevelLine.h"
#include "RTFString.h"
#include "Axis.h"
#include "xyplotconst.h"
#include "main.h"

static const char BEGINMARK[] = "[levelline]";
static const char ENDMARK[] = "[/levelline]";

using namespace xyplot;

LevelLine::LevelLine(XYPlot* parent, Axis* parentAxis, std::string labelText, COLORREF Color/* = RGB(255,255,255)*/, int Width/* = 2*/, int LineType/* = PLS_SOLID*/, double value/* = 0.0*/)
{
	this->parentAxis = parentAxis;
	this->value = value;
	
	pos = LLP_LEFT|LLP_ABOVE;
	enabledLabel = TRUE;

	label = new RTFString(parent->hmod);
	label->SetText(labelText);

	//����� ��� ����������� �������� �������
	line  = new LineData(Color, Width, LineType);	
}

LevelLine::~LevelLine(void)
{
	delete line;	
	delete label;
}

void LevelLine::SetLabel(std::string str)
{
	label->SetText(str);
}

const std::string& LevelLine::GetLabel() const
{
	return label->GetText();
}

long LevelLine::GetAxisToAttach() 
{ 
	return parentAxis->GetStyle();
};

void LevelLine::OnDraw( HDC hdc)
{
	if (!enabled)
		return;

	double axisMin, axisMax;	
	parentAxis->GetRange(axisMin, axisMax);

	if (value <= axisMin || value >= axisMax )
		return;

	int id = parentAxis->GetID();
	RECT rcEdge; 
	
	parentAxis->GetClientRect(rcEdge);

	double  k;
		
	int ClientWidth  = rcEdge.right - rcEdge.left; 
	int ClientHeight = rcEdge.bottom - rcEdge.top; 

    HPEN oldPen = NULL;

	POINT  ptFrom  = {0, 0};			
	POINT  ptTo    = {0, 0};
	POINT  ptLbl   = {0, 0};
			
	oldPen = (HPEN) SelectObject( hdc, (HPEN)*line);

	int width  = 0;
	int height = 0;
	int offset = 0;

	BOOL bLogMode = ( parentAxis->GetStyle() & AF_AXIS_LOG_MODE ) == AF_AXIS_LOG_MODE;

	if( id == PI_AXIS_TOP || id == PI_AXIS_BOTTOM )
	{
		/*���������� ��������� ��� ������������ ����� ������*/
		
		ptFrom.y = rcEdge.top;
		ptTo.y   = rcEdge.bottom;		

		if(bLogMode)
		{
			k = double( ClientWidth ) / (log10( axisMax) - log10(axisMin));

			double fx = value;
			double fLogRange = ( floor(log10(fx)) - floor(log10(axisMin))) * k;
			if( fx >= 1 )
				for( ;fx>=10; fx/=10 );
			else
				for( ;fx<=1; fx*=10 );

			fLogRange += log10( fx ) * k;

			ptFrom.x = ptTo.x = rcEdge.left + LONG(fLogRange);
		}
		else
		{
			k = double ( ClientWidth ) / (axisMax - axisMin);
			ptFrom.x = ptTo.x = rcEdge.left + long(ceil((value - axisMin)*k));
		}
				

		/*���������� ��������� ��� �������*/
		if (enabledLabel)
		{	
			label->SetOrientation(-90);		// ���������� �������
			SIZE size;
			label->GetDimensions(size);
			
			/* �������� ������� */
			width  = abs(size.cy); //lblRect.right - lblRect.left;
			height = abs(size.cx); //lblRect.bottom - lblRect.top;
			offset =  width / 3;

			/*���������� ������� ������� � ����������� �� ���������*/
			if ((pos & LLP_ABOVE) == LLP_ABOVE)				// ��� ������ ������ (����� �� �����)
				ptLbl.x  = ptFrom.x - height;

			if ((pos & LLP_UNDER) == LLP_UNDER)				// ��� ������ ������ (������ �� �����)
				ptLbl.x  = ptFrom.x;

			if ((pos & LLP_ATLINE) == LLP_ATLINE) // �� ����� ������
				ptLbl.x  = ptFrom.x - height / 2;

			if ((pos & LLP_LEFT) == LLP_LEFT)				// ������������ ������� �� ������ ���� (�� �������)
				ptLbl.y  = rcEdge.bottom - offset;

			if ((pos & LLP_RIGHT) == LLP_RIGHT)				// ������������ ������� �� ������� ���� (�� ��������)
				ptLbl.y  = rcEdge.top + width + offset;

			if ((pos & LLP_CENTER) == LLP_CENTER)			// ������������ ������� �� �������� 
				ptLbl.y  = rcEdge.bottom - ClientHeight / 2 + width / 2;
		}
		
	}
	else
	{
		ptFrom.x = rcEdge.left;
		ptTo.x   = rcEdge.right;

		/*���������� ��������� ��� �������������� ����� ������*/
		if(bLogMode)
		{
			k = double ( ClientHeight ) / (log10( axisMax) - log10(axisMin));

			double fx = value;
			double fLogRange = ( floor(log10(fx)) - floor(log10(axisMin)) ) * k;

			if( fx >= 1 )
				for( ;fx>=10; fx/=10 );
			else
				for( ;fx<=1; fx*=10 );

			fLogRange += log10( fx ) * k;

			ptFrom.y = ptTo.y = rcEdge.bottom - LONG(fLogRange);
		}
		else
		{
			k = double ( ClientHeight ) / (axisMax - axisMin);
			ptFrom.y = ptTo.y = rcEdge.bottom + long(ceil((value - axisMin)*k )) + 1;
		}


		/*���������� ��������� ��� �������*/
		if (enabledLabel)
		{	

			label->SetOrientation(0);		// ���������� �������
			SIZE size;
			label->GetDimensions(size);

			/* �������� ������� */
			width   = abs(size.cx);
			height  = abs(size.cy);
			offset  =  width / 3 + 1;

			if ((pos & LLP_ABOVE) == LLP_ABOVE)					// ��� ������ ������ (����� �� �����)
				ptLbl.y  = ptFrom.y - height;					
																
			if ((pos & LLP_UNDER) == LLP_UNDER)					// ��� ������ ������ (������ �� �����)
				ptLbl.y  = ptFrom.y;							
																
			if ((pos & LLP_ATLINE) == LLP_ATLINE)				// �� ����� ������
				ptLbl.y  = ptFrom.y - height / 2;				
																
			if ((pos & LLP_LEFT) == LLP_LEFT)					// ������������ ������� �� ������ ���� (�� �������)
				ptLbl.x  = rcEdge.left + offset;				
																
			if ((pos & LLP_RIGHT) == LLP_RIGHT)					// ������������ ������� �� ������� ���� (�� ��������)
				ptLbl.x  = rcEdge.right - width - offset;		
																
			if ((pos & LLP_CENTER) == LLP_CENTER)				// ������������ ������� �� �������� 
				ptLbl.x  = rcEdge.left  + ClientWidth / 2 - width / 2;
		}
	}

	MoveToEx(hdc, ptFrom.x, ptFrom.y, NULL);
	LineTo(hdc, ptTo.x, ptTo.y);
	SelectObject(hdc, oldPen);
	
	/*������������ ������� ���� ��������*/
	if (enabledLabel)
		label->Draw(hdc, ptLbl.x, ptLbl.y);
}



