#pragma once

#include "singletone.h"

#ifndef _APP_SCRIPTS_HELPER_H_INCLUDED_
#define _APP_SCRIPTS_HELPER_H_INCLUDED_

//////////////////////////////////////////////////////////////////////////
//  [12/1/2010 Johnny A. Matveichik]
//  OBA SCRIPT HELPER
//////////////////////////////////////////////////////////////////////////
class COBAScripter;

typedef struct tagScriptInfo 
{
	tagScriptInfo () : pScript(NULL), autostart(false)
	{

	};

	std::string name;
	std::string path;
	std::string ds;
	bool		autostart;

	COBAScripter* pScript;

}SCRIPTINFO,* LPSCRIPTINFO;

typedef struct tagSCRIPTIDLOCATION
{
	unsigned line;
	std::string function;
} SCRIPTIDLOCATION, *LPSCRIPTIDLOCATION;



typedef std::map< std::string, std::vector<SCRIPTIDLOCATION> > IDMap;

class auto_count
{
public:
	auto_count(unsigned& count) : c(count)
	{
		c++;
	}
	~auto_count()
	{
		c--;
	}
private:
	unsigned& c;
};



class AppOBAScriptsHelper : public Singletone<AppOBAScriptsHelper>
{
	friend class Singletone<AppOBAScriptsHelper>;

public: 
	
	virtual ~AppOBAScriptsHelper();

protected:

	AppOBAScriptsHelper();
	AppOBAScriptsHelper(const AppOBAScriptsHelper& );
	const AppOBAScriptsHelper& operator= (const AppOBAScriptsHelper&);

	std::string::size_type ProcessString(IDMap& ids, unsigned line, std::string str) const;

public:

	bool GetScriptIDs(const char* pszScriptPath, IDMap& ids) const;

	bool LoadScript(std::string name, std::string path) const;

	bool LoadScript(LPSCRIPTINFO lpsi) const;

	bool RunScript(std::string name, bool bDebug = false) const;

	bool TerminateScript(std::string name) const;

	void GetScriptInfo(std::string name, SCRIPTINFO& si) const; 

	void EnumScripts(std::vector<SCRIPTINFO>& scripts) const;

	//����� ���������
	static void OnScriptEnd(COBAScripter* pScript, int errCode, void* pParam);

	//����� ���������
	static void Print(const char* strMsg);

	//�������� ����������� ���������
	static int Check(const char* strID);

	//������������ ����������� ���������
	static int Switch(const char* strID, bool newState);	

	//��������� �������
	static double ValueGet(const char* strID);

	//������� �������
	static double ValueSet(const char* strID, double newFlow);

	//����������� ���������
	static void PopupMessage(const char* strTitle, const char* strMessage, int type);

	//������ ������������� ���������
	static int ConfirmDlg(const char* strTitle, const char* strMessage);

	//��������� ������ 
	static int Execute(const char* name);

	//���������� ��������� ����������
	static void StringSet(const char* strID, const char* text );

	static std::string StringGet(const char* strID);

	COBAScripter* ExecuteScript(std::string path, bool debug);

	bool IsActive(COBAScripter* pScript) const;

	bool HasActive() const;

	void TerminateAllActive() const;

	bool CheckScript(std::string path, std::size_t& nErrors, std::size_t& nWarnings ) const;

private:

	CRITICAL_SECTION m_cs;

	std::map <std::string, SCRIPTINFO> scriptsHost;
	static HWND m_hMainWnd;

};

#endif //_APP_SCRIPTS_HELPER_H_INCLUDED_
