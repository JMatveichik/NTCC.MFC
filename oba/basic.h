#pragma once

#include <iosfwd>
#include <string>
#include <map>
#include <windows.h>

enum e_code {
	OBA_ENDOFFILE = 1,	// end of bas-file 
	OBA_OPERATOREND,// operator "end" detected
	OBA_ENDOFLINE,	// break after one line due to debug regime
	OBA_BREAK,		// breakpoint reached
	OBA_NOLOADEND,	// bas file was not properly loaded
	OBA_BREAKBEFORE,// break before operator
	OBA_BREAKAFTER,	// break after operator
	OBA_EXCEPTION
};

typedef void	(*LPFNPRINT)(const char* strMsg);
typedef int		(*LPFNCHECK)(const char* strID);
typedef int		(*LPFNSWITCH)(const char* strID, bool value);
typedef double  (*LPFNGETVALUE)(const char* strID);
typedef double  (*LPFNSETVALUE)(const char* strID, double value);
typedef void	(*LPFNMESSAGE)(const char* strMsgTitle, const char* strMsg, int type);
typedef int		(*LPFNEXECUTE)(const char* strPath);
typedef void	(*LPFNSTRINGSET)(const char* strID, const char* text);
typedef std::string   (*LPFNSTRINGGET)(const char* strID);
typedef int	(*LPFNCONFIRM)(const char* strMsgTitle, const char* strMsg);


class oba_err
{
public:
	oba_err(int code);
	
	virtual ~oba_err();
	
	const char* what() const { return strMsg->c_str(); }
	int code() const { return _code; }

protected:

	std::string* strMsg;
	int _code;
	 
};

namespace ob_charspace
{
	class ob_obasic;
};

unsigned long Milliseconds();
unsigned long Milliseconds(const SYSTEMTIME& st);
unsigned long Milliseconds(const SYSTEMTIME& st1, const SYSTEMTIME& st2);

class oba
{
public:

	oba(LPFNPRINT pPrint, 
		LPFNCHECK pCheck, 
		LPFNSWITCH pSwitch, 
		LPFNGETVALUE pGetVal, 
		LPFNSETVALUE pSetVal, 
		LPFNMESSAGE pMessage,
		LPFNCONFIRM pConfirm,
		LPFNEXECUTE pExecute,
		LPFNSTRINGGET pStringGet,
		LPFNSTRINGSET pStringSet);

	~oba();

	void load(const char *pszFileName);
	int run();
	void debug(bool enable = true);

	std::streampos getfilepos() const;
	
	std::string getcurrentoperator() const;

	std::ostream* getoutputstream(); 

	void getbreakstrings(unsigned buffSize, char* pszPrev,  char* pszCurr,  char* pszNext);

protected:

	oba(const oba& rhs);
	const oba& operator= (const oba& rhs);
	void load(std::istream* pi);

	static	DWORD WINAPI FileMonitor(LPVOID lpParameter);
	void Monitor();

	std::ifstream *pbas;
	std::fstream  *pout;
	std::string* fileName;
	ob_charspace::ob_obasic* pimpl;

	std::string* lastString;
	DWORD lastPos;

	HANDLE hThread;
	HANDLE hBreak;
	HANDLE hFile;

	LPFNPRINT		lpfnPrint;
	LPFNCHECK		lpfnCheck;
	LPFNSWITCH		lpfnSwitch;
	LPFNGETVALUE	lpfnGetValue;
	LPFNSETVALUE	lpfnSetValue;
	LPFNMESSAGE		lpfnMessage;							
	LPFNCONFIRM		lpfnConfirm;
	LPFNEXECUTE		lpfnExecute;
	LPFNSTRINGSET	lpfnStringSet;
	LPFNSTRINGGET	lpfnStringGet;
	

	typedef struct {
		__int64 elapcedtime;
		__int64 endtime;
		__int64 lastcheck;
		bool    active;
	} SCRIPTTIMER, *LPSCRIPTTIMER;

	std::map<long, SCRIPTTIMER> m_timers_map;	
	
	long m_next_id;

