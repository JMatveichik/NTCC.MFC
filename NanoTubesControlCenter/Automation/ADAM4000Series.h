#if !defined(AFX_ADVANTECH_ADAM4000_SERIES_DEVICES_H_INCLUDED_)
#define AFX_ADVANTECH_ADAM4000_SERIES_DEVICES_H_INCLUDED_

#include "PhysDevice.h"
#include "common.h"

class CommandManager;
//Базовый класс для устройств Advantech 
class ADAM4000 : public IdentifiedPhysDevice
{

public:

	//Конструктор
	ADAM4000();

	//Деструктор
	virtual ~ADAM4000();

	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);
	
protected:
	
	//Конструктор
	ADAM4000(std::string model);

	//Создать объект модуля 
	// Parameter: unsigned char comPortNo	-	номер порта
	// Parameter: unsigned char deviceAdress -   адрес устройства
	//************************************
	bool Create(std::string name, unsigned long updateInterval, unsigned char address, unsigned port);
	
	
	//Адрес устройства
	int devAdress;	

	//Номер последовательного порта
	int comNo;

	//
	mutable std::string strName;
	
	//Менеджер команд обслуживающий модуль
	CommandManager* cmds;  

	//Строка для хранения последней комадны
	mutable std::string strLastCmd;	 

	//Строка для хранения последнего ответа
	mutable std::string strLastRes;

	//извлечение данных из строки ответа
	virtual std::string ExtractRespond(std::string fullRespond, std::size_t from, int count = -1) const;

	virtual bool OnCreate() = 0;
	
public:	
	
	//Обработчики событий
	void AddBeforeCmdSendHandler(boost::function< void (LPSERIAL_COMMAND)	> handler );
	
	void AddAfterCmdRespondHandler(boost::function< void (LPSERIAL_COMMAND) > handler );

	void AddCmdFailureHadler(boost::function< void (LPSERIAL_COMMAND) > handler );

	
private:

	mutable boost::signals2::signal< void (LPSERIAL_COMMAND) > evBeforeCmdSend;
	mutable boost::signals2::signal< void (LPSERIAL_COMMAND) > evAfterRespond;	
	mutable boost::signals2::signal< void (LPSERIAL_COMMAND) > evCmdFailure;

public:
	
	//Получение строки последней комадны
	const char* LastCommand() const;

	//Получение строки последнего ответа
	const char* LastRespond() const;

	//Синхронное отправление комманды  с ожиданием ответа	
	bool SendCommand(char cmdType, std::string strCommand, bool bHighPriority, std::string &strRespond) const;

	//Получить имя модуля 
	std::string ModuleName(bool bUpdate = true, int len = 4) const;

	//Получить версию прошивки модуля
	std::string ModuleFirmwareVersion() ;

	//Получить адрес модуля
	unsigned char ModuleAddress() const;

	//Получить номер последовательного порта
	unsigned char Port() const;


};

//Класс реализующий функциональность модуля ADVANTECH ADAM-4019+
class ADAM4019P : public ADAM4000, public IAnalogInput/*, public XMLObject<ADAM4019P>*/
{

public:
	
	//Конструктор	
	ADAM4019P();

	virtual ~ADAM4019P();

	static ADAM4019P* CreateFromXML(IXMLDOMNode* pNode, UINT& error );

	//virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

	//Обновить состояние каналов 
	bool UpdateChannelState() const;

	// Включение / выключение опроса аналогового входа 
	// для считывания данных
	bool EnableChannel(unsigned char ch, bool enable = true);
	
	//Получение состояния аналогового входа (включен ли он для опроса данных) 	
	bool	IsChannelEnabled(unsigned char channel) const;
	
	//////////////////////////////////////////////////////////////////////////
	//Реализация функций интерфейса IAnalogInput
	
	//Получение общего числа аналоговых входов
	//Для данного устройства 8 - входов
	virtual int GetChannelsCountAI() const;
	
	//Чтение данных с аналоговых входов во внешний буфер pdData 
	//размер буфера должен быть не меньше sizeof(double)*INPUT_ANALOG_CHANNELS_COUNT
	//При pdData = NULL происходит опрос включеных входов и сохранение во внутреннем буфере класаа  
	virtual void ReadChannels(double *pdData = NULL) const;

	//Получение данных с одного из 8 аналоговых входов (выполняется запрос на чтение данных)
	// номер входа должен быть от 0 до 8
	virtual double  ReadChannel(unsigned char channel)  const;

	// Получение данных из буфера для одного из аналоговых входов 
	// (запрос на чтение данных не выполняется)
	virtual double  GetChannelData(unsigned char channel)  const;


	//Получить  информацию об измеряемом диапазоне канала
	virtual void GetInputRangeInfo(unsigned char channel, LPMEASSURE_RANGE_INFO lpmci) const;

