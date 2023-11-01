#pragma once


// ExtControls

class CExtSliderWndColoredSelection : public CExtSliderWnd
{
public:

	CExtSliderWndColoredSelection();
	virtual ~CExtSliderWndColoredSelection();


	 bool Init( UINT IDB_PROGRESS_BMP, UINT IDB_BORDER_BMP);
	 

protected:

	bool bInit;

	virtual bool OnSliderDrawChannel(CDC & dc, const CRect & rcChannel, UINT uItemState );
	virtual bool OnSliderDrawThumb( CDC & dc, const CRect & rcThumb, UINT uItemState );
	bool PaintProgress( CDC & dc, CExtPaintManager::PAINTPROGRESSDATA & _ppd );	
	
	
	virtual void PmBridge_OnPaintManagerChanged( CExtPaintManager * pGlobalPM );

protected:

	CExtBitmapCache
		m_bmpProgressMainH, m_bmpProgressMainV,
		m_bmpProgressBorderH, m_bmpProgressBorderV;

	CRect
		m_rcProgressBarPartH,
		m_rcProgressBarPartV,
		m_rcProgressBackgroundPartH,
		m_rcProgressBackgroundPartV,
		m_rcProgressBarPaddingH,
		m_rcProgressBarPaddingV,
		m_rcProgressBackgroundPaddingH,
		m_rcProgressBackgroundPaddingV,
		m_rcProgressBorderPaddingH,
		m_rcProgressBorderPaddingV;
	
};

class CExtStatusBarLabel : public CExtLabel
{

	HICON m_iconEn;
	HICON m_iconDis;

public:

	void SetIcon(HICON hIconEn, HICON hIconDis )
	{
		m_iconEn = hIconEn;
		m_iconDis = hIconDis;
	}

protected:

	virtual void OnDrawLabelText( CDC & dc, const RECT & rcText, __EXT_MFC_SAFE_LPCTSTR strText, DWORD dwDrawTextFlags, bool bEnabled )
	{
		CExtCmdIcon icon; 
		icon.AssignFromHICON(IsWindowEnabled() ? m_iconEn : m_iconDis, false );

		CSize szIcon  = icon.GetSize();
		
		CRect rcNewText( rcText );
		
		rcNewText.left = szIcon.cx + szIcon.cx / 2;
		CRect rc( rcNewText );
		CRect rcIcon( 0, 0, szIcon.cx, szIcon.cy );

		
		icon.GetBitmap().AlphaBlendSkinParts(dc.m_hDC, rcIcon, CRect(0,0,0,0),  CExtBitmap::__EDM_STRETCH);
		//dc.DrawIcon(0,0, icon.ExtractHICON());
		CExtLabel::OnDrawLabelText(dc, rcNewText, strText, dwDrawTextFlags, bEnabled);
		
	}
	
	void OnEraseBackground( CDC & dc, const CRect & rcClient )
	{
		ASSERT_VALID( this );
		COLORREF clrBackground = GetBkColor();
		bool bTransparent = false;
		if( clrBackground == COLORREF(-1L) )
		{
			CExtStatusControlBar * pWndParent =  DYNAMIC_DOWNCAST( CExtStatusControlBar, GetParent() );
			ASSERT( pWndParent != NULL );

			CRect rcPaint;
			pWndParent->GetWindowRect( &rcPaint );
			ScreenToClient( &rcPaint );

			CRect _rcClient( rcClient );
			ClientToScreen( &_rcClient );
			pWndParent->ScreenToClient( &_rcClient );

			if( pWndParent->m_bCompleteRepaint )
			{
				dc.OffsetViewportOrg(  _rcClient.left,  0  );

				bool bRet =  PmBridge_GetPM()->StatusBar_EraseBackground( dc, rcPaint, pWndParent );

				dc.OffsetViewportOrg( -_rcClient.left, 0 );

				if( !bRet )
				{
					bool bTransparent = false;
					if( PmBridge_GetPM()->GetCb2DbTransparentMode(pWndParent) )
					{
						if( PmBridge_GetPM()->PaintDockerBkgnd( true, dc, this ) )
							bTransparent = true;
					}
					if( ! bTransparent )
						dc.FillSolidRect(  &rcPaint,  PmBridge_GetPM()->GetColor( CExtPaintManager::CLR_3DFACE_OUT, pWndParent ) );
				}

				//???
				INT nIndex = pWndParent->CommandToIndex( 0 ); 				

				dc.OffsetViewportOrg( _rcClient.left, 0 );

				PmBridge_GetPM()->StatusBar_ErasePaneBackground( dc, nIndex, rcPaint, pWndParent );

				dc.OffsetViewportOrg( -_rcClient.left,  0 );

				bTransparent = true;

			} // if( pWndParent->m_bCompleteRepaint )

		}
		if( ! bTransparent )
			dc.FillSolidRect( &rcClient, (clrBackground != COLORREF(-1L))  ? clrBackground  : PmBridge_GetPM()->GetColor( CExtPaintManager::CLR_3DFACE_OUT, this ) );	
	}

}; // class CExtStatusBarLabel



