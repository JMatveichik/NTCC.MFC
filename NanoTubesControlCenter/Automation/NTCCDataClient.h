#pragma once
#include "physdevice.h"
#include "common.h"



class NTCCDataClient : 
	public IdentifiedPhysDevice, 
	public IAnalogInput, 
	public IDigitalInput, 
	public IAnalogOutput,
	public IDigitalOutput
{
public:

	NTCCDataClient(void);
	virtual ~NTCCDataClient(void);

	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);
	
	static NTCCDataClient* NTCCDataClient::CreateFromXML(IXMLDOMNode* pNode, UINT& error);

protected:

	//Функция процесса выполяется циклически через заданный интервал времени
	virtual void ControlProc();

	//
	bool Send(std::string cmd, std::string& res) const;

	//отправление команды получения диапазона аналоговой величины
	void GetAnalogRange(int channel, LPMEASSURE_RANGE_INFO lpmci, bool isInputRng) const;

	//отправление команды задания аналоговой или дискретной величины
	std::string SetChannel(int channel, double val, bool isAnalog) const;

public:
	//////////////////////////////////////////////////////////////////////////
	//IAnalogInput
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

	//////////////////////////////////////////////////////////////////////////
	///IDigitalInput
	//Интерфейс для устройств включающих в себя один или несколько 
	//дискретных входов с которых считываются данные
	
	//Получение общего числа дискретных входов
	virtual int GetChannelsCountDI() const;

	//Обновить состояния дискретных входов	
	virtual bool UpdateChannelsStateDI() const;

	//Получение состояния одного из дискретных входов	
	virtual bool  GetChannelStateDI(unsigned char channel, bool update)  const;

	//////////////////////////////////////////////////////////////////////////
	//IAnalogOutput
	//Интерфейс для устройств включающих в себя один или несколько 
	//аналоговых выходов в которые записываются данные
	
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

	//////////////////////////////////////////////////////////////////////////
	//IDigitalOutput
	//Интерфейс для устройств включающих в себя один или несколько 
	//цифровых выходов в которые записываются данные

	//Получение общего числа цифровых выходов 
	virtual int GetChannelsCountDO() const;

	//Обновить текущее состояния дискретных выходов	
	virtual bool UpdateChannelsStateDO() const;

	//Получить состояние цифрового выхода
	virtual bool GetChannelStateDO(unsigned char channel, bool update) const;

	//Установить состояние цифрового выхода 
	virtual bool  SetChannelStateDO(unsigned char channel, bool enable);

private:

	HANDLE hPipe;
	CString strPipePath;

	int AIChannelsCount;		//количество входных аналоговых источников данных
	std::vector<int> IdxsAI;	//массив индексов входных аналоговых источников данных

	int AOChannelsCount;		//количество выходных аналоговых источников данных
	std::vector<int> IdxsAO;	//массив индексов выходных аналоговых  источников данных

	int DIChannelsCount;		//количество входных дискретных источников данных
	std::vector<int> IdxsDI;	//массив индексов входных дискретных источников данных

	int DOChannelsCount;		//количество выходных дискретных источников данных
	std::vector<int> IdxsDO;	//массив индексов выходных дискретных источников данных
	
	mutable std::vector<double> srvdata;
	
};

