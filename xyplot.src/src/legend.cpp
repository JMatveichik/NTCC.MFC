//#pragma warning( disable : 4786 )
#include "stdafx.h"
#include "legend.h"
#include "exceptions.h"
#include "linedata.h"
#include "main.h"
#include "rtfstring.h"

using namespace std;
using namespace xyplot;

static const char BEGINMARK[] = "[legend]";
static const char ENDMARK[] = "[/legend]";

Legend::Legend(XYPlot* parent) : plot (*parent)
{
	m_enabled = TRUE;
	m_hidden = FALSE;
	m_align = PLP_BOTTOM;
	m_cellWidth = 0;
	m_cellHeight = 0;
	m_nLinesCount = 0;
	m_basicEstr = new RTFString(parent->hmod);
	m_lineLength = 30;
	m_LegendOffset = 8;

	backGround.SetFillColors(RGB(54, 50, 144), RGB(255, 255, 255) );
	backGround.SetOpacity(50.0, 20.0);

	backGround.Enable(FALSE);

	border.GetLineData().SetColor( 0L );
	border.GetLineData().SetWidth( 1 );	

	border.Enable(TRUE);
}

Legend::~Legend()
{
	if( m_basicEstr )
		delete m_basicEstr;

	Clear();
}

void Legend::Clear()
{
	profiles.clear();
}

void Legend::SetRect( int left, int top, int right, int bottom )
{
	m_self.left = left;
	m_self.top = top;
	m_self.right = right;
	m_self.bottom = bottom;
}

void Legend::SetFont(const LOGFONT* plf)
{
	m_basicEstr->SetFont(plf);
}

void Legend::PreDraw( HDC hdc, RECT *ptotal)
{

	const ProfileMap& profileMap = plot.GetProfileMap();

	Clear();
	
	int nLegendWidth = 0;
	int nLegendHeight = 0;
	int nWidth = 0;

	if( !m_enabled )
	{
		m_align = PLP_BOTTOM;
		SetRect( ptotal->left, ptotal->bottom, ptotal->right, ptotal->bottom );
	}
	else
	{
		m_cellWidth = m_cellHeight = 0;

		profiles.clear();

		ProfileMapConstIterator it;
		for( it = profileMap.begin(); it != profileMap.end(); ++it )
		{ 		
			Profile* pProf = (*it).second;

			if ( !pProf->IsShowInLegend() || pProf->GetName().empty())
				continue;

			pair<std::set<Profile*, profile_less>::iterator, bool> res = profiles.insert(pProf);

			m_basicEstr->SetText((*it).second->GetName());
			SIZE size;
			m_basicEstr->GetDimensions(size);
			size.cx = abs(size.cx);
			size.cy = abs(size.cy);
			
			if( size.cx + m_lineLength > m_cellWidth )
				m_cellWidth = size.cx + m_lineLength;
			
			if( size.cy > m_cellHeight )
				m_cellHeight = size.cy;
		}

		if( m_cellWidth > (int)( (ptotal->right - ptotal->left) / 2 ) )
		{
			m_hidden = TRUE;
			SetRect( ptotal->left, ptotal->bottom, ptotal->left, ptotal->bottom );
			return;
		}
		
		m_hidden = FALSE;

		if( m_align == PLP_BOTTOM )
		{
			nLegendWidth = ptotal->right - ptotal->left - 2 * m_LegendOffset;
			int nRowSize = nLegendWidth / m_cellWidth;
			m_nLinesCount = int(profiles.size() / nRowSize) + 1;
			nLegendHeight = m_nLinesCount * ( m_cellHeight + 1 ) + 2 * m_LegendOffset;
			SetRect( ptotal->left, ptotal->bottom - nLegendHeight, ptotal->right, ptotal->bottom );
		}
		else // align = Right | align = Left
		{
			int nRowSize = 1;
			m_nLinesCount = (int)profiles.size();
			nLegendHeight = m_nLinesCount * m_cellHeight + 2 * m_LegendOffset;
			nLegendWidth = m_cellWidth + 2 * m_LegendOffset;
			if( m_align == PLP_LEFT )
				SetRect( ptotal->left, ptotal->top, ptotal->left + nLegendWidth, ptotal->bottom );
			else
				SetRect( ptotal->right - nLegendWidth, ptotal->top, ptotal->right, ptotal->bottom );
		}
	}
	backGround.SetRect(m_self);
	border.SetRect(m_self);
}

