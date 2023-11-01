#include "StdAfx.h"

#include "xyplotconst.h"
#include "main.h"
#include "axis.h"

static const char BEGINMARK[] = "[axis]";
static const char ENDMARK[] = "[/axis]";

using namespace xyplot;

Axis::Axis(XYPlot* parent)
{
	line.SetWidth(3);
	line.SetColor(RGB(0, 0, 0));	
	parentPlot = parent;
	pLabel		= new Label(parent);
	pDigits		= new Digits(parent, this); 
	pTicks		= new Ticks(this);
	pGridLines	= new GridLines(this);
}

Axis::~Axis(void)
{
	delete pLabel;
	delete pTicks;
	delete pDigits;
	delete pGridLines;
} 

void Axis::GetClientRect(RECT &rect) const 
{
	rect = parentPlot->ClientEdge();
}

void Axis::GetLimits(LPAXISLIMITS al) const
{
	al->minVal = rangeMin;
	al->maxVal = rangeMax;

	RECT rect = parentPlot->ClientEdge();
	if ( id == PI_AXIS_LEFT || id == PI_AXIS_RIGHT )
	{		
		al->minPos = rect.top;
		al->maxPos = rect.bottom;
	}
	else
	{
		al->minPos = rect.left;
		al->maxPos = rect.right;
	}
}

DWORD Axis::SetStyle (DWORD  dwStyles, BOOL bUpdate)
{
	DWORD dwOldStyle = this->dwStyle;

	//pLabel->SetStyle(id);

	if ((dwStyles & AF_AXIS_TITLED) == AF_AXIS_INVISIBLE)
		line.SetType(PLS_INVISIBLE);

	BOOL bEnabled = ((dwStyles & AF_AXIS_TITLED) == AF_AXIS_TITLED);
	pLabel->Enable(bEnabled);

	bEnabled =  ((dwStyles & AF_AXIS_MAJOR_TICKS) == AF_AXIS_MAJOR_TICKS);
	pTicks->EnableMajor(bEnabled);

	bEnabled =  ((dwStyles & AF_AXIS_MINOR_TICKS) == AF_AXIS_MINOR_TICKS);
	pTicks->EnableMinor(bEnabled);

	bEnabled =  ((dwStyles & AF_AXIS_MAJOR_GRIDLINES) == AF_AXIS_MAJOR_GRIDLINES);
	pGridLines->EnableMajor(bEnabled);

	bEnabled =  ((dwStyles & AF_AXIS_MINOR_GRIDLINES) == AF_AXIS_MINOR_GRIDLINES);
	pGridLines->EnableMinor(bEnabled);

	bEnabled =  ((dwStyles & AF_AXIS_MAJOR_LABELS) == AF_AXIS_MAJOR_LABELS);
	pDigits->EnableMajor(bEnabled);

	bEnabled =  ((dwStyles & AF_AXIS_MINOR_LABELS) == AF_AXIS_MINOR_LABELS);
	pDigits->EnableMinor(bEnabled);

	autoScale =  ((dwStyles & AF_AXIS_AUTOSCALED) == AF_AXIS_AUTOSCALED);	

	this->dwStyle = dwStyles;

	if ( parentPlot  && bUpdate )
		parentPlot->Update();

	return dwOldStyle;
}

DWORD  Axis::ModifyStyle(DWORD dwStyle, BOOL bAdd, BOOL bUpdate)
{
	DWORD dwOldStyle = this->dwStyle;

	if ( bAdd )
		this->dwStyle |= dwStyle;
	else
		this->dwStyle = dwOldStyle & ~dwStyle;
	
	SetStyle(this->dwStyle);

	if ( parentPlot  && bUpdate )
		parentPlot->Update();

	return dwOldStyle;
}

BOOL Axis::Create(XYPlot* parentPlot, int id,
				  DWORD dwStyles, 
				  const char* strTitle, /* = NULL*/
				  double dMin, /* = 0.0*/ 
				  double dMax  /* = 1.0*/)
{
	this->parentPlot = parentPlot;

	SetRange(dMin, dMax);

	SetStyle(dwStyles);
	this->id = id;

	pLabel->SetStyle(id);
	
	BOOL bEnabled = TRUE; //((dwStyles & AXIS_TITLED) == AXIS_TITLED);

	if (strTitle != NULL)
		pLabel->SetText( strTitle );

	pLabel->Enable(bEnabled && strTitle != NULL);

	return TRUE;
}


