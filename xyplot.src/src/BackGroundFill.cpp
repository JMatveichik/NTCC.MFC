#include "StdAfx.h"
#include "backgroundfill.h"
#include "xyplotconst.h"

using namespace xyplot;

static const char BEGINMARK[] = "[bgrd]";
static const char ENDMARK[] = "[/bgrd]";

BackGround::BackGround() : 
enabled(TRUE), 
	clrStart(0), 
	clrFinish(0), 
	alphaStart(0), 
	alphaFinish(0), 
	vertical(FALSE),
	hatchStyle(BGHS_NONE)
{

}

void BackGround::OnDraw( HDC hdc ) const
{
	if (!enabled)
		return;
	
	SetBkMode(hdc, TRANSPARENT);

	FillRectWithGradient (hdc, rcFill, clrStart, clrFinish, alphaStart, alphaFinish, vertical);	
	//отрисовка штриховки
	if ( hatchStyle != BGHS_NONE )
	{
		
		HBRUSH hBrush = CreateHatchBrush(hatchStyle, hatchColor);
		HBRUSH hbrushOld = (HBRUSH)SelectObject(hdc, hBrush);
		HPEN hpenOld = (HPEN)SelectObject(hdc, GetStockObject(NULL_PEN));

		Rectangle(hdc, rcFill.left, rcFill.top, rcFill.right, rcFill.bottom );

		SelectObject(hdc, hbrushOld);
		SelectObject(hdc, hpenOld);
		DeleteObject(hBrush);	
	}	
}

/* Return Hatch Styles */	
long BackGround::GetHatchStyle() const 
{ 
	return hatchStyle; 
}

/* Set Hatch Styles */	
void BackGround::SetHatchStyle(long hstyle) 
{ 
	hatchStyle = hstyle; 
}

COLORREF BackGround::GetHatchColor() const
{
	return hatchColor;
}

void BackGround::SetHatchColor(COLORREF clr)
{
	hatchColor = clr;
}

void BackGround::GetFillColors(COLORREF& clrStart, COLORREF& clrFinish) const
{
	clrStart  = this->clrStart;
	clrFinish = this->clrFinish;
}


void BackGround::SetFillColors(COLORREF clrStart, COLORREF clrFinish)
{
	this->clrStart  = clrStart;
	this->clrFinish = clrFinish;
}

void BackGround::GetOpacity(double& opacityStart, double& opacityFinish) const
{
	opacityStart = this->opacityStart;
	opacityFinish = this->opacityFinish;
}

void BackGround::SetOpacity(double opacityStart, double opacityFinish)
{
	this->opacityStart  =  ( opacityStart < 0.0 || opacityStart > 100.0 ) ? 100.0 : opacityStart;	
	this->opacityFinish =  ( opacityFinish < 0.0 || opacityFinish > 100.0 ) ? 100.0 : opacityFinish;

	alphaStart  = BYTE( 255 - 255 * (100 - opacityStart) / 100 );
	alphaFinish = BYTE( 255 - 255 * (100 - opacityFinish) / 100 );
}

void BackGround::SetRect(const RECT& rc)
{
	CopyRect(&rcFill, &rc); 
}

