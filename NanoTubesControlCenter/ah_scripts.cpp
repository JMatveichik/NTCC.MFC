#include "stdafx.h"

#include "ah_scripts.h"
#include "ah_msgprovider.h"
#include "ah_loadcfg.h"

#include "OBA/obascripter.h"
#include "Automation/DataSrc.h"
#include "resource.h"


using namespace std;
//////////////////////////////////////////////////////////////////////////
//  [12/1/2010 Johnny A. Matveichik]

HWND AppOBAScriptsHelper::m_hMainWnd = NULL;

AppOBAScriptsHelper::AppOBAScriptsHelper()
{
	if (::AfxGetApp()->m_pMainWnd != NULL )
		m_hMainWnd = ::AfxGetApp()->m_pMainWnd->m_hWnd;

	::InitializeCriticalSection(&m_cs);

	COBAScripter::Initialize( OnScriptEnd, Print, Check, Switch, ValueGet, ValueSet, PopupMessage, ConfirmDlg, Execute, StringGet, StringSet);
}

AppOBAScriptsHelper::~AppOBAScriptsHelper()
{
	::DeleteCriticalSection(&m_cs);
}

std::string::size_type AppOBAScriptsHelper::ProcessString(IDMap& ids, unsigned line, std::string str) const
{
	// List of specific function names
	static list<string> funName;
	static string strFirstLetters;

	if ( funName.empty() )
	{
		funName.insert(funName.begin(), "CHECK");		
		funName.insert(funName.begin(), "SWITCH");
		funName.insert(funName.begin(), "VALUE");
		funName.insert(funName.begin(), "EXECUTE");		
		funName.insert(funName.begin(), "STRINGSET");	

		set<char> fl;
		for(list<string>::const_iterator it=funName.begin(); it!=funName.end(); ++it)
			fl.insert(it->at(0));

		strFirstLetters.assign(fl.begin(), fl.end());
	}

	string name;
	string::size_type pos = 0;

	for (;;)
	{
		pos = str.find_first_of(strFirstLetters, pos);
		if (pos == string::npos)
			return string::npos;

		for(list<string>::const_iterator it = funName.begin(); it != funName.end(); ++it)
		{
			string::size_type res = str.find(*it, pos);
			if (res == string::npos || res != pos)
				continue;

			res += it->length();

			if ( str[res] != '(')
				continue;

			static string sequence("(\"\"");

			name.clear();
			string::size_type next;
			for (size_t i=0; i<sequence.length(); ++i)
			{
				next = str.find(sequence[i], res);
				if (next == string::npos)
					break;

				name.assign(&str[res], &str[next]);
				res = next + 1;
				if (res >= str.length())
					break;
			}

			if (name.empty())
				continue;

			std::transform(name.begin(), name.end(), name.begin(), toupper);

			if (name == "ON" || name =="OFF")
				continue;

			SCRIPTIDLOCATION loc;
			loc.function = *it;
			loc.line = line;

			vector<SCRIPTIDLOCATION> locs;
			locs.push_back(loc);
			pair<IDMap::iterator, bool> result = ids.insert(make_pair(name, locs));

			if ( !result.second )
				result.first->second.push_back( loc );

			if (res < str.length())
				return ProcessString(ids, line, string(&str[res], &str[ str.length() - 1] ));

			break;
		}
		++pos;
	}

	return pos;
}

bool AppOBAScriptsHelper::GetScriptIDs(const char* pszScriptPath, IDMap& ids) const
{
	ids.clear();

	std::map <std::string, SCRIPTINFO>::const_iterator fnd = scriptsHost.find(pszScriptPath);
	if ( fnd == scriptsHost.end() )
		return false;

	ifstream in((*fnd).second.path, ios::in|ios::binary);

	if (!in.good())
		return false;

	string line;
	unsigned lineCounter = 0;
	while (getline(in, line).good())
		ProcessString(ids, ++lineCounter, line);

	return true;
}

void AppOBAScriptsHelper::GetScriptInfo(std::string name, SCRIPTINFO& si) const
{
	si.name = "";
	si.path = "";
	si.pScript = NULL;

	//::EnterCriticalSection(&m_cs);

	std::map <std::string, SCRIPTINFO>::const_iterator fnd = scriptsHost.find(name);

	if ( fnd != scriptsHost.end() )
		si = (*fnd).second;

	//::LeaveCriticalSection(&m_cs);
}

void AppOBAScriptsHelper::EnumScripts(std::vector<SCRIPTINFO>& scripts) const
{
	scripts.clear();
	std::map <std::string, SCRIPTINFO>::const_iterator it;

	//::EnterCriticalSection(&m_cs);

	for(it  = scriptsHost.begin(); it != scriptsHost.end(); it++)
		scripts.push_back((*it).second);

	//::LeaveCriticalSection(&m_cs);
}

