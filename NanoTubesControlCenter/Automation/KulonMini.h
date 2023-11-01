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
	//��������� ������ ����� ���������� ������
	virtual int GetChannelsCountAI() const;

	//��������� ������ �� ���� ���������� ������ (����������� ������ �� ������ ������)
	virtual void	ReadChannels(double* data  = NULL) const;

	//��������� ������ � ������ �� ���������� ������ (����������� ������ �� ������ ������)
	virtual double  ReadChannel(unsigned char channel)  const;

	// ��������� ������ �� ������ ��� ������ �� ���������� ������ 
	// (������ �� ������ ������ �� �����������)
	virtual double  GetChannelData(unsigned char channel)  const;

	//��������  ���������� �� ���������� ��������� ������
	virtual void GetInputRangeInfo(unsigned char channel, LPMEASSURE_RANGE_INFO lpmci) const;

	//����������  ��������  ����������� �����
	virtual bool SetInputRange(unsigned char ch, unsigned char range);

	//�������� ��� ���������  ���������  ���������
	virtual void EnumInputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const;


	//////////////////////////////////////////////////////////////////////////
	/// IAnalogOutput
	//��������� ������ ����� ���������� ������� 
	virtual int GetChannelsCountAO() const;

	//�������� ������ � ���������� ����� 
	virtual double  WriteToChannel(unsigned char channel, double val);

	//�������� ������ �� ��� ������
	virtual void WriteToAllChannels(const std::vector<double>& vals); 

	//�������� ��������� ���������� � ���������� ����� �������� 
	virtual double GetChannelStateAO (unsigned char channel) const;  

	//�������� �������� �������� ������ 
	virtual void GetMinMax(unsigned char channel, double& minOut, double& maxOut) const;

	//��������  ���������� �� ��������� ������
	virtual void GetOutputRangeInfo(unsigned char channel, LPMEASSURE_RANGE_INFO lpmci) const;

	//����������  ��������  ����������� ������
	virtual bool SetOutputRange(unsigned char ch, unsigned char range);

	//�������� ��� ���������  ���������  
	virtual void EnumOutputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const ;

	/////////////////////////////////////
	//���������� ���������� IDigitalInput
	virtual int GetChannelsCountDI() const; 

	virtual bool GetChannelStateDI(unsigned char nChannel, bool update) const;

	virtual bool UpdateChannelsStateDI() const;

	/////////////////////////////////////
	//���������� ���������� IDigitalOutput
	//��������� ������ ����� �������� ������� 
	virtual int GetChannelsCountDO() const;

	//�������� ������� ��������� ���������� �������	
	virtual bool UpdateChannelsStateDO() const;

	//�������� ��������� ��������� ������
	virtual bool GetChannelStateDO(unsigned char channel, bool update) const;

	//���������� ��������� ��������� ������ 
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
	
	//������� �������� ���������� ���������� ����� �������� �������� �������
	virtual void ControlProc();

	//���������� ���������� �� ���������� ����������
	virtual void BuildMeassureRangeInfo();

	//���������� ���������� �� ������ ���������
	virtual void BuildRegestersMaps();

	//������������� ������ ������ � ������ ������� �������� ��������� ��������� �����
	//virtual void PrepareChannelValue(unsigned char channel, double& val, bool forWrite  ) const = 0;


	int INPUT_ANALOG_CHANNELS_COUNT;
	int INPUT_DIGITAL_CHANNELS_COUNT;
	int OUTPUT_ANALOG_CHANNELS_COUNT;
	int OUTPUT_DIGITAL_CHANNELS_COUNT;

	mutable CRITICAL_SECTION m_ModBusCS;
	
	//����� �������� ��������� ����������� ������ ��� ���������� ������
	mutable std::vector<double> dataAI;

	//����� �������� ��������� ���������� ������ ��� ���������� �������
	mutable std::vector<double> dataAO;

	//����� �������� ��������� ���������� ������ ��� �������� �������
	mutable std::vector<bool> dataDO;

	//���������� � ���������� ���������� ��� ���������� ������
	std::vector<MEASSURE_RANGE_INFO> mrciIn;

	//���������� � ���������� ���������� ��� ���������� �������
	std::vector<MEASSURE_RANGE_INFO> mrciOut;

	//����� ����� -> Modbus ������� ��� ���������� �������
	std::map<int, int> aoChannelsMap;

	//����� ����� -> Modbus ������� -> bit ��� �������� �������
	std::map<int, DO_FULL_ADRESS> doChannelsMap;

	mutable std::map<int, unsigned short> doRegs;

	//��������� �������� ������
	mutable unsigned int curStatus;

	//����� ����������������� ����� � �������� 
	//��������� ������
	int comPortNo;

	//����� ����������
	unsigned char devAddres;
};

