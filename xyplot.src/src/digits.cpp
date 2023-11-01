//#pragma warning( disable : 4786 )
#include "stdafx.h"
#include "digits.h"
#include "exceptions.h"
#include "fontmanager.h"
#include "global.h"
#include "axis.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static const char BEGINMARK[] = "[digits]";
static const char ENDMARK[] = "[/digits]";

using namespace std;
using namespace xyplot;

Digits::Digits(XYPlot* parent, Axis* parentAxis)
{
	this->parentAxis = parentAxis;

	m_enabledMajor = TRUE;
	m_enabledMinor = TRUE;

	majorTemplate = "%1.1f";
	minorTemplate = "%1.1f";

	HDC hDC = ::GetDC(GetDesktopWindow());

	FontManager& fm = FontManager::Instance();
	LOGFONT lf;

	lf.lfHeight = -MulDiv(10, GetDeviceCaps(hDC, LOGPIXELSY), 72);
	lf.lfWidth = 0;
	lf.lfEscapement = lf.lfOrientation = 0;
	lf.lfWeight = FW_BOLD; 
	lf.lfItalic = FALSE;
	lf.lfUnderline = FALSE;
	lf.lfStrikeOut = FALSE;
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfOutPrecision = OUT_TT_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = DEFAULT_QUALITY;
	lf.lfPitchAndFamily = DEFAULT_PITCH|FF_DONTCARE;
	strcpy_s(lf.lfFaceName, LF_FACESIZE, "Tahoma");
	nFontMinor = nFontMajor = fm.RegisterFont(lf);

	lf.lfWeight = FW_NORMAL; 
	lf.lfHeight = -MulDiv(8, GetDeviceCaps(hDC, LOGPIXELSY), 72);
	nFontMinor = fm.RegisterFont(lf);
	
	crMajor = crMinor = 0L;

	m_fLower = 0.0;
	m_fUpper = 1.0;

	offsetFromTicks = 3;

	::ReleaseDC(GetDesktopWindow(), hDC);
}

Digits::~Digits()
{
	Clear();
}

void Digits::SetColor( COLORREF color, BOOL bMajor)
{
	if (bMajor)
		crMajor = color;
	else
		crMinor = color;
}

COLORREF Digits::GetColor(BOOL bMajor) const
{
	if (bMajor)
		return crMajor;
	else
		return crMinor;
}

void Digits::SetRect( int left, int top, int right, int bottom )
{
	m_self.left = left;
	m_self.top = top;
	m_self.right = right;
	m_self.bottom = bottom;
}

int Digits::CalcRect(HDC hDC, RECT& rc) const
{
	int nOffset  = 2;
	int nMaxSize = 0;
	string s;
	RECT r = {0, 0, 0, 0};
	char str[256];

	if ( !m_enabledMinor && !m_enabledMajor)
		return nOffset;
	
	if ( m_enabledMajor )
	{
		std::vector <double> majVal = parentAxis->GetMajorTicksValues();

		for( unsigned i = 0; i < majVal.size(); i++ )
		{
			Global::SafeDouble2Ascii(str, 256, majorTemplate.c_str(), majVal[i]);
			s = str;		

			HFONT hFont = (HFONT)::SelectObject(hDC, FontManager::Instance().GetFont(nFontMajor));
			::SetRect(&r, 0, 0, 0, 0);
			::DrawText(hDC, s.c_str(), int(s.length()), &r, DT_CALCRECT);
			::SelectObject(hDC, hFont);
			
			if (nMaxSize < abs(r.right - r.left))
			{
				nMaxSize = abs(r.right - r.left);
				CopyRect(&rc, &r);
			}
		}
	}

	if ( m_enabledMinor )
	{
		std::vector <double> minVal = parentAxis->GetMinorTicksValues();

		for( unsigned i = 0; i < minVal.size(); i++ )
		{
			Global::SafeDouble2Ascii(str, 256, minorTemplate.c_str(), minVal[i]);
			s = str;		

			HFONT hFont = (HFONT)::SelectObject(hDC, FontManager::Instance().GetFont(nFontMajor));
			::SetRect(&r, 0, 0, 0, 0);
			::DrawText(hDC, s.c_str(), int(s.length()), &r, DT_CALCRECT);
			::SelectObject(hDC, hFont);

			if (nMaxSize < abs(r.right - r.left))
			{
				nMaxSize = abs(r.right - r.left);
				CopyRect(&rc, &r);
			}
		}
	}

	int size = abs(r.top - rc.bottom) / 4;
	nOffset  = nOffset >  size ? nOffset : size;

	return nOffset;
}

