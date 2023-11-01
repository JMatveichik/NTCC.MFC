#pragma once

#include "TimedControl.h"

class NTCCDataSrv
{

public:

	NTCCDataSrv(void);
	virtual ~NTCCDataSrv(void);

	// ������� ������� �������
	bool Start(const char* pszName);

	// ������� ��������� ������� �������
	bool Stop();

protected:
	
	//��������� �������� ��������
	std::string ProcessCommand( std::string cmd );
	
	//������������� ���������� ������
	bool Init(const char* pszName);

private:

	//������ ��� ��������� ������ ������
 	typedef struct tagThreadData{

		tagThreadData(NTCCDataSrv* p, HANDLE h) : hPipe(h), pParent(p) {};

		HANDLE hPipe;
		NTCCDataSrv* pParent;
	} PIPE_THREAD_DATA;

	///���� � ����� ��������
	std::string importPath;

	///������ ��� ������
	std::string pipeName;

	///������ �� ���������� ������
	std::vector<double> data;

	///������� ������� ���������� ������� 
	std::vector<int> IdxsAI;
	///������� �������� ���������� ������� 
	std::vector<int> IdxsAO;
	///������� ������� ���������� ������� 
	std::vector<int> IdxsDI;
	///������� �������� ���������� ������� 
	std::vector<int> IdxsDO;
	
	// ������� ��������� �������
	HANDLE	hStopEvent;

	// ����� ������������ �������� ��������
	HANDLE	hMainThread;			 

	mutable CRITICAL_SECTION m_cs; 

private:

	static DWORD WINAPI MainThread(LPVOID lpvParam);
	static DWORD WINAPI PipeThread(LPVOID lpvParam);

};

