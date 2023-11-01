// Feel free to use this code in your own applications.
// The Author does not guarantee anything about this code.
// Author : Yves Maurer

// FormattedTextDraw.cpp : Implementation of CFormattedTextDraw
#include "stdafx.h"
#include "FormattedTextDraw.h"
#include <tom.h>

// These definitions might give you problems.  If they do, open your
// textserv.h and comment the following lines out. They are right at the top
// EXTERN_C const IID IID_ITextServices;
// EXTERN_C const IID IID_ITextHost;

const IID IID_ITextServices = { // 8d33f740-cf58-11ce-a89d-00aa006cadc5
    0x8d33f740,
    0xcf58,
    0x11ce,
    {0xa8, 0x9d, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5}
  };

const IID IID_ITextHost = { // c5bdd8d0-d26e-11ce-a89e-00aa006cadc5
    0xc5bdd8d0,
    0xd26e,
    0x11ce,
    {0xa8, 0x9e, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5}
  };


/////////////////////////////////////////////////////////////////////////////
// CallBack functions

DWORD CALLBACK EditStreamInCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
	PCOOKIE pCookie = (PCOOKIE) dwCookie;

	if(pCookie->dwSize - pCookie->dwCount < (DWORD) cb)
		*pcb = pCookie->dwSize - pCookie->dwCount;
	else
		*pcb = cb;

	CopyMemory(pbBuff, pCookie->bstrText, *pcb);
	pCookie->dwCount += *pcb;

	return 0;	//	callback succeeded - no errors
}

/////////////////////////////////////////////////////////////////////////////
// CFormattedTextDraw

CFormattedTextDraw::~CFormattedTextDraw()
{
	if (m_RTFText != NULL)
		SysFreeString(m_RTFText);

	free(m_pCF);
	if (m_spTextServices != NULL)
		m_spTextServices->Release();
	//if (m_spTextDocument != NULL)
	//	m_spTextDocument->Release();
}

HRESULT CFormattedTextDraw::get_RTFText(BSTR *pVal)
{
	*pVal = SysAllocStringLen(m_RTFText, SysStringLen(m_RTFText));
	return S_OK;
}

HRESULT CFormattedTextDraw::put_RTFText(BSTR newVal)
{
	HRESULT hr;
	long    len;
	LRESULT lResult = 0;
	EDITSTREAM editStream;

	if (m_RTFText != NULL)
		SysFreeString(m_RTFText);
	len = SysStringLen(newVal);
	m_RTFText = SysAllocStringLen(newVal, len);

	if (!m_spTextServices) 
		return S_FALSE;

	m_editCookie.bstrText = (BSTR) malloc(len + 1);
	WideCharToMultiByte(CP_ACP, 0, m_RTFText, len, (char *) m_editCookie.bstrText, len, NULL, NULL);
	m_editCookie.dwSize = lstrlenA((LPSTR) m_editCookie.bstrText);
	m_editCookie.dwCount = 0;

	editStream.dwCookie = (DWORD_PTR) &m_editCookie;
	editStream.dwError = 0;
	editStream.pfnCallback = EditStreamInCallback;
	hr = m_spTextServices->TxSendMessage(EM_STREAMIN, (WPARAM)(SF_RTF | SF_UNICODE), (LPARAM)&editStream, &lResult);
	free(m_editCookie.bstrText);
	return S_OK;
}

HRESULT CFormattedTextDraw::SetTextColor(COLORREF clr)
{
	LRESULT hr;
	m_spTextServices->TxSendMessage(EM_HIDESELECTION, 1L, 0L, &hr);

	CHARRANGE cr;
	cr.cpMin = 0;
	cr.cpMax = -1;
	m_spTextServices->TxSendMessage(EM_EXSETSEL, 0L, (LPARAM)&cr, &hr);

	CHARFORMAT cf;
	cf.cbSize = sizeof(cf);
	cf.crTextColor = clr;
	cf.dwMask = CFM_COLOR;
	cf.dwEffects = 0;
	m_spTextServices->TxSendMessage(EM_SETCHARFORMAT, 0L, (LPARAM)&cf, &hr);

	return S_OK;
}