/*
void Digits::SetFontAttributes(unsigned long lFontStyle, BOOL MajorDigits )
{
	if (MajorDigits)
	{
		m_majorEstr->EnableProperty( ESP_BOLD, (lFontStyle & ESP_BOLD) == ESP_BOLD );
		m_majorEstr->EnableProperty( ESP_ITALIC, (lFontStyle & ESP_ITALIC) == ESP_ITALIC );
		m_majorEstr->EnableProperty( ESP_UNDERLINE, (lFontStyle & ESP_UNDERLINE) == ESP_UNDERLINE );
	}
	else
	{
		m_minorEstr->EnableProperty( ESP_BOLD, (lFontStyle & ESP_BOLD) == ESP_BOLD );
		m_minorEstr->EnableProperty( ESP_ITALIC, (lFontStyle & ESP_ITALIC) == ESP_ITALIC );
		m_minorEstr->EnableProperty( ESP_UNDERLINE, (lFontStyle & ESP_UNDERLINE) == ESP_UNDERLINE );
	}	
}

unsigned long Digits::GetFontAttributes( BOOL MajorDigits) const
{
	unsigned long lFontStyle = ESP_NORMAL;

	if ( MajorDigits )
	{
		if (m_majorEstr->IsPropertyEnabled( ESP_BOLD ) )
			lFontStyle |= ESP_BOLD;

		if (m_majorEstr->IsPropertyEnabled( ESP_ITALIC ))
			lFontStyle |= ESP_ITALIC;
		
		if (m_majorEstr->IsPropertyEnabled( ESP_UNDERLINE ))
			lFontStyle |= ESP_UNDERLINE;
	}
	else
	{
		if (m_minorEstr->IsPropertyEnabled( ESP_BOLD ) )
			lFontStyle |= ESP_BOLD;

		if (m_minorEstr->IsPropertyEnabled( ESP_ITALIC ))
			lFontStyle |= ESP_ITALIC;

		if (m_minorEstr->IsPropertyEnabled( ESP_UNDERLINE ))
			lFontStyle |= ESP_UNDERLINE;
	}
	
	return lFontStyle;
}
*/

void Digits::SetFont(const LOGFONT& lf, BOOL bMajor)
{
	bMajor ? nFontMajor : nFontMinor = FontManager::Instance().RegisterFont(lf);
}

void Digits::GetFont(LOGFONT& lf, BOOL bMajor) const
{
	HFONT hFont = FontManager::Instance().GetFont(bMajor ? nFontMajor : nFontMinor);
	::GetObject(hFont, sizeof(LOGFONT), &lf);
}

void Digits::SetTemplate(std::string strTemplate, BOOL bMajor) 
{ 
	if (bMajor)
		majorTemplate = strTemplate; 
	else
		minorTemplate = strTemplate; 
}

const std::string& Digits::GetTemplate(BOOL bMajor) const 
{
	if (bMajor)
		return majorTemplate;
	else
		return minorTemplate; 
	
}

void Digits::PushBackMajor( std::string s )
{
	size_t first = s.find_first_not_of(" ");
	string dig;

	if (first != string::npos)
		dig.assign(s, first, s.length() - first);
	else
		dig = s;

	m_majorDigitsVector.push_back( dig );
}

void Digits::PushBackMinor( std::string s )
{
	size_t first = s.find_first_not_of(" ");
	string dig;

	if (first != string::npos)
		dig.assign(s, first, s.length() - first);
	else
		dig = s;

	m_minorDigitsVector.push_back( dig );
}

void Digits::Clear()
{
	m_majorDigitsVector.clear();
	m_minorDigitsVector.clear();
}

int Digits::MaxItemLength( HDC hdc, double fmin, double fmax ) const
{	
	RECT rectMin, rectMax;
	int res = 0;

	char str[128];
	bool bSuccess = Global::SafeDouble2Ascii(str, 128, majorTemplate.c_str(), fmin);

	HFONT hFont = (HFONT)::SelectObject(hdc, FontManager::Instance().GetFont(nFontMajor));
	::SetRect(&rectMin, 0, 0, 0, 0);
	::DrawText(hdc, str, int(strlen(str)), &rectMin, DT_CALCRECT);
	::SelectObject(hdc, hFont);

	int minDifference = rectMin.right - rectMin.left;

	bSuccess = Global::SafeDouble2Ascii(str, 128, majorTemplate.c_str(), fmax);
	::SelectObject(hdc, FontManager::Instance().GetFont(nFontMajor));
	::SetRect(&rectMax, 0, 0, 0, 0);
	::DrawText(hdc, str, int(strlen(str)), &rectMax, DT_CALCRECT);
	::SelectObject(hdc, hFont);
	int maxDifference = rectMax.right - rectMax.left;
	
	if( minDifference > maxDifference )
		res = minDifference;
	else
		res = maxDifference;

	return res;
}

