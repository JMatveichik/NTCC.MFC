#include "StdAfx.h"

#include "NTCCDataSrv.h"
#include "DataSrc.h"

#include "..\ah_xml.h"
#include "..\ah_project.h"
#include "..\NanoTubesControlCenter.h"

#ifdef _DEBUG
#define new DEBUG_NEW	
#endif

#define BUFSIZE 4096

NTCCDataSrv::NTCCDataSrv(void) :  hMainThread(NULL), hStopEvent(NULL)
{
}


NTCCDataSrv::~NTCCDataSrv(void)
{
 	
}

bool NTCCDataSrv::Init( const char* pszSrvName )
{
	CComPtr<IXMLDOMDocument> pXMLDoc;
	HRESULT hr = pXMLDoc.CoCreateInstance(__uuidof(DOMDocument));
	if ( FAILED(hr) )
		return false;

	VARIANT_BOOL bSuccess = false;
	hr = pXMLDoc->loadXML(CComBSTR(L"<?xml version=\"1.0\" encoding=\"UTF-8\"?><DATASOURCES></DATASOURCES>"), &bSuccess);

	if ( FAILED(hr) )
		return false;

	CComPtr<IXMLDOMNode> pParentNode;
	hr = pXMLDoc->selectSingleNode(CComBSTR(L"DATASOURCES"), &pParentNode);
	if (FAILED(hr))
		return false;

	//Подготовка информации об источниках данных
	int size = DataSrc::Count();
	data.assign(size, 0.0);

	//Получаем все источники данных 
	vector<const DataSrc*> dsAll;
	DataSrc::EnumDataSources(dsAll, NULL);

	AppXMLHelper& xmlh = AppXMLHelper::Instance();

	//строим таблицы индексов
	for (int i = 0; i < (int)dsAll.size(); i++)
	{
		int wire = -1;
		if ( dynamic_cast<const AnalogInputDataSrc*>( dsAll[i] ) != NULL )
		{
			IdxsAI.push_back(i);
			wire = IdxsAI.size() - 1;
		}

		else if ( dynamic_cast<const DiscreteInputDataSrc*>( dsAll[i] ) != NULL )
		{
			IdxsDI.push_back(i);
			wire = IdxsDI.size() - 1;			
		}

		else if ( dynamic_cast<const AnalogOutputDataSrc*>( dsAll[i] ) != NULL )
		{
			IdxsAO.push_back(i);
			wire = IdxsAO.size() - 1;			
		}

		else if ( dynamic_cast<const DiscreteOutputDataSrc*>( dsAll[i] ) != NULL )
		{
			IdxsDO.push_back(i);
			wire = IdxsDO.size() - 1;			
		}
		
		else if (dynamic_cast<const CompositeAnalogDataSrc*>( dsAll[i] ) != NULL)
		{
			IdxsAI.push_back(i);
			wire = IdxsAI.size() - 1;
		}

		else if (dynamic_cast<const CompositeDiscreteDataSrc*>( dsAll[i] ) != NULL)
		{
			IdxsDI.push_back(i);
			wire = IdxsDI.size() - 1;
		}

		else if (dynamic_cast<const DiscreteDataSrc*>( dsAll[i] ) != NULL)
		{
			IdxsDI.push_back(i);
			wire = IdxsDI.size() - 1;
		}
		else if (dynamic_cast<const AnalogDataSrc*>( dsAll[i] ) != NULL)
		{
			IdxsAI.push_back(i);
			wire = IdxsAI.size() - 1;
		}
		else {
			TRACE("\n\t%s - Unknown data source type", dsAll[i]->Name().c_str());
			continue;
		}

		boost::format dsname("%s.%s");
		dsname % pszSrvName % dsAll[i]->Name().c_str();
		std::string strNewName = dsname.str();
		boost::algorithm::to_upper(strNewName);

		std::string strDevName = pszSrvName;
		boost::algorithm::to_upper(strDevName);
		
		if ( !xmlh.SaveDataSourceToXMLNode(pXMLDoc, pParentNode, dsAll[i], strNewName.c_str(), strDevName.c_str(), wire) )
		{
			TRACE("\n\t%-20s was not save (%-20s %-20s %03d)", dsAll[i]->Name().c_str(), strNewName.c_str(), strDevName.c_str(), wire);
		}
		///
	}

	
	boost::format fmt("%s_import.xml");
	fmt % pszSrvName;

	std::string fileName = fmt.str();

	importPath = NTCCProject::Instance().GetPath(NTCCProject::PATH_CONFIG, fileName, true);

	hr = pXMLDoc->save( CComVariant( importPath.c_str() ) );
	if ( FAILED(hr) )
	{
		Globals::CheckError();
		return false;
	}

	return true;
}

