// OutMessagesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NanoTubesControlCenter.h"
#include "OutMessagesDlg.h"
#include "OutMessage.h"
#include "./Automation/common.h"
#include "AppHelpers.h"

#ifdef _DEBUG
#define new DEBUG_NEW	
#endif
// COutMessagesDlg dialog

IMPLEMENT_DYNAMIC(COutMessagesDlg, CExtResizableDialog)

COutMessagesDlg::COutMessagesDlg(CWnd* pParent /*=NULL*/)
	: CExtResizableDialog(COutMessagesDlg::IDD, pParent)
{

}

COutMessagesDlg::~COutMessagesDlg()
{
	m_wndMsgList.DestroyWindow();
}

void COutMessagesDlg::DoDataExchange(CDataExchange* pDX)
{
	CExtResizableDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_MSG, m_wndMsgList);
	DDX_Control(pDX, IDC_OUT_MSG_TOOL_BAR, m_wndToolBar);
}

BOOL COutMessagesDlg::InitMsgList()
{
	
	VERIFY( m_ImageList.Create( 24, 24, ILC_COLOR24|ILC_MASK, 4, 4 ) );
	m_bitmap.LoadBitmap(IDB_MSG_ICONS_LIST);

	m_ImageList.Add(&m_bitmap, RGB(255, 0, 255));

	int i = m_ImageList.GetImageCount();

	m_wndMsgList.SetImageList(&m_ImageList, LVSIL_NORMAL);
	m_wndMsgList.SetImageList(&m_ImageList, LVSIL_SMALL);

	m_wndMsgList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_FLATSB);

	LV_COLUMN lvc;
	memset(&lvc, 0, sizeof(LV_COLUMN));

	lvc.mask = LVCF_TEXT|LVCF_WIDTH|LVCF_FMT|LVCFMT_IMAGE;

	lvc.iSubItem = 0; lvc.pszText = "#"; lvc.fmt = LVCFMT_LEFT|LVCFMT_IMAGE; lvc.cx = 40;
	m_wndMsgList.InsertColumn(0, &lvc);

	lvc.iSubItem = 1; lvc.pszText = "Источник"; lvc.fmt = LVCFMT_LEFT; lvc.cx = 80;
	m_wndMsgList.InsertColumn(1, &lvc);

	lvc.iSubItem = 2; lvc.pszText = "Время"; lvc.fmt = LVCFMT_LEFT; lvc.cx = 80;
	m_wndMsgList.InsertColumn(2, &lvc);

	lvc.iSubItem = 3; lvc.pszText = "Сообщение"; lvc.fmt = LVCFMT_LEFT; lvc.cx = 800;
	m_wndMsgList.InsertColumn(3, &lvc);

	return TRUE;
}

BEGIN_MESSAGE_MAP(COutMessagesDlg, CExtResizableDialog)
	ON_MESSAGE( RM_SHOW_USER_MESSAGE, OnMessageComming )
	ON_WM_SIZE()	
END_MESSAGE_MAP()


// COutMessagesDlg message handlers

BOOL COutMessagesDlg::OnInitDialog()
{
	CExtResizableDialog::OnInitDialog();

	InitMsgList();

	RepositionBars(0, 0xFFFF, IDC_LIST_MSG); 

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT COutMessagesDlg::OnMessageComming(WPARAM wParam, LPARAM lParam)
{
	COutMessage* pOutMsg = (COutMessage*)lParam;
	
	LVITEM lvi;
	memset(&lvi, 0, sizeof(LVITEM));
	lvi.mask = LVIF_IMAGE;

	lvi.iImage = pOutMsg->MessageType();
	lvi.iItem =  m_wndMsgList.GetItemCount();
	lvi.iSubItem = 0;

	
	m_wndMsgList.InsertItem( &lvi );	
	m_wndMsgList.SetItemText(lvi.iItem , 2, PrintTime().c_str() );

// 	CString msg = pOutMsg->MessageText();
// 	int curPos = 0;
// 	CString res = msg.Tokenize("\n", curPos);
// 	CArray<CString> lines;
// 
// 	while (res != "")
// 	{
// 		lines.Add(res);
// 		res = msg.Tokenize("\n", curPos);
// 	}

	
	m_wndMsgList.SetItemText(lvi.iItem , 3,  pOutMsg->MessageText());
	m_wndMsgList.EnsureVisible(lvi.iItem, FALSE);
	m_wndMsgList.UpdateWindow();
	m_wndMsgList.InvalidateRect(NULL);

	

	return 0L;	
}

void COutMessagesDlg::OnSize(UINT nType, int cx, int cy)
{
	CExtResizableDialog::OnSize(nType, cx, cy);
	RepositionBars(0, 0xFFFF, IDC_LIST_MSG); 
	// TODO: Add your message handler code here
}