void Digits::Beautify( string& s )
{
	while (s[0] == ' ') s.erase(0, 1);

	if( s.length() < 4 )
		return;

	string::size_type index = s.find( "e" );
	
	if( index == string::npos )
		return;

	// so far i indeces "e" -- shift it once
	// *
	index += 2;
	
	// remove one "0"
	// *
	if( s[index] == '0' )
		s.erase( index, 1 );

}

void Digits::PreDraw( HDC hdc )
{	
	Clear();

	std::vector <double> majVal = parentAxis->GetMajorTicksValues();
	std::vector <double> minVal = parentAxis->GetMinorTicksValues();

	const size_t BUFFERSIZE = 256;
	char str[BUFFERSIZE];

	if ( !parentAxis->IsTimeHistory() )
	{
		for (size_t i = 0; i < majVal.size(); i++)
		{
			Global::SafeDouble2Ascii(str, BUFFERSIZE, majorTemplate.c_str(), majVal[i]);

			string s = str;
			Beautify( s );

			PushBackMajor( s );
		}

		for (size_t i = 0; i < minVal.size(); i++)
		{
			Global::SafeDouble2Ascii(str, BUFFERSIZE, minorTemplate.c_str(), minVal[i]);

			string s = str;
			Beautify( s );

			PushBackMinor( s );
		}
	}
	else
	{
		for (size_t i = 0; i < majVal.size(); i++)
		{
			time_t t = time_t(majVal[i]);
			if (ctime_s(str, BUFFERSIZE, &t))
				throw XYPlotException(string(__FUNCTION__) + " - axis does not contain valid time data");
			string s(&str[11], &str[16]);
			PushBackMajor( s );
		}

		for (size_t i = 0; i < minVal.size(); i++)
		{
			time_t t = time_t(minVal[i]);
			if (ctime_s(str, BUFFERSIZE, &t))
				throw XYPlotException(string(__FUNCTION__) + " - axis does not contain valid time data");
			string s(&str[11], &str[16]); 
			PushBackMinor( s );
		}
	}
}

void Digits::OnDraw( HDC hdc )
{
	if( !m_enabledMajor && !m_enabledMinor )
		return;

	int id = parentAxis->GetID();
	int offset = parentAxis->GetTicksOffset();

	if ( m_enabledMajor )
	{
		HFONT hFont = (HFONT)::SelectObject(hdc, FontManager::Instance().GetFont(nFontMajor));
		::SetTextColor(hdc, crMajor);

		const std::vector<POINT>& majorTicksPos = parentAxis->GetMajorTicksPos();
		for (unsigned i = 0; i < m_majorDigitsVector.size(); i++)
		{
			POINT  pt	  = majorTicksPos.at(i);
			string strDig = m_majorDigitsVector.at(i);
			RECT   rcDig;

			::SetRect(&rcDig, 0, 0, 0, 0);
			::DrawText(hdc, strDig.c_str(), int(strDig.length()), &rcDig, DT_CALCRECT);

			int Width = rcDig.right - rcDig.left;
			int Height = rcDig.bottom - rcDig.top;

			if( id == PI_AXIS_TOP || id == PI_AXIS_BOTTOM )
			{
				if (id == PI_AXIS_BOTTOM)
					pt.y = pt.y + offset + offsetFromTicks; //Height = 0 not allow;
				else
					pt.y = pt.y -  Height - offset -  offsetFromTicks;

				pt.x -= int( Width / 2.0 );
				
				::OffsetRect(&rcDig, pt.x, pt.y);
				::DrawText(hdc, strDig.c_str(), int(strDig.length()), &rcDig, DT_CENTER);
			}
			else
			{
				if (id == PI_AXIS_LEFT)
					pt.x = pt.x - Width - offset - offsetFromTicks;
				else
					pt.x = pt.x + offset + offsetFromTicks; //Width = 0 not allow;

				pt.y -= int( Height / 2.0 );				
				
				::OffsetRect(&rcDig, pt.x, pt.y);
				::DrawText(hdc, strDig.c_str(), int(strDig.length()), &rcDig, DT_CENTER);
			}	
		}
		::SelectObject(hdc, hFont);
	}

	if ( m_enabledMinor )
	{
		HFONT hFont = (HFONT)::SelectObject(hdc, FontManager::Instance().GetFont(nFontMinor));
		::SetTextColor(hdc, crMinor);

		const std::vector<POINT>& minorTicksPos = parentAxis->GetMinorTicksPos();
		for (unsigned i = 0; i < m_minorDigitsVector.size(); i++)
		{
			POINT pt = minorTicksPos.at(i);
			string strDig = m_minorDigitsVector.at(i);
			RECT   rcDig;

			::SetRect(&rcDig, 0, 0, 0, 0);
			::DrawText(hdc, strDig.c_str(), int(strDig.length()), &rcDig, DT_CALCRECT);

			int Width = rcDig.right - rcDig.left;
			int Height = rcDig.bottom - rcDig.top;;

			if( id == PI_AXIS_TOP || id == PI_AXIS_BOTTOM )
			{
				if (id == PI_AXIS_BOTTOM)
					pt.y = pt.y + offset + offsetFromTicks; //Height = 0 not allow;
				else
					pt.y = pt.y -  Height - offset -  offsetFromTicks;

				pt.x -= int( Width / 2.0 );

				::OffsetRect(&rcDig, pt.x, pt.y);
				::DrawText(hdc, strDig.c_str(), int(strDig.length()), &rcDig, DT_CENTER);
			}
			else
			{
				if (id == PI_AXIS_LEFT)
					pt.x = pt.x - Width - offset - offsetFromTicks;
				else
					pt.x = pt.x + offset + offsetFromTicks; //Width = 0 not allow;

				pt.y -= int( Height / 2.0 );

				::OffsetRect(&rcDig, pt.x, pt.y);
				::DrawText(hdc, strDig.c_str(), int(strDig.length()), &rcDig, DT_CENTER);
			}
		}
		::SelectObject(hdc, hFont);
	}

}

