	// FSView.cpp : implementation of the CFSView class
//

#include "stdafx.h"
#include "NanoTubesControlCenter.h"

#include "FSDoc.h"
#include "FSDocItem.h"
#include "FSView.h"

#include "ModuleFrm.h"
#include "MainFrm.h"

#include "Automation\DataCollector.h"

#include "winmessages.h"
#include "ah_msgprovider.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CFSView

IMPLEMENT_DYNCREATE(CFSView, CView)

BEGIN_MESSAGE_MAP(CFSView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_COMMAND(ID_SHOW_HINT, OnShowHints)
	ON_WM_CREATE()
	ON_WM_PAINT()

	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MBUTTONDBLCLK()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDOWN()

	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
	ON_WM_TIMER()

	ON_MESSAGE( RM_SHOW_USER_MESSAGE, OnMessageComming )
	ON_MESSAGE( RM_ADD_CHECKPOINT, OnAddCheckPoint )

	ON_MESSAGE( RM_TIMER_STARTED,    OnTimerStart )
	ON_MESSAGE( RM_TIMER_STOPED,	 OnTimerStop )
	ON_MESSAGE( RM_TIMER_OVERFLOWED, OnTimerOverflow)

	ON_WM_CONTEXTMENU()

END_MESSAGE_MAP()



// CFSView construction/destruction

CFSView::CFSView() : m_bShowHints(TRUE), plotDlg(NULL), 
	pFntMarks ( new Gdiplus::Font (L"Vernada", 10.0f, Gdiplus::FontStyleRegular, Gdiplus::UnitPoint) )	//Шрифт по умолчанию
{
	

}

CFSView::~CFSView()
{
//	delete m_pdo;
//	delete m_pdoADS;
	m_wndCoolTip.DestroyWindow();
	m_wndMsgList.DestroyWindow();

	CFrameWnd* pFrame = (CFrameWnd*)AfxGetMainWnd();

	if (pFrame->GetActiveView() == this)
	{
		pFrame->SetActiveView(0, TRUE);		
	}
}

LPCSTR CFSView::lpszViewClassName = NULL;

BOOL CFSView::PreCreateWindow(CREATESTRUCT& cs)
{

	if (lpszViewClassName == NULL) {

		CView::PreCreateWindow(cs);
		WNDCLASS wc;

		HINSTANCE hInst = ::AfxGetInstanceHandle();

		//пытаемся получить информацию о классе
		//если не удается, выходим с ошибкой
		if ( ! ::GetClassInfo(hInst , cs.lpszClass, &wc) ) 
			return FALSE;

		wc.hCursor = ::LoadCursor(hInst, MAKEINTRESOURCE(IDC_CUR_ARROW));

		// регистрируем класс
		lpszViewClassName = ::AfxRegisterWndClass(wc.style, wc.hCursor, wc.hbrBackground, wc.hIcon);

	}

	cs.style |= WS_CLIPSIBLINGS|WS_CLIPCHILDREN;

	// изменяем класс окна на созданный нами:
	cs.lpszClass = lpszViewClassName;

	return CView::PreCreateWindow(cs);
}

// CFSView drawing

void CFSView::OnDraw(CDC* pDC)
{
	CFSDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	
	if (!pDoc)
		return;
}


// CFSView printing

BOOL CFSView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CFSView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CFSView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CFSView::OnShowHints()
{
	m_bShowHints = !m_bShowHints;
}

// CFSView diagnostics

#ifdef _DEBUG
void CFSView::AssertValid() const
{
	CView::AssertValid();
}

void CFSView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CFSDoc* CFSView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CFSDoc)));
	return (CFSDoc*)m_pDocument;
}
#endif //_DEBUG



// CFSView message handlers

