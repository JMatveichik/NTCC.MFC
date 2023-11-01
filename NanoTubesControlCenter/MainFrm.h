// MainFrm.h : interface of the CMainFrame class
//
#pragma once

#if !defined(__EXT_TEMPL_H)
	#include <ExtTempl.h>
#endif

 //__PROFUISAPPWIZ_KEY_RCB2
#include "DLGs/ExtControls.h"
#include "GUI/LogCtrl.h"

#define DEBUG_OUT_WND

//#define __USE_PROFUIS_TAB_MDI_ONE_NOTE_WND


class CMainFrame : 
	public CExtNCW < CMDIFrameWnd >, 
	public CExtPmBridge
{
	friend CNanoTubesControlCenterApp;

	DECLARE_DYNAMIC(CMainFrame)

	DECLARE_CExtPmBridge_MEMBERS( CMainFrame );

public:
	CMainFrame();

// Attributes
protected:
	WINDOWPLACEMENT m_dataFrameWP;
 //__PROFUISAPPWIZ_KEY_PERS_POS_MAINFRM
public:

// Operations
public:

// Overrides
public:
	virtual BOOL DestroyWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
 //__PROFUISAPPWIZ_KEY_USE_MCB
	virtual void ActivateFrame(int nCmdShow = -1);
 //__PROFUISAPPWIZ_KEY_PERS_POS_MAINFRM


	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL );

	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);

//Комбинирование объектов 
	void _CombineMixedStoreAll();
	void _CombineMixedStoreSelection();
	

	CFrameWnd* pProjectFrm;

// Implementation
public:

	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	
protected:
	virtual void PmBridge_OnPaintManagerChanged( CExtPaintManager * pGlobalPM );

private:  // control bar embedded members
	
	CString m_strPopupMsgWndClass;

#ifdef __USE_PROFUIS_TAB_MDI_ONE_NOTE_WND
	CExtTabMdiOneNoteWnd m_wndMdiTabs;
#else
	CSizableTabMdiOneNoteWnd m_wndMdiTabs;
#endif

	
	CExtThemeSwitcherToolControlBar m_wndToolBarUiLook;
	

	

	//Для задания свойств 
#ifdef USE_ITEMS_PROPERTY_STORE


	CExtControlBar					m_wndPropBar;

	CExtPPVW < CExtWRB< CExtPropertyGridCtrl > > m_PGC;

	CExtPropertyStore m_PropertyStoreCompoundSelection;
	CExtPropertyStore m_PropertyStoreAll;

	BOOL initPropertyesBar();

#endif

	/////////////////////////////////////////////////

#ifdef DEBUG_OUT_WND
	
	CExtControlBar    m_wndLogBar;
	//CExtDynamicControlBar    m_wndLogBar;
	CLogCtrl		  m_wndLogCtrl;

#endif

	static LPCTSTR lpszMainClassName;

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	
	afx_msg void OnUpdateControlBarMenu(CCmdUI* pCmdUI);
	afx_msg BOOL OnBarCheck(UINT nID);

	//afx_msg void OnViewLogBar();
	afx_msg void OnViewDebugWnd();


public:
	
	afx_msg LRESULT OnMessageComming( WPARAM wParam, LPARAM lParam);

	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);

	
protected:
	virtual void PostNcDestroy();
};
