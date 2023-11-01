// ScriptsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "..\NanoTubesControlCenter.h"
#include "..\Automation\DataSrc.h"
#include "ScriptsDlg.h"

#include "..\ah_images.h"

#ifdef _DEBUG
#define new DEBUG_NEW	
#endif
// CScriptsDlg dialog


// File interaction, 
DWORD CALLBACK readFunction(DWORD dwCookie,
	LPBYTE lpBuf,	//the buffer to fill
	LONG nCount,	//the no. of bytes to read
	LONG* nRead)	// no. of bytes read
{
	CFile* fp = (CFile *)dwCookie;
	*nRead = fp->Read(lpBuf,nCount);
	return 0;
}

DWORD CALLBACK writeFunction(DWORD dwCookie, 
	LPBYTE pbBuff,	//the buffer to fill
	LONG cb,		//the no. of bytes to read
	LONG *pcb)		
{
	CFile* pFile = (CFile*) dwCookie;
	pFile->Write(pbBuff, cb);
	*pcb = cb;

	return 0;
}


IMPLEMENT_DYNAMIC(CScriptsDlg, CExtResizableDialog)

CScriptsDlg::CScriptsDlg(CWnd* pParent /*=NULL*/)
	: CExtResizableDialog(CScriptsDlg::IDD, pParent), m_nPrevSel(-1)
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
	ON_WM_CREATE()
	ON_COMMAND(ID_SCRIPT_OPEN, OnScriptOpen)
	ON_COMMAND(ID_SCRIPT_SAVE, OnScriptSave)
	ON_COMMAND(ID_SCRIPT_SAVE_ALL, OnScriptSaveAll)
	ON_COMMAND(ID_SCRIPT_RUN,	OnScriptRun)	
	ON_COMMAND(ID_SCRIPT_STOP,	OnScriptStop)

	ON_LBN_DBLCLK(IDC_LIST_SCRIPTS, OnListScriptsDoubleClick)
	ON_LBN_SELCHANGE(IDC_LIST_SCRIPTS, OnListScriptsSelChange)
	ON_LBN_SELCANCEL(IDC_LIST_SCRIPTS, OnListScriptsSelCancel)

	ON_EN_CHANGE(IDC_SCRIPT_TEXT, OnChangeScriptText)
	ON_NOTIFY(EN_SELCHANGE, IDC_SCRIPT_TEXT, OnSelChangeScriptText)
END_MESSAGE_MAP()


// CScriptsDlg message handlers
int CScriptsDlg::OnCreate( LPCREATESTRUCT lpCreateStruct  )
{
	if ( !m_wndStatus.Create(this) )
		TRACE("\n\tCScriptsDlg-->Status bar creation error");

	m_wndStatus.AddPane(1250, 0); m_wndStatus.SetPaneWidth(0, 100);
	m_wndStatus.AddPane(1251, 1); m_wndStatus.SetPaneWidth(1, 100);
	m_wndStatus.AddPane(1252, 2); m_wndStatus.SetPaneWidth(2, 100);
	m_wndStatus.AddPane(1253, 3); m_wndStatus.SetPaneWidth(3, 100);
	
	return CExtResizableDialog::OnCreate(lpCreateStruct);
}

void CScriptsDlg::UpdateStatusBar()
{
	long nStart, nEnd;
	m_wndSriptText.GetSel(nStart, nEnd);
	
	int line = m_wndSriptText.LineFromChar(nStart);
	// LineIndex возвращает индеск символа, с которого начинается строка
	int col  = nStart - m_wndSriptText.LineIndex();

	CString str;
	str.Format("Строка : %d", line);
	m_wndStatus.SetPaneText(0, str);

	str.Format("Позиция : %d", col);
	m_wndStatus.SetPaneText(1, str);

	str.Format("Позиция2 : %d", line);
	m_wndStatus.SetPaneText(2, str);

	str.Format("Позиция3 : %d", col);
	m_wndStatus.SetPaneText(3, str);
}

