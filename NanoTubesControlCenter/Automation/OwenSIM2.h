#pragma once
#include "owenTPM201.h"

class DataSrc;
class OwenSIM2 : public OwenBase, public IAnalogOutput, public IDigitalOutput
{

public:

	OwenSIM2();
	virtual ~OwenSIM2(void);

	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);
	virtual bool OnPrepareDevice();

	static OwenSIM2* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	enum tagDisplayValueType
	{
		DVT_INT  = 0x00,
		DVT_WRD  = 0x01,
		DVT_FLT  = 0x02,
		DVT_STR4 = 0x03,
		DVT_PRT  = 0x04,
	};

	
	
protected:
	//Инициализация команды 
	void InitCmd(LPMODBUS_SERIAL_COMMAND lpmbsc, unsigned char fn, int regCount, int regAddr) const;

	//Отобразить 
	bool DisplayValueType(unsigned short vt);

	//Функция процесса выполяется циклически через заданный интервал времени
	virtual void ControlProc();

public:
	bool Blink(bool blink);

	bool Write(std::string word);
	bool Write(short int val);
	bool Write(float val);


	enum {
		DOUT_CH_MODE	= 0,
		DOUT_CH_WARNING = 1,
		DOUT_CH_ERRORS  = 2,
		DOUT_CH_VALUE   = 3,
		DOUT_CH_BLINK   = 4
	};

	enum {
		AOUT_CH_MODE	= 0,
		AOUT_CH_WARNING = 1,
		AOUT_CH_ERRORS  = 2,
		AOUT_CH_VALUE   = 3
	};

	//////////////////////////////////////////////////////////////////////////
	/// IAnalogOutput
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
	virtual void EnumOutputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const ;

	//////////////////////////////////////////////////////////////////////////
	/// IDigitalOutput
	//Получение общего числа цифровых выходов 
	virtual int GetChannelsCountDO() const;

	//Обновить текущее состояния дискретных выходов	
	virtual bool UpdateChannelsStateDO() const;

	//Получить состояние цифрового выхода
	virtual bool GetChannelStateDO(unsigned char channel, bool update) const;

	//Установить состояние цифрового выхода 
	virtual bool  SetChannelStateDO(unsigned char channel, bool enable);

public:
	bool PostConfigProcessing(IXMLDOMNode* pNode);

private:
	
	int AOUT_CHANNELS_COUNT;
	int DOUT_CHANNELS_COUNT;

	void string2chara(std::string word, char* pszOut);
	std::vector < const DataSrc* > outValues;
	std::vector < std::string > outNames;

	std::vector < double > aoData;
	std::vector < bool   > doData;

	//диапазоны задания 
	std::vector<MEASSURE_RANGE_INFO> rngAO;

	int curValInd;

	double dMinDiff;
};

