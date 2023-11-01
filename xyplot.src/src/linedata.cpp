//#pragma warning( disable : 4786 )

#include "stdafx.h"
#include "linedata.h"

using namespace xyplot;

const DWORD dwMarker = 0xDADA;

std::string	LineData::DASHTEMPLATE = "35 20";
std::string	LineData::DOTTEMPLATE = "2 2";
std::string	LineData::DASHDOTTEMPLATE = "35 20 7 20";
std::string	LineData::DASHDOTDOTTEMPLATE = "35 20 7 15 7 20";

template<class T>
void tokenize(T const & input, T const & delimiters, std::vector<T>& tokens)
{
	using namespace std;

	T::size_type last_pos = 0;
	T::size_type pos = 0; 

	while(true)
	{
		pos = input.find_first_of(delimiters, last_pos);
		if( pos == T::npos )
		{
			tokens.push_back(input.substr(last_pos));
			break;
		}
		else
		{
			tokens.push_back(input.substr(last_pos, pos - last_pos));
			last_pos = pos + 1;
		} 
	}
}






LineData::LineData() :
m_pen(NULL), 
m_type(PLS_SOLID), 
m_width(1), 
m_color(0x000000), 
m_endCaps(ROUND), 
m_template("")
{	
	CreatePen();
}

LineData::LineData(COLORREF clr, int nWidth,  int nType, 
				   int nEndCaps/* = ROUND */,  std::string strTemplate/* = ""*/) :
m_pen(NULL), 
m_type(nType),
m_width(nWidth), 
m_color(clr),
m_endCaps(nEndCaps), 
m_template(strTemplate)
{
	CreatePen();
}


const LineData& LineData::operator= (const LineData &rhs )
{
	if (this == &rhs)
		return *this;

	m_color = rhs.m_color;
	m_type = rhs.m_type;
	m_width = rhs.m_width;
	m_template = rhs.m_template;
	m_endCaps = rhs.m_endCaps;
	m_pen = NULL;
	UpdateLineTemplate();
	CreatePen();

	return *this;
}

LineData::~LineData()
{
	DeleteObject( m_pen );
}

BOOL LineData::Write(HANDLE hFile) const
{
	DWORD dwRes;	

	if (!WriteFile( hFile, &dwMarker, sizeof(dwMarker), &dwRes, NULL ) || dwRes != sizeof(dwMarker))
		return FALSE;

	if (!WriteFile( hFile, &m_type, sizeof(m_type), &dwRes, NULL ) || dwRes != sizeof(m_type))
		return FALSE;

	if (!WriteFile( hFile, &m_width, sizeof(m_width), &dwRes, NULL ) || dwRes != sizeof(m_width))
		return FALSE;

	if (!WriteFile( hFile, &m_endCaps, sizeof(m_endCaps), &dwRes, NULL ) || dwRes != sizeof(m_endCaps))
		return FALSE;

	if (!WriteFile( hFile, &m_color, sizeof(m_color), &dwRes, NULL ) || dwRes != sizeof(m_color))
		return FALSE;

	int nSize = int(m_template.length());

	if (!WriteFile(hFile, &nSize, sizeof(nSize), &dwRes, NULL) || dwRes != sizeof(nSize))
		return FALSE;

	if (!WriteFile(hFile, m_template.c_str(), DWORD(m_template.length()), &dwRes, NULL) || dwRes != m_template.length())
		return FALSE;	

	if (!WriteFile( hFile, &dwMarker, sizeof(dwMarker), &dwRes, NULL ) || dwRes != sizeof(dwMarker))
		return FALSE;

	return TRUE;
}
BOOL LineData::Read(HANDLE hFile)
{
	DWORD dwRes;
	DWORD dwMark;

	if (!ReadFile( hFile, &dwMark, sizeof(dwMark), &dwRes, NULL ) || dwRes != sizeof(dwMark))
		return FALSE;
	if (dwMarker != dwMark)
		return FALSE;

	if (!ReadFile( hFile, &m_type, sizeof(m_type), &dwRes, NULL ) || dwRes != sizeof(m_type))
		return FALSE;
	SetType(m_type);

	if (!ReadFile( hFile, &m_width, sizeof(m_width), &dwRes, NULL ) || dwRes != sizeof(m_width))
		return FALSE;
	SetWidth(m_width);

	if (!ReadFile( hFile, &m_endCaps, sizeof(m_endCaps), &dwRes, NULL ) || dwRes != sizeof(m_endCaps))
		return FALSE;
	SetEndCaps(m_endCaps);

	if (!ReadFile( hFile, &m_color, sizeof(m_color), &dwRes, NULL ) || dwRes != sizeof(m_color))
		return FALSE;
	SetColor(m_color);

	int nSize;

	if (!ReadFile(hFile, &nSize, sizeof(nSize), &dwRes, NULL) || dwRes != sizeof(nSize))
		return FALSE;

	const int MAX_SIZE = 1024;
	if (nSize > MAX_SIZE)
		return FALSE;

	char* pStr;
	try{
		pStr = new char[nSize+1];
	}
	catch (...) 
	{
		return FALSE; 
	}

	if (!ReadFile(hFile, pStr, nSize, &dwRes, NULL ) || int(dwRes) != nSize)
	{
		delete [] pStr ;
		return FALSE;
	}
	pStr[nSize] = 0;

	SetTemplate(pStr);	
	delete [] pStr;

	if (!ReadFile( hFile, &dwMark, sizeof(dwMark), &dwRes, NULL ) || dwRes != sizeof(dwMark))
		return FALSE;
	if (dwMarker != dwMark)
		return FALSE;

	return TRUE;
}