bool AppOBAScriptsHelper::LoadScript(LPSCRIPTINFO lpsi) const
{
	//::EnterCriticalSection(&m_cs);
	std::map <std::string, SCRIPTINFO>::const_iterator fnd = scriptsHost.find(lpsi->name);
	//::LeaveCriticalSection(&m_cs);

	if ( fnd != scriptsHost.end() )
		return false;

	if ( !lpsi->path.length() )
		return false;

	SCRIPTINFO si;

	std::ifstream inp(lpsi->path.c_str());
	if ( inp.fail() )
		return false;

	inp.close();

	si.name = lpsi->name;
	si.path = lpsi->path;
	si.ds = lpsi->ds;
	si.autostart = lpsi->autostart;


	//::EnterCriticalSection(&m_cs);

	if ( !const_cast<AppOBAScriptsHelper*>(this)->scriptsHost.insert( std::make_pair(si.name, si) ).second  )
	{
		//::LeaveCriticalSection(&m_cs);
		return false;
	}

	//::LeaveCriticalSection(&m_cs);
	return true;
}


bool AppOBAScriptsHelper::LoadScript(std::string name, std::string path) const
{
	//::EnterCriticalSection(&m_cs);
	std::map <std::string, SCRIPTINFO>::const_iterator fnd = scriptsHost.find(name);
	//::LeaveCriticalSection(&m_cs);

	if ( fnd != scriptsHost.end() )
		return false;

	if ( !path.length() )
		return false;

	SCRIPTINFO si;

	std::ifstream inp(path.c_str());
	if ( inp.fail() )
		return false;

	inp.close();

	si.name = name;
	si.path = path;

	//::EnterCriticalSection(&m_cs);
	
	if ( !const_cast<AppOBAScriptsHelper*>(this)->scriptsHost.insert( std::make_pair(name, si) ).second  )
	{
		//::LeaveCriticalSection(&m_cs);
		return false;
	}

	//::LeaveCriticalSection(&m_cs);
	return true;
}

bool AppOBAScriptsHelper::RunScript(std::string name, bool bDebug /*= false*/) const
{
	//::EnterCriticalSection(&m_cs);
	std::map <std::string, SCRIPTINFO>::const_iterator fnd = scriptsHost.find(name);

	if (fnd == scriptsHost.end())
	{	
		//::LeaveCriticalSection(&m_cs);
		return false;
	}

	SCRIPTINFO*   psi = const_cast<SCRIPTINFO*>( &((*fnd).second) );
	COBAScripter* p = psi->pScript = COBAScripter::Create();

	//::LeaveCriticalSection(&m_cs);

	Switch( psi->ds.c_str(), true );
	p->Execute(psi->path, psi);
	return true;
}

int AppOBAScriptsHelper::Execute(const char* name)
{
	if ( AppOBAScriptsHelper::Instance().RunScript(name, false) )
		return 1;
	else {
		
		CString str;
		str.Format("EXECUTE : Invalid script name %s", name);
		Print((LPCTSTR)str);

		return 0;
	}
}

bool AppOBAScriptsHelper::TerminateScript(std::string name) const
{
	//::EnterCriticalSection(&m_cs);
	std::map <std::string, SCRIPTINFO>::const_iterator fnd = scriptsHost.find(name);
	//::LeaveCriticalSection(&m_cs);

	if (fnd == scriptsHost.end())
		return false;

	const SCRIPTINFO& si = (*fnd).second;

	si.pScript->Terminate();
	return true;
}

//после завершение выполнения скрипта
void AppOBAScriptsHelper::OnScriptEnd(COBAScripter* pScript, int errCode, void* pParam)
{
	LPSCRIPTINFO lpsi = (SCRIPTINFO*)pParam;
	//ASSERT (pScript == lpsi->pScript);

	lpsi->pScript = NULL;

	CString name( (lpsi == NULL) ? "UNKNOWN" : lpsi->name.c_str() );
	CString str;
	str.Format("SCRIPT %s terminated with code %d (%s)", name, errCode, pScript->GetErrorMessage(errCode).c_str());
	
	Switch( lpsi->ds.c_str(), false );
	

	if (errCode != 0)
		AppMessagesProviderHelper::Instance().ShowPopupMessage((LPCTSTR)str, "SCRIPT " + name ,COutMessage::MSG_ERROR);

	AppMessagesProviderHelper::Instance().ShowMessage((LPCTSTR)str, COutMessage::MSG_OBA_SCRIPT);
	
}

