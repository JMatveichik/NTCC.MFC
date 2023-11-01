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

	CFont* pFnt = GetFont();

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
		pDC->FillSolidRect(  rcErase,  g_PaintManager->GetColor( COLOR_WINDOW, (CObject*)this ) );

		CRect rcBottomArea( rcErase );
		rcBottomArea.top = rcItem.bottom;
	
		CExtPaintManager::PAINTMENUITEMDATA _pmid( NULL, rcBottomArea, rcBottomArea, GetIconAreaWidth(), _T(""),
			_T(""), NULL, false, false, false, false, false, true, false, false, false, 0, (HFONT)pFnt );
	
		g_PaintManager->PaintMenuItem( *pDC, _pmid );
	}

	m_arrItems[ lpDrawItemStruct->itemID ]
		-> DrawItem( GetIconAreaWidth(), lpDrawItemStruct, pFnt );
}

void CIconListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	ASSERT_VALID( this );
	ASSERT( lpMeasureItemStruct != NULL );
	ASSERT( ((INT)lpMeasureItemStruct->itemID) >= 0 );
	
	CRect rcClient;
	GetClientRect( &rcClient );
	
	CFont* pFont = GetFont();

	CSize _sizeClientArea = rcClient.Size();
	m_arrItems[ lpMeasureItemStruct->itemID ]-> MeasureItem( lpMeasureItemStruct, _sizeClientArea, pFont );
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

void CIconListBox::ITEM_DATA::DrawItem(INT nIconAreaWidth, LPDRAWITEMSTRUCT lpDrawItemStruct, CFont* pFont)
{
	CRect rcItem = lpDrawItemStruct->rcItem;
	CDC dcTmp;
	dcTmp.Attach( lpDrawItemStruct->hDC );
	CExtMemoryDC dc( &dcTmp, &rcItem );

	bool bSelected = (lpDrawItemStruct->itemState & ODS_SELECTED) != 0;

	CExtPaintManager::PAINTMENUITEMDATA _pmid( NULL, rcItem, rcItem, nIconAreaWidth,
		_T(""), _T(""), &m_icon, false, bSelected, false, false, false, true,
		false, false, false, 0, (HFONT)pFont );
	
	g_PaintManager->PaintMenuItem( dc, _pmid );

	CRect rcMeasureText( rcItem );
	//dc.DrawText( m_strText, m_strText.GetLength(), &rcMeasureText, DT_CALCRECT|DT_SINGLELINE|DT_LEFT );
	//dc.DrawText( m_strText, m_strText.GetLength(), &rcMeasureText, DT_WORDBREAK|DT_CALCRECT );

	CRect rcDrawText( rcItem );
 	int cx = (nIconAreaWidth + 4);	
	rcDrawText.left += cx;
	

	//CFont * pOldFont = dc.SelectObject(  bSelected  ? &g_PaintManager->m_FontBold  : &g_PaintManager->m_FontNormal );
	CFont * pOldFont = dc.SelectObject(  pFont );

	int nOldBkMode = dc.SetBkMode( TRANSPARENT );
	COLORREF clrOldText = dc.SetTextColor( g_PaintManager->GetColor(  bSelected  ? CExtPaintManager::CLR_TEXT_OUT : CExtPaintManager::CLR_3DHILIGHT_IN, (CObject*)this ) );
	
	//dc.DrawText( m_strText, &rcDrawText, DT_LEFT );
	dc.DrawText( m_strText, &rcDrawText, DT_LEFT|DT_VCENTER|DT_WORDBREAK );

	dc.SetTextColor( clrOldText );
	dc.SetBkMode( nOldBkMode );
	dc.SelectObject( pOldFont );
	
	dc.__Flush();
	dcTmp.Detach();
}

void CIconListBox::ITEM_DATA::MeasureItem( LPMEASUREITEMSTRUCT lpMeasureItemStruct, CSize _sizeClientArea, CFont* pFont ) 
{
	CSize _sizeIcon = GetSize();

	lpMeasureItemStruct->itemWidth = _sizeClientArea.cx;
	lpMeasureItemStruct->itemHeight = _sizeIcon.cy;

	CWindowDC dcDesktop( NULL );
	CRect rcText( 0, 0, _sizeClientArea.cx, 0 );
	
	//CFont * pOldFont = dcDesktop.SelectObject( &g_PaintManager->m_FontNormal );
	CFont * pOldFont = dcDesktop.SelectObject( pFont );
	
	
	//UINT nTextHeight = (UINT) dcDesktop.DrawText( m_strText, &rcText, DT_LEFT|DT_TOP|DT_CALCRECT );
	UINT nTextHeight = (UINT) dcDesktop.DrawText( m_strText, &rcText, DT_WORDBREAK|DT_CALCRECT );

	dcDesktop.SelectObject( pOldFont );
	lpMeasureItemStruct->itemHeight = max( nTextHeight, lpMeasureItemStruct->itemHeight );

	// vertical gaps
	lpMeasureItemStruct->itemHeight += 8;
	rcText = CalcTextRect( CRect( 0, 0, lpMeasureItemStruct->itemWidth, lpMeasureItemStruct->itemHeight ), _sizeIcon );
}

LRESULT CIconListBox::WindowProc(  UINT message,  WPARAM wParam,  LPARAM lParam )
{
	if( message == WM_ERASEBKGND )
	{
		return TRUE;
	}

	if( message == WM_PAINT )
	{
		CRect rcClient;
		GetClientRect( &rcClient );
		CPaintDC dcPaint( this );
		CExtMemoryDC dc( &dcPaint, &rcClient );

		HFONT hFnt = (HFONT)GetFont();
		CFont* pFont = GetFont();
		LOGFONT lf;
		pFont->GetLogFont(&lf);

// 		CExtPaintManager::PAINTMENUITEMDATA _pmid( NULL, rcClient, rcClient,
// 			GetIconAreaWidth(), _T(""), _T(""), NULL, false, false, false, false, false,
// 			true, false, false, false, 0, hFnt );

		g_PaintManager->PaintDockerBkgnd(false, dc, rcClient, rcClient);
//		g_PaintManager->PaintMenuItem( dc, _pmid );
		
		DefWindowProc( WM_PAINT, WPARAM(dc.GetSafeHdc()), 0 );
		return 0;
	} // if( message == WM_PAINT )

	return CListBox::WindowProc( message, wParam, lParam );
}

void CIconListBox::AddIcon( CExtCmdIcon& icon, LPCTSTR strComment)
{
	ASSERT_VALID( this );
	ASSERT( !icon.IsEmpty() );
	m_arrItems.Add( new ITEM_DATA( icon, strComment) );
	int ind = CListBox::AddString( _T("") );
	CListBox::SetCurSel(ind);
	CListBox::SetTopIndex(ind);
}

void CIconListBox::RemoveAllIcons()
{
	ASSERT_VALID( this );
	if(		GetSafeHwnd() != NULL &&	::IsWindow( GetSafeHwnd() ) )
		CListBox::ResetContent();

	for( INT i = 0; i < m_arrItems.GetSize(); i++ )
		delete m_arrItems[i];

	m_arrItems.RemoveAll();
}

CExtCmdIcon * CIconListBox::GetIconPtr( INT nPos )
{
	ASSERT_VALID( this );
	ASSERT( 0 <= nPos && nPos < m_arrItems.GetSize() );
	ITEM_DATA * pData = m_arrItems[nPos];
	ASSERT( pData != NULL );
	return pData->GetIconPtr();
}