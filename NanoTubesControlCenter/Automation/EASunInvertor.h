#pragma once
#include "PhysDevice.h"
#include "common.h"

class CommandManager;

class EASunInvertor : 
	public IdentifiedPhysDevice, 
	public IDigitalInput, 
	public IDigitalOutput, 
	public IAnalogInput, 
	public IAnalogOutput
{

public:

	EASunInvertor();
	virtual ~EASunInvertor(void);

	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);	

	static EASunInvertor* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

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

	bool  UpdateChannelsStateAO() const;

	//Получить выходной диапазон канала 
	virtual void GetMinMax(unsigned char ch, double& minOut, double& maxOut) const;

	//Получить  информацию об диапазоне канала
	virtual void GetOutputRangeInfo(unsigned char ch, LPMEASSURE_RANGE_INFO lpmci) const;

	//Установить  диапазон  аналогового выхода
	virtual bool SetOutputRange(unsigned char ch, unsigned char range);

	//Получить все возможные  диапазоны  
	virtual void EnumOutputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const ;

protected:

	typedef struct tagDataBlockInfo
	{
		tagDataBlockInfo(std::string name, int request_length, int response_length)
		{
			this->name = name;
			this->request_length = request_length;
			this->response_length = response_length;
		}

		std::string name;
		int request_length;
		int response_length;

	}DATA_BLOCK_INFO, *LPDATA_BLOCK_INFO; 

	enum {
		VFW = 0,
		ID, 
		MOD,
		PIRI,
		GS
	};

	std::vector<DATA_BLOCK_INFO> dbi;

	enum {
		DOUT_TYPE0 = 0,
		DOUT_TYPE1 = 1
	};

	typedef struct tagDigitalOutputChannelInfo {

		tagDigitalOutputChannelInfo( unsigned short channelType, std::string variableName, unsigned short cmdLegth)
		{
			this->channelType = channelType;
			this->variableName = variableName;
			this->cmdLegth = cmdLegth;
		}

		unsigned short channelType;
		std::string variableName;
		unsigned short cmdLegth;
	}DOUT_CHANNEL_INFO, *LPDOUT_CHANNEL_INFO;

	std::vector<DOUT_CHANNEL_INFO> doutci;


	typedef struct tagAnalogInputCannelInfo {
		tagAnalogInputCannelInfo(unsigned short blockID, unsigned short blockIdx, std::string description, double scale, double minValue, double maxValue, std::string units)
		{
			this->blockID = blockID;
			this->blockIdx = blockIdx;
			this->description = description;
			this->scale = scale;
			this->minValue = minValue;
			this->maxValue = maxValue;
			this->units = units;
		}
		unsigned short blockID;
		unsigned short blockIdx;
		std::string description;
		double scale;
		double minValue;
		double maxValue;
		std::string units;
	}AIN_CHANNEL_INFO, *LPAIN_CHANNEL_INFO;
	
	std::vector<AIN_CHANNEL_INFO> ainci;


	typedef struct tagAnalogOutputCannelInfo {
		tagAnalogOutputCannelInfo(std::string blockIDr, unsigned short blockIdxr, std::string cmd, std::string description, double scale, double minValue, double maxValue, std::string units, int secondValueBlockIdxr = -1)
		{
			this->blockIDr = blockIDr;
			this->blockIdxr = blockIdxr;
			this->cmd = cmd;
			this->description = description;
			this->scale = scale;
			this->minValue = minValue;
			this->maxValue = maxValue;
			this->units = units;
			this->seconValueBlockIdxr = secondValueBlockIdxr;
		}
		std::string blockIDr;
		unsigned short blockIdxr;
		int seconValueBlockIdxr;
		std::string cmd;
		std::string description;
		double scale;
		double minValue;
		double maxValue;
		std::string units;

	}AOUT_CHANNEL_INFO, *LPAOUT_CHANNEL_INFO;

	std::vector<AOUT_CHANNEL_INFO> aoutci;


	bool ReadDataBlock(unsigned int blockID, std::vector<double>& data) const;

	int AIN_CHANNELS_COUNT;
	int AOUT_CHANNELS_COUNT;
	int DOUT_CHANNELS_COUNT;
	int DIN_CHANNELS_COUNT;
	

	virtual bool OnPrepareDevice();
	
	//номер последовательного порта к которому 
	//подключен прибор
	int portNo;

	//адрес устройства
	int devAdr;

	//состояние цифровых входов
	mutable std::bitset<48>	dinChState;

	//состояние цифровых выходов
	mutable std::bitset<32>	doutChState;

	//состояние аналоговых входов
	mutable std::vector<double>	dataAI;

	//последние заданные значения
	mutable std::vector<double> dataAO;

	//последние данные блока PIRI
	mutable std::vector<double> dataPIRI;

	//последние данные блока GS
	mutable std::vector<double> dataGS;
	
	//диапазоны задания 
	std::vector<MEASSURE_RANGE_INFO> rngAO;

	//диапазоны измерения 
	std::vector<MEASSURE_RANGE_INFO> rngAI;
};