	CRITICAL_SECTION m_timers_guard;

	SYSTEMTIME stStart;

	static int wait(ob_charspace::ob_obasic* basic_ptr, const char* parstring, 
			  const unsigned *descrf, const unsigned *descri, const unsigned *descrc, 
			  const float* parf, const int* pari, const char* parc);
	
	static int settimer_function(ob_charspace::ob_obasic* basic_ptr, const char* parstring, 
		const unsigned *descrf, const unsigned *descri, const unsigned *descrc, 
		const float* parf, const int* pari, const char* parc);

	static int checktimer_function(ob_charspace::ob_obasic* basic_ptr, const char* parstring, 
		const unsigned *descrf, const unsigned *descri, const unsigned *descrc, 
		const float* parf, const int* pari, const char* parc);

	static int pausetimer_function(ob_charspace::ob_obasic* basic_ptr, const char* parstring, 
		const unsigned *descrf, const unsigned *descri, const unsigned *descrc, 
		const float* parf, const int* pari, const char* parc);

	static int resumetimer_function(ob_charspace::ob_obasic* basic_ptr, const char* parstring, 
		const unsigned *descrf, const unsigned *descri, const unsigned *descrc, 
		const float* parf, const int* pari, const char* parc);

	static int modifytimer_function(ob_charspace::ob_obasic* basic_ptr, const char* parstring, 
		const unsigned *descrf, const unsigned *descri, const unsigned *descrc, 
		const float* parf, const int* pari, const char* parc);

	static int check_function(ob_charspace::ob_obasic* basic_ptr, const char* parstring, 
			  const unsigned *descrf, const unsigned *descri, const unsigned *descrc, 
			  const float* parf, const int* pari, const char* parc);

	static int switch_function(ob_charspace::ob_obasic* basic_ptr, const char* parstring, 
			  const unsigned *descrf, const unsigned *descri, const unsigned *descrc, 
			  const float* parf, const int* pari, const char* parc);

	static float value_function(ob_charspace::ob_obasic* basic_ptr, const char* parstring, 
			  const unsigned *descrf, const unsigned *descri, const unsigned *descrc, 
			  const float* parf, const int* pari, const char* parc);

	static int message_function(ob_charspace::ob_obasic* basic_ptr, const char* parstring, 
		const unsigned *descrf, const unsigned *descri, const unsigned *descrc, 
		const float* parf, const int* pari, const char* parc);

	static int confirm_function(ob_charspace::ob_obasic* basic_ptr, const char* parstring, 
		const unsigned *descrf, const unsigned *descri, const unsigned *descrc, 
		const float* parf, const int* pari, const char* parc);

	static int execute_function(ob_charspace::ob_obasic* basic_ptr, const char* parstring, 
			const unsigned *descrf, const unsigned *descri, const unsigned *descrc, 
			const float* parf, const int* pari, const char* parc);

	static int milliseconds_function(ob_charspace::ob_obasic* basic_ptr, const char* parstring, 
		const unsigned *descrf, const unsigned *descri, const unsigned *descrc, 
		const float* parf, const int* pari, const char* parc);

	static char* prepare_string(std::string str);

	static char* string_function(ob_charspace::ob_obasic* basic_ptr, const char* parstring,
		const unsigned *descrf, const unsigned *descri, const unsigned *descrc, 
		const float* parf, const int* pari, const char* parc);

	static char* join_function(ob_charspace::ob_obasic* basic_ptr, const char* parstring, 
		const unsigned *descrf, const unsigned *descri, const unsigned *descrc, 
		const float* parf, const int* pari, const char* parc);

	static char* format_function(ob_charspace::ob_obasic* basic_ptr, const char* parstring, 
		const unsigned *descrf, const unsigned *descri, const unsigned *descrc, 
		const float* parf, const int* pari, const char* parc);

	static char* date_time_function(ob_charspace::ob_obasic* basic_ptr, const char* parstring,
		const unsigned *descrf, const unsigned *descri, const unsigned *descrc,
		const float* parf, const int* pari, const char* parc);

};
