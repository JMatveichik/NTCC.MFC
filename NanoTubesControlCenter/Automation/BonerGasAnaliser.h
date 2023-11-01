#if !defined(AFX_BONER_GAS_ANALISER_DEVICES_H_INCLUDED_)
#define AFX_BONER_GAS_ANALISER_DEVICES_H_INCLUDED_

#include "PhysDevice.h"
#include "common.h"
#include "ModbusTCPConnection.h"



class BonerGasAnaliserTest01 : public IdentifiedPhysDevice, public IAnalogInput
{

public:

	BonerGasAnaliserTest01();
	virtual ~BonerGasAnaliserTest01(void);

	bool BonerGasAnaliserTest01::CreateFromXMLNode(IXMLDOMNode* pNode);

	static BonerGasAnaliserTest01* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

public:
	
	virtual bool Create(std::string name, int portNo, unsigned long updateInterval);

	//////////////////////////////////////////////////////////////////////////
	//interface IAnalogInput
	//��������� ������ ����� ���������� ������
	virtual int GetChannelsCountAI() const 
	{
		return INPUT_ANALOG_CHANNELS_COUNT; 
	};

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

protected:

	//������� �������� ���������� ���������� ����� �������� �������� �������
	virtual void ControlProc();

	//����� ���������� ������ = 2
	const int INPUT_ANALOG_CHANNELS_COUNT; 

	//����� COM ����� 
	int comPortNo;

	//������ ������������
	mutable double data[2];
};

//*****************************************************************************
class BonerGasAnaliserTest14 : public IdentifiedPhysDevice, public IAnalogInput
{

public:

	BonerGasAnaliserTest14();
	virtual ~BonerGasAnaliserTest14(void);

	
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

	static BonerGasAnaliserTest14* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

public:	
	
	////////////////////////////////////////////////////////////////////////////
	//interface IAnalogInput
	//��������� ������ ����� ���������� ������
	virtual int GetChannelsCountAI() const {return INPUT_ANALOG_CHANNELS_COUNT; };

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


protected:

	bool SendModbus(MODBUS_COMMAND cmd, const char* data, unsigned data_size, std::vector<unsigned char>& response, std::ostream* pLog);

	virtual bool Create(std::string name, unsigned long updateInterval, int port, std::string strIP = "");


	//������� �������� ���������� ���������� ����� �������� �������� �������
	virtual void ControlProc();

	virtual bool OnFailure();

	//����� ���������� ������ = 4
	const int INPUT_ANALOG_CHANNELS_COUNT; 

	//����� COM ����� 
	int comPortNo;

	std::string devAdress;

	CModbusTCPConnection* pModbusTCP;

	//��������� ��������� 
	std::vector<MEASSURE_RANGE_INFO> rngAI;

	//������ ������������
	mutable double data[4];
};

#endif //AFX_BONER_GAS_ANALISER_DEVICES_H_INCLUDED_