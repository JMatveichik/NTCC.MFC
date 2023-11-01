// Feel free to use this code in your own applications.
// The Author does not guarantee anything about this code.
// Author : Yves Maurer

// FormattedTextDraw.h : Declaration of the CFormattedTextDraw

#pragma once

#ifndef __FORMATTEDTEXTDRAW_H_
#define __FORMATTEDTEXTDRAW_H_

//#include "..\resource.h"       // main symbols

// be sure to link this project with riched20.lib
//#include <richedit.h>
#include <windows.h>
#include <imm.h>
#include <textserv.h>
#include <tom.h>

#ifndef LY_PER_INCH
#define LY_PER_INCH   1440
#define HOST_BORDER 0
#endif

//class ITextServices;
//class ITextDocument;

typedef struct tagCOOKIE
{
	BSTR	bstrText;
	DWORD	dwSize;
	DWORD	dwCount;
} COOKIE, *PCOOKIE;

/////////////////////////////////////////////////////////////////////////////
// IFormatttedTextDraw
interface IFormattedTextDraw
{
public:
	virtual ~IFormattedTextDraw() {};
	virtual HRESULT get_NaturalHeight(long Width, /*[out, retval]*/ long *pVal) = 0;
	virtual HRESULT get_NaturalWidth(long Height, /*[out, retval]*/ long *pVal) = 0;
	virtual HRESULT Create() = 0;
	virtual HRESULT Draw(void *hdcDraw, RECT *prc) = 0;
	virtual HRESULT get_RTFText(/*[out, retval]*/ BSTR *pVal) = 0;
	virtual HRESULT put_RTFText(/*[in]*/ BSTR newVal) = 0;

// COM-like functions
    virtual ULONG STDMETHODCALLTYPE AddRef(void) = 0;
    virtual ULONG STDMETHODCALLTYPE Release(void) = 0;
};