void BackGround::FillRectWithGradient(HDC hdc, const RECT& rc, COLORREF clr1, COLORREF clr2, BYTE alpha1, BYTE alpha2, BOOL bVertical)
{
	/*************** градиент *************************/
	TRIVERTEX        vert[2] ;
	GRADIENT_RECT    gRect;
	vert[0].x      = rc.left;
	vert[0].y      = rc.top;
	vert[0].Red    = (clr1 & 0xFF) << 8;
	vert[0].Green  = clr1 & 0xFF00;
	vert[0].Blue   = COLOR16((clr1 & 0xFF0000) >> 8);
	vert[0].Alpha  = alpha1;

	vert[1].x      = rc.right;
	vert[1].y      = rc.bottom; 
	vert[1].Red    = (clr2 & 0xFF) << 8;
	vert[1].Green  = clr2 & 0xFF00;
	vert[1].Blue   = COLOR16((clr2 & 0xFF0000) >> 8);
	vert[1].Alpha  = alpha2;

	gRect.UpperLeft  = 0;
	gRect.LowerRight = 1;	
	
/*
	int x = rc.left;
	int y = rc.top;
	int w = rc.right - rc.left;
	int h = rc.bottom - rc.top;

	HDC memDC = CreateCompatibleDC(hdc);
	HBITMAP bitmap = CreateCompatibleBitmap( hdc, w, h);
	HBITMAP holdbmp = (HBITMAP) SelectObject( memDC, bitmap );*/

	GradientFill(hdc, vert, 2, &gRect, 1, bVertical ? GRADIENT_FILL_RECT_V  : GRADIENT_FILL_RECT_H);
/*

	BLENDFUNCTION blend; 
	blend.BlendOp = AC_SRC_OVER; 
	blend.BlendFlags = 0; 
	blend.SourceConstantAlpha = alpha1; 
	blend.AlphaFormat = AC_SRC_ALPHA;

	AlphaBlend(hdc, x, y, w, h, memDC, x, y, w, h, blend);
	
	SelectObject(memDC, holdbmp);
	DeleteObject(bitmap);
	DeleteDC(memDC);*/
}

BOOL BackGround::Write(HANDLE hFile) const
{
	if (!WriteString(hFile, BEGINMARK))
		return FALSE;	

	DWORD dwRes;

	if (!WriteFile( hFile, &enabled, sizeof(enabled), &dwRes, NULL ) || dwRes != sizeof(enabled))
		return FALSE;

	if (!WriteFile( hFile, &clrStart, sizeof(clrStart), &dwRes, NULL ) || dwRes != sizeof(clrStart))
		return FALSE;

	if (!WriteFile( hFile, &clrFinish, sizeof(clrFinish), &dwRes, NULL ) || dwRes != sizeof(clrStart))
		return FALSE;

	if (!WriteFile( hFile, &alphaStart, sizeof(alphaStart), &dwRes, NULL ) || dwRes != sizeof(alphaStart))
		return FALSE;

	if (!WriteFile( hFile, &alphaFinish, sizeof(alphaStart), &dwRes, NULL ) || dwRes != sizeof(alphaFinish))
		return FALSE;

	if (!WriteFile( hFile, &vertical, sizeof(vertical), &dwRes, NULL ) || dwRes != sizeof(vertical))
		return FALSE;

	if (!WriteString(hFile, ENDMARK))
		return FALSE;

	return TRUE;
}

BOOL BackGround::Read(HANDLE hFile)
{
	if (ReadString(hFile) != BEGINMARK)
		return FALSE;

	DWORD dwRes;

	if (!ReadFile( hFile, &enabled, sizeof(enabled), &dwRes, NULL ) || dwRes != sizeof(enabled))
		return FALSE;

	if (!ReadFile( hFile, &clrStart, sizeof(clrStart), &dwRes, NULL ) || dwRes != sizeof(clrStart))
		return FALSE;

	if (!ReadFile( hFile, &clrFinish, sizeof(clrFinish), &dwRes, NULL ) || dwRes != sizeof(clrStart))
		return FALSE;

	if (!ReadFile( hFile, &alphaStart, sizeof(alphaStart), &dwRes, NULL ) || dwRes != sizeof(alphaStart))
		return FALSE;

	if (!ReadFile( hFile, &alphaFinish, sizeof(alphaStart), &dwRes, NULL ) || dwRes != sizeof(alphaFinish))
		return FALSE;

	if (!ReadFile( hFile, &vertical, sizeof(vertical), &dwRes, NULL ) || dwRes != sizeof(vertical))
		return FALSE;

	if (ReadString(hFile) != ENDMARK)
		return FALSE;

	return TRUE;
}
