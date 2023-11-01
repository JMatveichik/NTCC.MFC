#include "stdafx.h"
#include "basic.h"
#include "obaerrormsg.h"
#include <assert.h>
#include <ob.h>
#include <map>
#include <vector>
#include <sys/timeb.h>
#include <time.h>
#include <iomanip>
#include <mmsystem.h>


using namespace ob_charspace;

inline unsigned long Milliseconds()
{
	SYSTEMTIME st;
	GetSystemTime(&st);

	return 24*3600*1000*st.wDay + 3600*1000*st.wHour + 60*1000*st.wMinute + 1000*st.wSecond + st.wMilliseconds;
}

inline unsigned long Milliseconds(const SYSTEMTIME& st)
{
	return 24*3600*1000*st.wDay + 3600*1000*st.wHour + 60*1000*st.wMinute + 1000*st.wSecond + st.wMilliseconds;
}

inline unsigned long Milliseconds(const SYSTEMTIME& st1, const SYSTEMTIME& st2)
{
	return Milliseconds(st2) - Milliseconds(st1);
}

#define BAS_VAR_OBA_POINTER_HIGH "OBA_PTR_HIGH_BY_BR%"
#define BAS_VAR_OBA_POINTER_LOW "OBA_PTR_LOW_BY_BR%"
#define DECLARE_OBA_PTR(name) \
	ob_type_int lo; ob_type_int hi; \
	basic_ptr->readvar(BAS_VAR_OBA_POINTER_HIGH, &hi); \
	basic_ptr->readvar(BAS_VAR_OBA_POINTER_LOW, &lo); \
	__int64 ph = hi; \
	__int64 pl = lo;\
	ph <<= 32; \
	oba* name = reinterpret_cast<oba*>(ph|pl);


int oba::wait(ob_obasic* basic_ptr, const char* parstring, 
	const unsigned *descrf, const unsigned *descri, const unsigned *descrc, 
	const float* parf, const int* pari, const char* parc)
{
	DECLARE_OBA_PTR(self); 

	if (descri[0] != 1 || descrf[0] > 0 || descrc[0] > 0)
		throw oba_err( 88 );

	static UINT wTimerRes = 0L;

	if (!wTimerRes)
	{
#define TARGET_RESOLUTION 10         // 10-millisecond target resolution enough for us

		TIMECAPS tc;
		if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) != TIMERR_NOERROR) 
			throw oba_err( 104 );

		wTimerRes = min(max(tc.wPeriodMin, TARGET_RESOLUTION), tc.wPeriodMax);
	}

	struct _timeb timebuffer;
	_ftime_s( &timebuffer );
	__int64 start = timebuffer.time*1000 + timebuffer.millitm;

	for(;;)
	{
		if (WaitForSingleObject(self->hBreak, wTimerRes) == WAIT_OBJECT_0)
			break;

		_ftime_s( &timebuffer );

		if (timebuffer.time*1000 + timebuffer.millitm - start >= pari[0])
			break;
	}

	return 0;
}

int oba::settimer_function(ob_obasic* basic_ptr, const char* parstring, 
	const unsigned *descrf, const unsigned *descri, const unsigned *descrc, 
	const float* parf, const int* pari, const char* parc)
{
	DECLARE_OBA_PTR(self);

	if (descri[0] != 1 || descrf[0] > 0 || descrc[0] > 0)
		throw oba_err(105);

	EnterCriticalSection(&self->m_timers_guard);
	
	SCRIPTTIMER stm;
	struct _timeb cur_time;
	_ftime_s( &cur_time ); 
	

	stm.lastcheck   = cur_time.time*1000 + cur_time.millitm;
	stm.elapcedtime = 0; 
	stm.endtime		= pari[0];	
	stm.active		= true;

	int tmid = self->m_next_id;
	self->m_timers_map[tmid]   = stm;	
	self->m_next_id++;

	LeaveCriticalSection(&self->m_timers_guard);
	
	return tmid;
}

