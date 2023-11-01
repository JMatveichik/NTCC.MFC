// ProjectFrame.cpp : implementation file
//

#include "stdafx.h"
#include "NanoTubesControlCenter.h"
#include "ProjectFrame.h"
#include "ProjectDoc.h"



#ifdef _DEBUG
#define new DEBUG_NEW	
#endif
// CProjectFrame

IMPLEMENT_DYNCREATE(CProjectFrame, CMDIChildWnd)

CProjectFrame::CProjectFrame()
{

}

CProjectFrame::~CProjectFrame()
{
}


BEGIN_MESSAGE_MAP(CProjectFrame, CMDIChildWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CProjectFrame message handlers

BOOL CProjectFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class
// 	cs.style = WS_CHILD | FWS_ADDTOTITLE | WS_OVERLAPPED | WS_CAPTION | WS_MAXIMIZE;
// 	cs.dwExStyle &= ~(WS_EX_CLIENTEDGE|WS_EX_STATICEDGE|WS_EX_DLGMODALFRAME);

	if(cs.hMenu != NULL)
	{
		::DestroyMenu(cs.hMenu);      // удаляем меню, если загружено
		cs.hMenu = NULL;              // для этого окна меню нет
	}

	return CMDIChildWnd::PreCreateWindow(cs);
}


int CProjectFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if( !CExtControlBar::FrameEnableDocking(this) )
	{
		ASSERT( FALSE );
		return -1;
	}

	if( !CExtControlBar::FrameInjectAutoHideAreas(this) )
	{
		ASSERT( FALSE );
		return -1;
	}

	::SetClassLong(m_hWnd, GCL_HCURSOR, (LONG)AfxGetApp()->LoadCursor(IDC_CUR_ARROW));

	CExtDynamicBarSite::Install( this );	

	//
	//AppUsersHelper::Instance().AddOnUserChanged( boost::bind( &CProjectFrame::OnChangeUser, this, _1 ) );

	return 0;
}


void CProjectFrame::OnChangeUser(const NTCC_APP_USER* pUser)
{
	if ( pUser->ug > TECHNOLOGISTS )
		ActivateFrame( SW_HIDE );
	else {		
		ActivateFrame(SW_SHOWNOACTIVATE);
	}
}

void CProjectFrame::ActivateFrame(int nCmdShow)
{
	if (nCmdShow != SW_HIDE)
		nCmdShow = SW_SHOWMAXIMIZED;

	CMDIChildWnd::ActivateFrame(nCmdShow);	
}
