#pragma once
#include "PhysDevice.h"
#include "common.h"

class CommandManager;

class BatteryMeassureUnit : 
	public IdentifiedPhysDevice, 
	public IDigitalInput,
	public IAnalogInput, 
	public IAnalogOutput
{

public:

	BatteryMeassureUnit();
	virtual ~BatteryMeassureUnit(void);

	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);	

	static BatteryMeassureUnit* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

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
	int DIN_CHANNELS_COUNT;
	int AOUT_CHANNELS_COUNT;

	virtual bool OnPrepareDevice();
	
	//����� ����������������� ����� � �������� 
	//��������� ������
	int portNo;

	//����� ����������
	unsigned int devAdr;

	//������ ��������� ��� ���������� ������� 
	std::vector<int> aoutRegs;

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

