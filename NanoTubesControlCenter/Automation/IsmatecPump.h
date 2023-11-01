#pragma once
#include "PhysDevice.h"
#include "common.h"

class CommandManager;

class IsmatecPump : public IdentifiedPhysDevice, 
	public IDigitalOutput, 
	public IAnalogOutput,
	public IAnalogInput
{

public:

	IsmatecPump();
	virtual ~IsmatecPump(void);

	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);	

	static IsmatecPump* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

public:
	//Функция процесса выполяется циклически через заданный интервал времени
	virtual void ControlProc();

	//////////////////////////////////////////////////////////////////////////
	//IAnalogInput
	
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
	virtual void EnumInputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs)  const;

	
	//////////////////////////////////////////////////////////////////////////
	//IDigitalOutput

	//Получение общего числа цифровых выходов 
	virtual int GetChannelsCountDO() const;

	//Обновить текущее состояния дискретных выходов	
	virtual bool UpdateChannelsStateDO() const;

	//Получить состояние цифрового выхода
	virtual bool GetChannelStateDO(unsigned char ch, bool update) const;

	//Установить состояние цифрового выхода 
	virtual bool  SetChannelStateDO(unsigned char ch, bool enable);

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

	int DOUT_CHANNELS_COUNT;
	int AOUT_CHANNELS_COUNT;
	int AIN_CHANNELS_COUNT;

	enum {
		DOUT_CH_START  = 0,
		DOUT_CH_DIRECT = 1,
		DOUT_CH_MODE   = 2
	};

	enum {
		AOUT_CH_SPEED = 0,
		AOUT_CH_MLMIN = 1
	};

	enum {
		AIN_CH_SPEED = 0,
		AIN_CH_MLMIN = 1
	};

	virtual bool OnPrepareDevice();
	
	//номер последовательного порта к которому 
	//подключен прибор
	int portNo;

	//адрес устройства
	unsigned int devAdr;

	mutable std::bitset<8>		outChState;

	mutable std::vector<double> lastValsAO;
	mutable std::vector<double> lastValsAI;

	

	std::vector<MEASSURE_RANGE_INFO> rngAO;
	std::vector<MEASSURE_RANGE_INFO> rngAI;
};