	//Установить  диапазон  аналогового входа
	virtual bool SetInputRange(unsigned char ch, unsigned char range);

	//Получить все возможные  диапазоны  измерения
	virtual void EnumInputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const;

	
	//////////////////////////////////////////////////////////////////////////	
	//Обновить информацию об измеряемом диапазоне канала
	virtual bool UpdateInputRangeInfo( unsigned char channel ) const;
	
	//Обновить информацию об измеряемых диапазонов 
	void UpdateInputRangesInfo( ) const;

protected:	
	
	ADAM4019P(std::string model);

	virtual bool OnCreate();

	//состояние аналоговых входов  
	mutable unsigned char channelsState;	 
	
	mutable std::vector<long>		inputRanges; 
	mutable std::vector<double>		lastData;

	
	virtual void ControlProc();


private:

	const int AI_CHANNELS_COUNT;
	
};

class ADAM4018P : public ADAM4019P
{
public:

	ADAM4018P();
	virtual ~ADAM4018P();

	//virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);
	static ADAM4018P* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//Получение общего числа аналоговых входов
	//Для данного устройства 8 - входов
	virtual int ADAM4018P::GetChannelsCountAI();


public:
	
	//Получить все возможные  диапазоны  измерения
	virtual void EnumInputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const;

protected:

	virtual bool OnCreate();

private:

	const int AI_CHANNELS_COUNT;

};

class ADAM4011 : public ADAM4019P
{

public:

	ADAM4011();
	virtual ~ADAM4011();

	static ADAM4011* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//Получение общего числа аналоговых входов
	//Для данного устройства 1 - входов
	virtual int GetChannelsCountAI() const;

	//////////////////////////////////////////////////////////////////////////	
	//Обновить информацию об измеряемом диапазоне канала
	virtual bool UpdateInputRangeInfo( unsigned char channel ) const;

	//Получить все возможные  диапазоны  измерения
	virtual void EnumInputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const;

	//Чтение данных с аналоговых входов во внешний буфер pdData 
	//размер буфера должен быть не меньше sizeof(double)*INPUT_ANALOG_CHANNELS_COUNT
	//При pdData = NULL происходит опрос включеных входов и сохранение во внутреннем буфере класаа  
	virtual void ReadChannels(double *pdData = NULL) const;

protected:

	virtual bool OnCreate();

private:

	const int AI_CHANNELS_COUNT;
};

class ADAM4015 : public ADAM4019P
{

public:

	ADAM4015();
	virtual ~ADAM4015();

	static ADAM4015* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//Получение общего числа аналоговых входов
	//Для данного устройства 6 - входов
	virtual int GetChannelsCountAI() const;

protected:
	
	virtual bool OnCreate();

private:

	const int AI_CHANNELS_COUNT;
};

//////////////////////////////////////////////////////////////////////////
// Преобразователь интерфейса RS-485 в RS-232
//////////////////////////////////////////////////////////////////////////
class ADAM4521 : public ADAM4000, public IDataProvider
{

public:
	
	//Конструктор 
	ADAM4521();

	//Деструктор
	virtual ~ADAM4521();

	static ADAM4521* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//Установить идентификатор модуля
	bool SetID(std::string strID) const;

	//Получить идентификатор модуля
	std::string GetID() const;

	//Отправить данные внешнему устройству
	virtual std::string DataPass(const IdentifiedPhysDevice* pSender, std::string outerCmd, bool bHigthPriority) const;

protected:

	virtual void ControlProc();

	virtual bool OnCreate();
	
	//Получить разделитель команд
	char GetDelimiter() const;

	mutable char delim;
	mutable std::string strID;

	
};


//////////////////////////////////////////////////////////////////////////
// Класс реализующий модуль ADVANTECH ADAM-4024
//////////////////////////////////////////////////////////////////////////
class ADAM4024 : public ADAM4000, public IDigitalInput, public IAnalogOutput
{

public:
		
	//Конструктор
	ADAM4024();

	//Деструктор
	virtual ~ADAM4024();
	
	static ADAM4024* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	/////////////////////////////////////
	//Реализация интерфейса IAnalogOutput

	//Получение общего числа аналоговых выходов 
	virtual int GetChannelsCountAO() const;

	//Записать данные в аналоговый выход 
	virtual double  WriteToChannel(unsigned char channel, double val);

	//Записать данные во все каналы
	virtual void WriteToAllChannels(const std::vector<double>& vals) ; 

	//Получить последнее записанное в аналоговый выход значение 
	virtual double GetChannelStateAO (unsigned char channel) const;  

	//Получить выходной диапазон канала 
	virtual void GetMinMax(unsigned char channel, double& minOut, double& maxOut) const;

