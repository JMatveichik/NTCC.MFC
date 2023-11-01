#pragma once
#include "PhysDevice.h"
#include "common.h"

class CommandManager;

class OwenBase : public IdentifiedPhysDevice
{

public:

	OwenBase(std::string model);
	virtual ~OwenBase(void);

	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);	

protected:

	virtual bool OnPrepareDevice();
	

	//����� ����������������� ����� � �������� 
	//��������� ������
	int comPortNo;

	//����� ����������
	unsigned char devAddres;
};


class OwenTPM : public OwenBase, public IAnalogInput, public IAnalogOutput, public IDigitalInput, public IDigitalOutput
{

public:

	OwenTPM(std::string model, int coutAI, int coutAO, int coutDO);

	virtual ~OwenTPM(void);

	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);	
	
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
	
	//������� �������� ���������� ���������� ����� �������� �������� �������
	virtual void ControlProc();

	//���������� ���������� �� ���������� ����������
	virtual void BuildMeassureRangeInfo() = 0;

	//���������� ���������� �� ������ ���������
	virtual void BuildRegestersMaps() = 0;

	//������������� ������ ������ � ������ ������� �������� ��������� ��������� �����
	virtual void PrepareChannelValue(unsigned char channel, double& val, bool forWrite  ) const = 0;


	int INPUT_ANALOG_CHANNELS_COUNT;
	int INPUT_DIGITAL_CHANNELS_COUNT;
	int OUTPUT_ANALOG_CHANNELS_COUNT;
	int OUTPUT_DIGITAL_CHANNELS_COUNT;

	mutable CRITICAL_SECTION m_ModBusCS;

	//������ ������� ��������� �����������
	double minT;

	//������� ������� ��������� �����������
	double maxT;

	//����������� ��� ��������� ����������� 
	//(�������� ������ ����������� ������� 0x0201)
	double dTCoef;

	double dTLimitsCoef;

	mutable bool bRelayState;

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

	//����� ����� -> Modbus ������� ��� ���������� ������
	std::map<int, int> aiChannelsMap;

	//����� ����� -> Modbus ������� ��� ���������� �������
	std::map<int, int> aoChannelsMap;

	//����� ����� -> Modbus ������� ��� �������� �������
	std::map<int, int> doChannelsMap;

	//��������� �������� �������
	mutable int curStatus;

	mutable std::vector<unsigned short> regs;
};

class OwenTPM201 : public OwenTPM
{

public:

	OwenTPM201();
	virtual ~OwenTPM201();

	static OwenTPM201* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//���������� ���������� �� ���������� ����������
	virtual void BuildMeassureRangeInfo();

	//���������� ���������� �� ������ ���������
	virtual void BuildRegestersMaps();

	//������������� ������ ������ � ������ ������� �������� ��������� ��������� �����
	virtual void PrepareChannelValue(unsigned char channel, double& val, bool forWrite ) const;
};


class OwenTPM210 : public OwenTPM //, public IDigitalOutput 
{

public:

	OwenTPM210();

	virtual ~OwenTPM210();
	
	static OwenTPM210* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

public:
	
	//���������� ���������� �� ���������� ����������
	virtual void BuildMeassureRangeInfo();

	//���������� ���������� �� ������ ���������
	virtual void BuildRegestersMaps();

	//������������� ������ ������ � ������ ������� �������� ��������� ��������� �����
	virtual void PrepareChannelValue(unsigned char channel, double& val, bool forWrite ) const;

protected:

};