HRESULT CFormattedTextDraw::SetTextFont(HFONT font)
{
	LRESULT hr;
	m_spTextServices->TxSendMessage(EM_HIDESELECTION, 1L, 0L, &hr);

	CHARRANGE cr;
	cr.cpMin = 0;
	cr.cpMax = -1;
	m_spTextServices->TxSendMessage(EM_EXSETSEL, 0L, (LPARAM)&cr, &hr);

	CHARFORMAT2W cf;
	cf.cbSize = sizeof(cf);

	CharFormatFromHFONT(&cf, font);

	m_spTextServices->TxSendMessage(EM_SETCHARFORMAT, 0L, (LPARAM)&cf, &hr);

	return S_OK;
}

HRESULT CFormattedTextDraw::Draw(void *hdcDraw, RECT *prc)
{
	m_spTextServices->TxDraw(
	    DVASPECT_CONTENT,  		// Draw Aspect
		0,						// Lindex
		NULL,					// Info for drawing optimization
		NULL,					// target device information
		(HDC) hdcDraw,				// Draw device HDC
		NULL,			 	   	// Target device HDC
		(RECTL *) prc,			// Bounding client rectangle
		NULL,					// Clipping rectangle for metafiles
		(RECT *) NULL,			// Update rectangle
		NULL, 	   				// Call back function
		NULL,					// Call back parameter
		TXTVIEW_INACTIVE);		// What view of the object could be TXTVIEW_ACTIVE
	return S_OK;
}

HRESULT CFormattedTextDraw::Create()
{
	return CreateTextServicesObject();
}

HRESULT CFormattedTextDraw::get_NaturalWidth(long Height, long *pVal)
{
	long lWidth;
	SIZEL szExtent;
	HDC	hdcDraw;

	if (!m_spTextServices)
		return S_FALSE;

	hdcDraw = GetDC(NULL);
	szExtent.cy = -1;
	szExtent.cx = -1;
	lWidth = 10000;
	m_spTextServices->TxGetNaturalSize(DVASPECT_CONTENT, 
		hdcDraw, 
		NULL,
		NULL,
		TXTNS_FITTOCONTENT,
		&szExtent,
		&lWidth,
		&Height);

	ReleaseDC(NULL, hdcDraw);
	*pVal = lWidth;
	return S_OK;
}

HRESULT CFormattedTextDraw::get_NaturalHeight(long Width, long *pVal)
{
	long lHeight;
	SIZEL szExtent;
	HDC	hdcDraw;

	if (!m_spTextServices)
		return S_FALSE;

	hdcDraw = GetDC(NULL);
	szExtent.cx = -1;
	szExtent.cy = -1;
	lHeight = 0;
	m_spTextServices->TxGetNaturalSize(DVASPECT_CONTENT, 
		hdcDraw, 
		NULL,
		NULL,
		TXTNS_FITTOCONTENT,
		&szExtent,
		&Width,
		&lHeight);

	ReleaseDC(NULL, hdcDraw);
	*pVal = lHeight;
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// ITextHost functions
HDC CFormattedTextDraw::TxGetDC()
{
	return NULL;
}

INT CFormattedTextDraw::TxReleaseDC(HDC hdc)
{
	return 1;
}

BOOL CFormattedTextDraw::TxShowScrollBar(INT fnBar, BOOL fShow)
{
	return FALSE;
}

BOOL CFormattedTextDraw::TxEnableScrollBar(INT fuSBFlags, INT fuArrowflags)
{
	return FALSE;
}

BOOL CFormattedTextDraw::TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw)
{
	return FALSE;
}

BOOL CFormattedTextDraw::TxSetScrollPos(INT fnBar, INT nPos, BOOL fRedraw)
{
	return FALSE;
}

void CFormattedTextDraw::TxInvalidateRect(LPCRECT prc, BOOL fMode)
{
}

void CFormattedTextDraw::TxViewChange(BOOL fUpdate)
{
}

BOOL CFormattedTextDraw::TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight)
{
	return FALSE;
}

BOOL CFormattedTextDraw::TxShowCaret(BOOL fShow)
{
	return FALSE;
}

BOOL CFormattedTextDraw::TxSetCaretPos(INT x, INT y)
{
	return FALSE;
}