bool NTCCDataSrv::Start(const char* pszName)
{
	if ( !Init(pszName) )
		return false;
	
	///Создаем именованый канал
	boost::format pn("\\\\.\\pipe\\%s");
	pn % pszName;
	pipeName = pn.str();

	//Пробуем создать именованый канал
	HANDLE hPipe = CreateNamedPipe(pipeName.c_str(),	//имя канала  
							PIPE_ACCESS_DUPLEX ,	//режим канала The pipe is bi-directional; both server and client processes can read from and write to the pipe.
							PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,  
							PIPE_UNLIMITED_INSTANCES,	// максимальное количество  реализаций канала 
							0,			// размер выходного буфера в байтах 
							0,			// размер входного буфера в байтах
							NMPWAIT_USE_DEFAULT_WAIT, // время ожидания в миллисекундах
							NULL
							);

	if ( hPipe == INVALID_HANDLE_VALUE )
	{	
		Globals::CheckError();
		return false;
	}	
			
	CloseHandle(hPipe);

	//запускаем поток обслуживания клинтов
	if (hMainThread) 
	{
		DWORD dwExitCode;

		::GetExitCodeThread(hMainThread, &dwExitCode);

		if (dwExitCode == STILL_ACTIVE)
			return false;

		::CloseHandle(hMainThread);
	}

	if (hStopEvent)
		::CloseHandle(hStopEvent);

	hStopEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!hStopEvent)
		return false;

	hMainThread = ::CreateThread(NULL, 0, MainThread, this, 0, NULL);
	return hMainThread != NULL;
}

bool NTCCDataSrv::Stop()
{
	if (!hStopEvent || !hMainThread)
		return false;

	::SetEvent(hStopEvent);	
	bool bResult = ::WaitForSingleObject(hMainThread, 2000) == WAIT_OBJECT_0;

	return bResult;
}

// DataSrc* NTCCDataSrv::FindDataSourceByChannel()
// {
// 
// }

