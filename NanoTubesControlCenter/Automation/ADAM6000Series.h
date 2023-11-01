#if !defined(AFX_ADVANTECH_ADAM6000_SERIES_DEVICES_H_INCLUDED_)
#define AFX_ADVANTECH_ADAM6000_SERIES_DEVICES_H_INCLUDED_

#include "PhysDevice.h"
#include "common.h"
#include "ModbusTCPConnection.h"


const short GCL_INTERNAL_FLAGS_COUNT = 16;

class BaseConverter;
//Базовый класс для устройств Advantech серии 6000
class ADAM6000 : public IdentifiedPhysDevice
{

public:

	//Деструктор
	virtual ~ADAM6000();

	//создание объекта из XML узла  
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

public:	
	
	//Получить имя модуля 
	std::string ModuleName();

	//Получить версию прошивки модуля
	std::string ModuleFirmwareVersion();

	//Получить адрес модуля
	std::string ModuleAddress() const { return  devAdress; } 

	//Получить номер последовательного порта
	unsigned char Port() const { return  portNo; } 	

protected :

	
	//Создать объект модуля 
	// Parameter: unsigned char comPortNo	-	номер порта
	// Parameter: unsigned char deviceAdress -   адрес устройства
	// Parameter: unsigned long updateInterval -   интервал опроса устройства, ms
	//************************************
	bool Create(std::string name, unsigned long updateInterval, std::string ip, unsigned port);

	//Конструктор
	ADAM6000(std::string model);
	
	virtual bool OnFailure();

	std::string SendASCII(const char* szData);

	bool SendModbus(MODBUS_COMMAND cmd, const char* data, unsigned data_size, std::vector<unsigned char>& response, std::ostream* pLog);
		

	bool SetInternalFlag(short idx, bool state) const;
	bool GetInternalFlag(short idx) const;
	std::bitset<GCL_INTERNAL_FLAGS_COUNT> GetInternalFlags() const;

	virtual bool OnCreate() = 0;
	
protected:

	//IP Адрес устройства "xxx.xxx.xxx.xxx"
	std::string devAdress;	

	//номер порта 
	unsigned portNo;

	//Соединение modbus over TCP 
	CModbusTCPConnection adamSocket;  
	
};



//Класс реализующий функциональность модуля ADVANTECH ADAM-6018
class ADAM6018 : public ADAM6000, public IAnalogInput, public IDigitalOutput
{

public:
	
	//Конструктор	
	ADAM6018(std::string model);

	virtual ~ADAM6018();

	static ADAM6018* CreateFromXML(IXMLDOMNode* pNode, UINT& error );

	//Обновить состояние каналов 
	bool UpdateChannelState();
	
	// Включение / выключение опроса аналогового входа 
	// для считывания данных
	bool EnableChannel(unsigned char ch, bool enable = true);

	
	//Получение состояния аналогового входа (включен ли он для опроса данных) 	
	bool	IsChannelEnabled(unsigned char channel);

	//Обновить информацию об измеряемых диапазонов 
	void UpdateInputRangesInfo( ) const;

	//Обновить информацию об измеряемых диапазонов 
	bool UpdateInputRangeInfo( unsigned char channel ) const;

	
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
	
	//Получить информациюю о всех доступных диапазонах 
	virtual void EnumInputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const;

	//////////////////////////////////////////////////////////////////////////


	//Получение общего числа цифровых выходов 
	// 0-7 - digital output
	// 8-23 - aux digital output
	virtual int GetChannelsCountDO() const;

	//Обновить текущее состояния дискретных выходов	
	virtual bool UpdateChannelsStateDO() const;

	//Получить состояние цифрового выхода
	virtual bool GetChannelStateDO(unsigned char channel, bool update) const;

	//Установить состояние цифрового выхода 
	virtual bool  SetChannelStateDO(unsigned char channel, bool enable);	

	//Записать данные во все каналы
	virtual void SetChannelsStateDO(unsigned char val);

	

protected:	

	unsigned char channelsState;

	virtual bool OnCreate();
	
	mutable std::vector<int>	inputRanges; 
	mutable std::vector<double>	lastData;

