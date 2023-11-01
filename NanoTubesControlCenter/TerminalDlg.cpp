// TerminalDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NanoTubesControlCenter.h"
#include "TerminalDlg.h"
#include "Automation\commandmanager.h"

#ifdef _DEBUG
#define new DEBUG_NEW	
#endif
// CTerminalDlg dialog

IMPLEMENT_DYNAMIC(CTerminalDlg, CExtResizableDialog)

CTerminalDlg::CTerminalDlg(CWnd* pParent /*=NULL*/)
	: CExtResizableDialog(CTerminalDlg::IDD, pParent)
{

}

CTerminalDlg::~CTerminalDlg()
{
}

void CTerminalDlg::DoDataExchange(CDataExchange* pDX)
{
	CExtResizableDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTerminalDlg, CExtResizableDialog)
	ON_BN_CLICKED(IDC_BTN_SEND, &CTerminalDlg::OnBnClickedBtnSend)
END_MESSAGE_MAP()


// CTerminalDlg message handlers

void CTerminalDlg::UpdateCommandManagersList()
{
	std::vector<CommandManager*> cmdman;

	CommandManager::EnumCommandManagers(cmdman);

	CExtComboBox* pBox = (CExtComboBox*)GetDlgItem(IDC_CMB_POTR_NO);
	pBox->ResetContent();

	for(std::size_t i = 0; i < cmdman.size(); i++)
	{
		CString str;
		str.Format("COM %d", cmdman[i]->ComPortNumber());

		int  ind = pBox->AddString(str);
		pBox->SetItemData ( ind, (DWORD_PTR)cmdman[i] );
	}
	pBox->SetCurSel(0);
}

BOOL CTerminalDlg::OnInitDialog()
{
	CExtResizableDialog::OnInitDialog();

	SubclassChildControls();

	AddAnchor(IDC_LST_RESULTS, __RDA_LT, __RDA_RB);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CTerminalDlg::OnBnClickedBtnSend()
{
	CExtComboBox* pBox = (CExtComboBox*)GetDlgItem(IDC_CMB_POTR_NO);
	
	int ind = pBox->GetCurSel();
	CommandManager* pMan = (CommandManager*)pBox->GetItemData ( ind );	

	CString strCmd;
	GetDlgItemText(IDC_ED_CMD, strCmd);
	strCmd += '\r';

	std::string res = pMan->DataPass(strCmd.GetBuffer(), false);
	strCmd.ReleaseBuffer();

	CString outLine;
	outLine.Format("COM%d >%s => %s", pMan->ComPortNumber(), strCmd, res.c_str() );

	CListBox* pLstBox = (CListBox*)GetDlgItem(IDC_LST_RESULTS);	
	pLstBox->AddString(outLine);
}