std::string NTCCDataSrv::ProcessCommand( std::string cmd )
{
	//Доступные разделители ",;|\t "
	boost::char_separator<char> sep(":");
	boost::tokenizer< boost::char_separator<char> > tokens(cmd, sep);
	vector<std::string>  parts;

	for ( boost::tokenizer< boost::char_separator<char> >::iterator it = tokens.begin(); it != tokens.end(); ++it)
	{
		parts.push_back(boost::lexical_cast<string>(*it));
	}

	
	std::strstream out;

	//команда на получение данных
#pragma region Обработка команд get

	if (parts[0] == "get")
	{
		//получение числа элементов
		if ( parts[1] == "count")
		{
			int count = 0;
			
			//количество аналоговых входных каналов
			if (parts[2] == "ai")
				count = IdxsAI.size();
			
			//количество аналоговых выходных каналов
			else if (parts[2] == "ao")
				count = IdxsAO.size();
			
			//количество дискретных входных каналов
			else if (parts[2] == "di")
				count = IdxsDI.size();

			//количество дискретных выходных каналов
			else if (parts[2] == "do")
				count = IdxsDO.size();

			else if (parts[2] == "all")
				count = DataSrc::Count();

			out << count << std::ends;
			return out.str();
		}
		//получение индексов каналов элементов
		else if ( parts[1] == "idxs")
		{
			std::vector<int>* idxs = NULL;
			//количество аналоговых входных каналов
			if (parts[2] == "ai")
				idxs = &IdxsAI;

			//количество аналоговых выходных каналов
			else if (parts[2] == "ao")
				idxs = &IdxsAO;

			//количество дискретных входных каналов
			else if (parts[2] == "di")
				idxs = &IdxsDI;

			//количество дискретных выходных каналов
			else if (parts[2] == "do")
				idxs = &IdxsDO;

			if (idxs == NULL)
				return "FAILURE";

			for (std::vector<int>::const_iterator it = idxs->begin(); it != idxs->end(); ++it)
				out << (*it) << ";";

			out << std::ends;
			return out.str();
		}
		//получение диапазона канала
		else if ( parts[1] == "range")
		{
			vector<const DataSrc*> dsAll;
			DataSrc::EnumDataSources(dsAll, NULL);

			int	channel = atoi(parts[3].c_str());
			std::pair<double, double > rng;

			//для аналоговых входных каналов
			if (parts[2] == "ai") 
			{
				//ищем источник данных
				int wire = -1;

				for (int i = 0; i < (int)dsAll.size(); i++)
				{	
					const AnalogInputDataSrc* pAIDS = dynamic_cast<const AnalogInputDataSrc*>( dsAll[i] );
					if ( pAIDS != NULL )
					{
						if ( ++wire == channel ) 
						{
							pAIDS->GetRange(rng.first, rng.second);
							break;
						}
					}						
				}	
			}
			//для аналоговых выходных каналов
			else if (parts[2] == "ao")
			{
				//ищем источник данных
				int wire = -1;

				for (int i = 0; i < (int)dsAll.size(); i++)
				{	
					const AnalogOutputDataSrc* pAODS = dynamic_cast<const AnalogOutputDataSrc*>( dsAll[i] );
					if ( pAODS != NULL )
					{
						if ( ++wire == channel ) 
						{
							pAODS->GetRange(rng.first, rng.second);
							break;
						}
					}
				}
			}
			else 
				return "FAILURE";

			out << rng.first << ";" << rng.second << std::ends;
			return out.str();
		}
		//получение данных
		else if ( parts[1] == "data")
		{
			vector<const DataSrc*> dsAll;
			DataSrc::EnumDataSources(dsAll, NULL);

			for (vector<const DataSrc*>::const_iterator it = dsAll.begin(); it != dsAll.end(); ++it)
			{
				double val = UNKNONW_VALUE;
				const AnalogDataSrc* pADS = dynamic_cast<const AnalogDataSrc*>( (*it) );
				if ( pADS != NULL )
					val = pADS->GetValue();

				const DiscreteDataSrc* pDDS = dynamic_cast<const DiscreteDataSrc*>( (*it) );
				if ( pDDS != NULL )
					val = pDDS->IsEnabled() ? 1.0 : 0.0;

				out << val << ";";
			}
			out << std::ends;

			return out.str();
		}
		//получить путь к файлу импорта источников данных 
		else if ( parts[1] == "importds")
		{
			return importPath;
		}
	}
	else if (parts[0] == "set")
	{
		vector<const DataSrc*> dsAll;
		DataSrc::EnumDataSources(dsAll, NULL);

		int	channel = atoi(parts[2].c_str());
		double val  = atof(parts[3].c_str());

		//для аналоговых входных каналов
		if (parts[1] == "do") 
		{
			//ищем источник данных
			int wire = -1;

			for (int i = 0; i < (int)dsAll.size(); i++)
			{	
				DiscreteOutputDataSrc* pDODS = const_cast<DiscreteOutputDataSrc*>(dynamic_cast<const DiscreteOutputDataSrc*>( dsAll[i] ));
				if ( pDODS != NULL )
				{
					if ( ++wire == channel ) 
					{
						pDODS->Enable(val > 0);
						break;
					}
				}						
			}	
		}
		//для аналоговых выходных каналов
		else if (parts[1] == "ao")
		{
			//ищем источник данных
			int wire = -1;

			for (int i = 0; i < (int)dsAll.size(); i++)
			{	
				AnalogOutputDataSrc* pAODS = const_cast<AnalogOutputDataSrc*>(dynamic_cast<const AnalogOutputDataSrc*>( dsAll[i] ));
				if ( pAODS != NULL )
				{
					if ( ++wire == channel ) 
					{
						pAODS->SetValue(val);
						break;
					}
				}
			}
		}
		else 
			return "FAILURE";
	}

#pragma endregion
	
	return "FAILURE";
}


