#include "StdAfx.h"
#include "profilemarks.h"
#include "xyplotconst.h"


static const char BEGINMARK[] = "[marks]";
static const char ENDMARK[] = "[/marks]";

using namespace xyplot;

ProfileMarks::ProfileMarks() : 
m_nFrequency(1), 
m_nSize(5), 
m_clrLine(0x000000), 
m_clrFill(0x000000), 
m_nType(PMT_CIRCLE),
m_bEnabled (FALSE)
{
}

ProfileMarks::~ProfileMarks()
{
}

void ProfileMarks::OnDraw(HDC hdc, const POINT* pPoints, unsigned nCount)
{
	if (!m_bEnabled)
		return;

	int nCurrPoint = m_nFrequency;	

	HPEN pNewPen = CreatePen(PS_SOLID, 1, m_clrLine);
	HPEN pOldPen = (HPEN)SelectObject(hdc, pNewPen);	

	HBRUSH pNewBrush = CreateSolidBrush(m_clrFill);
	HBRUSH pOldBrush = (HBRUSH)SelectObject(hdc, pNewBrush);	

	for ( unsigned i = 0; i < nCount; i++)
	{	
		if (nCurrPoint == m_nFrequency )
		{
			switch (m_nType)
			{
			case PMT_CIRCLE:
				{
					unsigned nTopLeft = m_nSize / 2;
					unsigned nBottomRight = (m_nSize + 1) / 2;
					if (m_nSize / 2 == 0)
					{
						SetPixel(hdc, pPoints[i].x, pPoints[i].y, m_clrFill);
						break;
					}
					if (nBottomRight == 1)
						Rectangle(hdc, pPoints[i].x - 1, pPoints[i].y - 1, pPoints[i].x + 1, pPoints[i].y + 1);
					else
						Ellipse(hdc, pPoints[i].x - nTopLeft, pPoints[i].y - nTopLeft, 
							pPoints[i].x + nBottomRight, pPoints[i].y + nBottomRight);
				}
				break;

			case PMT_SQUARE: 
				Rectangle(hdc, pPoints[i].x - m_nSize / 2, pPoints[i].y - m_nSize / 2, pPoints[i].x + m_nSize / 2, pPoints[i].y + m_nSize / 2);
				break;

			case PMT_DIAMOND:
				{
					const POINT pt[] = 
					{
						{pPoints[i].x - m_nSize / 2,	pPoints[i].y },
						{pPoints[i].x,					pPoints[i].y  - m_nSize / 2},
						{pPoints[i].x + m_nSize / 2,	pPoints[i].y},
						{pPoints[i].x,					pPoints[i].y  + m_nSize / 2}
					};

					Polygon(hdc, pt, 4);
				}

				break;

			case PMT_TRIANGLEUP:
				{
				
					const POINT pt[] = 
					{
						{pPoints[i].x - m_nSize / 3,   pPoints[i].y + m_nSize / 4},
						{pPoints[i].x + m_nSize / 3,   pPoints[i].y + m_nSize / 4},
						{pPoints[i].x,   pPoints[i].y  - m_nSize / 4},

					};
					
					Polygon(hdc, pt, 3);
				}
				break;

			case PMT_TRIANGLEDOWN: 
				{
				
					const POINT pt[] = 
					{
						{pPoints[i].x - m_nSize / 3,   pPoints[i].y - m_nSize / 4},
						{pPoints[i].x + m_nSize / 3,   pPoints[i].y - m_nSize / 4},
						{pPoints[i].x,  pPoints[i].y + m_nSize / 4},
						
					};

					Polygon(hdc, pt, 3);
				}

				break;

			case PMT_CROSS0: 
				
				MoveToEx(hdc, pPoints[i].x, pPoints[i].y  - m_nSize / 2, (LPPOINT)NULL);
				LineTo(hdc, pPoints[i].x, pPoints[i].y + m_nSize / 2);

				MoveToEx(hdc, pPoints[i].x  - m_nSize / 2, pPoints[i].y, (LPPOINT)NULL);
				LineTo(hdc, pPoints[i].x + m_nSize / 2, pPoints[i].y);				

				break;

			case PMT_CROSS45:
				MoveToEx(hdc, pPoints[i].x - m_nSize / 2 , pPoints[i].y  - m_nSize / 2, (LPPOINT)NULL);
				LineTo(hdc, pPoints[i].x + m_nSize / 2, pPoints[i].y + m_nSize / 2);

				MoveToEx(hdc, pPoints[i].x  + m_nSize / 2, pPoints[i].y  - m_nSize / 2, (LPPOINT)NULL);
				LineTo(hdc, pPoints[i].x - m_nSize / 2, pPoints[i].y + m_nSize / 2);

				break;
			};
			nCurrPoint = 0;
		}
		nCurrPoint++;
	}
	
	SelectObject(hdc, pOldBrush);
	SelectObject(hdc, pOldPen);

	DeleteObject(pNewBrush);
	DeleteObject(pNewPen);
}

void ProfileMarks::PreDraw(HDC hdc)
{
}

BOOL ProfileMarks::Write(HANDLE hFile) const
{
	if (!WriteString(hFile, BEGINMARK))
		return FALSE;	

	DWORD dwRes;
	if (!WriteFile( hFile, &m_nFrequency, sizeof(m_nFrequency), &dwRes, NULL ) || dwRes != sizeof(m_nFrequency))
		return FALSE;

	if (!WriteFile( hFile, &m_nSize, sizeof(m_nSize), &dwRes, NULL ) || dwRes != sizeof(m_nSize))
		return FALSE;

	if (!WriteFile( hFile, &m_clrLine, sizeof(m_clrLine), &dwRes, NULL ) || dwRes != sizeof(m_clrLine))
		return FALSE;

	if (!WriteFile( hFile, &m_clrFill, sizeof(m_clrFill), &dwRes, NULL ) || dwRes != sizeof(m_clrFill))
		return FALSE;

	if (!WriteFile( hFile, &m_nType, sizeof(m_nType), &dwRes, NULL ) || dwRes != sizeof(m_nType))
		return FALSE;

	if (!WriteString(hFile, ENDMARK))
		return FALSE;

	return TRUE;
}

BOOL ProfileMarks::Read(HANDLE hFile)
{
	try 
	{
		if (ReadString(hFile) != BEGINMARK)
			return FALSE;	

		DWORD dwRes;
		if (!ReadFile( hFile, &m_nFrequency, sizeof(m_nFrequency), &dwRes, NULL ) || dwRes != sizeof(m_nFrequency))
			return FALSE;

		if (!ReadFile( hFile, &m_nSize, sizeof(m_nSize), &dwRes, NULL ) || dwRes != sizeof(m_nSize))
			return FALSE;

		if (!ReadFile( hFile, &m_clrLine, sizeof(m_clrLine), &dwRes, NULL ) || dwRes != sizeof(m_clrLine))
			return FALSE;

		if (!ReadFile( hFile, &m_clrFill, sizeof(m_clrFill), &dwRes, NULL ) || dwRes != sizeof(m_clrFill))
			return FALSE;

		if (!ReadFile( hFile, &m_nType, sizeof(m_nType), &dwRes, NULL ) || dwRes != sizeof(m_nType))
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