BOOL CScriptsDlg::OnInitDialog()
{
	CExtResizableDialog::OnInitDialog();

	if ( !m_wndScriptsToolBar.LoadToolBar(IDR_SCRIPTS_TOOL_BAR) )
		return FALSE;

	// TODO:  Add extra initialization here
	AddAnchor( IDC_LIST_SCRIPTS, CPoint(0, 0), CPoint(10, 100));
	AddAnchor( IDC_SCRIPT_TEXT, CPoint(10, 0), CPoint(100, 100));

	//set the event mask to accept ENM_CHANGE messages
	long mask = m_wndSriptText.GetEventMask();
	m_wndSriptText.SetEventMask(mask |= ENM_CHANGE|ENM_SELCHANGE );

	//reconfigure CSyntaxColorizer's default keyword groupings
	LPTSTR sKeywords = "PRINT,WAIT,STOP,END,IF,THEN,ELSE,GOTO,FLOW,FOR,TO,NEXT,LET,ENDIF";
	LPTSTR sDirectives = "FLOW,SWITCH,CHECK";
	LPTSTR sPragmas = "comment,optimize,auto_inline,once,warning,component,pack,function,intrinsic,setlocale,hdrstop,message";

	vector<const DataSrc*> ds;
	string names;

	DataSrc::EnumDataSources(ds, NULL);
	for (vector<const DataSrc*>::const_iterator it = ds.begin(); it != ds.end(); ++it)
	{
		names += (*it)->Name();
		names += ",";
	}

	LPTSTR sDataSources = (LPTSTR)names.c_str();

	m_sc.ClearKeywordList();
	m_sc.AddKeyword(sKeywords,	RGB(0,0,255),GRP_KEYWORD);
	m_sc.AddKeyword(sDirectives,	0x9900CC, 2);
	m_sc.AddKeyword(sPragmas,	RGB(0,0,255), 3);
	m_sc.AddKeyword("REM,Rem,rem", RGB(255, 0, 255), 4);
	m_sc.AddKeyword(sDataSources, RGB(128, 0, 0), 5);
	
	int sel = m_wndList.GetCurSel();

	CRect r;
	m_wndSriptText.GetRect(&r);

	// Reduce the formatting rect of the rich edit control by 10 pixels on each side.

	if ( (r.Width() > 20) && (r.Height() > 20) )
	{
		r.DeflateRect(20, 5);
		m_wndSriptText.SetRect(&r);
	}

	m_wndSriptText.SetBackgroundColor(FALSE, RGB(250,250,250));

	UpdateStatusBar();

	RepositionBars(0, 0xFFFF, IDC_SCRIPT_TEXT); 

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CScriptsDlg::readFile(CString sFileName)
{
	m_wndSriptText.SetRedraw(FALSE);

	CFile file(sFileName, CFile::modeRead);
	
	m_es.dwCookie = (DWORD)&file;
	m_es.pfnCallback = readFunction;
	m_wndSriptText.StreamIn(SF_TEXT, m_es);

	file.Close();

	parse();

}

void CScriptsDlg::saveFile(CString sFileName)
{
	m_wndSriptText.SetRedraw(FALSE);

	CFile file(sFileName, CFile::modeReadWrite);

	m_es.dwCookie = (DWORD)&file;
	m_es.pfnCallback = writeFunction;
	m_wndSriptText.StreamOut(SF_TEXT, m_es);
	
	file.Close();
	//parse();
}


void CScriptsDlg::parse()
{
	//turn off response to onchange events
	long mask = m_wndSriptText.GetEventMask();
	m_wndSriptText.SetEventMask(mask ^= ENM_CHANGE );

	//set redraw to false to reduce flicker, and to speed things up
	m_wndSriptText.SetRedraw(FALSE);

	//call the colorizer
	m_sc.Colorize(0, -1, &m_wndSriptText);

	//do some cleanup
	m_wndSriptText.SetSel(0,0);
	m_wndSriptText.SetRedraw(TRUE);
	m_wndSriptText.RedrawWindow();

	m_wndSriptText.SetEventMask(mask |= ENM_CHANGE );
}
void CScriptsDlg::parse2()
{
	//get the current line of text from the control
	int len = m_wndSriptText.LineLength();
	int start = m_wndSriptText.LineIndex();

	//call the colorizer
	m_sc.Colorize(start, start + len, &m_wndSriptText);
}




void CScriptsDlg::OnChangeScriptText() 
{
	CHARRANGE cr;
	m_wndSriptText.GetSel(cr);
	parse2();
	m_wndSriptText.SetSel(cr);	
	UpdateStatusBar();
}

void CScriptsDlg::OnSelChangeScriptText( NMHDR* pNotifyStruct, LRESULT* lResult )
{
	UpdateStatusBar();
	*lResult = 0L;
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

void CScriptsDlg::OnListScriptsDoubleClick()
{
	OnScriptRun();
}

void CScriptsDlg::OnListScriptsSelCancel()
{
	int ind = m_wndList.GetCurSel();
	SCRIPTINFO* si = (SCRIPTINFO*)m_wndList.GetItemData(ind);	

	if ( m_wndSriptText.GetModify() )
	{
		CString strMsg;
		strMsg.Format("Управляющий алгоритм <%s> был изменнен. Желаете сохранить изменения", si->path.c_str() );

		if ( AfxMessageBox(strMsg, IDYES|IDNO|MB_ICONQUESTION) == IDYES )
		{
			////TRACE("\n\t Сохранение управляющего алгоритма <%s>", si->path.c_str());
			saveFile( si->path.c_str() );
		}

	}

	
}

void CScriptsDlg::OnListScriptsSelChange()
{	
	
	SCRIPTINFO* si = (SCRIPTINFO*)m_wndList.GetItemData(m_nPrevSel);	

	if ( si!= NULL && m_wndSriptText.GetModify() )
	{
		CString strMsg;
		strMsg.Format("Управляющий алгоритм <%s> был изменнен.\n\rЖелаете сохранить изменения?", si->path.c_str() );

		if ( AfxMessageBox(strMsg, MB_YESNO|MB_ICONQUESTION) == IDYES )
		{
			////TRACE("\n\t Сохранение управляющего алгоритма <%s>", si->path.c_str());
			saveFile(si->path.c_str() );
		}

	}

	int ind = m_wndList.GetCurSel();
	si = (SCRIPTINFO*)m_wndList.GetItemData(ind);	
	
	readFile(si->path.c_str());	

	m_wndSriptText.SetModify(FALSE);
	m_nPrevSel = ind;
}
