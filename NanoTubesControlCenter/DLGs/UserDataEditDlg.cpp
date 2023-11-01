// DLGs/UserDataEditDlg.cpp : implementation file
//

#include "stdafx.h"

//#include "..\NanoTubesControlCenter.h"
#include "..\ah_errors.h"

#include "UserDataEditDlg.h"
//#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW	
#endif
// �UserDataEditDlg dialog

IMPLEMENT_DYNAMIC(�UserDataEditDlg, CExtResizableDialog)

�UserDataEditDlg::�UserDataEditDlg(CWnd* pParent /*=NULL*/, LPNTCC_APP_USER lpUser/* = NULL*/)
	: CExtResizableDialog(�UserDataEditDlg::IDD, pParent), lpEditUser(lpUser)
{

}

�UserDataEditDlg::~�UserDataEditDlg()
{
}

void �UserDataEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);	

	DDX_Text(pDX, IDC_ED_ULOGIN, strULogin);
	DDX_Text(pDX, IDC_ED_UNAME, strUName);
	DDX_Text(pDX, IDC_ED_UPHONE, strUPhone);
	DDX_Text(pDX, IDC_ED_UPASS, strUPass);
	DDX_Text(pDX, IDC_ED_UPASSCONFIRM, strUPassConfirm);
	DDX_CBIndex(pDX, IDC_CMB_UGROUP, nUGroup);

}


BEGIN_MESSAGE_MAP(�UserDataEditDlg, CExtResizableDialog)
	
	

END_MESSAGE_MAP()



// �UserDataEditDlg message handlers

BOOL �UserDataEditDlg::OnInitDialog()
{
	CExtResizableDialog::OnInitDialog();
	
	SubclassChildControls();

	if (lpEditUser != NULL)
	{
		strULogin = lpEditUser->login.c_str();
		strUName  = lpEditUser->screenName.c_str();
		strUPhone  = lpEditUser->phoneNum.c_str();
		strUPass = "";
		strUPassConfirm = "";
		nUGroup = (int)lpEditUser->ug;

		GetDlgItem(IDOK)->SetWindowText("���������");
		UpdateData(FALSE);
	}
	
	return TRUE;
}

void �UserDataEditDlg::OnOK()
{
	UpdateData(TRUE);

	NTCC_APP_USER user;
	user.login = (LPCTSTR)strULogin;
	user.screenName = (LPCTSTR)strUName;
	user.phoneNum = (LPCTSTR)strUPhone;
	user.psw = (LPCTSTR)strUPass;
	user.ug = (USERS_GROUPS)nUGroup;

	if ( !AppUsersHelper::Instance().AddUser(&user) )
		return;

	CExtResizableDialog::OnOK();
}