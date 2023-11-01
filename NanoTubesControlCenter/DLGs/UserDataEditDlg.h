#pragma once

#include "..\resource.h"

#include "..\ah_users.h"

// ÑUserDataEditDlg dialog

class ÑUserDataEditDlg : public CExtResizableDialog
{
	DECLARE_DYNAMIC(ÑUserDataEditDlg)

public:
	ÑUserDataEditDlg(CWnd* pParent = NULL, LPNTCC_APP_USER lpUser = NULL);   // standard constructor
	virtual ~ÑUserDataEditDlg();

// Dialog Data
	enum { IDD = IDD_ADD_APP_USER };

protected:
	
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()

public:
	
	virtual BOOL OnInitDialog();	

protected:

	LPNTCC_APP_USER lpEditUser;

	CString strULogin;
	CString strUName;
	int nUGroup;
	CString  strUPhone;
	CString  strUPass;
	CString  strUPassConfirm;
};
