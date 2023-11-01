#if !defined(_ARTECH_DEVICES_EMUL_H_INCLUDED_)
#define _ARTECH_DEVICES_EMUL_H_INCLUDED_

#include "common.h"
#include "PhysDevice.h"

//TODO:: class EmulatorSignal : public TimedControl

//////////////////////////////////////////////////////////////////////////
class VirtualDevice : public IdentifiedPhysDevice, public IAnalogInput, public IAnalogOutput, public IDigitalInput, public IDigitalOutput
{

public:

	VirtualDevice();
	virtual ~VirtualDevice();

	
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

	static VirtualDevice* CreateFromXML(IXMLDOMNode* pNode, UINT& error );

public:

	////////////////////IAnalogInput
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


	////////////////////IAnalogOutput
	//��������� ������ ����� ���������� ������� 
	virtual int GetChannelsCountAO() const;

	//�������� ������ � ���������� ����� 
	virtual double  WriteToChannel(unsigned char channel, double val) ;

	//�������� ������ �� ��� ������
	virtual void WriteToAllChannels(const std::vector<double>& vals)  ; 

	//�������� ��������� ���������� � ���������� ����� �������� 
	virtual double GetChannelStateAO (unsigned char channel) const;  

	//�������� �������� �������� ������ 
	virtual void GetMinMax(unsigned char channel, double& minOut, double& maxOut) const;

	//��������  ���������� �� ��������� ������
	virtual void GetOutputRangeInfo(unsigned char channel, LPMEASSURE_RANGE_INFO lpmci) const;

	//����������  ��������  ����������� ������
	virtual bool SetOutputRange(unsigned char ch, unsigned char range);

	//�������� ��� ���������  ���������  
	virtual void EnumOutputRanges(unsigned char ch,  std::vector<MEASSURE_RANGE_INFO>& rngs) const;


	////////////////////IDigitalInput
	//��������� ������ ����� ���������� ������
	virtual int GetChannelsCountDI() const;

	//�������� ��������� ���������� ������	
	virtual bool UpdateChannelsStateDI() const;

	//��������� ��������� ������ �� ���������� ������	
	virtual bool  GetChannelStateDI(unsigned char channel, bool update) const;

	////////////////////IDigitalOutput
	//��������� ������ ����� �������� ������� 
	virtual int GetChannelsCountDO() const;

	//�������� ������� ��������� ���������� �������	
	virtual bool UpdateChannelsStateDO() const;

	//�������� ��������� ��������� ������
	virtual bool GetChannelStateDO(unsigned char channel, bool update) const;

	//���������� ��������� ��������� ������ 
	virtual bool  SetChannelStateDO(unsigned char channel, bool enable);	

	//�������� ������ �� ��� ������
	virtual void SetChannelsStateDO(unsigned char val);


	//////////////////////////////////////////////////////////////////////////

	virtual bool SetupGenerator(unsigned char channel, unsigned long highWidth, unsigned long lowWidth);

	virtual void ControlProc(); 
	
	bool SignalsFromFile(std::string file);


protected:
	
	virtual bool Create(std::string name, unsigned long updateInterval, int nCountAI, std::map<unsigned char, MEASSURE_RANGE_INFO> rangesAI, int nCountAO, std::map<unsigned char, MEASSURE_RANGE_INFO> rangesAO, int nCountDI, int nCountDO);
	void UpdateSignalsFromFile();

	double m_dt;

	mutable std::vector< MEASSURE_RANGE_INFO > channelsRangesAI;
	std::vector<double> channelsDataAI;

	mutable std::vector<double>  channelsDataAO;
	mutable std::vector< MEASSURE_RANGE_INFO > channelsRangesAO;

	std::vector<bool> channelsStateDI;

	mutable std::vector<bool> channelsStateDO;
	mutable std::pair<double, double> channelsPulseWidth;

	std::map< long, MEASSURE_RANGE_INFO > inpRanges;
	std::map< long, MEASSURE_RANGE_INFO > outRanges;

	//////////////////////////////////////////////////////////////////////////
	std::map< std::string, double > srteamSignals;
	std::ifstream inStream;

};

#endif //_ARTECH_DEVICES_EMUL_H_INCLUDED_