int oba::modifytimer_function(ob_obasic* basic_ptr, const char* parstring, 
	const unsigned *descrf, const unsigned *descri, const unsigned *descrc, 
	const float* parf, const int* pari, const char* parc)
{
	DECLARE_OBA_PTR(self);

	if (descri[0] != 2 || descrf[0] > 0 || descrc[0] > 0)
		throw oba_err(105);

	EnterCriticalSection(&self->m_timers_guard);

	std::map<long, SCRIPTTIMER>::iterator it = self->m_timers_map.find( pari[0] );

	if (it == self->m_timers_map.end())
	{
		LeaveCriticalSection(&self->m_timers_guard);
		throw oba_err(107);
	}

	it->second.endtime		= pari[1];

	LeaveCriticalSection(&self->m_timers_guard);

	return 0;
}

int oba::checktimer_function(ob_obasic* basic_ptr, const char* parstring, 
	const unsigned *descrf, const unsigned *descri, const unsigned *descrc, 
	const float* parf, const int* pari, const char* parc)
{
	DECLARE_OBA_PTR(self);

	if (descri[0] != 1 || descrf[0] > 0 || descrc[0] > 0)
		throw oba_err(106);

	struct _timeb cur_time;
	_ftime_s( &cur_time );
	__int64 cur = cur_time.time*1000 + cur_time.millitm;

	EnterCriticalSection(&self->m_timers_guard);
	std::map<long, SCRIPTTIMER>::iterator it = self->m_timers_map.find( pari[0] );

	if (it == self->m_timers_map.end())
	{
		LeaveCriticalSection(&self->m_timers_guard);
		throw oba_err(107);
	}

	if ( it->second.active )
	{
		it->second.elapcedtime += (cur - it->second.lastcheck);
		it->second.lastcheck = cur;
	}

	int res = 0;
	if ( it->second.elapcedtime >= it->second.endtime )
	{
		self->m_timers_map.erase(it);
		res = 1;
	}

	LeaveCriticalSection(&self->m_timers_guard);

	return res;
}

int oba::pausetimer_function(ob_charspace::ob_obasic* basic_ptr, const char* parstring, 
		const unsigned *descrf, const unsigned *descri, const unsigned *descrc, 
		const float* parf, const int* pari, const char* parc)
{
	DECLARE_OBA_PTR(self);

	if (descri[0] != 1 || descrf[0] > 0 || descrc[0] > 0)
		throw oba_err(108);

	EnterCriticalSection(&self->m_timers_guard);
	std::map<long, SCRIPTTIMER>::iterator it = self->m_timers_map.find( pari[0] );

	if (it == self->m_timers_map.end())
	{
		LeaveCriticalSection(&self->m_timers_guard);
		throw oba_err(109);
	}
	
	if ( !it->second.active )
	{
		LeaveCriticalSection(&self->m_timers_guard);
		return 0;
	}

	struct _timeb cur_time;
	
	_ftime_s( &cur_time );
	__int64 cur = cur_time.time*1000 + cur_time.millitm;
	
	//it->second.lastcheck = cur;
	it->second.elapcedtime += (cur - it->second.lastcheck);	

	it->second.active = false;	

	LeaveCriticalSection(&self->m_timers_guard);

	return 0;
}


int oba::resumetimer_function(ob_charspace::ob_obasic* basic_ptr, const char* parstring, 
		const unsigned *descrf, const unsigned *descri, const unsigned *descrc, 
		const float* parf, const int* pari, const char* parc)
{
	DECLARE_OBA_PTR(self);

	if (descri[0] != 1 || descrf[0] > 0 || descrc[0] > 0)
		throw oba_err(108);

	EnterCriticalSection(&self->m_timers_guard);
	std::map<long, SCRIPTTIMER>::iterator it = self->m_timers_map.find( pari[0] );

	if (it == self->m_timers_map.end())
	{
		LeaveCriticalSection(&self->m_timers_guard);
		throw oba_err(109);
	}
	
	if ( it->second.active )
	{
		LeaveCriticalSection(&self->m_timers_guard);
		return 0;
	}
	
	struct _timeb cur_time;
	
	_ftime_s( &cur_time );
	__int64 cur = cur_time.time*1000 + cur_time.millitm;
	
	it->second.lastcheck = cur;	
	it->second.active = true;

	LeaveCriticalSection(&self->m_timers_guard);

	return 0;
}

