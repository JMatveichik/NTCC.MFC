#if !defined(_COMMAND_MANAGER_H_INCLUDED_)
#define _COMMAND_MANAGER_H_INCLUDED_


#include <map>
#include "common.h"
#include "Serial.h"
#include "MessageLog.h"

#define CHECKCMDMANAGER(m, p)  \
	CommandManager* m = CommandManager::GetCommandManager(p); \
	if ( m == NULL ) \
	return false;  


#define CHECKCMDMANAGER_VOID(m, p)  \
	CommandManager* m = CommandManager::GetCommandManager(p); \
	if ( m == NULL ) \
		return;  



class CommandManager : public IDataProvider
{

public:
	
	CommandManager();
	virtual ~CommandManager(void);

protected:
	
// 	CommandManager( const CommandManager& );
// 	const CommandManager& operator= ( const CommandManager& );

public:

	bool Create(unsigned char comPortNum, unsigned int baudRate);

	
	virtual std::string DataPass(const IdentifiedPhysDevice* pSender, std::string data, bool bHiPriority) const;

	bool SendCommand(LPASCII_COMMAND lCmd, bool isControlCmd);
	bool SendCommand(LPMODBUS_SERIAL_COMMAND lCmd, bool isControlCmd);

	

	bool StartCommandProcessing(unsigned int cmdMaxRetry);
	void StopCommandProcessing();

	bool EnableCommandLog(const char* pszFileName);	
	


	static CommandManager* GetCommandManager(unsigned char nPortNo);
	static bool AddCommandManager(CommandManager* pCmd);
	static void ClearAll();

	unsigned int MaxRetry() const  { return maxRetry; };


	//получить номер последовательного порта
	int ComPortNumber() { return comPort.PortNo(); };
	

	//получить текущую конфигурацию порта
	void GetComState(DCB& dcb) const
	{
		comPort.GetComState( dcb );
	}

	//задать конфигурацию порта
	BOOL SetComState(DCB dcb)
	{
		return comPort.SetComState( dcb );
	}


	unsigned int ControlQueueSize() const { return (int)cmdsControl.size(); }
	unsigned int MonitorQueueSize() const { return (int)cmdsMonitor.size(); }

	static void EnumCommandManagers(std::vector<CommandManager*>& cmdmans);

	
protected:

	CRITICAL_SECTION cs;		// Критическая секция
	HANDLE	hCmdThread;			// Поток обработки команд 
	HANDLE hStopMsgProcessing;

	CMessageLog	logStream;	//лог команд 

	SyncSerialPort comPort;

	bool bLogEnabled;

	unsigned int maxRetry;	//Максимальное количество попыток 

	SERIAL_CMD_LIST cmdsControl;
	SERIAL_CMD_LIST cmdsMonitor;

	static std::map<unsigned char, CommandManager*> cms;

	bool ProcessCommandsList (SERIAL_CMD_LIST& cmdList, int maxCmdCount, bool bHigthPriority);
	bool IsHigthPriorityCmdStackEmpty() const { return (cmdsControl.size() == 0); };

	static DWORD WINAPI ThreadFunction(LPVOID lpParameter);
	
};
#endif //_COMMAND_MANAGER_H_INCLUDED_