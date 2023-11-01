#if !defined(AFX_ADVANTECH_ADAM6000_SERIES_DEVICES_H_INCLUDED_)
#define AFX_ADVANTECH_ADAM6000_SERIES_DEVICES_H_INCLUDED_

#include "PhysDevice.h"
#include "common.h"
#include "ModbusTCPConnection.h"


const short GCL_INTERNAL_FLAGS_COUNT = 16;

class BaseConverter;
//������� ����� ��� ��������� Advantech ����� 6000
class ADAM6000 : public IdentifiedPhysDevice
{

public:

	//����������
	virtual ~ADAM6000();

	//�������� ������� �� XML ����  
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

public:	
	
	//�������� ��� ������ 
	std::string ModuleName();

	//�������� ������ �������� ������
	std::string ModuleFirmwareVersion();

	//�������� ����� ������
	std::string ModuleAddress() const { return  devAdress; } 

	//�������� ����� ����������������� �����
	unsigned char Port() const { return  portNo; } 	

protected :

	
	//������� ������ ������ 
	// Parameter: unsigned char comPortNo	-	����� �����
	// Parameter: unsigned char deviceAdress -   ����� ����������
	// Parameter: unsigned long updateInterval -   �������� ������ ����������, ms
	//************************************
	bool Create(std::string name, unsigned long updateInterval, std::string ip, unsigned port);

	//�����������
	ADAM6000(std::string model);
	
	virtual bool OnFailure();

	std::string SendASCII(const char* szData);

	bool SendModbus(MODBUS_COMMAND cmd, const char* data, unsigned data_size, std::vector<unsigned char>& response, std::ostream* pLog);
		

	bool SetInternalFlag(short idx, bool state) const;
	bool GetInternalFlag(short idx) const;
	std::bitset<GCL_INTERNAL_FLAGS_COUNT> GetInternalFlags() const;

	virtual bool OnCreate() = 0;
	
protected:

	//IP ����� ���������� "xxx.xxx.xxx.xxx"
	std::string devAdress;	

	//����� ����� 
	unsigned portNo;

	//���������� modbus over TCP 
	CModbusTCPConnection adamSocket;  
	
};



//����� ����������� ���������������� ������ ADVANTECH ADAM-6018
class ADAM6018 : public ADAM6000, public IAnalogInput, public IDigitalOutput
{

public:
	
	//�����������	
	ADAM6018(std::string model);

	virtual ~ADAM6018();

	static ADAM6018* CreateFromXML(IXMLDOMNode* pNode, UINT& error );

	//�������� ��������� ������� 
	bool UpdateChannelState();
	
	// ��������� / ���������� ������ ����������� ����� 
	// ��� ���������� ������
	bool EnableChannel(unsigned char ch, bool enable = true);

	
	//��������� ��������� ����������� ����� (������� �� �� ��� ������ ������) 	
	bool	IsChannelEnabled(unsigned char channel);

	//�������� ���������� �� ���������� ���������� 
	void UpdateInputRangesInfo( ) const;

	//�������� ���������� �� ���������� ���������� 
	bool UpdateInputRangeInfo( unsigned char channel ) const;

	
	//////////////////////////////////////////////////////////////////////////
	//���������� ������� ���������� IAnalogInput

	//��������� ������ ����� ���������� ������
	//��� ������� ���������� 8 - ������
	virtual int GetChannelsCountAI() const;	

	//������ ������ � ���������� ������ �� ������� ����� pdData 
	//������ ������ ������ ���� �� ������ sizeof(double)*INPUT_ANALOG_CHANNELS_COUNT
	//��� pdData = NULL ���������� ����� ��������� ������ � ���������� �� ���������� ������ ������  
	virtual void ReadChannels(double *pdData = NULL) const;

	//��������� ������ � ������ �� 8 ���������� ������ (����������� ������ �� ������ ������)
	// ����� ����� ������ ���� �� 0 �� 8
	virtual double  ReadChannel(unsigned char channel)  const;

	// ��������� ������ �� ������ ��� ������ �� ���������� ������ 
	// (������ �� ������ ������ �� �����������)
	virtual double  GetChannelData(unsigned char channel)  const;

	//��������  ���������� �� ���������� ��������� ������
	virtual void GetInputRangeInfo(unsigned char channel, LPMEASSURE_RANGE_INFO lpmci) const;

	//����������  ��������  ����������� �����
	virtual bool SetInputRange(unsigned char ch, unsigned char range);
	
	//�������� ����������� � ���� ��������� ���������� 
	virtual void EnumInputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const;

	//////////////////////////////////////////////////////////////////////////


	//��������� ������ ����� �������� ������� 
	// 0-7 - digital output
	// 8-23 - aux digital output
	virtual int GetChannelsCountDO() const;

	//�������� ������� ��������� ���������� �������	
	virtual bool UpdateChannelsStateDO() const;

	//�������� ��������� ��������� ������
	virtual bool GetChannelStateDO(unsigned char channel, bool update) const;

	//���������� ��������� ��������� ������ 
	virtual bool  SetChannelStateDO(unsigned char channel, bool enable);	

