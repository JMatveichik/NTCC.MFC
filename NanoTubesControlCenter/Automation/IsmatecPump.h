#pragma once
#include "PhysDevice.h"
#include "common.h"

class CommandManager;

class IsmatecPump : public IdentifiedPhysDevice, 
	public IDigitalOutput, 
	public IAnalogOutput,
	public IAnalogInput
{

public:

	IsmatecPump();
	virtual ~IsmatecPump(void);

	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);	

	static IsmatecPump* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

public:
	//������� �������� ���������� ���������� ����� �������� �������� �������
	virtual void ControlProc();

	//////////////////////////////////////////////////////////////////////////
	//IAnalogInput
	
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
	virtual void EnumInputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs)  const;

	
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

	int DOUT_CHANNELS_COUNT;
	int AOUT_CHANNELS_COUNT;
	int AIN_CHANNELS_COUNT;

	enum {
		DOUT_CH_START  = 0,
		DOUT_CH_DIRECT = 1,
		DOUT_CH_MODE   = 2
	};

	enum {
		AOUT_CH_SPEED = 0,
		AOUT_CH_MLMIN = 1
	};

	enum {
		AIN_CH_SPEED = 0,
		AIN_CH_MLMIN = 1
	};

	virtual bool OnPrepareDevice();
	
	//����� ����������������� ����� � �������� 
	//��������� ������
	int portNo;

	//����� ����������
	unsigned int devAdr;

	mutable std::bitset<8>		outChState;

	mutable std::vector<double> lastValsAO;
	mutable std::vector<double> lastValsAI;

	

	std::vector<MEASSURE_RANGE_INFO> rngAO;
	std::vector<MEASSURE_RANGE_INFO> rngAI;
};
