// FSView.h : interface of the CFSView class
//
#pragma once
#include "GUI/IconLIstBox.h"
#include "PopupGraphWnd.h"
#include "PlotDlg.h"

class CFSView : public CView
{
protected: // create from serialization only
	CFSView();
	DECLARE_DYNCREATE(CFSView)

// Attributes
public:
	CFSDoc* GetDocument() const;

// Operations
public:	
	
	
// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);

	void DrawGrid(Gdiplus::Graphics& gr, Gdiplus::REAL dMajor = 100.0f, Gdiplus::REAL dMinor = 10.0f, Gdiplus::Color clrMajor = Gdiplus::Color::MakeARGB(128,128,128,128), Gdiplus::Color clrMinor= Gdiplus::Color::MakeARGB(128,128,128,128));

protected:

	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);


	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);

	afx_msg void OnShowHints();


	BOOL m_bShowHints;

	static LPCSTR CFSView::lpszViewClassName; 

// Implementation
public:
	virtual ~CFSView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;

#endif

protected:

	enum{
		TIMER_LMB_HOLDING,
		TIMER_MSG_SHOW,
		TIMER_UPDATE
	};

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);


private:
	//////////////////////////////////////////////////////////////////////////
	//CDrawObjList m_selection;

	//////////////////////////////////////////////////////////////////////////
	CExtPopupMenuTipWnd m_wndCoolTip;
	CIconListBox		m_wndMsgList;
	CFont				msgFont;
	HICON				hInfo;
	CExtCmdIcon			m_icon;

	Gdiplus::Font*	pFntMarks;
	

	ULONG m_ulUpdateTime;
	ULONG m_ulLMBHoldingTime;

	UINT	m_lastMouseEventFlags;
	CPoint  m_lastMouseEventPoint;

public:

	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint pt);	

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	afx_msg LRESULT OnMessageComming( WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAddCheckPoint( WPARAM wParam, LPARAM lParam);

	afx_msg LRESULT OnTimerStart( WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTimerStop( WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTimerOverflow( WPARAM wParam, LPARAM lParam);


	virtual void OnInitialUpdate();

private:

	//CPopupGraphWnd pGraphWnd;

	CPlotDlg* plotDlg;
};

#ifndef _DEBUG  // debug version in FSView.cpp
inline CFSDoc* CFSView::GetDocument() const
   { return reinterpret_cast<CFSDoc*>(m_pDocument); }
#endif