class CSizableTabMdiOneNoteWnd : public CExtTabMdiOneNoteWnd
{
	int m_nShift;
	int m_nSize;

public:

	CSizableTabMdiOneNoteWnd() : 
	    m_nShift(-1), m_nSize(-1) 		
	{ 
		CFont* pFnt = CFont::FromHandle( (HFONT) ::GetStockObject(DEFAULT_GUI_FONT) ) ;		
		pFnt->GetLogFont(&m_lfNormal);
		m_lfSelected = m_lfNormal;

	}

	virtual ~CSizableTabMdiOneNoteWnd()
	{
	}
	
	int GetShift()
	{
		CRect rc;
		GetWindowRect(rc);
		return _CalcRgnShift( OrientationIsHorizontal(), rc );
	}


	int GetSize()
	{
		return OnFlatTabWndGetSize( OrientationIsHorizontal() );
	}
	
	
	virtual void _GetTabWndFont( CFont * pFont, bool bSelected, DWORD dwOrientation = DWORD(-1) ) const
	{		
		pFont->CreateFontIndirect(bSelected ? &m_lfSelected : &m_lfNormal);
	}


	void SetShift( int nShift ){
		m_nShift = nShift;
	}
	void SetSize( int nSize ){
		m_nSize = nSize;
		
	}

	BOOL SetFont(int size, CString name, bool bSelected,  DWORD dwOrientation = DWORD(-1))
	{
		LOGFONT* pLF = bSelected ? &m_lfSelected : &m_lfNormal;

		LOGFONT lf = *pLF;		
		
		lf.lfWeight = bSelected ?  FW_BOLD : FW_NORMAL;			
		HDC hdc = GetDesktopWindow()->GetDC()->m_hDC;
		
		lf.lfHeight = -MulDiv(size, GetDeviceCaps(hdc, LOGPIXELSY), 72);
		
		::ReleaseDC( GetDesktopWindow()->m_hWnd, hdc );

		strcpy_s(lf.lfFaceName, (LPCTSTR)name);
		
		CFont font;		
		if ( !font.CreateFontIndirect(&lf) )
			return FALSE;

		*pLF = lf;
		// create the font for tabs		
		return   TRUE; 
	}	

protected:	


	LOGFONT m_lfNormal;
	LOGFONT m_lfSelected;


	//DECLARE_MESSAGE_MAP()

	virtual void OnTabWndUpdateItemMeasure( TAB_ITEM_INFO * pTii, CDC & dcMeasure, CSize & sizePreCalc )
	{
		CExtTabMdiOneNoteWnd::OnTabWndUpdateItemMeasure(pTii, dcMeasure, sizePreCalc);

		if( OrientationIsHorizontal() ) {
			sizePreCalc.cy = ( m_nSize > 0 ) ?  m_nSize : ::GetSystemMetrics( SM_CXHSCROLL );
			sizePreCalc.cx *= 2;
		}
		else {
			sizePreCalc.cx = ( m_nSize > 0 ) ?  m_nSize : ::GetSystemMetrics( SM_CYHSCROLL );
			sizePreCalc.cy *= 2;
		}
	}
	

	virtual CSize OnTabWndCalcButtonSize(
		CDC & dcMeasure,
		LONG nTabAreaMetric // vertical max width or horizontal max heights of all tabs
	)
	{
		ASSERT_VALID( this );
		ASSERT( dcMeasure.GetSafeHdc() != NULL );
		dcMeasure;
		nTabAreaMetric;
	
		int nMetric = m_nSize - 3;
		
		CSize _sizeButton(	max( nMetric, __EXTTAB_BTN_MIN_DX ), 
						max( nMetric, __EXTTAB_BTN_MIN_DY ) );
		return _sizeButton;
	}

	virtual int _CalcRgnShift( bool bHorz, const CRect & rc )
	{
		int nShift = 0;
		if( m_nShift >= 0 )
			nShift = m_nShift;
		else
			nShift = ::MulDiv( bHorz ? rc.Height() : rc.Width(), 1, 4 );
		return nShift;
	}


	virtual int OnFlatTabWndGetSize( bool bHorz )
	{
		int nSize = 0;
		if(bHorz)
			nSize = ( m_nSize > 0 ) ? m_nSize : ::GetSystemMetrics( SM_CXHSCROLL );
		else
			nSize = ( m_nSize > 0 ) ? m_nSize : ::GetSystemMetrics( SM_CYHSCROLL );

		return nSize;
	}
	

}; // class CSizableTabWhibeyWnd


class CDrawSelectObj;
class CAutoSelComboBox : 
	
	public CExtComboBox
{
public:
	DECLARE_MESSAGE_MAP()
	
	

public:

	CAutoSelComboBox(CDrawSelectObj* pObj);
	~CAutoSelComboBox();

protected:

	afx_msg void memberFxn ( );
	afx_msg void OnSelectionChanged(NMHDR* pNMHDR, LRESULT* lResult);

	virtual void PmBridge_OnPaintManagerChanged( CExtPaintManager * pGlobalPM );

private:

	CDrawSelectObj* pParent;

};