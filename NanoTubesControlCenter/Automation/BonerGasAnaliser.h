#if !defined(AFX_BONER_GAS_ANALISER_DEVICES_H_INCLUDED_)
#define AFX_BONER_GAS_ANALISER_DEVICES_H_INCLUDED_

#include "PhysDevice.h"
#include "common.h"
#include "ModbusTCPConnection.h"



class BonerGasAnaliserTest01 : public IdentifiedPhysDevice, public IAnalogInput
{

public:

	BonerGasAnaliserTest01();
	virtual ~BonerGasAnaliserTest01(void);

	bool BonerGasAnaliserTest01::CreateFromXMLNode(IXMLDOMNode* pNode);

	static BonerGasAnaliserTest01* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

public:
	
	virtual bool Create(std::string name, int portNo, unsigned long updateInterval);

	//////////////////////////////////////////////////////////////////////////
	//interface IAnalogInput
	//Получение общего числа аналоговых входов
	virtual int GetChannelsCountAI() const 
	{
		return INPUT_ANALOG_CHANNELS_COUNT; 
	};

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

protected:

	//Функция процесса выполяется циклически через заданный интервал времени
	virtual void ControlProc();

	//число аналоговых входов = 2
	const int INPUT_ANALOG_CHANNELS_COUNT; 

	//Номер COM порта 
	int comPortNo;

	//Данные концентрация
	mutable double data[2];
};

//*****************************************************************************
class BonerGasAnaliserTest14 : public IdentifiedPhysDevice, public IAnalogInput
{

public:

	BonerGasAnaliserTest14();
	virtual ~BonerGasAnaliserTest14(void);

	
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

	static BonerGasAnaliserTest14* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

public:	
	
	////////////////////////////////////////////////////////////////////////////
	//interface IAnalogInput
	//Получение общего числа аналоговых входов
	virtual int GetChannelsCountAI() const {return INPUT_ANALOG_CHANNELS_COUNT; };

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


protected:

	bool SendModbus(MODBUS_COMMAND cmd, const char* data, unsigned data_size, std::vector<unsigned char>& response, std::ostream* pLog);

	virtual bool Create(std::string name, unsigned long updateInterval, int port, std::string strIP = "");


	//Функция процесса выполяется циклически через заданный интервал времени
	virtual void ControlProc();

	virtual bool OnFailure();

	//число аналоговых входов = 4
	const int INPUT_ANALOG_CHANNELS_COUNT; 

	//Номер COM порта 
	int comPortNo;

	std::string devAdress;

	CModbusTCPConnection* pModbusTCP;

	//диапазоны измерения 
	std::vector<MEASSURE_RANGE_INFO> rngAI;

	//Данные концентрация
	mutable double data[4];
};

#endif //AFX_BONER_GAS_ANALISER_DEVICES_H_INCLUDED_