#pragma once
#include "PhysDevice.h"
#include "common.h"

class CommandManager;

class EngineControlBlock : 
	public IdentifiedPhysDevice, 
	public IDigitalInput, 
	public IDigitalOutput, 
	public IAnalogInput, 
	public IAnalogOutput
{

public:

	EngineControlBlock();
	virtual ~EngineControlBlock(void);

	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);	

	static EngineControlBlock* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

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
	int DOUT_CHANNELS_COUNT;
	int DIN_CHANNELS_COUNT;
	int AOUT_CHANNELS_COUNT;

	virtual bool OnPrepareDevice();
	
	//������ ��������� ��� �������� ������� 
	//std::vector<int> doutRegs;
	
	//������ ��������� ��� �������� ������ 
	//std::vector<int> dinRegs;

	//������ ��������� ��� ���������� ������� 
	std::vector<int> aoutRegs;

	//������ ��������� ��� ���������� ������ 
	std::vector<int> ainRegs;

	//�������
	std::vector< std::pair<int, int> > doutCmds;

	//����� ����������������� ����� � �������� 
	//��������� ������
	int portNo;

	//����� ����������
	unsigned int devAdr;

	//��������� �������� �������
	mutable std::bitset<8>		doutChState;

	//��������� �������� ������
	mutable std::bitset<16>		dinChState;

	//��������� ���������� ������
	mutable std::vector<double>	dataAI;

	//��������� �������� ��������
	mutable std::vector<double> dataAO;
	
	//��������� ������� 
	std::vector<MEASSURE_RANGE_INFO> rngAO;

	//��������� ��������� 
	std::vector<MEASSURE_RANGE_INFO> rngAI;
};

