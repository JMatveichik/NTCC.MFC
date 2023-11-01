#include "stdafx.h"
#include "CommandManager.h"
#include "PhysDevice.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


std::string BeautifyCmd(LPASCII_COMMAND pCmd)
{
	std::string cmd;

	if ( pCmd->length == 0 )
		cmd = pCmd->cmd;
	else
	{
		for( int i = 0; i < pCmd->length; i++ )
		{
			char szHex[16];
			sprintf_s(szHex, "%02X ", (unsigned char)pCmd->cmd[i]);
			cmd += szHex;
		}
		return cmd;
	}

	if ( cmd.length() == 0 )
		return cmd;

	std::string newCmd = cmd;
	
	size_t last = newCmd.length() - 1;

	if ( last < 0 )
		return "";

	while ( newCmd[last]  == '\r' || newCmd[last]  == '\n')
	{
		newCmd.erase(last);
		last = newCmd.length() - 1;
		
		if ( last < 0 )
			return "";
	}

	return newCmd;
}

std::map<unsigned char, CommandManager*> CommandManager::cms;


bool CommandManager::Create(unsigned char comPortNum, unsigned int baudRate)
{
	return (comPort.Open(comPortNum, baudRate) == TRUE);
}

bool CommandManager::AddCommandManager(CommandManager* pCmd)
{
	return cms.insert(std::make_pair(pCmd->ComPortNumber(), pCmd)).second;
}


CommandManager* CommandManager::GetCommandManager(unsigned char nPortNo)
{
	std::map<unsigned char, CommandManager*>::const_iterator fnd;
	fnd =  cms.find( nPortNo );

	if ( fnd == cms.end() )
		return NULL;
	else
		return (*fnd).second;
}

CommandManager::CommandManager(void) : hCmdThread(NULL), hStopMsgProcessing(NULL), bLogEnabled(false)
{
	::InitializeCriticalSection(&cs);
}

void CommandManager::ClearAll()
{
	for (std::map<unsigned char, CommandManager*>::const_iterator it = cms.begin(); it != cms.end();++it)
	{
		(*it).second->StopCommandProcessing();
		delete (*it).second;
	}
	cms.clear();
}

CommandManager::~CommandManager(void)
{
	
	StopCommandProcessing();
	::DeleteCriticalSection(&cs);
}

bool CommandManager::EnableCommandLog(const char* pszPathName)
{
	::EnterCriticalSection(&cs);
	
	string strPath = pszPathName;

	if (strPath.c_str()[strPath.length()-1] != '\\')
		strPath += "\\";

	std::strstream str;
	str << "CM_COM" <<  comPort.PortNo() << std::ends;
	
	strPath += str.str();

	if ( logStream.Create( strPath.c_str() ) )
		bLogEnabled = true;
	else	
		bLogEnabled = false;
	 
	::LeaveCriticalSection(&cs);

	return bLogEnabled;
}

std::string CommandManager::DataPass(const IdentifiedPhysDevice* pSender, std::string data, bool bHiPriority) const
{
	ASCII_COMMAND command;
	strcpy_s(command.cmd, data.c_str() );
	command.pSender = pSender;
	strcpy_s(command.response, "SEND");
	
	const_cast<CommandManager*>(this)->SendCommand(&command, bHiPriority);
	
	return command.response;
}

bool CommandManager::SendCommand(LPASCII_COMMAND lCmd, bool isControlCmd)
{
	ASSERT( lCmd->pSender != NULL );

	if (hCmdThread == NULL)
		return false;

	HANDLE hCmd = ::CreateEvent(NULL, FALSE, FALSE, NULL);	
	
	lCmd->hCommand = hCmd;
	
	::EnterCriticalSection(&cs);
	
	SERIAL_COMMAND scmd;
	scmd.cmdLife = 0;

	scmd.size = sizeof(ASCII_COMMAND);
	memcpy(&scmd.ascii, lCmd, scmd.size);
	scmd.ascii.pSrcCmd = lCmd;
	

	if ( isControlCmd )
	{
		cmdsControl.push_back(scmd);
	}
	else
	{
		cmdsMonitor.push_back(scmd);
	}
	::LeaveCriticalSection(&cs);


	while ( true )
	{
		if ( ::WaitForSingleObject(hCmd, 0 ) == WAIT_OBJECT_0 )
			break;

		if ( ::WaitForSingleObject(hStopMsgProcessing, 0) == WAIT_OBJECT_0 )
			break;

		Sleep(100);
	}
	
	::CloseHandle(hCmd);

	return (scmd.ascii.pSrcCmd->response != "FAILURE");
}