int CFSView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_wndCoolTip.SetTipStyle(CExtPopupMenuTipWnd::__ETS_BALLOON);
	m_wndCoolTip.SetShadowSize(5);
	m_wndCoolTip.m_nPeriodDelayShowNormal = 1500;
	m_wndCoolTip.m_nPeriodDelayShowQuick  = 500;
	
	LOGFONT lf;	

	g_PaintManager->m_FontNormal.GetLogFont(&lf);

	lf.lfHeight = -MulDiv(10, GetDeviceCaps(GetDC()->m_hDC, LOGPIXELSY), 72);

	m_wndCoolTip.m_fontAdvTip.DeleteObject();
	m_wndCoolTip.m_fontAdvTip.CreateFontIndirect(&lf);

	
	m_wndCoolTip.m_rcAlignment;

	m_wndMsgList.Create(WS_CHILD|WS_VSCROLL|LBS_OWNERDRAWVARIABLE, CRect(0,0,0,0), this, 1200);
	m_wndMsgList.ShowWindow(SW_HIDE);

	VERIFY(msgFont.CreateFontIndirect(&lf));
	m_wndMsgList.SetFont(&msgFont);
	
	hInfo = NULL;
	hInfo = (HICON) ::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_INFO), IMAGE_ICON, 24, 24, 0);
	CExtCmdIcon icon;
	icon.AssignFromHICON(hInfo, false);

	m_wndCoolTip.SetIcon(icon);

	m_icon.AssignFromHICON(hInfo, true);

	CRect rc;
	::GetWindowRect( GetDesktopWindow()->m_hWnd, &rc );

	int xOffset = rc.Width() / 5;
	int yOffset = rc.Height() / 4;

	rc.DeflateRect(xOffset, yOffset);

	HCURSOR hCur = ::LoadCursor(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_CUR_ARROW));

	/*
	if ( !pGraphWnd.CreateEx(WS_EX_TOPMOST, ::AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, hCur) , "Graph popup", WS_POPUPWINDOW, rc, this, IDC_STATIC ) )
	{
		////TRACE( "\nPopup graphic window creation failed" );		
		return -1;
	}
	*/

	plotDlg = ((CNanoTubesControlCenterApp*)AfxGetApp())->GetPlotDlg();
	if ( plotDlg == NULL )
	{
		TRACE( "\nPopup graphic window creation failed" );		
		return -1;
	}
	

	return 0;
}


void CFSView::OnPaint()
{
	CFSDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (!pDoc)
		return;

	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CView::OnPaint() for painting messages
	
	CExtMemoryDC mdc(&dc);
	Gdiplus::Graphics gr(mdc->m_hDC);

	long xPos = 0;//ScrollPos32Get(SB_HORZ);
	long yPos = 0;//ScrollPos32Get(SB_VERT);	

	CRect rc1;
	GetClientRect(rc1);
	g_PaintManager->PaintDockerBkgnd(true, mdc, rc1, rc1);
	g_PaintManager->PaintDocumentClientAreaBkgnd(mdc, this);
	
	const std::list<FSLayer*>& layers =  pDoc->GetLayersList();
	std::list<FSLayer*>::const_iterator it = layers.begin();

	for (; it != layers.end(); ++it)
	{
		FSLayer* pLayer = (*it);
		if ( pLayer != NULL )
			pLayer->Draw(gr);
	}
	
	/*Gdiplus::LinearGradientBrush* br = GetDocument()->GetFillStyle("Test1");
	if ( br != NULL ) 
		gr.FillRectangle(br, Gdiplus::RectF(0.0,0.0,100.0,100.0) );
	*/

	if ( GetDocument()->IsDebugMode() )
	{
		DrawGrid(gr);
	}
}