void Axis::SetRange(double dMin, double dMax)
{ 
	rangeMin = dMin; 
	rangeMax = dMax; 

	pDigits->SetRange(dMin, dMax);	
}

SIZE  Axis::GetSize() const
{
	SIZE selfSize = { rcSelf.right - rcSelf.left, rcSelf.bottom - rcSelf.top }; 
	return selfSize;
}

int Axis::Length() const
{
	if (id == PI_AXIS_LEFT || id == PI_AXIS_RIGHT)
		return rcSelf.bottom - rcSelf.top; 
	
	else
		return rcSelf.right - rcSelf.left;
}

void Axis::OptimizeScale()
{	
	/*************************/
	//std::map<long, Profile*>::iterator it;
	std::set<Profile*>::iterator it;

	double fMin = DBL_MAX;
	double fMax = DBL_MIN;

	for (it = attachedProfiles.begin(); it != attachedProfiles.end(); it++)
	{
        double profMinX, profMaxX, profMinY, profMaxY; 
		Profile* prof = (*it);

		prof->Range(profMinX, profMaxX, profMinY, profMaxY);
		
		if (id == PI_AXIS_TOP || id == PI_AXIS_BOTTOM)
		{
			fMin = fMin > profMinX ? profMinX : fMin;
			fMax = fMax < profMaxX ? profMaxX : fMax;
		}
		else
		{
			fMin = fMin > profMinY ? profMinY : fMin;
			fMax = fMax < profMaxY ? profMaxY : fMax;
		}
	}	
	/*************************/
	
	rangeMin = (fMin == DBL_MAX) ? 0.0 : fMin;
	rangeMax = (fMax == DBL_MIN) ? 1.0 : fMax;

	int nRoundPower = 10;

	double fEps = 0.0;
	int _pow = 0;

	// min = max	
	if( fMin == fMax )
	{
		rangeMin = fMin - fMin / 1000;
		rangeMax = fMax + fMax / 1000;
	}

	// min < max	
	if( (dwStyle & AF_AXIS_LOG_MODE) == 0 && fMax > fMin )
	{
		fEps = fabs(fMax - fMin)/nRoundPower;

		if( fEps >= 1 )
			for( ;fEps>=10; fEps/=10, _pow++ );
		else
			for( ;fEps<=1;  fEps*=10, _pow-- );

		fEps = pow( 10.0, _pow );


		if( fMin != floor(fMin/fEps)*fEps )
		{
			if( fMin >= 0 )
			{
				rangeMin = floor(fMin/fEps) * fEps;
			}
			else
			{
				rangeMin = floor(fMin/fEps) * fEps;
			}
		}

		if( fMax != floor(fMax/fEps)*fEps )
		{
			if( fMax >= 0 )
			{
				rangeMax = ceil(fMax/fEps) * fEps;
			}
			else
			{
				rangeMax = ceil(fMax/fEps) * fEps;
			}
		}

		pDigits->SetRange(rangeMin, rangeMax);
	}
}

RECT Axis::PreDraw(HDC hDC, RECT rect)
{
	RECT rc, labelRect, digitsRect;	

	CopyRect(&rc, &rect);

	::SetRect(&labelRect,  0, 0, 0, 0);
	::SetRect(&digitsRect, 0, 0, 0, 0);

	if (autoScale)
		OptimizeScale();	

	if ( IsLabelEnabled() )
	{		
		SIZE size = pLabel->GetLabelSize();
		labelRect.right = size.cx;
		labelRect.bottom = size.cy;
	}

	int nMaxDigitsWidth = pDigits->MaxItemLength( hDC, rangeMin, rangeMax );	

	pTicks->PreDraw();

	if (IsMinorDigitsEnabled() || IsMajorDigitsEnabled())
	{
		pDigits->PreDraw( hDC );
	}

	int nMaxOffset = 0;

	if( IsMinorDigitsEnabled() || IsMajorDigitsEnabled() )
	{
		nMaxOffset = pDigits->CalcRect( hDC, digitsRect );	
	}

	if (id == PI_AXIS_LEFT)
	{
		::SetRect (&rcSelf, rc.left, rc.top, rc.left, rc.bottom);
		
		if (dwStyle ^ AF_AXIS_INVISIBLE)
		{
			rcSelf.right += abs( labelRect.left - labelRect.right );
			rcSelf.right += nMaxDigitsWidth;

			rcSelf.right += nMaxOffset;
			rcSelf.right += pTicks->GetMaxLength();
		}
	}
	else if (id == PI_AXIS_RIGHT)
	{
		::SetRect (&rcSelf, rc.right, rc.top, rc.right, rc.bottom);		
		if (dwStyle ^ AF_AXIS_INVISIBLE)
		{
			rcSelf.left -= abs( labelRect.left  - labelRect.right );
			rcSelf.left -= nMaxDigitsWidth;
			
			rcSelf.left -= nMaxOffset;
			rcSelf.left -= pTicks->GetMaxLength();
		}
	}
	else if (id == PI_AXIS_BOTTOM)
	{
		::SetRect (&rcSelf, rc.left, rc.bottom, rc.right, rc.bottom);		
		if (dwStyle ^ AF_AXIS_INVISIBLE)
		{
			rcSelf.top -= abs( labelRect.top - labelRect.bottom );
			rcSelf.top -= abs( digitsRect.top - digitsRect.bottom );
			
			rcSelf.top -= nMaxOffset;
			rcSelf.top -= pTicks->GetMaxLength();
		}
	}
	else if (id == PI_AXIS_TOP)
	{
		
		::SetRect (&rcSelf, rc.left, rc.top, rc.right, rc.top);		
		if ((dwStyle & AF_AXIS_INVISIBLE) == 0)
		{
			rcSelf.bottom += abs( labelRect.top - labelRect.bottom );
			rcSelf.bottom += abs( digitsRect.top - digitsRect.bottom );
			
			rcSelf.bottom += nMaxOffset;
			rcSelf.bottom += pTicks->GetMaxLength();			
		}
	}

	return rcSelf;
}