BOOL CFormattedTextDraw::TxSetTimer(UINT idTimer, UINT uTimeout)
{
	return FALSE;
}

void CFormattedTextDraw::TxKillTimer(UINT idTimer)
{
}

void CFormattedTextDraw::TxScrollWindowEx(INT dx, INT dy, LPCRECT lprcScroll, LPCRECT lprcClip, HRGN hrgnUpdate, LPRECT lprcUpdate, UINT fuScroll)
{
}

void CFormattedTextDraw::TxSetCapture(BOOL fCapture)
{
}

void CFormattedTextDraw::TxSetFocus()
{
}

void CFormattedTextDraw::TxSetCursor(HCURSOR hcur, BOOL fText)
{
}

BOOL CFormattedTextDraw::TxScreenToClient(LPPOINT lppt)
{
	return FALSE;
}

BOOL CFormattedTextDraw::TxClientToScreen(LPPOINT lppt)
{
	return FALSE;
}

HRESULT	CFormattedTextDraw::TxActivate(LONG * plOldState)
{
	return S_OK;
}

HRESULT	CFormattedTextDraw::TxDeactivate(LONG lNewState)
{
	return S_OK;
}

HRESULT	CFormattedTextDraw::TxGetClientRect(LPRECT prc)
{
	*prc = m_rcClient;
	return S_OK;
}

HRESULT	CFormattedTextDraw::TxGetViewInset(LPRECT prc)
{
	*prc = m_rcViewInset;
	return S_OK;
}

HRESULT CFormattedTextDraw::TxGetCharFormat(const CHARFORMATW **ppCF)
{
	*ppCF = m_pCF;
	return S_OK;
}

HRESULT	CFormattedTextDraw::TxGetParaFormat(const PARAFORMAT **ppPF)
{
	*ppPF = &m_PF;
	return S_OK;
}

COLORREF CFormattedTextDraw::TxGetSysColor(int nIndex)
{
	return GetSysColor(nIndex);
}

HRESULT	CFormattedTextDraw::TxGetBackStyle(TXTBACKSTYLE *pstyle)
{
	*pstyle = TXTBACK_TRANSPARENT;
	return S_OK;
}

HRESULT	CFormattedTextDraw::TxGetMaxLength(DWORD *plength)
{
	*plength = m_dwMaxLength;
	return S_OK;
}

HRESULT	CFormattedTextDraw::TxGetScrollBars(DWORD *pdwScrollBar)
{
	*pdwScrollBar = m_dwScrollbar;
	return S_OK;
}

HRESULT	CFormattedTextDraw::TxGetPasswordChar(TCHAR *pch)
{
	return S_FALSE;
}

HRESULT	CFormattedTextDraw::TxGetAcceleratorPos(LONG *pcp)
{
	*pcp = -1;
	return S_OK;
}

HRESULT	CFormattedTextDraw::TxGetExtent(LPSIZEL lpExtent)
{
	return E_NOTIMPL;
}

HRESULT CFormattedTextDraw::OnTxCharFormatChange(const CHARFORMATW * pcf)
{
	memcpy(m_pCF, pcf, pcf->cbSize);
	return S_OK;
}

HRESULT	CFormattedTextDraw::OnTxParaFormatChange(const PARAFORMAT * ppf)
{
	memcpy(&m_PF, ppf, ppf->cbSize);
	return S_OK;
}

HRESULT	CFormattedTextDraw::TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits)
{
	*pdwBits = m_dwPropertyBits;
	return S_OK;
}

HRESULT	CFormattedTextDraw::TxNotify(DWORD iNotify, void *pv)
{
	return S_OK;
}

HIMC CFormattedTextDraw::TxImmGetContext()
{
	return NULL;
}

void CFormattedTextDraw::TxImmReleaseContext(HIMC himc)
{
}