void Legend::OnDraw( HDC hdc )
{	
	const ProfileMap& profileMap = plot.GetProfileMap();

	if( m_enabled && !m_hidden )
	{
		//Рисуем  фон
		backGround.OnDraw(hdc);

		//Рисуем границу
		border.OnDraw(hdc);

		unsigned nTotalHeight = m_nLinesCount * m_cellHeight;
		unsigned nTotalRows =  m_nLinesCount;
		unsigned nTotalColumns =  (m_self.right - m_self.left - 2 * m_LegendOffset) / m_cellWidth;

		HPEN oldpen = NULL;
		int nRow = 0;
		int nColumn = 0;

		for (set<Profile*, profile_less>::iterator it=profiles.begin(); it!=profiles.end(); ++it)
		{
			if ((*it)->GetName().empty())
				continue;

			// Calculate origin
			int x = m_self.left + m_LegendOffset + m_lineLength + m_cellWidth * nColumn;
			int y = m_self.top + m_LegendOffset + int(0.5*(m_self.bottom - m_self.top - nTotalHeight)) + (nRow+1) * m_cellHeight;

			// Draw text
			m_basicEstr->SetText((*it)->GetName());
			m_basicEstr->Draw( hdc, x, y - int(m_cellHeight * 0.5));
			
			if( !oldpen )
				oldpen = (HPEN) SelectObject( hdc, (*it)->GetLineData() );				
			else
				SelectObject( hdc, (*it)->GetLineData() );
				
			// Draw mark
			POINT pt;
			pt.x = x - (m_lineLength >> 1);
			pt.y = y;
			(*it)->GetMarks().OnDraw(hdc, &pt, 1);

			// Draw line sample
			MoveToEx( hdc, x, y, 0);
			LineTo( hdc, x - m_lineLength, y);

			if (++nColumn == nTotalColumns)
			{
				nColumn = 0;
				nRow++;
			}
		}
		SelectObject( hdc, oldpen );
	}
}

PROFILE_KEY Legend::HitTest( const POINT& ptHit )
{
	if( !PtInRect(&m_self, ptHit ))
		return 0L;

	unsigned nTotalHeight = m_nLinesCount * ( m_cellHeight + 1 ) + 2 * m_LegendOffset;
	int y = m_self.top + m_LegendOffset + int(0.5*(m_self.bottom - m_self.top)) + int(m_cellHeight * 0.5);

	// find hit cell
	int row = int( ceil( double( y - ptHit.y ) / m_cellHeight ) );
	int column = int( ceil( double( ptHit.x - m_self.left ) / double( m_cellWidth ) ) );

	int colCount = int( floor( double( m_self.right - m_self.left ) / double( m_cellWidth ) ) );

	unsigned index = (row-1) * colCount + (column-1);

	set<Profile*, profile_less>::const_iterator it = profiles.begin();
	if (index >= profiles.size())
		return 0L;

	advance(it, index);
	return plot.FindProfile(*it);
}

BOOL Legend::Write(HANDLE hFile) const
{
	if (!WriteString(hFile, BEGINMARK))
		return FALSE;	

	DWORD dwRes;
	if (!WriteFile( hFile, &m_enabled, sizeof(m_enabled), &dwRes, NULL ) || dwRes != sizeof(m_enabled))
		return FALSE;

	if (!WriteFile( hFile, &m_align, sizeof(m_align), &dwRes, NULL ) || dwRes != sizeof(m_align))
		return FALSE;

	if(!m_basicEstr->Write(hFile))
		return FALSE;	

	if (!WriteString(hFile, ENDMARK))
		return FALSE;

	return TRUE;
}

BOOL Legend::Read(HANDLE hFile)
{
	try {
		if (ReadString(hFile) != BEGINMARK)
			return FALSE;

		DWORD dwRes;
		if (!ReadFile( hFile, &m_enabled, sizeof(m_enabled), &dwRes, NULL ) || dwRes != sizeof(m_enabled))
			return FALSE;	

		if (!ReadFile( hFile, &m_align, sizeof(m_align), &dwRes, NULL ) || dwRes != sizeof(m_align))
			return FALSE;	

		if (!m_basicEstr->Read(hFile))
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