void Axis::SetRect(HDC hDC, RECT rect)
{
	RECT rc, labelRect, digitsRect;	

	CopyRect(&rc, &rect);	

	::SetRect(&labelRect,  0, 0, 0, 0);
	::SetRect(&digitsRect, 0, 0, 0, 0);

	if (autoScale)
		OptimizeScale();	

	if ( IsLabelEnabled() )
	{		
		SIZE size = pLabel->GetLabelSize();
		labelRect.right = size.cx;
		labelRect.bottom = size.cy;
	}

	int nMaxDigitsWidth = pDigits->MaxItemLength( hDC, rangeMin, rangeMax );	

	if (IsMinorDigitsEnabled() || IsMajorDigitsEnabled())
	{
		pDigits->PreDraw( hDC );
	}

	int nMaxOffset;

	if( IsMinorDigitsEnabled() || IsMajorDigitsEnabled() )
	{
		nMaxOffset = pDigits->CalcRect( hDC, digitsRect );	
	}

	RECT rcSet;			

	if (id == PI_AXIS_LEFT)
	{
		//for label rect
		CopyRect(&rcSet, &rect);
		rcSet.right = rcSet.left + abs(labelRect.left - labelRect.right);
		pLabel->SetRect(rcSet);

		//for label rect			
		rcSet.left  = rcSet.right;
		rcSet.right += nMaxDigitsWidth;
		pDigits->SetRect(rcSet);			

		POINT pt = {rect.right, rect.bottom};
		pTicks->SetOrigin(pt);
	}
	else if (id == PI_AXIS_RIGHT)
	{
		//for label rect
		CopyRect(&rcSet, &rect);
		rcSet.left = rcSet.right - abs(labelRect.left - labelRect.right);
		pLabel->SetRect(rcSet);

		//for label rect			
		rcSet.right  = rcSet.left;
		rcSet.left  -= nMaxDigitsWidth;
		pDigits->SetRect(rcSet);

		POINT pt = { rect.left, rect.bottom };
		pTicks->SetOrigin(pt);
	}
	else if (id == PI_AXIS_BOTTOM)
	{
		//for label rect
		CopyRect(&rcSet, &rect);
		rcSet.top = rcSet.bottom - abs(labelRect.bottom - labelRect.top);
		pLabel->SetRect(rcSet);

		//for label rect			
		rcSet.bottom  = rcSet.top;
		rcSet.top     -= abs(digitsRect.bottom - digitsRect.top);
		pDigits->SetRect(rcSet);

		POINT pt = {rect.left, rect.top};
		pTicks->SetOrigin(pt);
	}
	else if (id == PI_AXIS_TOP)
	{
		//for label rect
		CopyRect(&rcSet, &rect);
		rcSet.bottom = rcSet.top + abs(labelRect.bottom - labelRect.top);
		pLabel->SetRect(rcSet);

		//for label rect			
		rcSet.top     = rcSet.bottom;
		rcSet.bottom += abs(digitsRect.bottom - digitsRect.top);
		pDigits->SetRect(rcSet);

		POINT pt = {rect.left, rect.bottom};
		pTicks->SetOrigin(pt);
	}

	CopyRect(&rcSelf, &rect);	
}


