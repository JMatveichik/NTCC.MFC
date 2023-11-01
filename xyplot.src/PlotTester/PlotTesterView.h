// PlotTesterView.h : interface of the CPlotTesterView class
//
#include "..\src\XYPlotWrapper.h"

#pragma once


class CPlotTesterView : public CView
{
protected: // create from serialization only
	CPlotTesterView();
	DECLARE_DYNCREATE(CPlotTesterView)

// Attributes
public:
	CPlotTesterDoc* GetDocument() const;

// Operations
public:

// Overrides
	public:
		virtual void OnDraw(CDC* pDC) {}  // overridden to draw this view
		virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// Implementation
public:
	virtual ~CPlotTesterView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	xyplot::HPLOT m_hPlot;

	xyplot::HPROFILE m_hProf1;
	xyplot::HPROFILE m_hProf2;
	xyplot::HPROFILE m_hProf3;


	xyplot::HLEVEL lvl1;
	xyplot::HLEVEL lvl2;

	HANDLE flashThread;
	HANDLE evQuit;
	HANDLE evStart;

	static unsigned long __stdcall fnDataProc( LPVOID lpParam );

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

#ifndef _DEBUG  // debug version in PlotTesterView.cpp
inline CPlotTesterDoc* CPlotTesterView::GetDocument() const
   { return reinterpret_cast<CPlotTesterDoc*>(m_pDocument); }
#endif

