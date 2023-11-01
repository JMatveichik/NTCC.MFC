#pragma once

#include "..\GUI/IconListBox.h"
#include "..\GUI/SyntaxColorizer.h"
#include "..\ah_scripts.h"


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
	void UpdateStatusBar();

protected:
	
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	

	DECLARE_MESSAGE_MAP()

	CExtToolControlBar	m_wndScriptsToolBar;
	CIconListBox		m_wndList;
	CRichEditCtrl		m_wndSriptText;
	CExtStatusControlBar m_wndStatus;
	//CExtEdit			m_wndSriptText;

  	EDITSTREAM			m_es;
  	CSyntaxColorizer	m_sc;

	int m_nPrevSel;
	  
	std::vector<SCRIPTINFO> m_scripts;

	afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct  );

	afx_msg void OnScriptOpen();
	afx_msg void OnScriptSave();
	afx_msg void OnScriptSaveAll();
	afx_msg void OnScriptRun();	
	afx_msg void OnScriptStop();

 	void readFile(CString sFileName);
	void saveFile(CString sFileName);

 	void parse();
	void parse2();

public:

	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnListScriptsDoubleClick();
	afx_msg void OnListScriptsSelChange();
	afx_msg void OnListScriptsSelCancel();
	afx_msg void OnChangeScriptText();

	afx_msg void OnSelChangeScriptText( NMHDR * pNotifyStruct, LRESULT * result );

};
