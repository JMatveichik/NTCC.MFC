
#pragma once

#include "..\..\oba\oba.h"

class COBAScripter;

typedef void (*LPFNENDCALLBACK)(COBAScripter* pScripter, int nErrorCode, void* p) ;
typedef void (*LPFNBREAKCALLBACK)(COBAScripter* pScripter, unsigned curline, void* p);

class COBAScripter
{
public:
	static COBAScripter* Create();
	
	static void Initialize(LPFNENDCALLBACK pEndCallback, 
							LPFNPRINT pPrint, 
							LPFNCHECK pCheck, 
							LPFNSWITCH pSwitch, 
							LPFNGETVALUE pGetVal, 
							LPFNSETVALUE pSetVal, 
							LPFNMESSAGE pMessage, 
							LPFNCONFIRM pConfirm, 
							LPFNEXECUTE pExecute,
							LPFNSTRINGGET pStringGet,
							LPFNSTRINGSET pStringSet);
	
	static std::string GetErrorMessage(unsigned int code, bool bRussian = false);

	void Execute(std::string strScriptPath, void* p);
	
	void Terminate();

	void DebugMode(bool bEnable=false, LPFNBREAKCALLBACK pBreakCallBack=NULL);
	
	void MakeStep();

	bool IsRunnig() const;

protected:
	COBAScripter();
	~COBAScripter();

	DWORD execute(std::string strScriptPath);

	static LPFNENDCALLBACK lpfnCallBack;
	static LPFNPRINT lpfnPrint;
	static LPFNCHECK lpfnCheck;
	static LPFNSWITCH lpfnSwitch;
	static LPFNGETVALUE lpfnGetValue;
	static LPFNSETVALUE lpfnSetValue;	
	static LPFNMESSAGE  lpfnPopupMessage;
	static LPFNCONFIRM  lpfnConfirmDlg;
	static LPFNEXECUTE  lpfnExecute;
	static LPFNSTRINGGET lpfnStringGet;
	static LPFNSTRINGSET lpfnStringSet;
	

	LPFNBREAKCALLBACK lpfnBreakCallBack;
	HANDLE hThread;
	HANDLE hContinue;
	HANDLE hStopScript;
	

	std::string scriptPath;
	bool bDebug;
	void* pParam;

	static DWORD WINAPI ThreadProc(LPVOID lp);// { return static_cast<COBAScripter*>(lp)->Scripter(); };
	DWORD Scripter();
};