bool CommandManager::SendCommand(LPMODBUS_SERIAL_COMMAND lCmd, bool isControlCmd)
{
	ASSERT( lCmd->pSender != NULL );

	if (hCmdThread == NULL)
		return false;

	HANDLE hCmd = ::CreateEvent(NULL, FALSE, FALSE, NULL);	

	lCmd->hCommand = hCmd;

	::EnterCriticalSection(&cs);

	SERIAL_COMMAND scmd;
	scmd.cmdLife = 0;

	scmd.size = sizeof(MODBUS_SERIAL_COMMAND);
	memcpy(&scmd.modbus, lCmd, scmd.size);
	scmd.modbus.pSrcCmd = lCmd;


	if ( isControlCmd )
	{
		cmdsControl.push_back(scmd);
	}
	else
	{
		cmdsMonitor.push_back(scmd);
	}
	::LeaveCriticalSection(&cs);


	while ( true )
	{
		if ( ::WaitForSingleObject(hCmd, 0 ) == WAIT_OBJECT_0 )
			break;

		if ( ::WaitForSingleObject(hStopMsgProcessing, 0) == WAIT_OBJECT_0 )
			break;

		Sleep(100);
	}

	::CloseHandle(hCmd);

	return (scmd.modbus.pSrcCmd->regCount > 0);
}

