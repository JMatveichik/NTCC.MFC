#pragma once
//#include "DLGs/TerminalDlg.h"
#include "DLGs/DataSrcViewDlg.h"
#include "DLGs/ScriptsDlg.h"

// CProjectView view


enum { COL_IND_NAME,  COL_IND_SIGNAL,  COL_IND_VALUE,  COL_IND_UNITS,  COL_IND_DESCRIPTION };

class CProjectView : public CView
{
	DECLARE_DYNCREATE(CProjectView)

protected:
	CProjectView();           // protected constructor used by dynamic creation
	virtual ~CProjectView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:

	DECLARE_MESSAGE_MAP()

protected:
	
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void OnDraw(CDC* pDC) {};

public:

	virtual void OnInitialUpdate();
	
protected:
	
	CExtPageContainerWnd m_container;

//	CTerminalDlg		m_dlgTerminal;	// Закладка "Терминал"
	CDataSrcViewDlg		m_dlgDsView;	// "Иcточники данных"
//	CListCtrl			m_wndLoadList;	// "Результаты загрузки"
	
	CExtTreeGridWnd		m_wndNetList;	// "Промышленная сеть"	
//	CScriptsDlg			m_dlgScripts;
	
protected:

	BOOL InitLoadingListPage(); 
	BOOL InitTerminalPage(); 
	BOOL InitDataSourcesListPage();
	BOOL InitIndustrialNetListPage();
	BOOL InitScriptsPage();

public:

	afx_msg void OnSize(UINT nType, int cx, int cy);
};


