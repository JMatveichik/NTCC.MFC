#pragma once
#include "physdevice.h"
#include "common.h"

class PowercomUPS : public IdentifiedPhysDevice, public IAnalogInput
{

public:

	PowercomUPS(void);
	virtual ~PowercomUPS(void);

	static PowercomUPS* CreateFromXML(IXMLDOMNode* pNode, UINT& error );

	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

public:

	//////////////////////////////////////////////////////////////////////////
	//���������� ���������� IAnalogInput
   
	//��������� ������ ����� ���������� ������
	virtual int GetChannelsCountAI() const { return  INPUT_ANALOG_CHANNELS_COUNT; };

	//��������� ������ �� ���� ���������� ������ (����������� ������ �� ������ ������)
	virtual void	ReadChannels(double* data  = NULL) const;

	//��������� ������ � ������ �� ���������� ������ (����������� ������ �� ������ ������)
	virtual double  ReadChannel(unsigned char channel)  const;


	// ��������� ������ �� ������ ����� ���������� ����������
	// ��� ������ �� ���������� ������ (������ �� ������ ������ �� �����������)
	virtual double  GetChannelData(unsigned char channel)  const;

	//��������  ���������� �� ���������� ��������� ������
	virtual void GetInputRangeInfo(unsigned char channel, LPMEASSURE_RANGE_INFO lpmci) const;

	//����������  ��������  ����������� �����
	virtual bool SetInputRange(unsigned char ch, unsigned char range);

	//�������� ��� ���������  ���������  ���������
	virtual void EnumInputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const;

protected:
	
	virtual void ControlProc();

protected:

	std::vector<double> lastData;
	const int INPUT_ANALOG_CHANNELS_COUNT;
	int comPortNo;

};

