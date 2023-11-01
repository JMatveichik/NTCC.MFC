// ExtControls.cpp : implementation file
//

#include "stdafx.h"
#include "ExtControls.h"
#include "..\FSDocItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW	
#endif

// ExtControls


CExtSliderWndColoredSelection::CExtSliderWndColoredSelection() :
bInit(false)
, m_rcProgressBarPartH( 0, 0, 92, 8 )
, m_rcProgressBarPartV( 0, 92, 8, 182 )
, m_rcProgressBackgroundPartH( 92, 0, 182, 8 )
, m_rcProgressBackgroundPartV( 0, 0, 8, 92 )
, m_rcProgressBarPaddingH( 1, 1, 10, 1 )
, m_rcProgressBarPaddingV( 1, 10, 1, 1 )
, m_rcProgressBackgroundPaddingH( 0, 0, 0, 0 )
, m_rcProgressBackgroundPaddingV( 0, 0, 0, 0 )
, m_rcProgressBorderPaddingH( 2, 2, 2, 2 )
, m_rcProgressBorderPaddingV( 2, 2, 2, 2 ) 
{	

}

CExtSliderWndColoredSelection::~CExtSliderWndColoredSelection() 
{
}

bool CExtSliderWndColoredSelection::Init( UINT IDB_PROGRESS_BMP, UINT IDB_BORDER_BMP)
{
  if ( !m_bmpProgressMainH.LoadBMP_Resource(  MAKEINTRESOURCE( IDB_PROGRESS_BMP ) ) )
	  return bInit = false;


  if ( !m_bmpProgressMainV.CreateRotated9xStack( m_bmpProgressMainH, 270, 1, m_bmpProgressMainH.GetSize().cx,m_bmpProgressMainH.GetSize().cy, true, true) )
	  return bInit = false;

  if ( !m_bmpProgressBorderH.LoadBMP_Resource( MAKEINTRESOURCE( IDB_BORDER_BMP )  ) )
	  return bInit = false;

  if ( !m_bmpProgressBorderV.CreateRotated9xStack( m_bmpProgressBorderH, 270, 1, m_bmpProgressBorderH.GetSize().cx, m_bmpProgressBorderH.GetSize().cy, true, true) )
	  return bInit = false;

  return bInit = true;
}

bool CExtSliderWndColoredSelection::OnSliderDrawChannel(CDC & dc, const CRect & rcChannel, UINT uItemState )
{
	CRect rc = rcChannel;
	rc.DeflateRect(CSize(3, 3));		

	int maxR = GetRangeMax();

	int posMin, posMax;
	GetSelection(posMin, posMax);

	CExtPaintManager::PAINTPROGRESSDATA ppd(posMax, maxR, rc);			

	if ( bInit )
		PaintProgress(dc, ppd);
	else
		g_PaintManager->PaintProgress(dc, ppd);

	return true;
}