int oba::check_function(ob_obasic* basic_ptr, const char* parstring, 
			  const unsigned *descrf, const unsigned *descri, const unsigned *descrc, 
			  const float* parf, const int* pari, const char* parc)
{
	DECLARE_OBA_PTR(self);

	if (descri[0] > 0 || descrf[0] > 0 || descrc[0] != 1)
		throw oba_err(89);

	std::string id;
	char c;
	unsigned index = 0;
	while ((c = toupper(parc[index++])) != 0)
		id += c;

	return self->lpfnCheck(id.c_str());
}

int oba::switch_function(ob_obasic* basic_ptr, const char* parstring, 
			  const unsigned *descrf, const unsigned *descri, const unsigned *descrc, 
			  const float* parf, const int* pari, const char* parc)
{
	DECLARE_OBA_PTR(self);

	if (descri[0] > 0 || descrf[0] > 0 || descrc[0] != 2)
		throw oba_err(90);

	std::string id;
	char c;
	unsigned index = 0;
	while ((c = toupper(parc[index++])) != 0)
		id += c;

	std::string cmd;
	while ((c = tolower(parc[index++])) != 0)
		cmd += c;

	bool state;
	if (cmd == "off")
		state = false;
	else if (cmd == "on")
		state = true;
	else
		throw oba_err(91);

	return self->lpfnSwitch(id.c_str(), state);
}

float oba::value_function(ob_obasic* basic_ptr, const char* parstring, 
			  const unsigned *descrf, const unsigned *descri, const unsigned *descrc, 
			  const float* parf, const int* pari, const char* parc)
{
	DECLARE_OBA_PTR(self);

	if (descrc[0] != 1)
		throw oba_err(92);

	if ( descri[0] + descrf[0] > 1 )
		throw oba_err(92);

	std::string id;
	char c;
	unsigned index = 0;
	while ((c = toupper(parc[index++])) != 0)
		id += c;

	if (descrf[0] == 0)
		return float(self->lpfnGetValue(id.c_str()));

	return float(self->lpfnSetValue(id.c_str(), double((descri[0] == 1) ? pari[0] : parf[0])));
}

int oba::message_function(ob_charspace::ob_obasic* basic_ptr, const char* parstring, 
	const unsigned *descrf, const unsigned *descri, const unsigned *descrc, 
	const float* parf, const int* pari, const char* parc)
{
	DECLARE_OBA_PTR(self);

	if (descrc[0] != 2 || descri[0] != 1)
		throw oba_err(96);

	unsigned index = 0;
	while ( (parc[index++]) != 0);		

	self->lpfnMessage(&parc[0], &parc[index], pari[0]);
	return 0;
}

int oba::confirm_function(ob_charspace::ob_obasic* basic_ptr, const char* parstring, 
	const unsigned *descrf, const unsigned *descri, const unsigned *descrc, 
	const float* parf, const int* pari, const char* parc)
{
	DECLARE_OBA_PTR(self);

	if (descrc[0] != 2)
		throw oba_err(96);

	unsigned index = 0;
	while ( (parc[index++]) != 0);
	
	return self->lpfnConfirm(&parc[0], &parc[index]);
}

int oba::execute_function(ob_charspace::ob_obasic* basic_ptr, const char* parstring, 
	const unsigned *descrf, const unsigned *descri, const unsigned *descrc, 
	const float* parf, const int* pari, const char* parc)
{
	DECLARE_OBA_PTR(self);

	if (descrc[0] != 1)
		throw oba_err(96);

	self->lpfnExecute(&parc[0]);
	return 0;
}

int oba::milliseconds_function(ob_charspace::ob_obasic* basic_ptr, const char* parstring, 
	const unsigned *descrf, const unsigned *descri, const unsigned *descrc, 
	const float* parf, const int* pari, const char* parc)
{
	DECLARE_OBA_PTR(self);

	if (descri[0] != 0 || descrf[0] != 0 || descrc[0] != 0)
		throw oba_err(108);

	unsigned long msfromStart = Milliseconds(self->stStart);

	return (int)msfromStart;
}