	//Получить  информацию об диапазоне канала
	virtual void GetOutputRangeInfo(unsigned char channel, LPMEASSURE_RANGE_INFO lpmci) const;

	//Установить  диапазон  аналогового выхода
	virtual bool SetOutputRange(unsigned char ch, unsigned char range);

	//Получить все возможные  диапазоны  
	virtual void EnumOutputRanges(unsigned char ch,  std::vector<MEASSURE_RANGE_INFO>& rngs) const;


	/////////////////////////////////////
	//Реализация интерфейса IDigitalInput
	virtual int GetChannelsCountDI() const; 
	
	virtual bool GetChannelStateDI(unsigned char nChannel, bool update) const;

	virtual bool UpdateChannelsStateDI() const;
	

	//////////////////////////////////////////////////////////////////////////
	/// Реализация специальных функций модуля


	//Обновить информацию об диапазоне канала
	bool UpdateOutputRangeInfo( unsigned char channel ) const;

	//Обновить информацию о диапазонах 
	void UpdateOutputRangesInfo( ) const;

	//Задание начального состояния для одного канала
	bool SetStartupValue(unsigned char channel, double val) const;

	//Задание начального состояния для всех канала
	bool SetStartupValue(double* vals) const;

	//Задание аварийного состояния для одного канала 
	bool SetEmergencyValue(unsigned char channel, double val) const;

	//Задание аварийного состояния для всех каналов 
	bool SetEmergencyValue(double* vals) const;

	//Установление флага для отслеживания аварийного состояния для одного канала 
	bool SetEmergencyFlag(unsigned char channel, bool enable = true) const;

	//Получение флага для отслеживания аварийного состояния для одного канала 
	bool GetEmergencyFlag(unsigned char channel) const;

protected:

	virtual bool OnCreate();

	virtual void ControlProc();

	const int  OUTPUT_ANALOG_CHANNELS_COUNT;
	const int  INPUT_DIGITAL_CHANNELS_COUNT;

	mutable std::vector<long>		outputRanges; 

	std::vector<double> lastValues;
	mutable unsigned char stateDI;
};

//////////////////////////////////////////////////////////////////////////
// Класс реализующий модуль ADVANTECH ADAM-4050
//////////////////////////////////////////////////////////////////////////


class ADAM4050 : public ADAM4000, public IDigitalInput,  public IDigitalOutput
{

public:

	//Конструктор
	ADAM4050();

	//Деструктор
	virtual ~ADAM4050();

	static ADAM4050* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	///////////////////////////////////////////////////
	//Реализация интерфейса IDigitalInput

	//Получить число дискретных входов
	virtual int GetChannelsCountDI() const;

	//Обновить состояния дискретных входов
	virtual bool UpdateChannelsStateDI() const;

	//Прочитать состояние одного дискретного входа
	virtual bool GetChannelStateDI(unsigned char nChannel, bool update) const;


	///////////////////////////////////////////////////
	//Реализация интерфейса IDigitalOutput
//Получение общего числа цифровых выходов 
	virtual int GetChannelsCountDO() const;

	//Обновить текущее состояния дискретных выходов	
	virtual bool UpdateChannelsStateDO() const;

	//Получить состояние цифрового выхода
	virtual bool GetChannelStateDO(unsigned char channel, bool update) const;

	//Установить состояние цифрового выхода 
	virtual bool  SetChannelStateDO(unsigned char channel, bool enable);	

	//Записать данные во все каналы
	virtual void SetChannelsStateDO(unsigned char val);

	//Получить состояние цифровых выходов
	virtual unsigned char GetChannelsStateDO();

	//Получить состояние цифровых выходов
	virtual unsigned char GetChannelsStateDI();

protected:
	
	virtual bool OnCreate();
	virtual void ControlProc();

private:

	const int  OUTPUT_DIGITAL_CHANNELS_COUNT;	//количество цифровых входов
	const int  INPUT_DIGITAL_CHANNELS_COUNT;	//количество цифровых выходов

	mutable unsigned char stateDI;	//Состояние цифровых входов
	mutable unsigned char stateDO;	//Состояние цифровых выходов

	mutable unsigned char stateStoreDO; // сохрнение состояния цифровых выходов

};

//////////////////////////////////////////////////////////////////////////
// Класс реализующий модуль ADVANTECH ADAM-4053
//////////////////////////////////////////////////////////////////////////

class ADAM4053 : public ADAM4000, public IDigitalInput
{

public:

	//Конструктор
	ADAM4053();

	//Деструктор
	virtual ~ADAM4053();

	static ADAM4053* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	///////////////////////////////////////////////////
	//Реализация интерфейса IDigitalInput

	//Получить число дискретных входов
	virtual int GetChannelsCountDI() const;

	//Обновить состояния дискретных входов
	bool UpdateChannelsStateDI() const;