void Axis::OnDraw(HDC hDC)
{
	if (dwStyle == AF_AXIS_INVISIBLE)
		return;

	pTicks->CalcTicksPos();	

//	TraceRect(hDC, rcSelf, RGB(255, 0, 255) );

	if ( IsLabelEnabled() )
	{
		pLabel->OnDraw(hDC);		
	}

	if( IsMinorDigitsEnabled() || IsMajorDigitsEnabled() )
	{
		pDigits->OnDraw(hDC);	
	}	

	pGridLines->OnDraw(hDC);

	if ( IsMajorTicksEnabled() || IsMinorTicksEnabled())
	{
		pTicks->OnDraw(hDC);		
	}
	
	HPEN hOldPen = (HPEN)SelectObject(hDC, (HPEN)line);

	if (id ==PI_AXIS_LEFT)
	{
		MoveToEx(hDC, rcSelf.right, rcSelf.bottom, NULL);
		LineTo(hDC, rcSelf.right, rcSelf.top);
	}
	else if (id == PI_AXIS_RIGHT)
	{
		MoveToEx(hDC, rcSelf.left, rcSelf.bottom, NULL);
		LineTo(hDC, rcSelf.left, rcSelf.top);			
	}
	else if (id == PI_AXIS_BOTTOM)
	{
		MoveToEx(hDC, rcSelf.left, rcSelf.top, NULL);
		LineTo(hDC, rcSelf.right, rcSelf.top);
	}
	else if (id == PI_AXIS_TOP)
	{
		MoveToEx(hDC, rcSelf.left, rcSelf.bottom, NULL);
		LineTo(hDC, rcSelf.right, rcSelf.bottom);
	}

	SelectObject(hDC, (HPEN)hOldPen);

}

BOOL Axis::AttachProfile(Profile* prof)
{
	//LONG_PTR key = (long)prof;

	std::set<Profile*>::iterator it = attachedProfiles.find(prof);

	if (it == attachedProfiles.end())
	{
		attachedProfiles.insert(prof);
		return TRUE;
	}

	return FALSE;
}

BOOL Axis::DetachProfile(Profile* prof)
{
	std::set<Profile*>::iterator it = attachedProfiles.find(prof);

	if (it != attachedProfiles.end())
	{
		attachedProfiles.erase(it);
		return TRUE;
	}

	return FALSE;
}

BOOL Axis::Write(HANDLE hFile) const
{
	std::list<SavableItem*> to_be_saved;

	to_be_saved.push_back(pLabel);
	to_be_saved.push_back(pTicks);
	to_be_saved.push_back(pDigits);
	to_be_saved.push_back(pGridLines);
	
	if (!WriteString(hFile, BEGINMARK))
		return FALSE;

	for(std::list<SavableItem*>::iterator it=to_be_saved.begin(); it!=to_be_saved.end(); ++it)
	{
		try
		{
			if (!(*it)->Write(hFile))
				return FALSE;
		}
		catch (...)
		{
			return FALSE;
		}
	}

	DWORD dwRes;

	if (!WriteFile( hFile, &dwStyle, sizeof(dwStyle), &dwRes, NULL ) || dwRes != sizeof(dwStyle))
		return FALSE;

	if (!line.Write(hFile))
		return FALSE;

	if (!WriteString(hFile, ENDMARK))
		return FALSE;


	return TRUE;
}

BOOL Axis::Read(HANDLE hFile)
{
	std::list<SavableItem*> to_be_load;

	to_be_load.push_back(pLabel);
	to_be_load.push_back(pTicks);
	to_be_load.push_back(pDigits);
	to_be_load.push_back(pGridLines);

	if (ReadString(hFile) != BEGINMARK)
		return FALSE;

	for(std::list<SavableItem*>::iterator it=to_be_load.begin(); it!=to_be_load.end(); ++it)
	{
		try
		{
			if (!(*it)->Read(hFile))
				return FALSE;
		}
		catch (...)
		{
			return FALSE;
		}
	}

	DWORD dwRes;

	if (!ReadFile( hFile, &dwStyle, sizeof(dwStyle), &dwRes, NULL ) || dwRes != sizeof(dwStyle))
		return FALSE;

	if (!line.Read(hFile))
		return FALSE;

	if (ReadString(hFile) != ENDMARK)
		return FALSE;


	return TRUE;
}