//Вывод сообщения
void AppOBAScriptsHelper::Print(const char* strMsg)
{
	/*//TRACE("\t=> %s\n", strMsg);*/
	COutMessage msg(strMsg, "", COutMessage::MSG_OBA_SCRIPT); 
	//::SendMessage(m_hMainWnd, RM_SHOW_USER_MESSAGE, 0, (LPARAM)&msg);
	AppMessagesProviderHelper::Instance().ShowMessage(strMsg, COutMessage::MSG_OBA_SCRIPT);
	////TRACE("\n->%s", strMsg);

}

void AppOBAScriptsHelper::PopupMessage(const char* strTitle, const char* strMessage, int type)
{
	COutMessage msg(strMessage, strTitle, (COutMessage::MESSAGE_TYPE)type);
	AppMessagesProviderHelper::Instance().ShowPopupMessage(&msg);

	//::SendMessage(m_hMainWnd, RM_SHOW_USER_MESSAGE, 1, (LPARAM)&msg);
}

int AppOBAScriptsHelper::ConfirmDlg(const char* strTitle, const char* strMessage)
{
	CWnd* pMainWnd =  ::AfxGetApp()->GetMainWnd();
	/*if ( MessageBox(pMainWnd->m_hWnd, strMessage, strTitle, MB_ICONEXCLAMATION|MB_YESNO) == IDYES )
		return 1;*/
	std::string  str = strMessage;
	std::string snl;  snl += char(13);
	std::string stab; stab += '\t';
	boost::replace_all(str, "\\n", snl);
	boost::replace_all(str, "\\t", stab);

	if ( MessageBox(pMainWnd->m_hWnd, str.c_str(), strTitle, MB_ICONEXCLAMATION|MB_YESNO) == IDYES )
		return 1;
	return 0;
}

//проверка дискретного состояния
int AppOBAScriptsHelper::Check(const char* strID)
{
	DiscreteDataSrc* pDDS = dynamic_cast<DiscreteDataSrc*>(DataSrc::GetDataSource(string(strID)));
	if ( pDDS == NULL )
	{
		CString str;
		str.Format("CHECK : Invalid id %s", strID);
		Print((LPCTSTR)str);

		throw oba_err(97);
	}
	else
		return pDDS->IsEnabled();
}

//установить текстовую переменную
void AppOBAScriptsHelper::StringSet(const char* strID, const char* text )
{
	StringDataSrc* pSDS = dynamic_cast<StringDataSrc*>(DataSrc::GetDataSource(string(strID)));
	if ( pSDS == NULL )
	{
		CString str;
		str.Format("STRING : Invalid id %s", strID);
		Print((LPCTSTR)str);

		throw oba_err(97);
	}
	else
		pSDS->SetString(text);
}

std::string AppOBAScriptsHelper::StringGet(const char* strID)
{
	StringDataSrc* pSDS = dynamic_cast<StringDataSrc*>(DataSrc::GetDataSource(string(strID)));
	if ( pSDS == NULL )
	{
		CString str;
		str.Format("STRING : Invalid id %s", strID);
		Print((LPCTSTR)str);

		throw oba_err(97);
	}
	else
		return pSDS->GetString();
}

//установление дискретного состояния
int AppOBAScriptsHelper::Switch(const char* strID, bool newState)
{
	DiscreteOutputDataSrc* pDDS = dynamic_cast<DiscreteOutputDataSrc*>(DataSrc::GetDataSource(string(strID)));
	if ( pDDS == NULL )
	{
		CString str;
		str.Format("SWITCH : Invalid id %s", strID);
		Print((LPCTSTR)str);
		throw oba_err(98);
	}
	else
	{
		pDDS->Enable(newState);
		return pDDS->IsEnabled();
	}	
}

//получение расхода
double AppOBAScriptsHelper::ValueGet(const char* strID)
{
	AnalogDataSrc* pADS = dynamic_cast<AnalogDataSrc*>(DataSrc::GetDataSource(string(strID)));

	if ( pADS == NULL )
	{
		CString str;
		str.Format("VALUE GET: Invalid id %s", strID);
		Print((LPCTSTR)str);
		throw oba_err(99);
	}
	else
		return pADS->GetValue();
}

//задание расхода
double AppOBAScriptsHelper::ValueSet(const char* strID, double newFlow)
{
	AnalogOutputDataSrc* pADS = dynamic_cast<AnalogOutputDataSrc*>(DataSrc::GetDataSource(string(strID)));

	if ( pADS == NULL ) 
	{
		CString str;
		str.Format("VALUE SET : Invalid id %s", strID);
		Print((LPCTSTR)str);

		throw oba_err(99);
	}
	else
		return pADS->SetValue(newFlow);
}


