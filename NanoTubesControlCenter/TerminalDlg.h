#pragma once


// CTerminalDlg dialog

class CTerminalDlg : public CExtResizableDialog
{
	DECLARE_DYNAMIC(CTerminalDlg)

public:
	CTerminalDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTerminalDlg();

// Dialog Data
	enum { IDD = IDD_TERMINAL_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	
	void UpdateCommandManagersList();
	
	afx_msg void OnBnClickedBtnSend();
	afx_msg void OnEnChangeEdCmd();
};
