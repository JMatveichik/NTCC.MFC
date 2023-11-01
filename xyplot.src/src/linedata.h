#ifndef __LINEDATA_H_INCLUDED__
#define __LINEDATA_H_INCLUDED__

#include <string>
#include ".\xyplotconst.h"

class LineData
{
public:
//	static enum { SOLID, DASH, DOT, DASHDOT, DASHDOTDOT, CUSTOM };
	enum { ROUND, FLAT, SQUARE };

	LineData();
	LineData(COLORREF clr, 
			int nWidth, 
	   	    int nType, 
			int nEndCaps = ROUND , 
			std::string strTemplate = "");

	LineData(const LineData& rhs) { *this = rhs; };
	~LineData();

	const LineData& operator=(const LineData& rhs);

	void SetColor( COLORREF color ) { m_color = color; CreatePen();	}
	COLORREF GetColor() const { return m_color;	}

	void SetWidth( int nWidth )	{ m_width = nWidth; CreatePen(); }
	int	GetWidth() const { return m_width; }

	int SetType( int nType );
	int	GetType() const { return m_type; }
	
	void SetEndCaps( int nEndCaps ) { m_endCaps = nEndCaps; CreatePen(); }
	int	GetEndCaps() const { return m_endCaps; }

	void SetTemplate( std::string strTemplate);
	std::string& GetTemplate() { return m_template; }

	operator HPEN () { return m_pen; }
	HPEN GetPen() { return m_pen; }

	BOOL Write(HANDLE hFile) const;
	BOOL Read(HANDLE hFile);

protected:
	int		m_type;
	int		m_width;
	int		m_endCaps;
	COLORREF m_color;
	std::string	m_template;
	HPEN m_pen;

	static std::string DASHTEMPLATE;
	static std::string DOTTEMPLATE;
	static std::string DASHDOTTEMPLATE;
	static std::string DASHDOTDOTTEMPLATE;

	void UpdateLineTemplate();
	void CreatePen();
};


#endif