COBAScripter* AppOBAScriptsHelper::ExecuteScript(std::string path, bool debug)
{
	return NULL;
}

bool AppOBAScriptsHelper::IsActive(COBAScripter* pScript) const
{
	return false;
}

bool AppOBAScriptsHelper::HasActive() const
{

	for (std::map <std::string, SCRIPTINFO>::const_iterator it = scriptsHost.begin(); it != scriptsHost.end(); ++it)
	{
		const SCRIPTINFO& si = (*it).second;
		if ( si.pScript != NULL )
			return true;
	}
	return false;
}

void AppOBAScriptsHelper::TerminateAllActive() const
{
	for (std::map <std::string, SCRIPTINFO>::const_iterator it = scriptsHost.begin(); it != scriptsHost.end(); ++it)
	{
		const SCRIPTINFO& si = (*it).second;
		if ( si.pScript != NULL )
			si.pScript->Terminate();
	}
}


bool AppOBAScriptsHelper::CheckScript(std::string path, std::size_t& nErrors, std::size_t& nWarnings ) const
{

	CString msgOut;			
	msgOut.Format(IDS_SCRIPT_CHECK1, path.c_str());	//"Processing script '%s'..."
	
	const AppMessagesProviderHelper& mh = AppMessagesProviderHelper::Instance();
	mh.ShowMessage((LPCTSTR)msgOut, COutMessage::MSG_TEXT, TRUE, FALSE);

	AppLoadingHelper::Instance().AddContentMessage((LPCTSTR)msgOut, COutMessage::MSG_OK);

	static unsigned ncount = 0;
	auto_count n(ncount);
	if ( ncount > 10 )
	{
		msgOut.Format(IDS_SCRIPT_CHECK0);	//"Potential script recursion warning."
		AppLoadingHelper::Instance().AddContentMessage((LPCTSTR)msgOut, COutMessage::MSG_WARNING);		
		mh.ShowMessage((LPCTSTR)msgOut, COutMessage::MSG_WARNING, TRUE, FALSE);

		nWarnings++;
		return false;
	}

	IDMap ids;
	if ( !AppOBAScriptsHelper::Instance().GetScriptIDs(path.c_str(), ids) )
	{
		msgOut.Format(IDS_SCRIPT_CHECK2, path.c_str());	//"Script file '%s' was not found"
		AppLoadingHelper::Instance().AddContentMessage((LPCTSTR)msgOut, COutMessage::MSG_ERROR);
		
		mh.ShowMessage((LPCTSTR)msgOut, COutMessage::MSG_WARNING, TRUE, FALSE);
		nErrors++;

		return false;
	}	

	for(IDMap::const_iterator it=ids.begin(); it!=ids.end(); ++it)
	{
		const DataSrc* pDS = DataSrc::GetDataSource(it->first);

		if ( pDS == NULL )				
		{
			CString msgOut1;	
			for (std::vector<SCRIPTIDLOCATION>::const_iterator it2=it->second.begin(); it2!=it->second.end(); ++it2)
			{
				if ( it2->function == "EXECUTE" )
				{
					if ( it->first == path )
					{
						nWarnings++;
						CString msg;
						msg.Format(IDS_SCRIPT_CHECK5, path.c_str() );  //"\tRecursion warning : script '%s' calls itself\n"

						AppLoadingHelper::Instance().AddContentMessage((LPCTSTR)msgOut, COutMessage::MSG_WARNING);
						mh.ShowMessage((LPCTSTR)msgOut, COutMessage::MSG_WARNING, TRUE, FALSE);
						break;
					}

					return CheckScript( it->first, nErrors, nWarnings );					
				}

				CString msg;
				msg.Format(IDS_SCRIPT_CHECK4, it2->line , it2->function.c_str() );  //"\tdefined in line #%d '%s'\n"
				msgOut1 += msg;				
			}

			nErrors++;

			msgOut.Format(IDS_SCRIPT_CHECK3, it->first.c_str()); //"Identifier '%s' is not registered as valid data source" 
			AppLoadingHelper::Instance().AddContentMessage((LPCTSTR)(msgOut + ": " + msgOut1), COutMessage::MSG_ERROR);

			mh.ShowMessage((LPCTSTR)(msgOut + ": " + msgOut1), COutMessage::MSG_WARNING, TRUE, FALSE);
			//AppLoadingHelper::Instance().AddContentMessage((LPCTSTR)msgOut1, COutMessage::MSG_OK);
		}
	}
	return true;
}