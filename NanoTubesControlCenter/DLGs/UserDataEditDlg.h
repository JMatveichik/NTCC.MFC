#pragma once

#include "..\resource.h"

#include "..\ah_users.h"

// �UserDataEditDlg dialog

class �UserDataEditDlg : public CExtResizableDialog
{
	DECLARE_DYNAMIC(�UserDataEditDlg)

public:
	�UserDataEditDlg(CWnd* pParent = NULL, LPNTCC_APP_USER lpUser = NULL);   // standard constructor
	virtual ~�UserDataEditDlg();

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
