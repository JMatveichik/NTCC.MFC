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
// ÑUserDataEditDlg dialog

IMPLEMENT_DYNAMIC(ÑUserDataEditDlg, CExtResizableDialog)

ÑUserDataEditDlg::ÑUserDataEditDlg(CWnd* pParent /*=NULL*/, LPNTCC_APP_USER lpUser/* = NULL*/)
	: CExtResizableDialog(ÑUserDataEditDlg::IDD, pParent), lpEditUser(lpUser)
{

}

ÑUserDataEditDlg::~ÑUserDataEditDlg()
{
}

void ÑUserDataEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);	

	DDX_Text(pDX, IDC_ED_ULOGIN, strULogin);
	DDX_Text(pDX, IDC_ED_UNAME, strUName);
	DDX_Text(pDX, IDC_ED_UPHONE, strUPhone);
	DDX_Text(pDX, IDC_ED_UPASS, strUPass);
	DDX_Text(pDX, IDC_ED_UPASSCONFIRM, strUPassConfirm);
	DDX_CBIndex(pDX, IDC_CMB_UGROUP, nUGroup);

}


BEGIN_MESSAGE_MAP(ÑUserDataEditDlg, CExtResizableDialog)
	
	

END_MESSAGE_MAP()



// ÑUserDataEditDlg message handlers

BOOL ÑUserDataEditDlg::OnInitDialog()
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

		GetDlgItem(IDOK)->SetWindowText("Ñîõðàíèòü");
		UpdateData(FALSE);
	}
	
	return TRUE;
}

void ÑUserDataEditDlg::OnOK()
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