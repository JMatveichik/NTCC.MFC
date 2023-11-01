#pragma once
#include "physdevice.h"
#include "common.h"



class NTCCDataClient : 
	public IdentifiedPhysDevice, 
	public IAnalogInput, 
	public IDigitalInput, 
	public IAnalogOutput,
	public IDigitalOutput
{
public:

	NTCCDataClient(void);
	virtual ~NTCCDataClient(void);

	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);
	
	static NTCCDataClient* NTCCDataClient::CreateFromXML(IXMLDOMNode* pNode, UINT& error);

protected:

	//������� �������� ���������� ���������� ����� �������� �������� �������
	virtual void ControlProc();

	//
	bool Send(std::string cmd, std::string& res) const;

	//����������� ������� ��������� ��������� ���������� ��������
	void GetAnalogRange(int channel, LPMEASSURE_RANGE_INFO lpmci, bool isInputRng) const;

	//����������� ������� ������� ���������� ��� ���������� ��������
	std::string SetChannel(int channel, double val, bool isAnalog) const;

public:
	//////////////////////////////////////////////////////////////////////////
	//IAnalogInput
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
	virtual void EnumInputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs)  const;

	//////////////////////////////////////////////////////////////////////////
	///IDigitalInput
	//��������� ��� ��������� ���������� � ���� ���� ��� ��������� 
	//���������� ������ � ������� ����������� ������
	
	//��������� ������ ����� ���������� ������
	virtual int GetChannelsCountDI() const;

	//�������� ��������� ���������� ������	
	virtual bool UpdateChannelsStateDI() const;

	//��������� ��������� ������ �� ���������� ������	
	virtual bool  GetChannelStateDI(unsigned char channel, bool update)  const;

	//////////////////////////////////////////////////////////////////////////
	//IAnalogOutput
	//��������� ��� ��������� ���������� � ���� ���� ��� ��������� 
	//���������� ������� � ������� ������������ ������
	
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
	virtual void EnumOutputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs)  const;

	//////////////////////////////////////////////////////////////////////////
	//IDigitalOutput
	//��������� ��� ��������� ���������� � ���� ���� ��� ��������� 
	//�������� ������� � ������� ������������ ������

	//��������� ������ ����� �������� ������� 
	virtual int GetChannelsCountDO() const;

	//�������� ������� ��������� ���������� �������	
	virtual bool UpdateChannelsStateDO() const;

	//�������� ��������� ��������� ������
	virtual bool GetChannelStateDO(unsigned char channel, bool update) const;

	//���������� ��������� ��������� ������ 
	virtual bool  SetChannelStateDO(unsigned char channel, bool enable);

private:

	HANDLE hPipe;
	CString strPipePath;

	int AIChannelsCount;		//���������� ������� ���������� ���������� ������
	std::vector<int> IdxsAI;	//������ �������� ������� ���������� ���������� ������

	int AOChannelsCount;		//���������� �������� ���������� ���������� ������
	std::vector<int> IdxsAO;	//������ �������� �������� ����������  ���������� ������

	int DIChannelsCount;		//���������� ������� ���������� ���������� ������
	std::vector<int> IdxsDI;	//������ �������� ������� ���������� ���������� ������

	int DOChannelsCount;		//���������� �������� ���������� ���������� ������
	std::vector<int> IdxsDO;	//������ �������� �������� ���������� ���������� ������
	
	mutable std::vector<double> srvdata;
	
};

