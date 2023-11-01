// AutoHideDialog.cpp : implementation file
//

#include "stdafx.h"
#include "..\NanoTubesControlCenter.h"
#include "AutoHideDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW	
#endif
// CAutoHideDialog dialog

IMPLEMENT_DYNAMIC(CAutoHideDialog, CExtResizableDialog)

CAutoHideDialog::CAutoHideDialog(UINT nTemlateID, CWnd* pParent /*=NULL*/)
	: CExtResizableDialog(nTemlateID, pParent), 
	showTime(3000), updateTime(500)
{

}

CAutoHideDialog::~CAutoHideDialog()
{
	DestroyWindow();
}

BEGIN_MESSAGE_MAP(CAutoHideDialog, CExtResizableDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
END_MESSAGE_MAP()

void CAutoHideDialog::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if ( bShow )
	{
		SetTimer(TIMER_HIDE, showTime, NULL);
		SetTimer(TIMER_UPDATE, updateTime, NULL);
	}
	else
	{
		KillTimer(TIMER_HIDE);
		KillTimer(TIMER_UPDATE);	
	}
	::AnimateWindow(m_hWnd, 200, AW_ACTIVATE|AW_SLIDE|AW_VER_POSITIVE);
	ShowSizeGrip(FALSE);
	//ShowWindow(SW_SHOW);
	//CWnd::OnShowWindow(bShow, nStatus);	
}

void CAutoHideDialog::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{

	case TIMER_HIDE:
		
		KillTimer(TIMER_HIDE);
		KillTimer(TIMER_UPDATE);
		
		OnHideDialogWnd();
		::AnimateWindow(m_hWnd, 200, AW_HIDE|AW_SLIDE|AW_VER_NEGATIVE);
		//ShowWindow(SW_HIDE);

		break;

	case TIMER_UPDATE:
		
		OnUpdateDialogWnd();

		break;
	}	

	CWnd::OnTimer(nIDEvent);
}

void CAutoHideDialog::OnHideDialogWnd()
{

}

void CAutoHideDialog::OnUpdateDialogWnd()
{

}

// CAutoHideDialog message handlers

BOOL CAutoHideDialog::OnInitDialog()
{
	CExtResizableDialog::OnInitDialog();

	ShowSizeGrip(FALSE);
	SetTimer(TIMER_UPDATE, 1000, NULL);

	tme.cbSize = sizeof(tme);
	tme.hwndTrack = m_hWnd;
	tme.dwFlags = TME_LEAVE;

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CAutoHideDialog::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class

	cs.style &= ~WS_VISIBLE;

	cs.style |= WS_SYSMENU|WS_CAPTION;

	cs.dwExStyle = WS_EX_PALETTEWINDOW;
	

	return CExtResizableDialog::PreCreateWindow(cs);
}


BOOL CAutoHideDialog::PreTranslateMessage(MSG* pMsg)
{
	if ( pMsg->message == WM_MOUSEMOVE )
	{
		KillTimer(TIMER_HIDE);
		TrackMouseEvent(&tme);
	}
	if ( pMsg->message == WM_MOUSELEAVE || pMsg->message == WM_NCMOUSELEAVE )
	{
		SetTimer(TIMER_HIDE, 3000, NULL);		
	}
	if (pMsg->message == WM_LBUTTONDOWN )
	{
		WPARAM SC_DRAGMOVE = 0xF012;
		ReleaseCapture();
		::SendMessage(m_hWnd, WM_SYSCOMMAND, SC_DRAGMOVE, 0);
	}
	return CExtResizableDialog::PreTranslateMessage(pMsg);
}
