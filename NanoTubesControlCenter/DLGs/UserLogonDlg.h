#pragma once

#include "..\resource.h"

// CUserLogonDlg dialog

class CUserLogonDlg : public CExtResizableDialog
{
	DECLARE_DYNAMIC(CUserLogonDlg)

public:
	CUserLogonDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CUserLogonDlg();

// Dialog Data
	enum { IDD = IDD_USER_LOGIN };

	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage( MSG * pMsg );
protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	afx_msg LRESULT OnExtMenuPrepare(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDrawPopupMenuItem(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDrawPopupLeftArea(WPARAM wParam, LPARAM lParam);

	afx_msg void OnAppUserChange(UINT id);

	afx_msg void OnMeasureItem( int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct );
	afx_msg void OnEnSetFocusEditPass();
	afx_msg void OnEnKillFocusEditPass();
	afx_msg void OnEnChangeEditPass();

	afx_msg void OnNumButtonClicked( UINT nID  );
	afx_msg void OnNumClear();
	afx_msg void OnNumDel();
	afx_msg void OnNumEnter();

	virtual void OnOK();
protected :
	
	void HideNumericButtons(BOOL bHide =  true);

	UINT bottomDlg, topNumKeys;
	CExtButton btnUsers;
	CExtEdit   edPsw;

};