	//�������� ������ �� ��� ������
	virtual void SetChannelsStateDO(unsigned char val);

	

protected:	

	unsigned char channelsState;

	virtual bool OnCreate();
	
	mutable std::vector<int>	inputRanges; 
	mutable std::vector<double>	lastData;

	//  [11/18/2014 Johnny A. Matveichik]
	//std::bitset<8> stateDO;//��������� �������� �������
	std::bitset<24> stateDO;//��������� �������� �������


	virtual void ControlProc();

private:		

	const int DO_CHANNELS_COUNT;
	const int AI_CHANNELS_COUNT;
};

//����� ����������� ���������������� ������ ADVANTECH ADAM-6018
class ADAM6017 : public ADAM6018
{

public:
	
	//�����������	
	ADAM6017();

	virtual ~ADAM6017();

	static ADAM6017* CreateFromXML(IXMLDOMNode* pNode, UINT& error );

	
	//////////////////////////////////////////////////////////////////////////
	//���������� ������� ���������� IAnalogInput

	//��������� ������ ����� ���������� ������
	//��� ������� ���������� 8 - ������
	virtual int GetChannelsCountAI() const;	

	//������ ������ � ���������� ������ �� ������� ����� pdData 
	//������ ������ ������ ���� �� ������ sizeof(double)*INPUT_ANALOG_CHANNELS_COUNT
	//��� pdData = NULL ���������� ����� ��������� ������ � ���������� �� ���������� ������ ������  
	virtual void ReadChannels(double *pdData = NULL) const;

	//��������� ������ ����� �������� �������
	//0-1	�������� ������
	//2-17  ���������� �����
	virtual int GetChannelsCountDO() const;

	//�������� ���������� � ���� ��������� ���������� 
	virtual void EnumInputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const;
	
protected:

	virtual bool OnCreate(); 

private: 
	const int DO_CHANNELS_COUNT;
	const int AI_CHANNELS_COUNT;
};

//////////////////////////////////////////////////////////////////////////
// ����������� ������� ����� ��� ������� ADVANTECH ADAM-6000				//
// � ��������� ������� � ��������										//
//////////////////////////////////////////////////////////////////////////

class ADAM6000DIDO : public ADAM6000, public IDigitalInput,  public IDigitalOutput
{

public:

	
	//����������
	virtual ~ADAM6000DIDO();
	
public:
	
	///////////////////////////////////////////////////
	//���������� ���������� IDigitalInput

	//�������� ����� ���������� ������
	virtual int GetChannelsCountDI() const;

	//�������� ��������� ���������� ������
	virtual bool UpdateChannelsStateDI() const;

	//��������� ��������� ������ ����������� �����
	virtual bool GetChannelStateDI(unsigned char channel, bool update) const;

public:

	///////////////////////////////////////////////////
	//���������� ���������� IDigitalOutput

	//��������� ������ ����� �������� ������� 
	virtual int GetChannelsCountDO() const; 	

	//�������� ������� ��������� ���������� �������	
	virtual bool UpdateChannelsStateDO() const;
		
	//�������� ��������� ��������� ������
	virtual bool GetChannelStateDO(unsigned char channel, bool update) const;

	//���������� ��������� ��������� ������ 
	virtual bool  SetChannelStateDO(unsigned char channel, bool enable);	


	//��������� ���������
	virtual bool SetupGenerator(unsigned char channel, unsigned long highWidth, unsigned long lowWidth) = 0;

protected:

	//�����������
	ADAM6000DIDO(int channelsNumDI, int channelsNumDO, std::string model);
	
	virtual void ControlProc();

	
	const int  INPUT_DIGITAL_CHANNELS_COUNT;	//���������� �������� �������
	const int  OUTPUT_DIGITAL_CHANNELS_COUNT;	//���������� �������� ������
	

	std::bitset<64> stateDI;//��������� �������� ������ 
	std::bitset<64> stateDO;//��������� �������� �������
	
};

//////////////////////////////////////////////////////////////////////////
// ����� ����������� ������ ADVANTECH ADAM-4050
//////////////////////////////////////////////////////////////////////////
class ADAM6050 : public ADAM6000DIDO,  public IAnalogOutput
{

public:

	//�����������
	ADAM6050();
		
	//����������
	virtual ~ADAM6050();

	static ADAM6050* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

public:
	///////////////////////////////////////////////////
	//���������� ���������� IDigitalOutput

	//���������� ��������� ��������� ������ 
	virtual bool  SetChannelStateDO(unsigned char channel, bool enable);	

	///////////////////////////////////////////////////
	//���������� ���������� IAnalogOutput
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

protected:

	enum tagFreqMode { NONE=-1, HW_LW, FQ_DC, FQ_HW, FQ_LW, DC_HW, DC_LW };

	std::vector <tagFreqMode> conversionMode;
	std::vector < std::pair< const BaseConverter*, const BaseConverter*> > pcs; 
	std::vector <double> lastVals;
	
	//��������� ����� ������������ ������� ��������� ��� ����������� ������
	unsigned short usLowWidthStartAdr;	

