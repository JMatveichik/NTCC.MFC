#pragma once

#include "TimedControl.h"

class NTCCDataSrv
{

public:

	NTCCDataSrv(void);
	virtual ~NTCCDataSrv(void);

	// ‘ункци€ запуска сервера
	bool Start(const char* pszName);

	// ‘ункци€ остановки сервера сервера
	bool Stop();

protected:
	
	//обработка запросов клиентов
	std::string ProcessCommand( std::string cmd );
	
	//инициализаци€ внутренних данных
	bool Init(const char* pszName);

private:

	//данные дл€ обработки потока канала
 	typedef struct tagThreadData{

		tagThreadData(NTCCDataSrv* p, HANDLE h) : hPipe(h), pParent(p) {};

		HANDLE hPipe;
		NTCCDataSrv* pParent;
	} PIPE_THREAD_DATA;

	///путь к файлу экспорта
	std::string importPath;

	///полное им€ канала
	std::string pipeName;

	///данные от источников данных
	std::vector<double> data;

	///индексы входных аналоговых каналов 
	std::vector<int> IdxsAI;
	///индексы выходных аналоговых каналов 
	std::vector<int> IdxsAO;
	///индексы входных дискретных каналов 
	std::vector<int> IdxsDI;
	///индексы выходных дискретных каналов 
	std::vector<int> IdxsDO;
	
	// —обытие остановки сервера
	HANDLE	hStopEvent;

	// ѕоток обслуживани€ запросов клиентов
	HANDLE	hMainThread;			 

	mutable CRITICAL_SECTION m_cs; 

private:

	static DWORD WINAPI MainThread(LPVOID lpvParam);
	static DWORD WINAPI PipeThread(LPVOID lpvParam);

};

