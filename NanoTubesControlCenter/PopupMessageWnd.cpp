// PopupMessageWnd.cpp : implementation file
//

#include "stdafx.h"
//#include "NanoTubesControlCenter.h"
#include "PopupMessageWnd.h"
#include "ah_msgprovider.h"
#include "ah_images.h"

#ifdef _DEBUG
#define new DEBUG_NEW	
#endif

// CPopupMessageWnd

IMPLEMENT_DYNAMIC(CPopupMessageWnd, CWnd)


std::list<CWnd*> CPopupMessageWnd::activeWindows;
CCriticalSection CPopupMessageWnd::closeCS; 


//0 - невидимое
//255 - полностью непрозрачное
BYTE CPopupMessageWnd::transparencyLevel = 255; 

CPopupMessageWnd::CPopupMessageWnd()
{
	
}

CPopupMessageWnd::~CPopupMessageWnd()
{
	//::DeleteCriticalSection(&closeCS);
}


BEGIN_MESSAGE_MAP(CPopupMessageWnd, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_SHOWWINDOW()
	ON_WM_CREATE()
	ON_MESSAGE( WM_CTLCOLORSTATIC, OnCtlColorStatic )	
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()



// CPopupMessageWnd message handlers




BOOL CPopupMessageWnd::OnEraseBkgnd(CDC* pDC)
{
	::SetBkMode(pDC->m_hDC, TRANSPARENT );

	CExtMemoryDC mdc(pDC);
	Gdiplus::Graphics gr(mdc.m_hDC);

	Gdiplus::ColorMatrix ClrMatrix =         
	{ 
		1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 1.0f
	};

	//ClrMatrix.m[3][3] = 20.0 / 100.0;

	Gdiplus::ImageAttributes ImgAttr;

	ImgAttr.SetColorMatrix(&ClrMatrix, Gdiplus::ColorMatrixFlagsDefault, Gdiplus::ColorAdjustTypeBitmap);

	Gdiplus::Image* pImg = OnNeedBkImage();

	if (pImg != NULL)
		gr.DrawImage(pImg, 
				Gdiplus::RectF(0.0f, 0.0f, (Gdiplus::REAL)pImg->GetWidth(), (Gdiplus::REAL)pImg->GetHeight() ), 
				0, 
				0,  
				(Gdiplus::REAL)pImg->GetWidth(),
				(Gdiplus::REAL)pImg->GetHeight(), 
				Gdiplus::UnitPixel, 
				&ImgAttr);

	
	return TRUE;//CWnd::OnEraseBkgnd(pDC);
}

void CPopupMessageWnd::ShowMessage(COutMessage* pOutMsg)
{

	for ( std::list<CWnd*>::iterator it = activeWindows.begin(); it != activeWindows.end(); ++it)
	{
		CPopupMessageWnd* pWin = (CPopupMessageWnd*)(*it);		

		CString msgText = pOutMsg->MessageText();
		CString wndText;
		pWin->m_lblText.GetWindowText(wndText);
		if (msgText == wndText)
		{
			activeWindows.erase(itSelf);

			DestroyWindow();
			delete this;

			return;
		}
	}
	

	m_lblText.SetWindowText( pOutMsg->MessageText() );
	m_lblTitle.SetWindowText( pOutMsg->MessageTitle() );
	
	m_tmStart = CTime::GetCurrentTime();
	m_lblInfo.SetWindowText(m_tmStart.Format("%H:%M:%S"));
	
	UINT timeToLive = 0;

	switch ( pOutMsg->MessageType() )
	{
	case COutMessage::MSG_OK:
	case COutMessage::MSG_OBA_SCRIPT:
		timeToLive = 3000;
		type = 0;
		break;
	case COutMessage::MSG_WARNING:	
		timeToLive = 10000;
		type = 1;
		break;
	case COutMessage::MSG_ERROR:
		type= 2;
		break;
	}

	ShowWindow(SW_SHOW);

 	if ( timeToLive > 0 )
 		SetTimer(1, timeToLive, NULL);
}

Gdiplus::Image* CPopupMessageWnd::OnNeedBkImage() const
{
	Gdiplus::Image* pImg = NULL;
	
	switch (type)
	{
	case 0:
		pImg = AppImagesHelper::Instance().GetImage("popupbg_info.png");
		
		break;
	case 1:
		pImg = AppImagesHelper::Instance().GetImage("popupbg_warning.png");
		break;
	case 2:
		pImg = AppImagesHelper::Instance().GetImage("popupbg_alarm.png");
		break;
	}

	return pImg; 
}


BOOL CPopupMessageWnd::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style = WS_POPUP;
	cs.hMenu = (HMENU)NULL;

	closeCS.Lock();
	if ( !activeWindows.empty() )
	{
		CWnd* pTopWin = activeWindows.back();
		CRect topRect;
		pTopWin->GetWindowRect(&topRect);

		UINT padding = 5;
		topRect.OffsetRect(0, -int( topRect.Height() + padding));

		cs.x =  topRect.left;
		cs.y =  topRect.top;

		cs.cx =  topRect.Width();
		cs.cy =  topRect.Height();

	}
	closeCS.Unlock();

	return CWnd::PreCreateWindow(cs);
}