	//��������� ����� ������������ �������� ��������� ��� ����������� ������ 
	unsigned short usHiWidthStartAdr; 

	//��������� ����� ��� ������������ ������ � ���������� ����� 
	unsigned short usAbsPulseStartAdr; 

	//�����������
	ADAM6050(int channelsNumDI, int channelsNumDO);

	virtual bool OnCreate();

	virtual void OnExit ();

	/////////////////////////////////////////////////
	// ��������� ���������
	virtual bool SetupGenerator(unsigned char channel, unsigned long highWidth, unsigned long lowWidth);
};

//////////////////////////////////////////////////////////////////////////
// ����� ����������� ������ ADVANTECH ADAM-4052
//////////////////////////////////////////////////////////////////////////
class ADAM6052 : public ADAM6050 , public IAnalogInput
{
public:

	//�����������
	ADAM6052();

	//����������
	virtual ~ADAM6052();

	static ADAM6052* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

protected:

	virtual bool OnCreate();

	virtual void ControlProc();
	
	/////////////////////////////////////////////////
	// ��������� ���������
	//virtual bool SetupGenerator(unsigned char channel, unsigned long highWidth, unsigned long lowWidth);

	//IAnalogInput
	//////////////////////////////////////////////////////////////////////////
	//���������� ������� ���������� IAnalogInput

	//��������� ������ ����� ���������� ������
	//��� ������� ���������� 16 - ������
	//0-7 - counter
	//8-15 - frequency
	virtual int GetChannelsCountAI() const;	

	//������ ������ � ���������� ������ �� ������� ����� pdData 
	//������ ������ ������ ���� �� ������ sizeof(double)*INPUT_ANALOG_CHANNELS_COUNT
	//��� pdData = NULL ���������� ����� ��������� ������ � ���������� �� ���������� ������ ������  
	virtual void ReadChannels(double *pdData = NULL) const;

	//��������� ������ � ������ �� 8 ���������� ������ (����������� ������ �� ������ ������)
	// ����� ����� ������ ���� �� 0 �� 8
	virtual double  ReadChannel(unsigned char channel)  const;

	// ��������� ������ �� ������ ��� ������ �� ���������� ������ 
	// (������ �� ������ ������ �� �����������)
	virtual double  GetChannelData(unsigned char channel)  const;

	//��������  ���������� �� ���������� ��������� ������
	virtual void GetInputRangeInfo(unsigned char channel, LPMEASSURE_RANGE_INFO lpmci) const;

	//����������  ��������  ����������� �����
	virtual bool SetInputRange(unsigned char ch, unsigned char range);

	//�������� ����������� � ���� ��������� ���������� 
	virtual void EnumInputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const;

	//////////////////////////////////////////////////////////////////////////
	mutable std::vector<double> data;
};


//////////////////////////////////////////////////////////////////////////
// ����� ����������� ������ ADVANTECH ADAM-4066
//////////////////////////////////////////////////////////////////////////

class ADAM6066 : public ADAM6050, public IAnalogInput
{

public:

	//�����������
	ADAM6066();

	//����������
	virtual ~ADAM6066();

	static ADAM6066* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

protected:
	
	virtual bool OnCreate();

	virtual void ControlProc();

	//IAnalogInput
	//////////////////////////////////////////////////////////////////////////
	//���������� ������� ���������� IAnalogInput

	//��������� ������ ����� ���������� ������
	//��� ������� ���������� 16 - ������
	//0-7 - counter
	//8-15 - frequency
	virtual int GetChannelsCountAI() const;	

	//������ ������ � ���������� ������ �� ������� ����� pdData 
	//������ ������ ������ ���� �� ������ sizeof(double)*INPUT_ANALOG_CHANNELS_COUNT
	//��� pdData = NULL ���������� ����� ��������� ������ � ���������� �� ���������� ������ ������  
	virtual void ReadChannels(double *pdData = NULL) const;

	//��������� ������ � ������ �� 8 ���������� ������ (����������� ������ �� ������ ������)
	// ����� ����� ������ ���� �� 0 �� 8
	virtual double  ReadChannel(unsigned char channel)  const;

	// ��������� ������ �� ������ ��� ������ �� ���������� ������ 
	// (������ �� ������ ������ �� �����������)
	virtual double  GetChannelData(unsigned char channel)  const;

	//��������  ���������� �� ���������� ��������� ������
	virtual void GetInputRangeInfo(unsigned char channel, LPMEASSURE_RANGE_INFO lpmci) const;

	//����������  ��������  ����������� �����
	virtual bool SetInputRange(unsigned char ch, unsigned char range);

	//�������� ����������� � ���� ��������� ���������� 
	virtual void EnumInputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const;

	//////////////////////////////////////////////////////////////////////////
	mutable std::vector<double> data;

};

class ADAM6000Impl
{
	public:

		ADAM6000Impl(const CModbusTCPConnection& sock);
		virtual ~ADAM6000Impl();

	protected:

	private:
		const CModbusTCPConnection& adamSocket;
};
 
#endif // AFX_ADVANTECH_ADAM6000_SERIES_DEVICES_H_INCLUDED_