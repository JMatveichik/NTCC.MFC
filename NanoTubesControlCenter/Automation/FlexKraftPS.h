#pragma once
#include "PhysDevice.h"
#include "common.h"

class CommandManager;

class FlexKraftPS : 
	public IdentifiedPhysDevice, 
	public IDigitalInput, 
	public IDigitalOutput, 
	public IAnalogInput, 
	public IAnalogOutput
{

public:

	FlexKraftPS();
	virtual ~FlexKraftPS(void);

	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);	

	static FlexKraftPS* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

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

	int AIN_CHANNELS_COUNT;
	int AOUT_CHANNELS_COUNT;
	int DOUT_CHANNELS_COUNT;
	int DIN_CHANNELS_COUNT;
	

	virtual bool OnPrepareDevice();

	std::vector<double> channelFactor;

	//������ ��������� ��� ���������� ������� 
	std::vector<int> aoutRegs;

	//������ ��������� ��� ���������� ������ 
	std::vector<int> ainRegs;
	
	//����� ����������������� ����� � �������� 
	//��������� ������
	int portNo;

	//����� ����������
	unsigned int devAdr;

	//��������� �������� ������
	mutable std::bitset<48>	dinChState;

	//��������� �������� �������
	mutable std::bitset<16>	doutChState;

	
	//������ ��������� (coil) ��� ���������� ������
	std::vector<int>	adrDI;

	//������ ��������� (coil) ��� ���������� �������
	std::vector<int>	adrDO;

	//��������� ���������� ������
	mutable std::vector<double>	dataAI;

	//��������� �������� ��������
	mutable std::vector<double> dataAO;
	
	//��������� ������� 
	std::vector<MEASSURE_RANGE_INFO> rngAO;

	//��������� ��������� 
	std::vector<MEASSURE_RANGE_INFO> rngAI;
};

