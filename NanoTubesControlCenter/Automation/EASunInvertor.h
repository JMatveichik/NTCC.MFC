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
	//������� �������� ���������� ���������� ����� �������� �������� �������
	virtual void ControlProc();

	///////////////////////////////////////////////////
	//���������� ���������� IDigitalInput

	//�������� ����� ���������� ������
	virtual int GetChannelsCountDI() const; 	

	//�������� ��������� ���������� ������
	virtual bool UpdateChannelsStateDI() const;

	//��������� ��������� ������ ����������� �����
	virtual bool GetChannelStateDI(unsigned char channel, bool update) const;

	//////////////////////////////////////////////////////////////////////////
	//IDigitalOutput
	//��������� ������ ����� �������� ������� 
	virtual int GetChannelsCountDO() const;

	//�������� ������� ��������� ���������� �������	
	virtual bool UpdateChannelsStateDO() const;

	//�������� ��������� ��������� ������
	virtual bool GetChannelStateDO(unsigned char ch, bool update) const;

	//���������� ��������� ��������� ������ 
	virtual bool  SetChannelStateDO(unsigned char ch, bool enable);

	//////////////////////////////////////////////////////////////////////////
	//interface IAnalogInput
	//��������� ������ ����� ���������� ������
	virtual int GetChannelsCountAI() const;

	//��������� ������ �� ���� ���������� ������ (����������� ������ �� ������ ������)
	virtual void	ReadChannels(double* data  = NULL) const;

	//��������� ������ � ������ �� ���������� ������ (����������� ������ �� ������ ������)
	virtual double  ReadChannel(unsigned char ch)  const;

	// ��������� ������ �� ������ ��� ������ �� ���������� ������ 
	// (������ �� ������ ������ �� �����������)
	virtual double  GetChannelData(unsigned char ch)  const;

	//��������  ���������� �� ���������� ��������� ������
	virtual void GetInputRangeInfo(unsigned char ch, LPMEASSURE_RANGE_INFO lpmci) const;

	//����������  ��������  ����������� �����
	virtual bool SetInputRange(unsigned char ch, unsigned char range);

	//�������� ��� ���������  ���������  ���������
	virtual void EnumInputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const;


	//////////////////////////////////////////////////////////////////////////
	/// IAnalogOutput
	//��������� ������ ����� ���������� ������� 
	virtual int GetChannelsCountAO() const;

	//�������� ������ � ���������� ����� 
	virtual double  WriteToChannel(unsigned char ch, double val);

	//�������� ������ �� ��� ������
	virtual void WriteToAllChannels(const std::vector<double>& vals) ; 

	//�������� ��������� ���������� � ���������� ����� �������� 
	virtual double GetChannelStateAO (unsigned char ch) const;  

	bool  UpdateChannelsStateAO() const;

	//�������� �������� �������� ������ 
	virtual void GetMinMax(unsigned char ch, double& minOut, double& maxOut) const;

	//��������  ���������� �� ��������� ������
	virtual void GetOutputRangeInfo(unsigned char ch, LPMEASSURE_RANGE_INFO lpmci) const;

	//����������  ��������  ����������� ������
	virtual bool SetOutputRange(unsigned char ch, unsigned char range);

	//�������� ��� ���������  ���������  
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
	
	//����� ����������������� ����� � �������� 
	//��������� ������
	int portNo;

	//����� ����������
	int devAdr;

	//��������� �������� ������
	mutable std::bitset<48>	dinChState;

	//��������� �������� �������
	mutable std::bitset<32>	doutChState;

	//��������� ���������� ������
	mutable std::vector<double>	dataAI;

	//��������� �������� ��������
	mutable std::vector<double> dataAO;

	//��������� ������ ����� PIRI
	mutable std::vector<double> dataPIRI;

	//��������� ������ ����� GS
	mutable std::vector<double> dataGS;
	
	//��������� ������� 
	std::vector<MEASSURE_RANGE_INFO> rngAO;

	//��������� ��������� 
	std::vector<MEASSURE_RANGE_INFO> rngAI;
};

