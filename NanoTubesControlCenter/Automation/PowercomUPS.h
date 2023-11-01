#pragma once
#include "physdevice.h"
#include "common.h"

class PowercomUPS : public IdentifiedPhysDevice, public IAnalogInput
{

public:

	PowercomUPS(void);
	virtual ~PowercomUPS(void);

	static PowercomUPS* CreateFromXML(IXMLDOMNode* pNode, UINT& error );

	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

public:

	//////////////////////////////////////////////////////////////////////////
	//Реализация интерфейса IAnalogInput
   
	//Получение общего числа аналоговых входов
	virtual int GetChannelsCountAI() const { return  INPUT_ANALOG_CHANNELS_COUNT; };

	//Получение данных со всех аналоговых входов (выполняется запрос на чтение данных)
	virtual void	ReadChannels(double* data  = NULL) const;

	//Получение данных с одного из аналоговых входов (выполняется запрос на чтение данных)
	virtual double  ReadChannel(unsigned char channel)  const;


	// Получение данных из буфера после последнего обновления
	// для одного из аналоговых входов (запрос на чтение данных не выполняется)
	virtual double  GetChannelData(unsigned char channel)  const;

	//Получить  информацию об измеряемом диапазоне канала
	virtual void GetInputRangeInfo(unsigned char channel, LPMEASSURE_RANGE_INFO lpmci) const;

	//Установить  диапазон  аналогового входа
	virtual bool SetInputRange(unsigned char ch, unsigned char range);

	//Получить все возможные  диапазоны  измерения
	virtual void EnumInputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const;

protected:
	
	virtual void ControlProc();

protected:

	std::vector<double> lastData;
	const int INPUT_ANALOG_CHANNELS_COUNT;
	int comPortNo;

};