void CFSView::DrawGrid(Gdiplus::Graphics& gr, Gdiplus::REAL dMajor, Gdiplus::REAL dMinor, Gdiplus::Color clrMajor, Gdiplus::Color clrMinor)
{
	//gr.DrawLines()
	Gdiplus::Pen penMajor(clrMajor, 1.0f);
	penMajor.SetDashStyle(Gdiplus::DashStyleSolid);

	Gdiplus::Pen penMinor(clrMinor, 1.0f);
	penMinor.SetDashStyle(Gdiplus::DashStyleDot);

	Gdiplus::SolidBrush br( Gdiplus::Color(0xff, 0x00, 0x80, 0xFF) );
	Gdiplus::StringFormat fmt;
	fmt.SetAlignment(Gdiplus::StringAlignmentNear);
	fmt.SetLineAlignment(Gdiplus::StringAlignmentNear);

	fmt.SetFormatFlags(Gdiplus::StringFormatFlagsNoFitBlackBox|
		Gdiplus::StringFormatFlagsMeasureTrailingSpaces|
		Gdiplus::StringFormatFlagsDisplayFormatControl|
		Gdiplus::StringFormatFlagsLineLimit);

	for (Gdiplus::REAL x = 0.0f; x < 2000.0f; x+= dMinor)
	{
		Gdiplus::PointF ptFrom(x, 0.0f);
		Gdiplus::PointF ptTo(x, 2000.0f);

		gr.DrawLine(&penMinor, ptFrom, ptTo);
	}
	for (Gdiplus::REAL x = 0.0f; x < 2000.0f; x+= dMajor)
	{
		Gdiplus::PointF ptFrom(x, 0.0f);
		Gdiplus::PointF ptTo(x, 2000.0f);

		gr.DrawLine(&penMajor, ptFrom, ptTo);
		
		CString strMark;
		strMark.Format("%04.0f", x);

		USES_CONVERSION;
		WCHAR* pOut = A2W((LPCTSTR)strMark);

		Gdiplus::RectF rcText;
		rcText.X = x - 2.0f; rcText.Width = 50.0f;
		rcText.Y = 5.0f;   rcText.Height = 20.0f;

		gr.DrawString(pOut, -1, pFntMarks, rcText, &fmt, &br);
	}

	for (Gdiplus::REAL y = 0.0f; y < 2000.0f; y+= dMinor)
	{
		Gdiplus::PointF ptFrom(0.0f, y);
		Gdiplus::PointF ptTo(2000.0f, y);

		gr.DrawLine(&penMinor, ptFrom, ptTo);
	}
	for (Gdiplus::REAL y = 0.0f; y < 2000.0f; y+= dMajor)
	{
		Gdiplus::PointF ptFrom(0.0f, y);
		Gdiplus::PointF ptTo(2000.0f, y);

		gr.DrawLine(&penMajor, ptFrom, ptTo);

		CString strMark;
		strMark.Format("%04.0f", y);

		USES_CONVERSION;
		WCHAR* pOut = A2W((LPCTSTR)strMark);

		Gdiplus::RectF rcText;
		rcText.Y = y - 2.0f; rcText.Width = 50.0f;
		rcText.X = 5.0f;   rcText.Height = 20.0f;

		gr.DrawString(pOut, -1, pFntMarks, rcText, &fmt, &br);
	}
}

void CFSView::OnMouseMove(UINT nFlags, CPoint point)
{
	CFSDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if ( !pDoc )
		return;

	CDrawObj* pObj = pDoc->HitTest( point );

	if ( pObj == NULL )
	{
		m_wndCoolTip.Hide();
		return; 
	}

	if ( m_bShowHints )
	{
		CRect rcItem;
		pObj->GetPosition(rcItem);

		ClientToScreen(&rcItem);

		CString strHintObj = pObj->OnNeedHintText().c_str();
		CString strHint;

		if ( pDoc->IsDebugMode() )
			strHint.Format("<html><b>Координаты:</b><i><p>X = %04d</p><p>Y = %04d</p></i><p>%s</p></html>", point.x, point.y, (LPCTSTR)strHintObj);
		else
			strHint.Format(strHintObj.GetLength() == 0 ? "%s" :"<html>%s</html>", (LPCTSTR)strHintObj );

		if (strHint.GetLength() > 0 )
		{
			m_wndCoolTip.SetText(strHint);		
			m_wndCoolTip.Show(this, rcItem);
		}		
	}

	CView::OnMouseMove(nFlags, point);
}

BOOL CFSView::OnEraseBkgnd(CDC* pDC)
{
	//return CView::OnEraseBkgnd(pDC);
	return TRUE;
}

