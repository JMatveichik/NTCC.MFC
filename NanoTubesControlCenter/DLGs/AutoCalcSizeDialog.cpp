// AutoCalcSizeDialog.cpp : implementation file
//

#include "stdafx.h"
#include "..\NanoTubesControlCenter.h"
#include "AutoCalcSizeDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW	
#endif
// CAutoCalcSizeDialog dialog

IMPLEMENT_DYNAMIC(CAutoCalcSizeDialog, CExtResizableDialog)

CAutoCalcSizeDialog::CAutoCalcSizeDialog(UINT ID, CWnd* pParent /*=NULL*/)
	: CExtResizableDialog(ID, pParent), m_sz(0,0)
{

}

CAutoCalcSizeDialog::~CAutoCalcSizeDialog()
{
}

void CAutoCalcSizeDialog::DoDataExchange(CDataExchange* pDX)
{
	CExtResizableDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAutoCalcSizeDialog, CExtResizableDialog)
END_MESSAGE_MAP()




// CAutoCalcSizeDialog message handlers

BOOL CAutoCalcSizeDialog::OnInitDialog()
{
	CExtResizableDialog::OnInitDialog();

	for (HWND hWndChild = ::GetTopWindow(m_hWnd); hWndChild != NULL;	hWndChild = ::GetNextWindow(hWndChild, GW_HWNDNEXT) )
	{
		RECT rc;
		::GetWindowRect(hWndChild, &rc);
		ScreenToClient(&rc);
		if ( rc.bottom > m_sz.cy )
			m_sz.cy = rc.bottom;

		if ( rc.right > m_sz.cx )
			m_sz.cx = rc.right;
	}
	m_sz.cy += 16;
	m_sz.cx += 16;

	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