//Функция процесса выполяется циклически через заданный интервал времени
DWORD NTCCDataSrv::MainThread(LPVOID lpvParam)
{
 	BOOL   fConnected = FALSE; 
	DWORD  dwThreadId = 0; 
	HANDLE hPipe = INVALID_HANDLE_VALUE;
	HANDLE hThread = NULL; 	

	// The main loop creates an instance of the named pipe and 
	// then waits for a client to connect to it. When the client 
	// connects, a thread is created to handle communications 
	// with that client, and this loop is free to wait for the
	// next client connect request. It is an infinite loop.
	NTCCDataSrv* pSelf = (NTCCDataSrv*)lpvParam;	
	TRACE( "\nPipe Server: Main thread awaiting client connection on %s\n", pSelf->pipeName.c_str() );

	///
	while(true)
	{
		///если сервер остановлен заканчиваем поток
		if (::WaitForSingleObject(pSelf->hStopEvent, 0) == WAIT_OBJECT_0) 
			break;
	
		hPipe = CreateNamedPipe( 
			pSelf->pipeName.c_str(),           // pipe name 
			PIPE_ACCESS_DUPLEX,       // read/write access 
			PIPE_TYPE_MESSAGE |       // message type pipe 
			PIPE_READMODE_MESSAGE |   // message-read mode 
			PIPE_WAIT,                // blocking mode 
			PIPE_UNLIMITED_INSTANCES, // max. instances  
			BUFSIZE,                  // output buffer size 
			BUFSIZE,                  // input buffer size 
			0,                        // client time-out 
			NULL);                    // default security attribute 

		if (hPipe == INVALID_HANDLE_VALUE) 
		{
			TRACE("CreateNamedPipe failed, %s.\n", Globals::CheckError().c_str());
			break;
		}
		
		// Wait for the client to connect; if it succeeds, 
		// the function returns a nonzero value. If the function
		// returns zero, GetLastError returns ERROR_PIPE_CONNECTED. 

		fConnected = ConnectNamedPipe(hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED); 

		if (fConnected) 
		{ 
			TRACE("Client connected, creating a processing thread.\n"); 

			// Create a thread for this client. 
			hThread = CreateThread( 
				NULL,              // no security attribute 
				0,                 // default stack size 
				PipeThread,			// thread proc
				(LPVOID)(new PIPE_THREAD_DATA(pSelf, hPipe)) ,    // thread parameter 
				0,                 // not suspended 
				&dwThreadId);      // returns thread ID 

			if (hThread == NULL) 
			{
				TRACE("CreateThread failed, %s.\n", Globals::CheckError().c_str());
				continue;
			}
			else 
				CloseHandle(hThread); 
		} 
		else 
			// The client could not connect, so close the pipe. 
			CloseHandle(hPipe); 
	}

	CloseHandle(hThread);
	CloseHandle(hPipe); 

	return 0L; 	
}