	//  [11/18/2014 Johnny A. Matveichik]
	//std::bitset<8> stateDO;//Состояние цифровых выходов
	std::bitset<24> stateDO;//Состояние цифровых выходов


	virtual void ControlProc();

private:		

	const int DO_CHANNELS_COUNT;
	const int AI_CHANNELS_COUNT;
};

//Класс реализующий функциональность модуля ADVANTECH ADAM-6018
class ADAM6017 : public ADAM6018
{

public:
	
	//Конструктор	
	ADAM6017();

	virtual ~ADAM6017();

	static ADAM6017* CreateFromXML(IXMLDOMNode* pNode, UINT& error );

	
	//////////////////////////////////////////////////////////////////////////
	//Реализация функций интерфейса IAnalogInput

	//Получение общего числа аналоговых входов
	//Для данного устройства 8 - входов
	virtual int GetChannelsCountAI() const;	

	//Чтение данных с аналоговых входов во внешний буфер pdData 
	//размер буфера должен быть не меньше sizeof(double)*INPUT_ANALOG_CHANNELS_COUNT
	//При pdData = NULL происходит опрос включеных входов и сохранение во внутреннем буфере класаа  
	virtual void ReadChannels(double *pdData = NULL) const;

	//Получение общего числа цифровых выходов
	//0-1	цифровые выходы
	//2-17  внутренние флаги
	virtual int GetChannelsCountDO() const;

	//Получить информацию о всех доступных диапазонах 
	virtual void EnumInputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const;
	
protected:

	virtual bool OnCreate(); 

private: 
	const int DO_CHANNELS_COUNT;
	const int AI_CHANNELS_COUNT;
};

//////////////////////////////////////////////////////////////////////////
// Абстрактный базовый класс для модулей ADVANTECH ADAM-6000				//
// с цифровыми входами и выходами										//
//////////////////////////////////////////////////////////////////////////

class ADAM6000DIDO : public ADAM6000, public IDigitalInput,  public IDigitalOutput
{

public:

	
	//Деструктор
	virtual ~ADAM6000DIDO();
	
public:
	
	///////////////////////////////////////////////////
	//Реализация интерфейса IDigitalInput

	//Получить число дискретных входов
	virtual int GetChannelsCountDI() const;

	//Обновить состояния дискретных входов
	virtual bool UpdateChannelsStateDI() const;

	//Прочитать состояние одного дискретного входа
	virtual bool GetChannelStateDI(unsigned char channel, bool update) const;

public:

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


	//Генерация импульсов
	virtual bool SetupGenerator(unsigned char channel, unsigned long highWidth, unsigned long lowWidth) = 0;

protected:

	//Конструктор
	ADAM6000DIDO(int channelsNumDI, int channelsNumDO, std::string model);
	
	virtual void ControlProc();

	
	const int  INPUT_DIGITAL_CHANNELS_COUNT;	//количество цифровых выходов
	const int  OUTPUT_DIGITAL_CHANNELS_COUNT;	//количество цифровых входов
	

	std::bitset<64> stateDI;//Состояние цифровых входов 
	std::bitset<64> stateDO;//Состояние цифровых выходов
	
};

//////////////////////////////////////////////////////////////////////////
// Класс реализующий модуль ADVANTECH ADAM-4050
//////////////////////////////////////////////////////////////////////////
class ADAM6050 : public ADAM6000DIDO,  public IAnalogOutput
{

public:

	//Конструктор
	ADAM6050();
		
	//Деструктор
	virtual ~ADAM6050();

	static ADAM6050* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

public:
	///////////////////////////////////////////////////
	//Реализация интерфейса IDigitalOutput

	//Установить состояние цифрового выхода 
	virtual bool  SetChannelStateDO(unsigned char channel, bool enable);	

	///////////////////////////////////////////////////
	//Реализация интерфейса IAnalogOutput
	//Получение общего числа аналоговых выходов 
	virtual int GetChannelsCountAO() const;

	//Записать данные в аналоговый выход 
	virtual double  WriteToChannel(unsigned char channel, double val);

