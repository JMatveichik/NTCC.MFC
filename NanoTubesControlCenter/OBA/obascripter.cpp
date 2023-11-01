#include "stdafx.h"
#include "obascripter.h"
#include "..\oba\obaerrormsg.h"

using namespace std;

LPFNENDCALLBACK COBAScripter::lpfnCallBack = NULL; 
LPFNPRINT COBAScripter::lpfnPrint = NULL;
LPFNCHECK COBAScripter::lpfnCheck = NULL;
LPFNSWITCH COBAScripter::lpfnSwitch = NULL;
LPFNGETVALUE COBAScripter::lpfnGetValue = NULL;
LPFNSETVALUE COBAScripter::lpfnSetValue = NULL;
LPFNMESSAGE COBAScripter::lpfnPopupMessage = NULL;
LPFNCONFIRM COBAScripter::lpfnConfirmDlg = NULL;
LPFNEXECUTE COBAScripter::lpfnExecute = NULL;
LPFNSTRINGSET COBAScripter::lpfnStringSet = NULL;
LPFNSTRINGGET COBAScripter::lpfnStringGet = NULL;

COBAScripter* COBAScripter::Create()
{
	return new COBAScripter;
}

void COBAScripter::Initialize(
	LPFNENDCALLBACK pEndCallback, 
	LPFNPRINT pPrint, 
	LPFNCHECK pCheck, 
	LPFNSWITCH pSwitch, 
	LPFNGETVALUE pGetVal, 
	LPFNSETVALUE pSetVal, 
	LPFNMESSAGE pMessage, 
	LPFNCONFIRM pConfirm, 
	LPFNEXECUTE pExecute,	
	LPFNSTRINGGET pStringGet,
	LPFNSTRINGSET pStringSet)
{
	lpfnCallBack = pEndCallback;
	lpfnPrint = pPrint;
	lpfnCheck = pCheck;
	lpfnSwitch = pSwitch;
	lpfnGetValue = pGetVal;
	lpfnSetValue = pSetVal;
	lpfnPopupMessage = pMessage;
	lpfnConfirmDlg = pConfirm;
	lpfnExecute = pExecute;
	lpfnStringGet = pStringGet;
	lpfnStringSet = pStringSet;

}

COBAScripter::COBAScripter()
{
	ASSERT(lpfnPrint!=NULL);
	ASSERT(lpfnCheck!=NULL);
	ASSERT(lpfnSwitch!=NULL);
	ASSERT(lpfnGetValue!=NULL);
	ASSERT(lpfnSetValue!=NULL);	
	ASSERT(lpfnPopupMessage!=NULL);
	ASSERT(lpfnConfirmDlg !=NULL);
	ASSERT(lpfnExecute !=NULL);
	ASSERT(lpfnStringSet !=NULL);

	hThread = NULL;
	bDebug = false;
	hStopScript = CreateEvent(NULL, TRUE, FALSE, NULL);
	hContinue = CreateEvent(NULL, FALSE, FALSE, NULL);
	
}

COBAScripter::~COBAScripter()
{
	CloseHandle(hStopScript);
	CloseHandle(hContinue);
}

void COBAScripter::Execute(std::string strScriptPath, void* p)
{
	scriptPath = strScriptPath;
	pParam = p;

	DWORD dwID;
	hThread = CreateThread(NULL, 0, ThreadProc, this, 0, &dwID);
}

bool COBAScripter::IsRunnig() const 
{
	return (hThread != NULL);
}

void COBAScripter::Terminate()
{
	if (!hThread)
		return;

	SetEvent(hStopScript);
	SetEvent(hContinue);
	
	if ( WaitForSingleObject(hThread, 5000) == WAIT_OBJECT_0 )
		return;
	
	TerminateThread(hThread, -1);
	CloseHandle(hThread);
	hThread = NULL;

	lpfnCallBack(this, -1, pParam);
	delete this;
}

void COBAScripter::MakeStep()
{
	SetEvent(hContinue);
}

DWORD WINAPI COBAScripter::ThreadProc(LPVOID lp) 
{ 
	return static_cast<COBAScripter*>(lp)->Scripter(); 
}

DWORD COBAScripter::Scripter()
{
	oba obasic(lpfnPrint, lpfnCheck, lpfnSwitch, lpfnGetValue, lpfnSetValue, lpfnPopupMessage,lpfnConfirmDlg, lpfnExecute, lpfnStringGet, lpfnStringSet);

	try {
		obasic.debug(bDebug);
        
		obasic.load(scriptPath.c_str());

		if (!bDebug)
		{
			int code;
			
			do {
				if (WaitForSingleObject(hStopScript, 50) == WAIT_OBJECT_0)
					break;

				code = obasic.run();
				
				ostream* pout = obasic.getoutputstream();
				if ( pout )
					pout->flush();

			} while (code == OBA_BREAKBEFORE || code == OBA_BREAKAFTER);

			lpfnCallBack(this, 0, pParam);

			delete this;

			return 0;
		}

		// Prepare line numbers vs file pointer
		ifstream bas(scriptPath.c_str(), ios::binary);
		string str;
		
		vector<streampos> string_map;
		
		do {
			if (WaitForSingleObject(hStopScript, 50) == WAIT_OBJECT_0)
				break;

			streampos pos = bas.tellg();

			string_map.push_back(pos);
			
			getline(bas, str);

		} while (bas.good());

		bas.close();

		lpfnBreakCallBack(this, 1, pParam);
		
		int code = 0;
		
		while(1) {
			
			if (code != OBA_BREAKAFTER && code != OBA_BREAKBEFORE)
				WaitForSingleObject(hContinue, INFINITE);
			
			
			if (WaitForSingleObject(hStopScript, 50) == WAIT_OBJECT_0)
				break;

			code = obasic.run();

			if (code == OBA_BREAKAFTER || code == OBA_BREAKBEFORE)
			{
				if ( obasic.getcurrentoperator() == "PRINT" )
				{
					ostream* pout = obasic.getoutputstream();
					if ( pout )
						pout->flush();
				}
				continue;
			}

			if ( !bDebug )
				break;

			if (code != OBA_ENDOFLINE && code != OBA_BREAK)
				break;

			streampos p = obasic.getfilepos();

			for (unsigned line = 1; line <= string_map.size(); ++line)
			{
				if (string_map[line-1] >= p)
				{
					lpfnBreakCallBack(this, line, pParam);
					break;
				}
			}
		}
	}
	catch (oba_err &e)
	{
		//cerr << e.what() << endl;
		lpfnCallBack(this, e.code(), pParam);
		
		//hThread = NULL;
		//delete this;

		return -1;
	}

	////TRACE("!!! THREAD - BEFORE CALLBACK!!!\n");
	
	lpfnCallBack(this, 1, pParam);
	
	////TRACE("*** THREAD - AFTER CALLBACK***\n");
	//hThread = NULL;
	//delete this;
	
	////TRACE("+++ THREAD - AFTER DELETING+++\n");
	return 0;
}

void COBAScripter::DebugMode(bool bEnable, LPFNBREAKCALLBACK pBreakCallBack)
{
	if ( bEnable )
		ASSERT(pBreakCallBack != NULL);
	else
		SetEvent(hContinue);
	
	bDebug = bEnable;
	lpfnBreakCallBack = pBreakCallBack;
}

std::string COBAScripter::GetErrorMessage(unsigned int code, bool bRussian)
{
	return OBAError(code, bRussian);
}