void CPopupMessageWnd::CloseWindow()
{
	closeCS.Lock();

	std::list<CWnd*>::iterator it = itSelf;

	AnimateWindow(300, AW_HIDE|AW_BLEND|AW_HOR_POSITIVE);	

	for ( ++it; it != activeWindows.end(); ++it)
	{
		CWnd* pWin = (*it);
		CRect topRect;
		pWin->GetWindowRect(&topRect);

		UINT padding = 5;
		topRect.OffsetRect(0, ( topRect.Height() + padding));

		pWin->SetWindowPos( NULL, topRect.left, topRect.top, 0, 0 , SWP_NOSIZE );
		pWin->AnimateWindow(300, AW_ACTIVATE|AW_SLIDE|AW_VER_POSITIVE);			
	}
	
	activeWindows.erase(itSelf);
	
	DestroyWindow();
	delete this;

	closeCS.Unlock();	
}


void CPopupMessageWnd::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if ( bShow )
	{
		AnimateWindow(500, AW_SLIDE|AW_VER_NEGATIVE);
	}
	else
	{
		CloseWindow();
	}
	//CWnd::OnShowWindow(bShow, nStatus);
}

int CPopupMessageWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	::SetWindowLongPtr(m_hWnd, GWL_EXSTYLE, GetWindowLongPtr(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	::SetLayeredWindowAttributes(m_hWnd, RGB(0,0,0), transparencyLevel, LWA_ALPHA);

	tme.cbSize		= sizeof(tme);
	tme.hwndTrack	= m_hWnd;
	tme.dwFlags		= TME_LEAVE;

	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));

	// TODO:  Add your specialized creation code here
	m_lblTitle.Create("«аголовок", WS_CHILD|WS_VISIBLE, CRect(5,5, 395, 35), this);
	
	lf.lfHeight = -MulDiv(14, GetDeviceCaps(GetDC()->m_hDC, LOGPIXELSY), 72);
	lf.lfWeight = FW_MEDIUM;
	lstrcpy( lf.lfFaceName, _T("Segoe UI"));

	VERIFY(msgTitleFont.CreateFontIndirect(&lf));
	m_lblTitle.SetFont(&msgTitleFont);

	m_lblText.Create("",WS_CHILD|WS_VISIBLE, CRect(10, 40, 395, 150), this);

	lf.lfHeight = -MulDiv(10, GetDeviceCaps(GetDC()->m_hDC, LOGPIXELSY), 72);
	lf.lfWeight = FW_REGULAR;
	VERIFY(msgMessageFont.CreateFontIndirect(&lf));

	m_lblText.SetFont(&msgMessageFont);

	m_lblInfo.Create("", WS_CHILD|WS_VISIBLE|SS_RIGHT, CRect(5, 5, 395, 150), this);

	lf.lfHeight = -MulDiv(14, GetDeviceCaps(GetDC()->m_hDC, LOGPIXELSY), 72);
	lf.lfWeight = FW_BOLD;
	VERIFY(msgInfoFont.CreateFontIndirect(&lf));

	m_lblInfo.SetFont(&msgInfoFont);

	closeCS.Lock();

	itSelf = activeWindows.insert(activeWindows.end(), this);

	closeCS.Unlock();


	return 0;
}


LRESULT CPopupMessageWnd::OnCtlColorStatic( WPARAM wParam, LPARAM lParam) 
{
	::SetBkMode( HDC( wParam ), TRANSPARENT );
	//::SetTextColor( HDC( wParam ), RGB(64,64,64) );
	return LRESULT( ::GetStockObject( NULL_BRUSH ) );
}

void CPopupMessageWnd::OnTimer(UINT_PTR nIDEvent)
{
	ShowWindow(SW_HIDE);
}


void CPopupMessageWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	//CWnd::OnLButtonDown(nFlags, point);
	ShowWindow(SW_HIDE);
}


void CPopupMessageWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	TrackMouseEvent(&tme);
	::SetLayeredWindowAttributes(m_hWnd,RGB(0,0,0), 255, LWA_ALPHA);

	CWnd::OnMouseMove(nFlags, point);
}

BOOL CPopupMessageWnd::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if ( message == WM_MOUSELEAVE || message == WM_NCMOUSELEAVE)
	{
		::SetLayeredWindowAttributes(m_hWnd,RGB(0,0,0), transparencyLevel, LWA_ALPHA);		
	}

	return CWnd::OnWndMsg(message, wParam, lParam, pResult);
}