char* oba::string_function(ob_charspace::ob_obasic* basic_ptr, const char* parstring, 
	const unsigned *descrf, const unsigned *descri, const unsigned *descrc, 
	const float* parf, const int* pari, const char* parc)
{
	DECLARE_OBA_PTR(self);

	if (descri[0] > 0 || descrf[0] > 0 || descrc[0] < 1)
		throw oba_err(114);

	std::string id;
	char c;
	unsigned index = 0;
	while ((c = toupper(parc[index++])) != 0)
		id += c;

	std::string str = self->lpfnStringGet(id.c_str());
	char* p = self->prepare_string(str);

	if (descrc[0] == 1)
		return p;

	if (descrc[0] == 2)
	{
		std::string txt;
		while ((c = parc[index++]) != 0)
			txt += c;

		self->lpfnStringSet(id.c_str(), txt.c_str());
		return p;
	}

	return "";
}

char* oba::join_function(ob_charspace::ob_obasic* basic_ptr, const char* parstring, 
	const unsigned *descrf, const unsigned *descri, const unsigned *descrc, 
	const float* parf, const int* pari, const char* parc)
{
	DECLARE_OBA_PTR(self);

	int count = descri[0] + descrf[0] + descrc[0];
	
	std::vector<std::string> subs;
	subs.assign(count, "");

	for (unsigned i = 0; i < descri[0]; i++)
	{
		char res[256]; 
		sprintf_s(res, 256, "%d", pari[i]);
		subs[ descri[i + 1] ] = res;
	}
	
	for (unsigned i = 0; i < descrf[0]; i++)
	{
		char res[256]; 
		sprintf_s(res, 256, "%f", parf[i]);
		subs[descrf[i + 1]] = res;
	}
	
	for (unsigned i = 0; i < descrc[0]; i++)
	{
		char res[256]; 
		sprintf_s(res, 256, "%s", ob_getstringparam(descrc, parc, i) );
		subs[descrc[i + 1]] = res;
	}

	std::string res;
	for (int i = 0; i < count; i++ )
	{
		res += subs[i];
		//res += " ";
	}

	char* p = self->prepare_string(res);
	return p;
}

char* oba::prepare_string(std::string str)
{
	int len = sizeof(char)*str.length();
	char* p = new char[len + 1];
	strcpy_s(p, len + 1, str.c_str());

	return p;
}

char* oba::format_function(ob_charspace::ob_obasic* basic_ptr, const char* parstring, 
	const unsigned *descrf, const unsigned *descri, const unsigned *descrc, 
	const float* parf, const int* pari, const char* parc)
{
	DECLARE_OBA_PTR(self);

	if (descrc[0] != 1)
		throw oba_err(113);

	char* res = new char[2048];
	std::string fmt = ob_getstringparam(descrc, parc, 0);
	
	try {
		if (descrf[0] > 0)
			sprintf_s(res, 2048, fmt.c_str(), parf[0]);
		else if (descri[0] > 0)
			sprintf_s(res, 2048, fmt.c_str(), pari[0]);
	}
	catch(...)
	{
		strcpy_s(res, 2048, "INVALID FORMAT");
		return res;
	}

	return res;	
}