HRESULT	CFormattedTextDraw::TxGetSelectionBarWidth(LONG *lSelBarWidth)
{
	*lSelBarWidth = 100;
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// custom functions

HRESULT CFormattedTextDraw::CharFormatFromHFONT(CHARFORMAT2W* pCF, HFONT hFont)
// Takes an HFONT and fills in a CHARFORMAT2W structure with the corresponding info
{

	HWND hWnd;
	LOGFONT lf;
	HDC hDC;
	LONG yPixPerInch;

	// Get LOGFONT for default font
	if (!hFont)
		hFont = (HFONT) GetStockObject(SYSTEM_FONT);

	// Get LOGFONT for passed hfont
	if (!GetObject(hFont, sizeof(LOGFONT), &lf))
		return E_FAIL;

	// Set CHARFORMAT structure
	memset(pCF, 0, sizeof(CHARFORMAT2W));
	pCF->cbSize = sizeof(CHARFORMAT2W);

	hWnd = GetDesktopWindow();
	hDC = GetDC(hWnd);
	yPixPerInch = GetDeviceCaps(hDC, LOGPIXELSY);
	pCF->yHeight = -lf.lfHeight * LY_PER_INCH / yPixPerInch;
	ReleaseDC(hWnd, hDC);

	pCF->yOffset = 0;
	pCF->crTextColor = 0;

	pCF->dwEffects = CFM_EFFECTS | CFE_AUTOBACKCOLOR;
	pCF->dwEffects &= ~(CFE_PROTECTED | CFE_LINK | CFE_AUTOCOLOR);

	if(lf.lfWeight < FW_BOLD)
		pCF->dwEffects &= ~CFE_BOLD;

	if(!lf.lfItalic)
		pCF->dwEffects &= ~CFE_ITALIC;

	if(!lf.lfUnderline)
		pCF->dwEffects &= ~CFE_UNDERLINE;

	if(!lf.lfStrikeOut)
		pCF->dwEffects &= ~CFE_STRIKEOUT;

	pCF->dwMask = CFM_ALL | CFM_BACKCOLOR | CFM_STYLE;
	pCF->bCharSet = lf.lfCharSet;
	pCF->bPitchAndFamily = lf.lfPitchAndFamily;

#ifdef UNICODE
	_tcscpy(pCF->szFaceName, lf.lfFaceName);
#else
	MultiByteToWideChar(CP_ACP, 0, lf.lfFaceName, LF_FACESIZE, pCF->szFaceName, LF_FACESIZE);
#endif

	return S_OK;
}

HRESULT CFormattedTextDraw::InitDefaultCharFormat()
{
	return CharFormatFromHFONT(m_pCF, NULL);
}

HRESULT CFormattedTextDraw::InitDefaultParaFormat()
{
	memset(&m_PF, 0, sizeof(PARAFORMAT2));
	m_PF.cbSize = sizeof(PARAFORMAT2);
	m_PF.dwMask = PFM_ALL;
	m_PF.wAlignment = PFA_LEFT;
	m_PF.cTabCount = 1;
	m_PF.rgxTabs[0] = lDefaultTab;
	return S_OK;
}

typedef HRESULT (STDAPICALLTYPE *TextServicesProcType)(IUnknown *punkOuter, ITextHost *pITextHost, IUnknown **ppUnk);

HRESULT CFormattedTextDraw::CreateTextServicesObject()
{
	HRESULT hr;
	IUnknown *spUnk;

	//const IID IID_ITextDocument = {0x8CC497C0, 0xA1DF, 0x11CE, {0x80, 0x98,	0x00, 0xAA, 0x00, 0x47, 0xBE, 0x5D}};


	TextServicesProcType TextServicesProc = 0;
	if (hmod) 
		TextServicesProc = (TextServicesProcType)GetProcAddress(hmod, "CreateTextServices");
	
	if (!TextServicesProc)
		return S_FALSE;
	hr = TextServicesProc(NULL, static_cast<ITextHost*>(this), &spUnk);
	if (hr == S_OK) {
		hr = spUnk->QueryInterface(IID_ITextServices, (void**)&m_spTextServices);
		//hr = spUnk->QueryInterface(IID_ITextDocument, (void**)&m_spTextDocument);
		//spUnk->Release();
	}
/*
	hr = CreateTextServices(NULL, static_cast<ITextHost*>(this), &spUnk);
	if (hr == S_OK) {
		hr = spUnk->QueryInterface(IID_ITextServices, (void**)&m_spTextServices);
		hr = spUnk->QueryInterface(IID_ITextDocument, (void**)&m_spTextDocument);
		spUnk->Release();
	}
*/
	return hr;
}