bool CExtSliderWndColoredSelection::OnSliderDrawThumb( CDC & dc, const CRect & rcThumb, UINT uItemState )
{
	CRect rc = rcThumb;
	rc.DeflateRect(CSize(0, 2));
	return CExtSliderWnd::OnSliderDrawThumb(dc, rc, uItemState);
}
bool CExtSliderWndColoredSelection::PaintProgress( CDC & dc, CExtPaintManager::PAINTPROGRESSDATA & _ppd )
{

	ASSERT_VALID( this );
	ASSERT_VALID( (&dc) );
	ASSERT( dc.GetSafeHdc() != NULL );

	if(		_ppd.m_rcClient.Width() <= 0
		||	_ppd.m_rcClient.Height() <= 0
		||	( ! dc.RectVisible( &_ppd.m_rcClient ) )
		)
		return false;
	CExtBitmap & bmpProgressMain = _ppd.m_bHorz ? m_bmpProgressMainH : m_bmpProgressMainV;
	if( bmpProgressMain.IsEmpty() )
		return false;
	CRect rcProgress( _ppd.m_rcClient );
	//	if(		_ppd.m_pHelperSrc == NULL
	//		||	( ! _ppd.m_pHelperSrc->IsKindOf( RUNTIME_CLASS( CWnd ) ) )
	//		||	( ! GetCb2DbTransparentMode( _ppd.m_pHelperSrc, _ppd.m_lParam ) )
	//		||	( ! PaintDockerBkgnd( true, dc, (CWnd*)_ppd.m_pHelperSrc, _ppd.m_lParam ) )
	//		)
	//	{
	//		COLORREF clrBackgroundArea = GetColor( COLOR_3DFACE, _ppd.m_pHelperSrc, _ppd.m_lParam );
	//		clrBackgroundArea = dc.GetNearestColor( clrBackgroundArea );
	//		dc.FillSolidRect( &rcProgress, clrBackgroundArea );
	//	}
	if( _ppd.m_bBorder )
	{
		CExtBitmap & bmpProgressBorder = _ppd.m_bHorz ? m_bmpProgressBorderH : m_bmpProgressBorderV;
		if( bmpProgressBorder.IsEmpty() )
			return false;
		CRect rcProgressBorderPadding = _ppd.m_bHorz ? m_rcProgressBorderPaddingH : m_rcProgressBorderPaddingV;
		if( ! bmpProgressBorder.AlphaBlendSkinParts(
			dc.m_hDC,
			rcProgress,
			rcProgressBorderPadding,
			CExtBitmap::__EDM_STRETCH,
			true,
			true
			)
			)
			return false;
		rcProgress.DeflateRect(
			rcProgressBorderPadding.left,
			rcProgressBorderPadding.top,
			rcProgressBorderPadding.right,
			rcProgressBorderPadding.bottom
			);
	} // if( _ppd.m_bBorder )
	if(		rcProgress.Width() <= 0
		||	rcProgress.Height() <= 0
		||	( ! dc.RectVisible( &rcProgress ) )
		)
		return true;
	CExtMemoryDC dcX(
		&dc,
		&rcProgress,
		CExtMemoryDC::MDCOPT_TO_MEMORY
		//| CExtMemoryDC::MDCOPT_FILL_BITS
		| CExtMemoryDC::MDCOPT_RTL_COMPATIBILITY
		);
	CRect rcProgressBackgroundPadding = _ppd.m_bHorz ? m_rcProgressBackgroundPaddingH : m_rcProgressBackgroundPaddingV;
	CRect rcProgressBackgroundPart = _ppd.m_bHorz ? m_rcProgressBackgroundPartH : m_rcProgressBackgroundPartV;
	if( ! bmpProgressMain.AlphaBlendSkinParts(
		dcX.m_hDC,
		rcProgress,
		rcProgressBackgroundPart,
		rcProgressBackgroundPadding,
		CExtBitmap::__EDM_STRETCH,
		true,
		true
		)
		)
		return false;
	CRect rcProgressBarPadding = _ppd.m_bHorz ? m_rcProgressBarPaddingH : m_rcProgressBarPaddingV;
	if( _ppd.m_bHorz )
		rcProgress.right =
		rcProgress.left
		+ ::MulDiv( rcProgress.Width(), _ppd.m_nPos, _ppd.m_nRange )
		+ rcProgressBarPadding.right
		;
	else
		rcProgress.top =
		rcProgress.bottom
		- ::MulDiv( rcProgress.Height(), _ppd.m_nPos, _ppd.m_nRange )
		- rcProgressBarPadding.top
		;
	if(		rcProgress.Width() <= 0
		||	rcProgress.Height() <= 0
		||	( ! dcX.RectVisible( &rcProgress ) )
		)
		return true;
	CRect rcProgressBarPart = _ppd.m_bHorz ? m_rcProgressBarPartH : m_rcProgressBarPartV;
	if( ! bmpProgressMain.AlphaBlendSkinParts(
		dcX.m_hDC,
		rcProgress,
		rcProgressBarPart,
		rcProgressBarPadding,
		CExtBitmap::__EDM_STRETCH,
		true,
		true
		)
		)
		return false;
	return true;
}

void CExtSliderWndColoredSelection::PmBridge_OnPaintManagerChanged( CExtPaintManager * pGlobalPM )
{
	CExtSliderWnd::PmBridge_OnPaintManagerChanged(pGlobalPM);
	SetFont(&pGlobalPM->m_FontNormal, TRUE);
}




BEGIN_MESSAGE_MAP(CAutoSelComboBox, CExtComboBox)
	ON_NOTIFY_REFLECT( CBN_SELENDOK, OnSelectionChanged )
	ON_NOTIFY_REFLECT( CBN_SELCHANGE, OnSelectionChanged )
	ON_NOTIFY_REFLECT( CBN_DROPDOWN, OnSelectionChanged )
	ON_NOTIFY_REFLECT( CBN_SELENDCANCEL, OnSelectionChanged )
	
	
	ON_CONTROL_REFLECT(CBN_SELENDOK, memberFxn)
END_MESSAGE_MAP()


CAutoSelComboBox::CAutoSelComboBox(CDrawSelectObj* pObj) : pParent(pObj)
{
}

CAutoSelComboBox::~CAutoSelComboBox()
{

}

void CAutoSelComboBox::memberFxn ( )
{
	CString str1;
	
	int ind = GetCurSel();
	int len = GetLBTextLen(ind);

	GetLBText( ind, str1.GetBuffer(len) );
	str1.ReleaseBuffer(len);

	pParent->OnSelectionChanged(GetItemData(ind));
	GetParent()->SetFocus();
}

void CAutoSelComboBox::OnSelectionChanged(NMHDR* pNMHDR, LRESULT* lResult)
{
	CString str1;
	
	int ind = GetCurSel();
	int len = GetLBTextLen(ind);

	GetLBText( ind, str1.GetBuffer(len) );
	str1.ReleaseBuffer(len);

	//TRACE("\n\nNew Selection => %s ",  (LPCTSTR)str1);
}

void CAutoSelComboBox::PmBridge_OnPaintManagerChanged( CExtPaintManager * pGlobalPM )
{
	CExtComboBox::PmBridge_OnPaintManagerChanged(pGlobalPM);
	SetFont(&pGlobalPM->m_FontNormal, TRUE);
}