char* oba::date_time_function(ob_charspace::ob_obasic* basic_ptr, const char* parstring,
	const unsigned *descrf, const unsigned *descri, const unsigned *descrc,
	const float* parf, const int* pari, const char* parc)
{
	DECLARE_OBA_PTR(self);

	if (descrc[0] != 1)
		throw oba_err(113);

	char* res = new char[2048];
	std::string fmt = ob_getstringparam(descrc, parc, 0);

	time_t curTime = time(NULL);

	try {
		std::stringstream ss;
		std::tm t;
		localtime_s(&t, &curTime);
		ss << std::put_time(&t, fmt.c_str());
		strcpy_s(res, 2048, ss.str().c_str());
		
	}
	catch (...)
	{
		strcpy_s(res, 2048, "INVALID FORMAT");
		return res;
	}

	return res;
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
oba::oba(LPFNPRINT pPrint, 
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
	assert(hThread);

	InitializeCriticalSection(&m_timers_guard);
	m_next_id = 0;

	lpfnPrint = pPrint;
	lpfnCheck = pCheck;
	lpfnSwitch = pSwitch; 
	lpfnGetValue = pGetVal;
	lpfnSetValue = pSetVal;
	lpfnMessage = pMessage;
	lpfnConfirm = pConfirm;
	lpfnExecute  = pExecute;
	lpfnStringGet = pStringGet;
	lpfnStringSet = pStringSet;
	

	pimpl = new ob_obasic;
	pbas = new ifstream;

	fileName = new std::string;
	lastString  = new std::string;

	// Get the temp path

	char lpPathBuffer[MAX_PATH];
	GetTempPath(MAX_PATH,   lpPathBuffer);      

	// Create a temporary file. 
	char lpFileName[MAX_PATH];
	GetTempFileName(lpPathBuffer, "oba", 0, lpFileName);
	*fileName = lpFileName;

	pout = new fstream(lpFileName, ios::in|ios::out);

	pimpl->setout(pout);

	pimpl->setfun("WAIT", oba::wait);
	pimpl->setfun("CHECK", check_function);
	pimpl->setfun("SWITCH", switch_function);
	pimpl->setfun("VALUE", value_function);
	
	pimpl->setfun("MESSAGE", message_function);
	pimpl->setfun("CONFIRM", confirm_function);
	pimpl->setfun("EXECUTE", execute_function);
	pimpl->setfun("SETTIMER", settimer_function);
	pimpl->setfun("CHECKTIMER", checktimer_function);
	pimpl->setfun("PAUSETIMER", pausetimer_function);
	pimpl->setfun("RESUMETIMER", resumetimer_function);
	pimpl->setfun("MODIFYTIMER", modifytimer_function);
	pimpl->setfun("MILLISECONDS", milliseconds_function);
	pimpl->setfun("STRING", string_function, OB_DELETE);
	pimpl->setfun("JOIN", join_function, OB_DELETE);
	pimpl->setfun("FORMAT", format_function, OB_DELETE);
	pimpl->setfun("DATETIME", date_time_function, OB_DELETE);

	pimpl->setoperatorbreak("FOR", ob_operator::BREAK, ob_operator::BREAK);
	pimpl->setoperatorbreak("GOTO", ob_operator::BREAK, ob_operator::BREAK);

	pimpl->setoperatorbreak("PRINT", ob_operator::NOBREAK, ob_operator::BREAK);
	pimpl->setoperatorbreak("WAIT", ob_operator::BREAK, ob_operator::BREAK);
	pimpl->setoperatorbreak("CHECK", ob_operator::BREAK, ob_operator::BREAK);
	pimpl->setoperatorbreak("SWITCH", ob_operator::BREAK, ob_operator::BREAK);
	pimpl->setoperatorbreak("VALUE", ob_operator::BREAK, ob_operator::BREAK);
	pimpl->setoperatorbreak("SETTIMER", ob_operator::BREAK, ob_operator::BREAK);
	pimpl->setoperatorbreak("CHECKTIMER", ob_operator::BREAK, ob_operator::BREAK);
	pimpl->setoperatorbreak("PAUSETIMER", ob_operator::BREAK, ob_operator::BREAK);
	pimpl->setoperatorbreak("RESUMETIMER", ob_operator::BREAK, ob_operator::BREAK);
	pimpl->setoperatorbreak("MODIFYTIMER", ob_operator::BREAK, ob_operator::BREAK);

	hBreak = ::CreateEvent(NULL, TRUE, FALSE, NULL);

	hFile = NULL;

	DWORD dwID;
	hThread = ::CreateThread(NULL, 0, FileMonitor, this, 0, &dwID);
	
}

DWORD WINAPI oba::FileMonitor(LPVOID lpParameter)
{
	oba* pSelf = (oba*)lpParameter;
	
	while(true)
	{
		if ( WaitForSingleObject(pSelf->hBreak, 100) == WAIT_OBJECT_0 )
			break;

		pSelf->Monitor();		
	}

	return 0L;
}

void oba::Monitor()
{
	if ( !hFile )
		return;

	FlushFileBuffers(hFile);

	DWORD curPos = GetFileSize(hFile, NULL);

	if ( curPos < 0  || curPos == lastPos )
		return;

	const int BUFFSIZE = 4096;
	char szBuff[BUFFSIZE];

	SetFilePointer(hFile, lastPos, NULL, FILE_BEGIN);

	DWORD dwRead;
	ReadFile(hFile, szBuff, curPos - lastPos, &dwRead, NULL);
	
	szBuff[curPos - lastPos] = 0;
	lastPos = curPos;

	string str = *lastString;
	str.append(szBuff);

	string::size_type pos;
	
	while ((pos = str.find('\n')) != string::npos)
	{
		string msg(&str[0], &str[pos]);
		lpfnPrint(msg.c_str());
		//str.erase(&str[0], &str[pos+1]);
		str.erase(0, pos+1);
	}

	*lastString = str;
}

oba::~oba()
{
	Monitor();

	::SetEvent(hBreak);

	if ( WaitForSingleObject(hThread, 5000) != WAIT_OBJECT_0 )
		TerminateThread(hThread, -1);	

	::CloseHandle(hBreak);
	::CloseHandle(hThread);
	::CloseHandle(hFile);

	delete pimpl;
	
	pbas->close();
	delete pbas;

	pout->close();
	delete pout;

	delete lastString;
	delete fileName;

	DeleteCriticalSection(&m_timers_guard);
}

void oba::load(const char *pszFileName)
{
	pout->close();
	DeleteFile( fileName->c_str() );

	*lastString = "";

	pout->open( fileName->c_str(), ios::in|ios::out|ios::trunc);

	CloseHandle(hFile);
	hFile = CreateFile(fileName->c_str(), GENERIC_READ, FILE_SHARE_WRITE|FILE_SHARE_READ, NULL, 
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	
	lastPos = 0;

	pbas->open(pszFileName, ios::binary);
	load(pbas);
}

void oba::load(istream* pi)
{
// 	std::ofstream out("1.txt");
// 	for(int i = 0; i < 100; i++)
// 	{
// 		out << "{\"" << ob_englmessage[i]<< "\"}, //" << i << endl;
// 	}
// 	return;
	
	try {
		pimpl->load(pi, ob_obasic::NOOVERLAY);
		__int64 val = reinterpret_cast<__int64>(this);
		ob_type_int hi = ob_type_int((val & 0xFFFFFFFF00000000) >> 32);
		ob_type_int lo = ob_type_int(val & 0xFFFFFFFF);

		pimpl->writevar(BAS_VAR_OBA_POINTER_HIGH,  hi);
		pimpl->writevar(BAS_VAR_OBA_POINTER_LOW, lo );
	}
	catch(ob_err& e){
		pbas->close();
		throw oba_err(e.getcode());
	}
}

int oba::run()
{
	try
	{   
		GetSystemTime(&stStart);
		return pimpl->run();
	}
	catch(ob_err &e)
	{
		pbas->close();
		throw oba_err(e.getcode());
	}
}

void oba::debug(bool enable)
{
	pimpl->setstep(enable ? ob_obasic::STEP : ob_obasic::NOSTEP);
}

std::streampos oba::getfilepos() const
{
	return pbas->tellg();
}

std::string oba::getcurrentoperator() const
{
	return pimpl->getcurrentoperatorname();
}

std::ostream* oba::getoutputstream()
{
	return pout;
}

void oba::getbreakstrings(unsigned buffSize, char* pszPrev,  char* pszCurr,  char* pszNext)
{
	ob_loadbreakstr(pbas, buffSize, pszPrev,  pszCurr,  pszNext);
}

//////////////////////////////////////////////////////////////////////////
oba_err::oba_err(int code)
{
	_code = code;
	strMsg = new std::string(OBAError(code));
}

oba_err::~oba_err()
{
	delete strMsg;
}