void CFSView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CFSDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (!pDoc)
		return CView::OnKeyDown(nChar, nRepCnt, nFlags);


	if (nChar == VK_F5)
	{
		pDoc->Rebuild();
	}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CFSView::OnLButtonDblClk(UINT nFlags, CPoint pt)
{
	CFSDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (!pDoc)
		return CView::OnLButtonDblClk(nFlags, pt);

	CDrawObj* pObj = pDoc->HitTest( pt );
	//FSDocItem* pDocItem = pDoc->HitTest( pt );

	BOOL bComplete = FALSE;
	if ( pObj != NULL )
		bComplete = pObj->OnLButtonDoubleClick( nFlags );
	
	if ( pObj != NULL )
	{
		DataSrc* pDS = pObj->GetAssignedDataSource();
		
		if (dynamic_cast<DiscreteOutputDataSrc*>(pDS))
		{
			CView::OnLButtonDblClk(nFlags, pt);
			UpdateWindow();
			InvalidateRect(NULL);
			return; 
		}

		if (pDS != NULL )
		{
			if ( (nFlags & MK_CONTROL) !=  MK_CONTROL) 
			{
				//pGraphWnd.Clear();
				plotDlg->Clear();
			}


			GRAPHIC_GROUP group;
			pDoc->GetDSGroup(pDS->Name(), &group);

			if (!group.dslya.empty() || !group.dsrya.empty() )
			{
				//pGraphWnd.Clear();
				//pGraphWnd.AddDataSrc(group);

				plotDlg->Clear();
				plotDlg->AddDataSrc(group);

// 				for(int  i = 0; i < (int)group.ds.size(); i++)// std::vector<std::string>::const_iterator it=group.ds.begin(); it != group.ds.end(); ++it)
// 				{	
// 					std::string name = group.ds[i];
// 					int lt   = group.lt[i];
// 					int lw   = group.lw[i];
// 					std::string lp   = group.lp[i];
// 
// 					pGraphWnd.AddDataSrc(group);
// 				}
// 
// 				pGraphWnd.SetTitle(group.title);
				//pGraphWnd.SetYAxisScale(group.minY, group.maxY);
			}
			else
			{
				//pGraphWnd.AddDataSrc(pDS->Name());
				//pGraphWnd.SetTitle( pDS->Description() );

				plotDlg->AddDataSrc( pDS->Name(), true, std::make_pair(0.0, 0.0) );
				plotDlg->SetTitle( pDS->Description() );
			}
				
				
			//pGraphWnd.ShowWindow(SW_SHOW);
			//pGraphWnd.AutoHideSet(true);

			plotDlg->ShowWindow(SW_SHOW);
			//plotDlg.AutoHideSet(true);
			
			
		}
	}
}

void CFSView::OnRButtonUp(UINT nFlags, CPoint point)
{
	KillTimer(TIMER_LMB_HOLDING);
	CView::OnRButtonUp(nFlags, point);
}

void CFSView::OnRButtonDown(UINT nFlags, CPoint pt)
{

	CView::OnRButtonDown(nFlags, pt);
}

void CFSView::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{

	case TIMER_LMB_HOLDING:
		TRACE("\n\t Updating document TIMER_LMB_HOLDING");
		OnLButtonDblClk(m_lastMouseEventFlags, m_lastMouseEventPoint);
		KillTimer(TIMER_LMB_HOLDING);
		break;

	case TIMER_UPDATE:
		{
		
		CWnd* pWndMain = AfxGetMainWnd();
		ASSERT(pWndMain);
		ASSERT(pWndMain->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd))); // Not an MDI app.
		CFrameWnd* pFrame = ((CMDIFrameWnd*)pWndMain)->MDIGetActive();

		if ( pFrame == GetParentFrame() )
		{
			InvalidateRect(NULL);
			UpdateWindow();
		}
		
		break;

		}

	case TIMER_MSG_SHOW:
		TRACE("\n\t Updating document TIMER_MSG_SHOW");
		m_wndMsgList.ShowWindow(SW_HIDE);
		KillTimer(TIMER_MSG_SHOW);
		break;
	}
		
	CView::OnTimer(nIDEvent);
}