/////////////////////////////////////////////////////////////////////////////
// CFormattedTextDraw
class CFormattedTextDraw : 
	public ITextHost,
	public IFormattedTextDraw
{
public:
	CFormattedTextDraw(HMODULE hmod)
	{
		this->hmod = hmod;
		HDC hdcScreen;

		m_RTFText = NULL;

		hdcScreen = GetDC(NULL);
		nPixelsPerInchX = GetDeviceCaps(hdcScreen, LOGPIXELSX);
		nPixelsPerInchY = GetDeviceCaps(hdcScreen, LOGPIXELSY);
		ReleaseDC(NULL, hdcScreen);

		SetRectEmpty(&m_rcClient);
		SetRectEmpty(&m_rcViewInset);

		m_pCF = (CHARFORMAT2W*) malloc(sizeof(CHARFORMAT2W));

		InitDefaultCharFormat();
		InitDefaultParaFormat();
		m_spTextServices = NULL;
		//m_spTextDocument = NULL;

		m_dwPropertyBits = TXTBIT_RICHTEXT | TXTBIT_MULTILINE | TXTBIT_WORDWRAP | TXTBIT_USECURRENTBKG;
		m_dwScrollbar = 0;
		m_dwMaxLength = INFINITE;
	}

	~CFormattedTextDraw();

// Minimal COM functionality
    HRESULT STDMETHODCALLTYPE QueryInterface( 
        /* [in] */ REFIID riid,
        /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject)
	{
		*ppvObject = NULL;
		return S_FALSE;
	}
    
    ULONG STDMETHODCALLTYPE AddRef(void)
	{
		return 0;
	}
    
    ULONG STDMETHODCALLTYPE Release(void)
	{
		return 0;
	}

// IFormattedTextDraw
public:
	HRESULT get_NaturalHeight(long Width, /*[out, retval]*/ long *pVal);
	HRESULT get_NaturalWidth(long Height, /*[out, retval]*/ long *pVal);
	HRESULT Create();
	HRESULT Draw(void *hdcDraw, RECT *prc);
	HRESULT get_RTFText(/*[out, retval]*/ BSTR *pVal);
	HRESULT put_RTFText(/*[in]*/ BSTR newVal);

// ITextHost
	HDC TxGetDC();
	INT TxReleaseDC(HDC hdc);
	BOOL TxShowScrollBar(INT fnBar, BOOL fShow);
	BOOL TxEnableScrollBar(INT fuSBFlags, INT fuArrowflags);
	BOOL TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw);
	BOOL TxSetScrollPos(INT fnBar, INT nPos, BOOL fRedraw);
	void TxInvalidateRect(LPCRECT prc, BOOL fMode);
	void TxViewChange(BOOL fUpdate);
	BOOL TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight);
	BOOL TxShowCaret(BOOL fShow);
	BOOL TxSetCaretPos(INT x, INT y);
	BOOL TxSetTimer(UINT idTimer, UINT uTimeout);
	void TxKillTimer(UINT idTimer);
	void TxScrollWindowEx(INT dx, INT dy, LPCRECT lprcScroll, LPCRECT lprcClip, HRGN hrgnUpdate, LPRECT lprcUpdate, UINT fuScroll);
	void TxSetCapture(BOOL fCapture);
	void TxSetFocus();
	void TxSetCursor(HCURSOR hcur, BOOL fText);
	BOOL TxScreenToClient(LPPOINT lppt);
	BOOL TxClientToScreen(LPPOINT lppt);
	HRESULT	TxActivate(LONG * plOldState);
	HRESULT	TxDeactivate(LONG lNewState);
	HRESULT	TxGetClientRect(LPRECT prc);
	HRESULT	TxGetViewInset(LPRECT prc);
	HRESULT TxGetCharFormat(const CHARFORMATW **ppCF);
	HRESULT	TxGetParaFormat(const PARAFORMAT **ppPF);
	COLORREF TxGetSysColor(int nIndex);
	HRESULT	TxGetBackStyle(TXTBACKSTYLE *pstyle);
	HRESULT	TxGetMaxLength(DWORD *plength);
	HRESULT	TxGetScrollBars(DWORD *pdwScrollBar);
	HRESULT	TxGetPasswordChar(TCHAR *pch);
	HRESULT	TxGetAcceleratorPos(LONG *pcp);
	HRESULT	TxGetExtent(LPSIZEL lpExtent);
	HRESULT OnTxCharFormatChange(const CHARFORMATW * pcf);
	HRESULT	OnTxParaFormatChange(const PARAFORMAT * ppf);
	HRESULT	TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits);
	HRESULT	TxNotify(DWORD iNotify, void *pv);
	HIMC TxImmGetContext();
	void TxImmReleaseContext(HIMC himc);
	HRESULT	TxGetSelectionBarWidth(LONG *lSelBarWidth);

// Custom functions
	HRESULT SetTextColor(COLORREF clr);
	HRESULT SetTextFont(HFONT font);
	HRESULT CharFormatFromHFONT(CHARFORMAT2W* pCF, HFONT hFont);
	HRESULT InitDefaultCharFormat();
	HRESULT InitDefaultParaFormat();
	HRESULT CreateTextServicesObject();

// Variables
	RECT			m_rcClient;			// Client Rect
	RECT			m_rcViewInset;		// view rect inset
	SIZEL			m_sizelExtent;		// Extent array

	int				nPixelsPerInchX;    // Pixels per logical inch along width
	int				nPixelsPerInchY;    // Pixels per logical inch along height

	CHARFORMAT2W	*m_pCF;
	PARAFORMAT2		m_PF;
	DWORD			m_dwScrollbar;		// Scroll bar style
	DWORD			m_dwPropertyBits;	// Property bits
	DWORD			m_dwMaxLength;
	COOKIE			m_editCookie;

	ITextServices	*m_spTextServices;
	//ITextDocument	*m_spTextDocument;
	BSTR			m_RTFText;

	HMODULE hmod;
};

#endif //__FORMATTEDTEXTDRAW_H_