	//Прочитать состояние одного дискретного входа
	bool GetChannelStateDI(unsigned char ch, bool update) const;

	
protected:

	//посылает комманду на получение текущего сосотяния 
	//входных каналов
	bool GetStateDI(std::bitset<16>& state) const;

	virtual bool OnCreate();
	virtual void ControlProc();

private:

	const int  INPUT_DIGITAL_CHANNELS_COUNT;	//количество цифровых выходов

	std::bitset<16> stateDI;					//cостояние цифровых выходов

	bool changesConfirm;		//проверить еще раз состояние при изменении бита

};

class ADAM4052 : public ADAM4000, public IDigitalInput
{

public:

	//Конструктор
	ADAM4052();

	//Деструктор
	virtual ~ADAM4052();

	static ADAM4052* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	///////////////////////////////////////////////////
	//Реализация интерфейса IDigitalInput

	//Получить число дискретных входов
	virtual int GetChannelsCountDI() const;

	//Обновить состояния дискретных входов
	bool UpdateChannelsStateDI() const;

	//Прочитать состояние одного дискретного входа
	bool GetChannelStateDI(unsigned char ch, bool update) const;


protected:

	//посылает комманду на получение текущего сосотяния 
	//входных каналов
	bool GetStateDI(std::bitset<8>& state) const;

	virtual bool OnCreate();
	virtual void ControlProc();

private:

	const int  INPUT_DIGITAL_CHANNELS_COUNT;	//количество цифровых выходов

	std::bitset<8> stateDI;					//cостояние цифровых выходов

	bool changesConfirm;		//проверить еще раз состояние при изменении бита

};


class ADAM4069 : public ADAM4000,  public IDigitalOutput
{

public:

	//Конструктор
	ADAM4069();

	//Деструктор
	virtual ~ADAM4069();

	static ADAM4069* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//Получение общего числа цифровых выходов 
	virtual int GetChannelsCountDO() const;

	//Обновить текущее состояния дискретных выходов	
	virtual bool UpdateChannelsStateDO() const;

	//Получить состояние цифрового выхода
	virtual bool GetChannelStateDO(unsigned char channel, bool update) const;

	//Установить состояние цифрового выхода 
	virtual bool  SetChannelStateDO(unsigned char channel, bool enable);	

	//Записать данные во все каналы
	virtual void SetChannelsStateDO(unsigned char val);

	//Сохранить состояние цифровых выходов
	//virtual void SaveChannelsStateDO() const; 

	//Восстановить состояние цифровых выходов 
	//virtual void RestoreChannelsStateDO() const; 

protected:
	
	virtual bool OnCreate();
	virtual void ControlProc();

private:

	const int  OUTPUT_DIGITAL_CHANNELS_COUNT_4069;

	mutable unsigned char stateDO;
	mutable unsigned char stateStoreDO;
};

class ADAM4068 : public ADAM4069
{

public:

	//Конструктор
	ADAM4068();

	//Деструктор
	virtual ~ADAM4068();


public:

	static ADAM4068* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//Получение общего числа цифровых выходов 
	virtual int GetChannelsCountDO() const;

protected:
	virtual bool OnCreate();

private:

	const int  OUTPUT_DIGITAL_CHANNELS_COUNT_4068;
};

class ADAM4080 : public ADAM4000, public IAnalogInput
{

public:

	//Конструктор
	ADAM4080();

	//Деструктор
	virtual ~ADAM4080();


public:

	static ADAM4080* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//////////////////////////////////////////////////////////////////////////
	//Реализация функций интерфейса IAnalogInput

	//Получение общего числа аналоговых входов
	virtual int GetChannelsCountAI() const;

	//Получение данных со всех аналоговых входов (выполняется запрос на чтение данных)
	virtual void	ReadChannels(double* data  = NULL) const;

	//Получение данных с одного из аналоговых входов (выполняется запрос на чтение данных)
	virtual double  ReadChannel(unsigned char channel)  const;

	// Получение данных из буфера для одного из аналоговых входов 
	// (запрос на чтение данных не выполняется)
	virtual double  GetChannelData(unsigned char channel)  const;

	//Получить  информацию об измеряемом диапазоне канала
	virtual void GetInputRangeInfo(unsigned char channel, LPMEASSURE_RANGE_INFO lpmci) const;

	//Установить  диапазон  аналогового входа
	virtual bool SetInputRange(unsigned char ch, unsigned char range);

	//Получить все возможные  диапазоны  измерения
	virtual void EnumInputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs)  const;
	
protected:
	
	virtual bool OnCreate();

	virtual void ControlProc();

private:

	const int  INPUT_ANALOG_CHANNELS_COUNT_4080;
	std::vector<double> dataBuffer;
};

#endif // AFX_ADVANTECH_ADAM4000_SERIES_DEVICES_H_INCLUDED_