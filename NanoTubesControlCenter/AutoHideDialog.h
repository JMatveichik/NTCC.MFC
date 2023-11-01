#pragma once
#include "resource.h"

// CAutoHideDialog dialog
// CPopupGraphWnd
class CAutoHideDialog : public CExtResizableDialog
{
	DECLARE_DYNAMIC(CAutoHideDialog)

public:
	
	CAutoHideDialog(UINT nTemlateID, CWnd* pParent = NULL);   // standard constructor
	virtual ~CAutoHideDialog();



protected :

	enum {
		TIMER_HIDE	 = 1, 
		TIMER_UPDATE = 2
	};
	
	TRACKMOUSEEVENT tme;

	UINT showTime;
	UINT updateTime;

protected:

	
	virtual void OnUpdateDialogWnd();
	virtual void OnHideDialogWnd();

protected:

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	void SetShowTime(UINT time) { showTime		= time; };
	void SetUpdateTime(UINT time){ updateTime	= time; };

protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

public:

	virtual BOOL PreTranslateMessage(MSG* pMsg);
};


