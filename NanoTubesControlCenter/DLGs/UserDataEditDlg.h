#pragma once

#include "..\resource.h"

#include "..\ah_users.h"

// ĐUserDataEditDlg dialog

class ĐUserDataEditDlg : public CExtResizableDialog
{
	DECLARE_DYNAMIC(ĐUserDataEditDlg)

public:
	ĐUserDataEditDlg(CWnd* pParent = NULL, LPNTCC_APP_USER lpUser = NULL);   // standard constructor
	virtual ~ĐUserDataEditDlg();

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
