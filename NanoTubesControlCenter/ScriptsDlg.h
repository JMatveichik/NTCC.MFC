#pragma once

#include "GUI/IconListBox.h"
#include "AppHelpers.h"

// CScriptsDlg dialog

class CScriptsDlg : public CExtResizableDialog
{
	DECLARE_DYNAMIC(CScriptsDlg)

public:
	CScriptsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CScriptsDlg();

// Dialog Data
	enum { IDD = IDD_SCRIPTS_DLG };

	void UpdateScriptsList();

protected:
	
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	

	DECLARE_MESSAGE_MAP()

	CExtToolControlBar	m_wndScriptsToolBar;
	CIconListBox		m_wndList;
	CEdit				m_wndSriptText;

	std::vector<SCRIPTINFO> m_scripts;

	afx_msg void OnScriptOpen();
	afx_msg void OnScriptSave();
	afx_msg void OnScriptSaveAll();
	afx_msg void OnScriptRun();	
	afx_msg void OnScriptStop();

public:

	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLbnDblclkListScripts();
};
