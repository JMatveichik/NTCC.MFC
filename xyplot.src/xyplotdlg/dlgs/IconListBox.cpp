// IconListBox.cpp : implementation file
//

#include "stdafx.h"
#include "IconListBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIconListBox

CIconListBox::CIconListBox()
{
}

CIconListBox::~CIconListBox()
{
	RemoveAllIcons();
}


BEGIN_MESSAGE_MAP(CIconListBox, CListBox)
	//{{AFX_MSG_MAP(CIconListBox)
	ON_CONTROL_REFLECT(LBN_SELCANCEL, OnSelcancel)
	ON_CONTROL_REFLECT(LBN_KILLFOCUS, OnKillfocus)
	ON_CONTROL_REFLECT(LBN_SETFOCUS, OnSetfocus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIconListBox message handlers

void CIconListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	ASSERT_VALID( this );
	ASSERT( lpDrawItemStruct != NULL );
	ASSERT( lpDrawItemStruct->hDC != NULL );
	if( ((INT)lpDrawItemStruct->itemID) < 0 )
		return;

	CDC * pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect rcItem = lpDrawItemStruct->rcItem;

	if( lpDrawItemStruct->itemAction & (ODA_DRAWENTIRE | ODA_SELECT) )
	{
		CRect rcErase( rcItem );
		if( INT(lpDrawItemStruct->itemID) == (GetCount()-1) )
		{
			CRect rcClient;
			GetClientRect( &rcClient );
			if( rcErase.bottom < rcClient.bottom )
				rcErase.bottom = rcClient.bottom;
		}
		pDC->FillSolidRect( 
			rcErase, 
			g_PaintManager->GetColor( COLOR_WINDOW, (CObject*)this )
			);

		CRect rcBottomArea( rcErase );
		rcBottomArea.top = rcItem.bottom;

		CExtPaintManager::PAINTMENUITEMDATA _pmid(
			NULL,
			rcBottomArea,
			rcBottomArea,
			GetIconAreaWidth(),
			_T(""),
			_T(""),
			NULL,
			false,
			false,
			false,
			false,
			false,
			true,
			false,
			false,
			false,
			0,
			NULL
			);
	
		g_PaintManager->PaintMenuItem( *pDC, _pmid );
	}

	m_arrItems[ lpDrawItemStruct->itemID ]
		-> DrawItem( GetIconAreaWidth(), lpDrawItemStruct );
}

void CIconListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	ASSERT_VALID( this );
	ASSERT( lpMeasureItemStruct != NULL );
	ASSERT( ((INT)lpMeasureItemStruct->itemID) >= 0 );
CRect rcClient;
	GetClientRect( &rcClient );
CSize _sizeClientArea = rcClient.Size();
	m_arrItems[ lpMeasureItemStruct->itemID ]
		-> MeasureItem( lpMeasureItemStruct, _sizeClientArea );
}

void CIconListBox::OnSelcancel() 
{
	_Invalidate();
}

void CIconListBox::OnKillfocus() 
{
	_Invalidate();
}

void CIconListBox::OnSetfocus() 
{
	_Invalidate();
}

void CIconListBox::ITEM_DATA::DrawItem(INT nIconAreaWidth, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CRect rcItem = lpDrawItemStruct->rcItem;
	CDC dcTmp;
	dcTmp.Attach( lpDrawItemStruct->hDC );
	CExtMemoryDC dc( &dcTmp, &rcItem );

	bool bSelected = (lpDrawItemStruct->itemState & ODS_SELECTED) != 0;

	CExtPaintManager::PAINTMENUITEMDATA _pmid(
		NULL,
		rcItem,
		rcItem,
		nIconAreaWidth,
		_T(""),
		_T(""),
		&m_icon,
		false,
		bSelected,
		false,
		false,
		false,
		true,
		false,
		false,
		false,
		0,
		NULL
		);
	
	g_PaintManager->PaintMenuItem( dc, _pmid );

	CRect rcMeasureText( 0, 0, 0, 0 );
	dc.DrawText(
		m_strText,
		m_strText.GetLength(),
		&rcMeasureText,
		DT_CALCRECT|DT_SINGLELINE|DT_LEFT
		);
	CRect rcDrawText( rcItem );
	rcDrawText.OffsetRect(
		0,
		( rcDrawText.Height() - rcMeasureText.Height() ) / 2
		);
	
	rcDrawText.left += (nIconAreaWidth+4);

	CFont * pOldFont =
		dc.SelectObject( 
		bSelected 
		? &g_PaintManager->m_FontBold 
		: &g_PaintManager->m_FontNormal 
		);
	int nOldBkMode = dc.SetBkMode( TRANSPARENT );
	COLORREF clrOldText =
		dc.SetTextColor(
			g_PaintManager->GetColor( bSelected  ? CExtPaintManager::CLR_TEXT_OUT : CExtPaintManager::CLR_3DHILIGHT_IN , (CObject*)this )
		);

	dc.DrawText(
		m_strText,
		&rcDrawText,
		DT_LEFT
		);
	dc.SetTextColor( clrOldText );
	dc.SetBkMode( nOldBkMode );
	dc.SelectObject( pOldFont );
	
	dc.__Flush();
	dcTmp.Detach();
}

void CIconListBox::ITEM_DATA::MeasureItem(
	LPMEASUREITEMSTRUCT lpMeasureItemStruct,
	CSize _sizeClientArea
	) 
{
	CSize _sizeIcon = GetSize();
	lpMeasureItemStruct->itemWidth = _sizeClientArea.cx;
	lpMeasureItemStruct->itemHeight = _sizeIcon.cy;
	CWindowDC dcDesktop( NULL );
	CRect rcText( 0, 0, _sizeClientArea.cx, 0 );
	CFont * pOldFont =
		dcDesktop.SelectObject( &g_PaintManager->m_FontNormal );
	UINT nTextHeight = (UINT)
		dcDesktop.DrawText(
		m_strText,
		&rcText,
		DT_LEFT|DT_TOP|DT_CALCRECT
		);
	dcDesktop.SelectObject( pOldFont );
	lpMeasureItemStruct->itemHeight =
		max(
		nTextHeight,
		lpMeasureItemStruct->itemHeight
		);
	// vertical gaps
	lpMeasureItemStruct->itemHeight += 8;
	rcText =
		CalcTextRect(
		CRect(
		0,
		0,
		lpMeasureItemStruct->itemWidth,
		lpMeasureItemStruct->itemHeight
		),
		_sizeIcon
		);
}

LRESULT CIconListBox::WindowProc( 
	UINT message, 
	WPARAM wParam, 
	LPARAM lParam
	)
{
	if( message == WM_ERASEBKGND )
		return TRUE;

	if( message == WM_PAINT )
	{
		CRect rcClient;
		GetClientRect( &rcClient );
		CPaintDC dcPaint( this );
		CExtMemoryDC dc( &dcPaint, &rcClient );

		CExtPaintManager::PAINTMENUITEMDATA _pmid(
			NULL,
			rcClient,
			rcClient,
			GetIconAreaWidth(),
			_T(""),
			_T(""),
			NULL,
			false,
			false,
			false,
			false,
			false,
			true,
			false,
			false,
			false,
			0,
			NULL
			);

		g_PaintManager->PaintMenuItem( dc, _pmid );
		
		DefWindowProc( WM_PAINT, WPARAM(dc.GetSafeHdc()), 0 );
		return 0;
	} // if( message == WM_PAINT )

	return CListBox::WindowProc( message, wParam, lParam );
}
