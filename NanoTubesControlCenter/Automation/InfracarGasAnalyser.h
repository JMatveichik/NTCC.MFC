#pragma once

#include "PhysDevice.h"
#include "common.h"

class InfracarGasAnalyser : public IdentifiedPhysDevice, public IAnalogInput
{
public:

	
	InfracarGasAnalyser(void);
	virtual ~InfracarGasAnalyser(void);

	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);	
	static InfracarGasAnalyser* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

public:

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

	//����� ���������� ������ = 7
	const int INPUT_ANALOG_CHANNELS_COUNT; 

	//����� COM ����� 
	int comPortNo;

	//������ �������
	mutable double data[7];

	//���������� � ���������� ����������
	std::vector<MEASSURE_RANGE_INFO> ranges;
};