void CFSView::OnInitialUpdate()
{
	CFSDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CRect rcMsgList = pDoc->GetMessageListPlace();

	m_ulUpdateTime = pDoc->GetUpdateInterval();
	m_ulLMBHoldingTime = pDoc->GetLMBHoldInterval();

	SetTimer(TIMER_UPDATE, m_ulUpdateTime, NULL);

	if ( rcMsgList.Width() > 0 && rcMsgList.Height() > 0 )
	{
		m_wndMsgList.MoveWindow(rcMsgList, TRUE);		
	}

	CMainFrame*  pFrm = (CMainFrame*) (GetParentFrame()->GetActiveFrame() );
	pFrm->_CombineMixedStoreAll();

	AppMessagesProviderHelper::Instance().RegisterOutputMessageDestination(this);

	CView::OnInitialUpdate();	
}

void CFSView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	switch (lHint)
	{
	case HINT_UPDATE_WINDOW:    // redraw entire window
		
		Invalidate(FALSE);
		break;

	case HINT_UPDATE_DRAWOBJ:   // a single object has changed
		//InvalidateObject((CDrawObj*)pHint);
		break;

	case HINT_UPDATE_SELECTION: // an entire selection has changed
		
		break;

	case HINT_DELETE_SELECTION: // an entire selection has been removed		
		break;
	
	default:
		ASSERT(FALSE);
		break;
	}

	CView::OnUpdate(pSender, lHint, pHint);
}


LRESULT CFSView::OnMessageComming( WPARAM wParam, LPARAM lParam)
{
	COutMessage* pOutMsg = (COutMessage*)lParam;
	std::string iconName;

	switch ( pOutMsg->MessageType() )
	{
	case COutMessage::MSG_OK: 
		iconName = "ok.ico";
		break;

	case COutMessage::MSG_WARNING: 
		iconName = "warning.ico";
		break;

	case COutMessage::MSG_ERROR:
		iconName = "error.ico";
		break;	

	case COutMessage::MSG_OBA_SCRIPT:
		iconName = "script.ico";
		break;
	}
	m_wndMsgList.ShowWindow(SW_SHOW);

	CExtCmdIcon* pIcon = AppImagesHelper::Instance().GetIcon(iconName, ICON_SIZE_24);
	m_wndMsgList.AddIcon(*pIcon, pOutMsg->MessageText());

	CFSDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);	

	SetTimer(TIMER_MSG_SHOW, 15000, NULL);
	return 0L;
}

void CFSView::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	return;	
}

LRESULT CFSView::OnAddCheckPoint( WPARAM wParam, LPARAM lParam)
{
	std::vector<double> vals;
	DataCollector::Instance().GetTimeBuffer(vals);

	double val = ( vals.size() == 0 ) ? 0.0 : vals.back();
	DataSrc* pDS = (DataSrc*)lParam;
	//pGraphWnd.AddCheckPoint(pDS->Name(), val);
	
	plotDlg->AddCheckPoint(pDS->Name(), val);

	return 0L;
}

LRESULT CFSView::OnTimerStart( WPARAM wParam, LPARAM lParam)
{
	BaseTimer* pTimer = (BaseTimer*)lParam;
	LPCREGIONINFO lpri = GetDocument()->GetRegion(pTimer);	
	if ( lpri != NULL ) {
		//pGraphWnd.AddRegion(pTimer->Name(), *lpri);
		plotDlg->AddRegion(pTimer->Name(), *lpri);
	}

	return 0L;
}

LRESULT CFSView::OnTimerStop( WPARAM wParam, LPARAM lParam)
{
	BaseTimer* pTimer = (BaseTimer*)lParam;
	LPCREGIONINFO lpri = GetDocument()->GetRegion(pTimer);	
	if ( lpri != NULL ){
		//pGraphWnd.AddRegion(pTimer->Name(), *lpri);
		plotDlg->AddRegion(pTimer->Name(), *lpri);
	}
	return 0L;
}

LRESULT CFSView::OnTimerOverflow( WPARAM wParam, LPARAM lParam)
{
	BaseTimer* pTimer = (BaseTimer*)lParam;
	LPCREGIONINFO lpri = GetDocument()->GetRegion(pTimer);	
	if ( lpri != NULL ) {
		//pGraphWnd.AddRegion(pTimer->Name(), *lpri);
		plotDlg->AddRegion(pTimer->Name(), *lpri);
	}

	return 0L;
}