bool CommandManager::ProcessCommandsList (SERIAL_CMD_LIST& cmdList, int maxCmdCount, bool bHigthPriority)
{
	int cmdInd = maxCmdCount;

	SYSTEMTIME st1;
	SYSTEMTIME st2;
	
	while ( !cmdList.empty() )
	{
		//прекращаем если количество попыток исчерпано
		if ( cmdInd-- == 0 )
			break;

		CString msg;

		//если обрабатывается очередь с низким приоритетом
		//а в очереди с высоки с приоритетом есть команды прекращаем текущий цикл обработки 
		//и переключаемся на очередь с высоки приоритетом
		if ( !bHigthPriority && !IsHigthPriorityCmdStackEmpty() )
		{
			logStream.LogMessage("SWITCH TO CONTROL QUEUE");
			break;
		}

		//Получаем команду из головы очереди 
		//::EnterCriticalSection(&cs);		
		
		if ( bLogEnabled )
			::GetLocalTime(&st1);

		SERIAL_COMMAND& cmd = cmdList.front();

		if ( cmd.size == sizeof(ASCII_COMMAND) )
		{
			LPASCII_COMMAND lpCmd = &cmd.ascii;
			//отправляем команду
			
			int cmdLen = (lpCmd->length > 0) ? lpCmd->length : strlen(lpCmd->cmd);   //(int)lpCmd->cmd.length();

			int nBytesSent = comPort.SendData( (unsigned char*)lpCmd->cmd, cmdLen);

			if ( bLogEnabled )
			{
				msg  = (lpCmd->pSender) ? lpCmd->pSender->Name().c_str() : "UNKNOWN";
				msg += "=> ASCII : ";				
				msg += BeautifyCmd(lpCmd).c_str(); 
			}

			//Записана ли команда целиком
			if ( nBytesSent != cmdLen ) 
			{
				//если нет перемещает команду в хвост очереди
				::EnterCriticalSection(&cs);	

				//TODO: Check urgently
				if ( ++cmd.cmdLife <= maxRetry )
				{
					cmdList.emplace(cmdList.end(), cmdList.front());
					//cmdList.push_back(lpCmd);

					if ( bLogEnabled )
						msg  += "\tRETRY SEND\n";
				}	
				else
				{
					if ( bLogEnabled )
						msg += "\tDELETED FROM QUEUE\n";

					strcpy_s(lpCmd->pSrcCmd->response, "FAILURE");
					::SetEvent(lpCmd->hCommand);
				}

				CString sz;
				sz.Format("\tQUEUE SIZE : %d", cmdList.size());
				msg += sz;

				logStream.LogMessage(msg);
				cmdList.pop_front(); //RemoveHead();
				
				::LeaveCriticalSection(&cs);

				//продолжаем цикл обработки			
				continue;
			}

			//Получение ответа 
			char szRespond[MAX_RESPONSE_LEN];
			szRespond[0] = 0;			
			int nBytesRead = comPort.ReadData(szRespond, MAX_RESPONSE_LEN);
			
			//szRespond[nBytesRead] = 0;

			if ( bLogEnabled )
			{
				::GetLocalTime(&st2);			
				CString dif;
				dif.Format("\t%d ms\t", Milliseconds(st2) - Milliseconds(st1));
				msg += dif;	
			}

			::EnterCriticalSection(&cs);		

			//Прочитан ли ответ
			if ( nBytesRead == 0 )
			{
				//если нет перемещает команду в хвост очереди
				
				if ( ++cmd.cmdLife <= maxRetry )
				{
					cmdList.emplace(cmdList.end(), cmdList.front());
					//cmdList.push_back(lpCmd);
					if ( bLogEnabled )
						msg += "\tRETRY READ\n";
				}
				else
				{
					if ( bLogEnabled )
						msg += "\tDELETED FROM QUEUE\n";

					strcpy_s(lpCmd->pSrcCmd->response, "FAILURE");
					::SetEvent(lpCmd->pSrcCmd->hCommand);
				}
				
				CString sz;
				sz.Format("\tQUEUE SIZE : %d\n", cmdList.size());

				logStream.LogMessage(msg);
				cmdList.pop_front();

				::LeaveCriticalSection(&cs);

				//продолжаем цикл обработки			
				continue;
			}
			
			//strcpy_s(lpCmd->pSrcCmd->response, MAX_RESPONSE_LEN, szRespond);
			memcpy(lpCmd->pSrcCmd->response, szRespond, nBytesRead );
			lpCmd->pSrcCmd->length = nBytesRead;

			//Команда успешно обработана
			::SetEvent(lpCmd->pSrcCmd->hCommand);

			//Удаляем из очереди и сохраняем ответ
			cmdList.pop_front();

			::LeaveCriticalSection(&cs);

			if ( bLogEnabled )
			{
				CString sz;
				sz.Format("\tQUEUE SIZE : %d\n", cmdList.size());
				
				msg += "COMPLETE\t"; msg += szRespond; 
				msg += sz;

				logStream.LogMessage(msg);
			}

			
		}
		else if (cmd.size == sizeof(MODBUS_SERIAL_COMMAND) )
		{

			LPMODBUS_SERIAL_COMMAND lpCmd = &cmd.modbus;
			
			BOOL cmdSendState;
			
			if ( bLogEnabled )
			{
				msg  = (lpCmd->pSender) ? lpCmd->pSender->Name().c_str() : "UNKNOWN";
				msg += "=>MODBUS : ";
			}

			
			std::string str;

			switch (lpCmd->func)
			{
			
				case 0x01:
					cmdSendState = comPort.ModbusReadCoils(lpCmd->slaveAddress, lpCmd->regAddress, lpCmd->regCount, lpCmd->pSrcCmd->response, &str);
					break;

				case 0x02:
					cmdSendState = comPort.ModbusReadDiscreteInputs(lpCmd->slaveAddress, lpCmd->regAddress, lpCmd->regCount, lpCmd->pSrcCmd->response, &str);
					break;

				case 0x03:
					cmdSendState = comPort.ModbusReadInputRegs(lpCmd->slaveAddress, lpCmd->regAddress, lpCmd->regCount, lpCmd->pSrcCmd->response, &str);
					break;
			
				case 0x04:
					cmdSendState = comPort.ModbusReadHoldingRegs(lpCmd->slaveAddress, lpCmd->regAddress, lpCmd->regCount, lpCmd->pSrcCmd->response, &str);
					break;

				case 0x05:
					cmdSendState = comPort.ModbusWriteSingleCoil(0x05, lpCmd->slaveAddress, lpCmd->regAddress, lpCmd->pSrcCmd->response, &str);
					break;

				case 0x06:
					//cmdSendState = comPort.ModbusWriteRegs(0x06, lpCmd->slaveAddress, lpCmd->regAddress, lpCmd->regCount, lpCmd->pSrcCmd->response, &str);
					cmdSendState = comPort.ModbusWriteSingleCoil(0x06, lpCmd->slaveAddress, lpCmd->regAddress, lpCmd->pSrcCmd->response, &str);
					break;

				case 0x10:
					cmdSendState = comPort.ModbusWriteRegs(0x10, lpCmd->slaveAddress, lpCmd->regAddress, lpCmd->regCount, lpCmd->pSrcCmd->response, &str);
					break;

				case 0x0F:
					//cmdSendState = comPort.ModbusWriteCoils(0x0F, lpCmd->slaveAddress, lpCmd->regAddress, lpCmd->regCount, lpCmd->pSrcCmd->response, &str);
					ASSERT(FALSE);
					break;

				default:
					ASSERT(FALSE);
							
			}

			if (bLogEnabled)
				msg += str.c_str(); 
			
			// если комадна удачно обработана удаляем ее из очереди
			// выставляем флаг завершения обработки
			if ( cmdSendState ) 
			{
				if (bLogEnabled)
					msg += "COMPLETE\t";

				::SetEvent(lpCmd->pSrcCmd->hCommand);

				CString sz;
				sz.Format("\tQUEUE SIZE : %d", cmdList.size());
				msg += sz;

				cmdList.pop_front();
				
			}

			//если ошибка при обработке команды
			//проверяем количество попыток если лимит попыток не исчерпан
			//помещаем команду в хвост и продалжаем обработку
			else if ( ++cmd.cmdLife <= maxRetry ) 
			{
				CString sz;
				sz.Format("\tQUEUE SIZE : %d", cmdList.size());
				msg += sz;

				if (bLogEnabled)
				{
					msg += "\tRETRY\n";
					logStream.LogMessage(msg);
				}
				cmdList.emplace(cmdList.end(), cmdList.front());
				cmdList.pop_front();
				continue;
			}
			//проверяем количество попыток если лимит попыток не исчерпан
			//удаляем команду
			//TODO: Информировать об отказе устройства
			else 
			{
				if ( bLogEnabled )
					msg += "\tDELETED FROM QUEUE\n";


				lpCmd->pSrcCmd->regCount = 0;
				lpCmd->pSrcCmd->response[0] = 0xB;
				lpCmd->pSrcCmd->response[1] = 0xA;
				lpCmd->pSrcCmd->response[2] = 0xD;

				::SetEvent(lpCmd->pSrcCmd->hCommand);

				CString sz;
				sz.Format("\tQUEUE SIZE : %d", cmdList.size());
				msg += sz;

				cmdList.pop_front();
			}

			if ( bLogEnabled )
				logStream.LogMessage(msg);
		}
		else
		{
			ASSERT(FALSE);
		}
		//::LeaveCriticalSection(&cs);
				
	}

	return true;
}

