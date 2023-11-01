// ScriptsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NanoTubesControlCenter.h"
#include "ScriptsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW	
#endif
// CScriptsDlg dialog

IMPLEMENT_DYNAMIC(CScriptsDlg, CExtResizableDialog)

CScriptsDlg::CScriptsDlg(CWnd* pParent /*=NULL*/)
	: CExtResizableDialog(CScriptsDlg::IDD, pParent)
{

}

CScriptsDlg::~CScriptsDlg()
{
}

void CScriptsDlg::DoDataExchange(CDataExchange* pDX)
{
	CExtResizableDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_SCRIPTS_TOOL_BAR, m_wndScriptsToolBar);
	DDX_Control(pDX, IDC_LIST_SCRIPTS, m_wndList);
	DDX_Control(pDX, IDC_SCRIPT_TEXT, m_wndSriptText);
	
}


BEGIN_MESSAGE_MAP(CScriptsDlg, CExtResizableDialog)
	ON_WM_SIZE()

	ON_COMMAND(ID_SCRIPT_OPEN, OnScriptOpen)
	ON_COMMAND(ID_SCRIPT_SAVE, OnScriptSave)
	ON_COMMAND(ID_SCRIPT_SAVE_ALL, OnScriptSaveAll)
	ON_COMMAND(ID_SCRIPT_RUN,	OnScriptRun)	
	ON_COMMAND(ID_SCRIPT_STOP,	OnScriptStop)

	ON_LBN_DBLCLK(IDC_LIST_SCRIPTS, &CScriptsDlg::OnLbnDblclkListScripts)
END_MESSAGE_MAP()


// CScriptsDlg message handlers

BOOL CScriptsDlg::OnInitDialog()
{
	CExtResizableDialog::OnInitDialog();

	if ( !m_wndScriptsToolBar.LoadToolBar(IDR_SCRIPTS_TOOL_BAR) )
		return FALSE;

	// TODO:  Add extra initialization here
	AddAnchor( IDC_LIST_SCRIPTS, CPoint(0, 0), CPoint(10, 100));
	AddAnchor( IDC_SCRIPT_TEXT, CPoint(10, 0), CPoint(100, 100));

	

	RepositionBars(0, 0xFFFF, IDC_SCRIPT_TEXT); 

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CScriptsDlg::UpdateScriptsList()
{

	m_wndList.RemoveAllIcons();

	
	AppOBAScriptsHelper::Instance().EnumScripts(m_scripts);

	for (std::vector<SCRIPTINFO>::iterator it = m_scripts.begin(); it != m_scripts.end(); it++ )
	{
		CString str;
		str.Format("%-12s : %s\n%-12s : %s", "Script", (*it).name.c_str(), "Path", (*it).path.c_str() );

		CExtCmdIcon* pIcon = AppImagesHelper::Instance().GetIcon("script.ico", ICON_SIZE_24);
		m_wndList.AddIcon(*pIcon, str);
		
		int ind = m_wndList.GetCount() - 1;
		m_wndList.SetItemData(ind, (DWORD_PTR)&(*it));

	}
}
void CScriptsDlg::OnSize(UINT nType, int cx, int cy)
{
	CExtResizableDialog::OnSize(nType, cx, cy);

	RepositionBars(0, 0xFFFF, IDC_SCRIPT_TEXT); 
}

void CScriptsDlg::OnScriptOpen()
{

}
void CScriptsDlg::OnScriptSave()
{

}
void CScriptsDlg::OnScriptSaveAll()
{

}
void CScriptsDlg::OnScriptRun()
{
	int ind = m_wndList.GetCurSel();
	SCRIPTINFO* si = (SCRIPTINFO*)m_wndList.GetItemData(ind);
	AppOBAScriptsHelper::Instance().RunScript(si->name);
}

void CScriptsDlg::OnScriptStop()
{

}

void CScriptsDlg::OnLbnDblclkListScripts()
{
	OnScriptRun();
}