void Digits::OptimizeTicks( double &fmin, double &fmax, int &majorCount, int &minorCount )
{		
	double fMin = log10( fmin );
	double fMax = log10( fmax );

	if( fmin == 0 )
	{
		fMin = fmin = -5.0;
		fmin = pow( 10.0, -5 );
	}
	if( fmax == 0 )
	{
		fMax = fmax = -5.0;
		fmax = pow( 10.0, -5 );
	}
	
	int nNum = (int)fMax - (int)fMin;
	majorCount = nNum + 1;

	if( m_enabledMinor )
		minorCount = 8;
}

BOOL Digits::Write(HANDLE hFile) const
{
	DWORD dwRes;

	if (!WriteString(hFile, BEGINMARK))
		return FALSE;	

	if (!WriteFile( hFile, &m_enabledMajor, sizeof(m_enabledMajor), &dwRes, NULL ) || dwRes != sizeof(m_enabledMajor))
		return FALSE;

	if (!WriteFile( hFile, &m_enabledMinor, sizeof(m_enabledMinor), &dwRes, NULL ) || dwRes != sizeof(m_enabledMinor))
		return FALSE;

	if (!WriteString(hFile, majorTemplate))
		return FALSE;

	if (!WriteString(hFile, minorTemplate))
		return FALSE;

	LOGFONT lf;

	GetFont(lf, TRUE);
	if (!WriteFile( hFile, &lf, sizeof(LOGFONT), &dwRes, NULL ) || dwRes != sizeof(LOGFONT))
		return FALSE;

	GetFont(lf, FALSE);
	if (!WriteFile( hFile, &lf, sizeof(LOGFONT), &dwRes, NULL ) || dwRes != sizeof(LOGFONT))
		return FALSE;

	if (!WriteFile( hFile, &crMajor, sizeof(crMajor), &dwRes, NULL ) || dwRes != sizeof(crMajor))
		return FALSE;

	if (!WriteFile( hFile, &crMinor, sizeof(crMinor), &dwRes, NULL ) || dwRes != sizeof(crMinor))
		return FALSE;

	if (!WriteString(hFile, ENDMARK))
		return FALSE;

	return TRUE;
}
BOOL Digits::Read(HANDLE hFile)
{
	try {
		if (ReadString(hFile) != BEGINMARK)
			return FALSE;

		DWORD dwRes;
		if (!ReadFile( hFile, &m_enabledMajor, sizeof(m_enabledMajor), &dwRes, NULL ) || dwRes != sizeof(m_enabledMajor))
			return FALSE;

		if (!ReadFile( hFile, &m_enabledMinor, sizeof(m_enabledMinor), &dwRes, NULL ) || dwRes != sizeof(m_enabledMinor))
			return FALSE;

		majorTemplate = ReadString(hFile);

		minorTemplate = ReadString(hFile);

		LOGFONT lf;
		if (!ReadFile( hFile, &lf, sizeof(LOGFONT), &dwRes, NULL ) || dwRes != sizeof(LOGFONT))
			return FALSE;
		nFontMajor = FontManager::Instance().RegisterFont(lf);

		if (!ReadFile( hFile, &lf, sizeof(LOGFONT), &dwRes, NULL ) || dwRes != sizeof(LOGFONT))
			return FALSE;
		nFontMinor = FontManager::Instance().RegisterFont(lf);

		if (!ReadFile( hFile, &crMajor, sizeof(crMajor), &dwRes, NULL ) || dwRes != sizeof(crMajor))
			return FALSE;

		if (!ReadFile( hFile, &crMinor, sizeof(crMinor), &dwRes, NULL ) || dwRes != sizeof(crMinor))
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