DWORD WINAPI NTCCDataSrv::PipeThread(LPVOID lpvParam)
	// This routine is a thread processing function to read from and reply to a client
	// via the open pipe connection passed from the main loop. Note this allows
	// the main loop to continue executing, potentially creating more threads of
	// of this procedure to run concurrently, depending on the number of incoming
	// client connections.
{ 
	HANDLE hHeap      = GetProcessHeap();
	TCHAR* pchRequest = (TCHAR*)HeapAlloc(hHeap, 0, BUFSIZE*sizeof(TCHAR));
	TCHAR* pchReply   = (TCHAR*)HeapAlloc(hHeap, 0, BUFSIZE*sizeof(TCHAR));
	NTCCDataSrv* pSelf = NULL;
	HANDLE hPipe = NULL; 

// 	while (TRUE)
// 	{
		DWORD cbBytesRead = 0, cbReplyBytes = 0, cbWritten = 0; 
		BOOL fSuccess = FALSE;
	
		// Do some extra error checking since the app will keep running even if this
		// thread fails.

		if (lpvParam == NULL)
		{
			TRACE( "\nERROR - Pipe Server Failure:\n");
			TRACE( "   InstanceThread got an unexpected NULL value in lpvParam.\n");
			TRACE( "   InstanceThread exitting.\n");
			if (pchReply != NULL) 
				HeapFree(hHeap, 0, pchReply);
			if (pchRequest != NULL) 
				HeapFree(hHeap, 0, pchRequest);
			return (DWORD)-1;
		}

		if (pchRequest == NULL)
		{
			TRACE( "\nERROR - Pipe Server Failure:\n");
			TRACE( "   InstanceThread got an unexpected NULL heap allocation.\n");
			TRACE( "   InstanceThread exitting.\n");
			if (pchReply != NULL) 
				HeapFree(hHeap, 0, pchReply);
			return (DWORD)-1;
		}

		if (pchReply == NULL)
		{
			TRACE( "\nERROR - Pipe Server Failure:\n");
			TRACE( "   InstanceThread got an unexpected NULL heap allocation.\n");
			TRACE( "   InstanceThread exitting.\n");
			if (pchRequest != NULL) 
				HeapFree(hHeap, 0, pchRequest);
			return (DWORD)-1;
		}

		// Print verbose messages. In production code, this should be for debugging only.
		TRACE("InstanceThread created, receiving and processing messages.\n");

		// The thread's parameter is a handle to a pipe object instance.
		PIPE_THREAD_DATA* data = (PIPE_THREAD_DATA*)lpvParam;

		pSelf = data->pParent;
		hPipe = data->hPipe; 

		delete data;

		// Loop until done reading
		while (TRUE) 
		{ 
			Sleep(100);
			// Read client requests from the pipe. This simplistic code only allows messages
			// up to BUFSIZE characters in length.
			fSuccess = ReadFile( 
				hPipe,        // handle to pipe 
				pchRequest,    // buffer to receive data 
				BUFSIZE*sizeof(TCHAR), // size of buffer 
				&cbBytesRead, // number of bytes read 
				NULL);        // not overlapped I/O 

			if (!fSuccess || cbBytesRead == 0)
			{   
				if ( GetLastError() == ERROR_BROKEN_PIPE )
					TRACE("InstanceThread: client disconnected.\n", Globals::CheckError().c_str());
				else
					TRACE("InstanceThread ReadFile failed, %s.\n", Globals::CheckError().c_str());
				
				break;
			}

			if (  fSuccess)
			{
				// Process the incoming message.
 				std::string cmd((char*)pchRequest);
 				std::string res = pSelf->ProcessCommand(cmd);

				TRACE("\n\t\t%s =>%s\n", cmd.c_str(), res.c_str()); 

				// Write the reply to the pipe. 
				fSuccess = WriteFile( 
					hPipe,        // handle to pipe 
					res.c_str(),     // buffer to write from 
					res.length() + 1, // number of bytes to write 
					&cbWritten,   // number of bytes written 
					NULL);        // not overlapped I/O 

				if (!fSuccess)
				{   
					TRACE("InstanceThread WriteFile failed, %s.\n", Globals::CheckError().c_str());
					break;
				}
			}
		}
	//}
	// Flush the pipe to allow the client to read the pipe's contents 
	// before disconnecting. Then disconnect the pipe, and close the 
	// handle to this pipe instance. 

 	FlushFileBuffers(hPipe); 
 	DisconnectNamedPipe(hPipe); 
 	CloseHandle(hPipe); 

	HeapFree(hHeap, 0, pchRequest);
	HeapFree(hHeap, 0, pchReply);


	TRACE("InstanceThread exitting.\n");
	return 1;
}

