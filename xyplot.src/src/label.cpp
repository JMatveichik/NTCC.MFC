//#pragma warning( disable : 4786 )
#include "stdafx.h"
#include "label.h"
#include "exceptions.h"
#include "xyplotconst.h"
#include "main.h"
#include "rtfstring.h"

using namespace std;
using namespace xyplot;

static const char BEGINMARK[] = "[label]";
static const char ENDMARK[] = "[/label]";


Label::Label(XYPlot* parent)
{
	SetStyle(PI_AXIS_BOTTOM);
	m_enabled = TRUE;
	m_label = new RTFString(parent->hmod);
}

Label::~Label()
{
	delete m_label;
}

void Label::SetText(string text)
{
	if( text.empty() )
	{
		m_enabled = FALSE;
	}
	else
		m_enabled = TRUE;

	m_label->SetText(text);
}

void Label::SetRect( int left, int top, int right, int bottom )
{
	m_self.left		= left;
	m_self.top		= top;
	m_self.right	= right;
	m_self.bottom	= bottom;
}

SIZE Label::GetLabelSize() const
{
	SIZE size;
	m_label->GetDimensions(size);
	return size;
}

void Label::SetStyle( DWORD align )
{
	if( align == m_align )
		return;
	
	m_align = align;
	if( m_align == PI_AXIS_LEFT )
		m_label->SetOrientation( -90 );
	else if (m_align == PI_AXIS_RIGHT)
		m_label->SetOrientation( 90 );
}

const std::string& Label::GetText() const
{
	return m_label->GetText(); 
}

void Label::SetFont(const LOGFONT* const plf)
{
	m_label->SetFont(plf); 
} 

void Label::SetColor(COLORREF clr)
{
	m_label->SetColor(clr); 
} 

void Label::PreDraw( HDC hdc )
{
	return;
}

void Label::OnDraw( HDC hdc )
{
	if (!m_enabled)
		return;

	SIZE size;
	m_label->GetDimensions(size);

	if( m_align == PI_AXIS_BOTTOM )
	{
		m_label->Draw(hdc, m_self.left+(m_self.right-m_self.left-size.cx)/2, m_self.top);
	}
	else if( m_align == PI_AXIS_TOP)
	{
		m_label->Draw(hdc, m_self.left+(m_self.right-m_self.left-size.cx)/2, m_self.top);
	}
	else if( m_align == PI_AXIS_LEFT)
	{
		m_label->Draw(hdc, m_self.left, m_self.top+(m_self.bottom-m_self.top-size.cy)/2);
	}
	else //AXIS_RIGHT
	{
		m_label->Draw(hdc, m_self.right-2, m_self.top+(m_self.bottom-m_self.top-size.cy)/2);
	}
}

/*
void Label::OnDraw( HDC hdc )
{
	if (!m_enabled)
		return;

	HFONT oldfont = NULL;
	UINT uiFormat = 0;
	RECT r;

	EnhancedString cuttedLabel;
	string sfx = "...";

	if( m_align & (AXIS_BOTTOM|AXIS_TOP) )
	{
		m_label->CalcRect( hdc, r );
		
		if( (r.right - r.left) > (m_self.right-m_self.left) )
		{
			cuttedLabel = *m_label;
			cuttedLabel.RemoveBack( unsigned(sfx.length()) + 1 );
			cuttedLabel.Append( sfx );
			cuttedLabel.CalcRect( hdc, r );
		}
		
		while( (r.right - r.left) > (m_self.right-m_self.left) )
		{
			if( cuttedLabel.SegmentCount() <= 1 && cuttedLabel.LastSegmentLength() <= sfx.length() + 2 )
				break;
			
			cuttedLabel.RemoveBack( unsigned(sfx.length()) + 1 );
			cuttedLabel.Append( sfx );

			cuttedLabel.CalcRect( hdc, r );
		}

		if( cuttedLabel.Body().empty() )
			m_label->Draw( hdc, (m_self.right + m_self.left - r.right)/2, m_self.top, 0L );
		else
			cuttedLabel.Draw( hdc, (m_self.right + m_self.left - r.right)/2, m_self.top, 0L );
	}
	else
	{
		cuttedLabel.SetOrientation( 900 );
		m_label->SetOrientation( 900 );

		int x0 = m_self.left;
		int y0 = m_self.top + ( m_self.bottom - m_self.top )/2;
		
		m_label->CalcRect( hdc, r );

		if( abs(r.bottom - r.top) > (m_self.bottom-m_self.top) )
		{
			cuttedLabel = *m_label;
			cuttedLabel.RemoveBack( unsigned(sfx.length()) + 1 );
			cuttedLabel.Append( sfx );
			cuttedLabel.CalcRect( hdc, r );
		}

		while( abs(r.bottom - r.top) > (m_self.bottom-m_self.top) )
		{
			if( cuttedLabel.SegmentCount() <= 1 && cuttedLabel.LastSegmentLength() <= sfx.length() + 2 )
				break;

			cuttedLabel.RemoveBack( unsigned(sfx.length()) + 1 );
			cuttedLabel.Append( sfx );

			cuttedLabel.CalcRect( hdc, r );
		}

		if( cuttedLabel.Body().empty() )
			m_label->Draw( hdc, m_self.left, (m_self.top + m_self.bottom + abs(r.bottom-r.top))/2, 0L );
		else
			cuttedLabel.Draw( hdc, m_self.left, (m_self.top + m_self.bottom + abs(r.bottom-r.top))/2, 0L );
	}
}
*/

BOOL Label::Write(HANDLE hFile) const
{
	if (!WriteString(hFile, BEGINMARK))
		return FALSE;

	DWORD dwRes;
	if (!WriteFile( hFile, &m_enabled, sizeof(m_enabled), &dwRes, NULL ) || dwRes != sizeof(m_enabled))
		return FALSE;

	if(!m_label->Write(hFile))
		return FALSE;	

	if (!WriteString(hFile, ENDMARK))
		return FALSE;

	return TRUE;
}

BOOL Label::Read(HANDLE hFile)
{
	if (ReadString(hFile) != BEGINMARK)
		return FALSE;

	DWORD dwRes;
	if (!ReadFile( hFile, &m_enabled, sizeof(m_enabled), &dwRes, NULL ) || dwRes != sizeof(m_enabled))
		return FALSE;	

	if (!m_label->Read(hFile))
		return FALSE;

	if (ReadString(hFile) != ENDMARK)
		return FALSE;

	return TRUE;
}