	//Записать данные во все каналы
	virtual void WriteToAllChannels(const std::vector<double>& vals); 

	//Получить последнее записанное в аналоговый выход значение 
	virtual double GetChannelStateAO (unsigned char channel) const;  

	//Получить выходной диапазон канала 
	virtual void GetMinMax(unsigned char channel, double& minOut, double& maxOut) const; 
	
	//Получить  информацию об диапазоне канала
	virtual void GetOutputRangeInfo(unsigned char channel, LPMEASSURE_RANGE_INFO lpmci) const;

	//Установить  диапазон  аналогового выхода
	virtual bool SetOutputRange(unsigned char ch, unsigned char range);

	//Получить все возможные  диапазоны  
	virtual void EnumOutputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs)  const;

protected:

	enum tagFreqMode { NONE=-1, HW_LW, FQ_DC, FQ_HW, FQ_LW, DC_HW, DC_LW };

	std::vector <tagFreqMode> conversionMode;
	std::vector < std::pair< const BaseConverter*, const BaseConverter*> > pcs; 
	std::vector <double> lastVals;
	
	//начальный адрес длительности низкого состояния для импульсного режима
	unsigned short usLowWidthStartAdr;	

	//начальный адрес длительности высокого состояния для импульсного режима 
	unsigned short usHiWidthStartAdr; 

	//начальный адрес для переключения канала в импульсный режим 
	unsigned short usAbsPulseStartAdr; 

	//Конструктор
	ADAM6050(int channelsNumDI, int channelsNumDO);

	virtual bool OnCreate();

	virtual void OnExit ();

	/////////////////////////////////////////////////
	// Генератор импульсов
	virtual bool SetupGenerator(unsigned char channel, unsigned long highWidth, unsigned long lowWidth);
};

//////////////////////////////////////////////////////////////////////////
// Класс реализующий модуль ADVANTECH ADAM-4052
//////////////////////////////////////////////////////////////////////////
class ADAM6052 : public ADAM6050 , public IAnalogInput
{
public:

	//Конструктор
	ADAM6052();

	//Деструктор
	virtual ~ADAM6052();

	static ADAM6052* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

protected:

	virtual bool OnCreate();

	virtual void ControlProc();
	
	/////////////////////////////////////////////////
	// Генератор импульсов
	//virtual bool SetupGenerator(unsigned char channel, unsigned long highWidth, unsigned long lowWidth);

	//IAnalogInput
	//////////////////////////////////////////////////////////////////////////
	//Реализация функций интерфейса IAnalogInput

	//Получение общего числа аналоговых входов
	//Для данного устройства 16 - входов
	//0-7 - counter
	//8-15 - frequency
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

	//Получить информациюю о всех доступных диапазонах 
	virtual void EnumInputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const;

	//////////////////////////////////////////////////////////////////////////
	mutable std::vector<double> data;
};


//////////////////////////////////////////////////////////////////////////
// Класс реализующий модуль ADVANTECH ADAM-4066
//////////////////////////////////////////////////////////////////////////

class ADAM6066 : public ADAM6050, public IAnalogInput
{

public:

	//Конструктор
	ADAM6066();

	//Деструктор
	virtual ~ADAM6066();

	static ADAM6066* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

protected:
	
	virtual bool OnCreate();

	virtual void ControlProc();

	//IAnalogInput
	//////////////////////////////////////////////////////////////////////////
	//Реализация функций интерфейса IAnalogInput

	//Получение общего числа аналоговых входов
	//Для данного устройства 16 - входов
	//0-7 - counter
	//8-15 - frequency
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

	//Получить информациюю о всех доступных диапазонах 
	virtual void EnumInputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const;

	//////////////////////////////////////////////////////////////////////////
	mutable std::vector<double> data;

};

class ADAM6000Impl
{
	public:

		ADAM6000Impl(const CModbusTCPConnection& sock);
		virtual ~ADAM6000Impl();

	protected:

	private:
		const CModbusTCPConnection& adamSocket;
};
 
#endif // AFX_ADVANTECH_ADAM6000_SERIES_DEVICES_H_INCLUDED_