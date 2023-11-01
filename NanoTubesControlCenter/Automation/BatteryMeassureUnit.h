#pragma once
#include "PhysDevice.h"
#include "common.h"

class CommandManager;

class BatteryMeassureUnit : 
	public IdentifiedPhysDevice, 
	public IDigitalInput,
	public IAnalogInput, 
	public IAnalogOutput
{

public:

	BatteryMeassureUnit();
	virtual ~BatteryMeassureUnit(void);

	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);	

	static BatteryMeassureUnit* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

public:
	//Функция процесса выполяется циклически через заданный интервал времени
	virtual void ControlProc();

	///////////////////////////////////////////////////
	//Реализация интерфейса IDigitalInput

	//Получить число дискретных входов
	virtual int GetChannelsCountDI() const; 	

	//Обновить состояния дискретных входов
	virtual bool UpdateChannelsStateDI() const;

	//Прочитать состояние одного дискретного входа
	virtual bool GetChannelStateDI(unsigned char channel, bool update) const;

	//////////////////////////////////////////////////////////////////////////
	//interface IAnalogInput
	//Получение общего числа аналоговых входов
	virtual int GetChannelsCountAI() const;

	//Получение данных со всех аналоговых входов (выполняется запрос на чтение данных)
	virtual void	ReadChannels(double* data  = NULL) const;

	//Получение данных с одного из аналоговых входов (выполняется запрос на чтение данных)
	virtual double  ReadChannel(unsigned char ch)  const;

	// Получение данных из буфера для одного из аналоговых входов 
	// (запрос на чтение данных не выполняется)
	virtual double  GetChannelData(unsigned char ch)  const;

	//Получить  информацию об измеряемом диапазоне канала
	virtual void GetInputRangeInfo(unsigned char ch, LPMEASSURE_RANGE_INFO lpmci) const;

	//Установить  диапазон  аналогового входа
	virtual bool SetInputRange(unsigned char ch, unsigned char range);

	//Получить все возможные  диапазоны  измерения
	virtual void EnumInputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const;


	//////////////////////////////////////////////////////////////////////////
	/// IAnalogOutput
	//Получение общего числа аналоговых выходов 
	virtual int GetChannelsCountAO() const;

	//Записать данные в аналоговый выход 
	virtual double  WriteToChannel(unsigned char ch, double val);

	//Записать данные во все каналы
	virtual void WriteToAllChannels(const std::vector<double>& vals) ; 

	//Получить последнее записанное в аналоговый выход значение 
	virtual double GetChannelStateAO (unsigned char ch) const;  

	//Получить выходной диапазон канала 
	virtual void GetMinMax(unsigned char ch, double& minOut, double& maxOut) const;

	//Получить  информацию об диапазоне канала
	virtual void GetOutputRangeInfo(unsigned char ch, LPMEASSURE_RANGE_INFO lpmci) const;

	//Установить  диапазон  аналогового выхода
	virtual bool SetOutputRange(unsigned char ch, unsigned char range);

	//Получить все возможные  диапазоны  
	virtual void EnumOutputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const ;
protected:

	int AIN_CHANNELS_COUNT;	
	int DIN_CHANNELS_COUNT;
	int AOUT_CHANNELS_COUNT;

	virtual bool OnPrepareDevice();
	
	//номер последовательного порта к которому 
	//подключен прибор
	int portNo;

	//адрес устройства
	unsigned int devAdr;

	//адреса регистров для аналоговых выходов 
	std::vector<int> aoutRegs;

	//состояние цифровых входов
	mutable std::bitset<16>		dinChState;

	//состояние аналоговых входов
	mutable std::vector<double>	dataAI;

	//последние заданные значения
	mutable std::vector<double> dataAO;
	
	//диапазоны задания 
	std::vector<MEASSURE_RANGE_INFO> rngAO;

	//диапазоны измерения 
	std::vector<MEASSURE_RANGE_INFO> rngAI;
};