int LineData::SetType( int nType )
{
	if (nType == PLS_CUSTOM && m_template.empty())
		return -1;

	int prevType = m_type;
	
	m_type = nType;
	UpdateLineTemplate();
	CreatePen();

	return prevType;
}

void LineData::SetTemplate( std::string str )
{
	m_template = str;

	if( m_type == PLS_CUSTOM )
		CreatePen();
}

void LineData::UpdateLineTemplate()
{
	switch( m_type )
	{
	case PLS_DASH:
		m_template = DASHTEMPLATE;
		break;
	case PLS_DOT:
		m_template = DOTTEMPLATE;
		break;
	case PLS_DASHDOT:
		m_template = DASHDOTTEMPLATE;
		break;
	case PLS_DASHDOTDOT:
		m_template = DASHDOTDOTTEMPLATE;
		break;
	}
}

void LineData::CreatePen()
{
	LOGBRUSH lb;
	lb.lbStyle = BS_SOLID;
	lb.lbHatch = 0;
	lb.lbColor = m_color;

	if( m_type == PLS_SOLID)
	{
		UINT nStyle = PS_SOLID|PS_GEOMETRIC|PS_JOIN_ROUND;
		switch( m_endCaps )
		{
		case ROUND:
			nStyle |= PS_ENDCAP_ROUND;
			break;
		case FLAT:
			nStyle |= PS_ENDCAP_FLAT;
			break;
		case SQUARE:
			nStyle |= PS_ENDCAP_SQUARE;
			break;
		}

		HPEN hpen = ExtCreatePen( nStyle, m_width, &lb, 0, NULL );
		if( hpen )
		{
			DeleteObject( m_pen );
			m_pen = hpen;
		}
	}
	else if (m_type == PLS_INVISIBLE)
	{
		UINT nStyle = PS_NULL;
		HPEN hpen = ExtCreatePen( nStyle, 0, &lb, 0, NULL);
		if( hpen )
		{
			DeleteObject( m_pen );
			m_pen = hpen;
		}
	}
	else
	{
		int nStyleCount = 0;
		int nLen = int(m_template.length());
		for( int i=0; i<nLen-1; i++)
		{
			if( m_template[i] == ' ' )
				nStyleCount++;
		}

		std::vector<DWORD> dwStyle;

		std::vector<std::string> tokens;
		tokenize<std::string>(m_template, " ", tokens);

		for(std::vector<std::string>::const_iterator it=tokens.begin(); it!=tokens.end(); ++it)
			dwStyle.push_back((DWORD)atoi(&it->at(0)));

		UINT nStyle = PS_GEOMETRIC|PS_USERSTYLE;
		
		switch (m_endCaps)
		{
		case ROUND:
			nStyle |= PS_ENDCAP_ROUND;
			break;
		case FLAT:
			nStyle |= PS_ENDCAP_FLAT;
			break;
		case SQUARE:
			nStyle |= PS_ENDCAP_SQUARE;
			break;
		}

		HPEN hpen = ExtCreatePen( nStyle, m_width, &lb, DWORD(dwStyle.size()), &dwStyle[0]);
		if( hpen )
		{
			DeleteObject( m_pen );
			m_pen = hpen;
		}
	}
}