#pragma once

#include "OutMessage.h"

// CPopupMessageWnd
class COutMessage;

class CPopupMessageWnd : public CWnd
{
	DECLARE_DYNAMIC(CPopupMessageWnd)

public:
	CPopupMessageWnd();
	virtual ~CPopupMessageWnd();

protected:
	DECLARE_MESSAGE_MAP()

	Gdiplus::Image* OnNeedBkImage() const;

public:

	void ShowMessage(COutMessage* pOutMsg);

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);


	static BYTE transparencyLevel;

private:
	
	TRACKMOUSEEVENT tme;

	CStatic m_lblTitle;
	CStatic m_lblText;
	CStatic m_lblInfo;

	CFont msgTitleFont;
	CFont msgMessageFont;
	CFont msgInfoFont;
	CTime m_tmStart;

	UINT type;	

	std::list<CWnd*>::iterator itSelf;
	static std::list<CWnd*> activeWindows;
	static CCriticalSection closeCS;

	void CloseWindow();

protected:

	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);

public:



	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnCtlColorStatic( WPARAM wParam, LPARAM lParam);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};


