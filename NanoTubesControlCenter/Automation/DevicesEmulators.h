#if !defined(_ARTECH_DEVICES_EMUL_H_INCLUDED_)
#define _ARTECH_DEVICES_EMUL_H_INCLUDED_

#include "common.h"
#include "PhysDevice.h"

//TODO:: class EmulatorSignal : public TimedControl

//////////////////////////////////////////////////////////////////////////
class VirtualDevice : public IdentifiedPhysDevice, public IAnalogInput, public IAnalogOutput, public IDigitalInput, public IDigitalOutput
{

public:

	VirtualDevice();
	virtual ~VirtualDevice();

	
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

	static VirtualDevice* CreateFromXML(IXMLDOMNode* pNode, UINT& error );

public:

	////////////////////IAnalogInput
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


	////////////////////IAnalogOutput
	//Получение общего числа аналоговых выходов 
	virtual int GetChannelsCountAO() const;

	//Записать данные в аналоговый выход 
	virtual double  WriteToChannel(unsigned char channel, double val) ;

	//Записать данные во все каналы
	virtual void WriteToAllChannels(const std::vector<double>& vals)  ; 

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


	////////////////////IDigitalInput
	//Получение общего числа дискретных входов
	virtual int GetChannelsCountDI() const;

	//Обновить состояния дискретных входов	
	virtual bool UpdateChannelsStateDI() const;

	//Получение состояния одного из дискретных входов	
	virtual bool  GetChannelStateDI(unsigned char channel, bool update) const;

	////////////////////IDigitalOutput
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


	//////////////////////////////////////////////////////////////////////////

	virtual bool SetupGenerator(unsigned char channel, unsigned long highWidth, unsigned long lowWidth);

	virtual void ControlProc(); 
	
	bool SignalsFromFile(std::string file);


protected:
	
	virtual bool Create(std::string name, unsigned long updateInterval, int nCountAI, std::map<unsigned char, MEASSURE_RANGE_INFO> rangesAI, int nCountAO, std::map<unsigned char, MEASSURE_RANGE_INFO> rangesAO, int nCountDI, int nCountDO);
	void UpdateSignalsFromFile();

	double m_dt;

	mutable std::vector< MEASSURE_RANGE_INFO > channelsRangesAI;
	std::vector<double> channelsDataAI;

	mutable std::vector<double>  channelsDataAO;
	mutable std::vector< MEASSURE_RANGE_INFO > channelsRangesAO;

	std::vector<bool> channelsStateDI;

	mutable std::vector<bool> channelsStateDO;
	mutable std::pair<double, double> channelsPulseWidth;

	std::map< long, MEASSURE_RANGE_INFO > inpRanges;
	std::map< long, MEASSURE_RANGE_INFO > outRanges;

	//////////////////////////////////////////////////////////////////////////
	std::map< std::string, double > srteamSignals;
	std::ifstream inStream;

};

#endif //_ARTECH_DEVICES_EMUL_H_INCLUDED_