DWORD WINAPI CommandManager::ThreadFunction(LPVOID lpParameter)
{
	CommandManager* pSelf = (CommandManager*)lpParameter;

	while (true)
	{
		if ( ::WaitForSingleObject(pSelf->hStopMsgProcessing, 0) == WAIT_OBJECT_0)
			break;

		::EnterCriticalSection(&pSelf->cs);
		
		//обрабатываем очередь команд с высоким приоритетом		
		if ( !pSelf->cmdsControl.empty() )
			pSelf->ProcessCommandsList(pSelf->cmdsControl, 5, true);

		//обрабатываем очередь команд с низким приоритетом
		if ( !pSelf->cmdsMonitor.empty() )
			pSelf->ProcessCommandsList(pSelf->cmdsMonitor, 10, false);		

		::LeaveCriticalSection(&pSelf->cs);

		::Sleep (10);
	}

	return 0L;
}


bool CommandManager::StartCommandProcessing(unsigned int cmdMaxRetry)
{
	if ( hStopMsgProcessing == NULL)
		hStopMsgProcessing = ::CreateEvent(NULL, FALSE, FALSE, NULL);

	maxRetry = cmdMaxRetry;

	if ( hCmdThread != NULL) 
		return true;	

	hCmdThread = ::CreateThread(NULL, 0, ThreadFunction, this, 0, NULL);
	return hCmdThread != NULL;
}

void CommandManager::StopCommandProcessing()
{	
	::SetEvent(hStopMsgProcessing);
	
	::EnterCriticalSection(&cs);
	
	if ( ::WaitForSingleObject(hCmdThread, 5000 ) == WAIT_TIMEOUT )
		::TerminateThread(hCmdThread, 0);

	::CloseHandle(hCmdThread);
	::CloseHandle(hStopMsgProcessing);

	hCmdThread = NULL;
	hStopMsgProcessing = NULL;

	if ( bLogEnabled )
		logStream.LogMessage("Command processing terminate..");

	cmdsControl.clear();
	cmdsMonitor.clear();

	::LeaveCriticalSection(&cs);
}

void CommandManager::EnumCommandManagers(std::vector<CommandManager*>& cmdmans)
{
	cmdmans.clear();

	std::map<unsigned char, CommandManager*>::const_iterator it;
	for(it = cms.begin(); it != cms.end(); it++)
		cmdmans.push_back((*it).second);
	
}