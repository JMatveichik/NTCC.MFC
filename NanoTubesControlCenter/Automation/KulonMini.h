#pragma once
#include "PhysDevice.h"
#include "common.h"

class CommandManager;

class KulonMini : public IdentifiedPhysDevice, public IAnalogInput, public IAnalogOutput, public IDigitalInput, public IDigitalOutput
{

public:

	KulonMini();

	virtual ~KulonMini(void);

	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);	

	static KulonMini* CreateFromXML(IXMLDOMNode* pNode, UINT& error);
	
	virtual bool OnPrepareDevice();

public:

	//interface IAnalogInput
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
	virtual void EnumInputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const;


	//////////////////////////////////////////////////////////////////////////
	/// IAnalogOutput
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
	virtual void EnumOutputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const ;

	/////////////////////////////////////
	//Реализация интерфейса IDigitalInput
	virtual int GetChannelsCountDI() const; 

	virtual bool GetChannelStateDI(unsigned char nChannel, bool update) const;

	virtual bool UpdateChannelsStateDI() const;

	/////////////////////////////////////
	//Реализация интерфейса IDigitalOutput
	//Получение общего числа цифровых выходов 
	virtual int GetChannelsCountDO() const;

	//Обновить текущее состояния дискретных выходов	
	virtual bool UpdateChannelsStateDO() const;

	//Получить состояние цифрового выхода
	virtual bool GetChannelStateDO(unsigned char channel, bool update) const;

	//Установить состояние цифрового выхода 
	virtual bool  SetChannelStateDO(unsigned char channel, bool enable);

protected:

	typedef struct tagDOFullAdress
	{
		tagDOFullAdress()
		{

		}
		tagDOFullAdress(int r, int b) : reg(r), bit(b)
		{
			
		}
		int reg;
		int bit;

	}DO_FULL_ADRESS, *LPDO_FULL_ADRESS;
	
	//Функция процесса выполяется циклически через заданный интервал времени
	virtual void ControlProc();

	//Заполнение информации по измеряемым диапазонам
	virtual void BuildMeassureRangeInfo();

	//Заполнение информации по картам регистров
	virtual void BuildRegestersMaps();

	//Преобразовать данные канала с учетом текущих настроек положения плавающей точки
	//virtual void PrepareChannelValue(unsigned char channel, double& val, bool forWrite  ) const = 0;


	int INPUT_ANALOG_CHANNELS_COUNT;
	int INPUT_DIGITAL_CHANNELS_COUNT;
	int OUTPUT_ANALOG_CHANNELS_COUNT;
	int OUTPUT_DIGITAL_CHANNELS_COUNT;

	mutable CRITICAL_SECTION m_ModBusCS;
	
	//буфер хранения последних прочитанных данных для аналоговых входов
	mutable std::vector<double> dataAI;

	//буфер хранения последних записанных данных для аналоговых выходов
	mutable std::vector<double> dataAO;

	//буфер хранения последних записанных данных для цифровых выходов
	mutable std::vector<bool> dataDO;

	//информация о измеряемых диапазонах для аналоговых входов
	std::vector<MEASSURE_RANGE_INFO> mrciIn;

	//информация о задаваемых диапазонах для аналоговых выходов
	std::vector<MEASSURE_RANGE_INFO> mrciOut;

	//карта канал -> Modbus регистр для аналоговых выходов
	std::map<int, int> aoChannelsMap;

	//карта канал -> Modbus регистр -> bit для цифровых выходов
	std::map<int, DO_FULL_ADRESS> doChannelsMap;

	mutable std::map<int, unsigned short> doRegs;

	//состояние цифровых входов
	mutable unsigned int curStatus;

	//номер последовательного порта к которому 
	//подключен прибор
	int comPortNo;

	//адрес устройства
	unsigned char devAddres;
};

