#include "stdafx.h"
#include "border.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace xyplot;

static const char BEGINMARK[] = "border";
static const char ENDMARK[] = "/border";

Border::Border()
{
	m_enabled = TRUE;
	m_line.SetWidth( 1 );
	m_line.SetEndCaps( LineData::SQUARE );
	m_line.SetColor( ::GetSysColor(COLOR_WINDOWTEXT) );
	m_line.SetType(PLS_SOLID);
}

Border::~Border()
{
}

void Border::SetLineData(COLORREF clr, int nWidth, int nType, std::string  templ)
{
	m_line.SetColor(clr);
	m_line.SetWidth(nWidth);
	m_line.SetType(nType);
	m_line.SetTemplate(templ);
}

void Border::OnDraw( HDC hdc )
{
	if( m_enabled )
	{
		int half = m_line.GetWidth() >> 1;
		int rest = half*2 == m_line.GetWidth() ? 0 : 1;
		
		HPEN oldpen = (HPEN) SelectObject( hdc, (HPEN) m_line );
		MoveToEx( hdc, m_self.left + half, m_self.top + half, NULL );
		LineTo( hdc, m_self.left + half, m_self.bottom - half - rest );
		LineTo( hdc, m_self.right - half - rest, m_self.bottom - half - rest);
		LineTo( hdc, m_self.right - half - rest, m_self.top + half );
		LineTo( hdc, m_self.left + half, m_self.top + half );
		SelectObject( hdc, oldpen);
	}
}

BOOL Border::Write(HANDLE hFile) const
{
	if (!WriteString(hFile, BEGINMARK))
		return FALSE;

	DWORD dwRes;
	if (!WriteFile(hFile, &m_enabled, sizeof(m_enabled), &dwRes, NULL) || dwRes != sizeof(m_enabled))
		return FALSE;

	if (!m_line.Write(hFile))
		return FALSE;

	if (!WriteString(hFile, ENDMARK))
		return FALSE;
	
	return TRUE;
}

BOOL Border::Read(HANDLE hFile)
{
	try {
		if (ReadString(hFile) != BEGINMARK)
			return FALSE;

		DWORD dwRes;
		if (!ReadFile(hFile, &m_enabled, sizeof(m_enabled), &dwRes, NULL) || dwRes != sizeof(m_enabled))
			return FALSE;

		if (!m_line.Read(hFile))
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
