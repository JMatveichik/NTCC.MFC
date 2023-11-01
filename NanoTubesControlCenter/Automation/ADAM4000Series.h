#if !defined(AFX_ADVANTECH_ADAM4000_SERIES_DEVICES_H_INCLUDED_)
#define AFX_ADVANTECH_ADAM4000_SERIES_DEVICES_H_INCLUDED_

#include "PhysDevice.h"
#include "common.h"

class CommandManager;
//������� ����� ��� ��������� Advantech 
class ADAM4000 : public IdentifiedPhysDevice
{

public:

	//�����������
	ADAM4000();

	//����������
	virtual ~ADAM4000();

	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);
	
protected:
	
	//�����������
	ADAM4000(std::string model);

	//������� ������ ������ 
	// Parameter: unsigned char comPortNo	-	����� �����
	// Parameter: unsigned char deviceAdress -   ����� ����������
	//************************************
	bool Create(std::string name, unsigned long updateInterval, unsigned char address, unsigned port);
	
	
	//����� ����������
	int devAdress;	

	//����� ����������������� �����
	int comNo;

	//
	mutable std::string strName;
	
	//�������� ������ ������������� ������
	CommandManager* cmds;  

	//������ ��� �������� ��������� �������
	mutable std::string strLastCmd;	 

	//������ ��� �������� ���������� ������
	mutable std::string strLastRes;

	//���������� ������ �� ������ ������
	virtual std::string ExtractRespond(std::string fullRespond, std::size_t from, int count = -1) const;

	virtual bool OnCreate() = 0;
	
public:	
	
	//����������� �������
	void AddBeforeCmdSendHandler(boost::function< void (LPSERIAL_COMMAND)	> handler );
	
	void AddAfterCmdRespondHandler(boost::function< void (LPSERIAL_COMMAND) > handler );

	void AddCmdFailureHadler(boost::function< void (LPSERIAL_COMMAND) > handler );

	
private:

	mutable boost::signals2::signal< void (LPSERIAL_COMMAND) > evBeforeCmdSend;
	mutable boost::signals2::signal< void (LPSERIAL_COMMAND) > evAfterRespond;	
	mutable boost::signals2::signal< void (LPSERIAL_COMMAND) > evCmdFailure;

public:
	
	//��������� ������ ��������� �������
	const char* LastCommand() const;

	//��������� ������ ���������� ������
	const char* LastRespond() const;

	//���������� ����������� ��������  � ��������� ������	
	bool SendCommand(char cmdType, std::string strCommand, bool bHighPriority, std::string &strRespond) const;

	//�������� ��� ������ 
	std::string ModuleName(bool bUpdate = true, int len = 4) const;

	//�������� ������ �������� ������
	std::string ModuleFirmwareVersion() ;

	//�������� ����� ������
	unsigned char ModuleAddress() const;

	//�������� ����� ����������������� �����
	unsigned char Port() const;


};

//����� ����������� ���������������� ������ ADVANTECH ADAM-4019+
class ADAM4019P : public ADAM4000, public IAnalogInput/*, public XMLObject<ADAM4019P>*/
{

public:
	
	//�����������	
	ADAM4019P();

	virtual ~ADAM4019P();

	static ADAM4019P* CreateFromXML(IXMLDOMNode* pNode, UINT& error );

	//virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

	//�������� ��������� ������� 
	bool UpdateChannelState() const;

	// ��������� / ���������� ������ ����������� ����� 
	// ��� ���������� ������
	bool EnableChannel(unsigned char ch, bool enable = true);
	
	//��������� ��������� ����������� ����� (������� �� �� ��� ������ ������) 	
	bool	IsChannelEnabled(unsigned char channel) const;
	
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

	//�������� ��� ���������  ���������  ���������
	virtual void EnumInputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const;

	
	//////////////////////////////////////////////////////////////////////////	
	//�������� ���������� �� ���������� ��������� ������
	virtual bool UpdateInputRangeInfo( unsigned char channel ) const;
	
	//�������� ���������� �� ���������� ���������� 
	void UpdateInputRangesInfo( ) const;

protected:	
	
	ADAM4019P(std::string model);

	virtual bool OnCreate();

	//��������� ���������� ������  
	mutable unsigned char channelsState;	 
	
	mutable std::vector<long>		inputRanges; 
	mutable std::vector<double>		lastData;

	
	virtual void ControlProc();


private:

	const int AI_CHANNELS_COUNT;
	
};

class ADAM4018P : public ADAM4019P
{
public:

	ADAM4018P();
	virtual ~ADAM4018P();

	//virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);
	static ADAM4018P* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//��������� ������ ����� ���������� ������
	//��� ������� ���������� 8 - ������
	virtual int ADAM4018P::GetChannelsCountAI();


public:
	
	//�������� ��� ���������  ���������  ���������
	virtual void EnumInputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const;

protected:

	virtual bool OnCreate();

private:

	const int AI_CHANNELS_COUNT;

};

class ADAM4011 : public ADAM4019P
{

public:

	ADAM4011();
	virtual ~ADAM4011();

	static ADAM4011* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//��������� ������ ����� ���������� ������
	//��� ������� ���������� 1 - ������
	virtual int GetChannelsCountAI() const;

	//////////////////////////////////////////////////////////////////////////	
	//�������� ���������� �� ���������� ��������� ������
	virtual bool UpdateInputRangeInfo( unsigned char channel ) const;

	//�������� ��� ���������  ���������  ���������
	virtual void EnumInputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const;

	//������ ������ � ���������� ������ �� ������� ����� pdData 
	//������ ������ ������ ���� �� ������ sizeof(double)*INPUT_ANALOG_CHANNELS_COUNT
	//��� pdData = NULL ���������� ����� ��������� ������ � ���������� �� ���������� ������ ������  
	virtual void ReadChannels(double *pdData = NULL) const;

protected:

	virtual bool OnCreate();

private:

	const int AI_CHANNELS_COUNT;
};

class ADAM4015 : public ADAM4019P
{

public:

	ADAM4015();
	virtual ~ADAM4015();

	static ADAM4015* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//��������� ������ ����� ���������� ������
	//��� ������� ���������� 6 - ������
	virtual int GetChannelsCountAI() const;

protected:
	
	virtual bool OnCreate();

private:

	const int AI_CHANNELS_COUNT;
};

//////////////////////////////////////////////////////////////////////////
// ��������������� ���������� RS-485 � RS-232
//////////////////////////////////////////////////////////////////////////
class ADAM4521 : public ADAM4000, public IDataProvider
{

public:
	
	//����������� 
	ADAM4521();

	//����������
	virtual ~ADAM4521();

	static ADAM4521* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//���������� ������������� ������
	bool SetID(std::string strID) const;

	//�������� ������������� ������
	std::string GetID() const;

	//��������� ������ �������� ����������
	virtual std::string DataPass(const IdentifiedPhysDevice* pSender, std::string outerCmd, bool bHigthPriority) const;

protected:

	virtual void ControlProc();

	virtual bool OnCreate();
	
	//�������� ����������� ������
	char GetDelimiter() const;

	mutable char delim;
	mutable std::string strID;

	
};


//////////////////////////////////////////////////////////////////////////
// ����� ����������� ������ ADVANTECH ADAM-4024
//////////////////////////////////////////////////////////////////////////
class ADAM4024 : public ADAM4000, public IDigitalInput, public IAnalogOutput
{

public:
		
	//�����������
	ADAM4024();

	//����������
	virtual ~ADAM4024();
	
	static ADAM4024* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	/////////////////////////////////////
	//���������� ���������� IAnalogOutput

	//��������� ������ ����� ���������� ������� 
	virtual int GetChannelsCountAO() const;

	//�������� ������ � ���������� ����� 
	virtual double  WriteToChannel(unsigned char channel, double val);

	//�������� ������ �� ��� ������
	virtual void WriteToAllChannels(const std::vector<double>& vals) ; 

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


	/////////////////////////////////////
	//���������� ���������� IDigitalInput
	virtual int GetChannelsCountDI() const; 
	
	virtual bool GetChannelStateDI(unsigned char nChannel, bool update) const;

	virtual bool UpdateChannelsStateDI() const;
	

	//////////////////////////////////////////////////////////////////////////
	/// ���������� ����������� ������� ������


	//�������� ���������� �� ��������� ������
	bool UpdateOutputRangeInfo( unsigned char channel ) const;

	//�������� ���������� � ���������� 
	void UpdateOutputRangesInfo( ) const;

	//������� ���������� ��������� ��� ������ ������
	bool SetStartupValue(unsigned char channel, double val) const;

	//������� ���������� ��������� ��� ���� ������
	bool SetStartupValue(double* vals) const;

	//������� ���������� ��������� ��� ������ ������ 
	bool SetEmergencyValue(unsigned char channel, double val) const;

	//������� ���������� ��������� ��� ���� ������� 
	bool SetEmergencyValue(double* vals) const;

	//������������ ����� ��� ������������ ���������� ��������� ��� ������ ������ 
	bool SetEmergencyFlag(unsigned char channel, bool enable = true) const;

	//��������� ����� ��� ������������ ���������� ��������� ��� ������ ������ 
	bool GetEmergencyFlag(unsigned char channel) const;

protected:

	virtual bool OnCreate();

	virtual void ControlProc();

	const int  OUTPUT_ANALOG_CHANNELS_COUNT;
	const int  INPUT_DIGITAL_CHANNELS_COUNT;

	mutable std::vector<long>		outputRanges; 

	std::vector<double> lastValues;
	mutable unsigned char stateDI;
};

//////////////////////////////////////////////////////////////////////////
// ����� ����������� ������ ADVANTECH ADAM-4050
//////////////////////////////////////////////////////////////////////////


class ADAM4050 : public ADAM4000, public IDigitalInput,  public IDigitalOutput
{

public:

	//�����������
	ADAM4050();

	//����������
	virtual ~ADAM4050();

	static ADAM4050* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	///////////////////////////////////////////////////
	//���������� ���������� IDigitalInput

	//�������� ����� ���������� ������
	virtual int GetChannelsCountDI() const;

	//�������� ��������� ���������� ������
	virtual bool UpdateChannelsStateDI() const;

	//��������� ��������� ������ ����������� �����
	virtual bool GetChannelStateDI(unsigned char nChannel, bool update) const;


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

	//�������� ������ �� ��� ������
	virtual void SetChannelsStateDO(unsigned char val);

	//�������� ��������� �������� �������
	virtual unsigned char GetChannelsStateDO();

	//�������� ��������� �������� �������
	virtual unsigned char GetChannelsStateDI();

protected:
	
	virtual bool OnCreate();
	virtual void ControlProc();

private:

	const int  OUTPUT_DIGITAL_CHANNELS_COUNT;	//���������� �������� ������
	const int  INPUT_DIGITAL_CHANNELS_COUNT;	//���������� �������� �������

	mutable unsigned char stateDI;	//��������� �������� ������
	mutable unsigned char stateDO;	//��������� �������� �������

	mutable unsigned char stateStoreDO; // ��������� ��������� �������� �������

};

//////////////////////////////////////////////////////////////////////////
// ����� ����������� ������ ADVANTECH ADAM-4053
//////////////////////////////////////////////////////////////////////////

class ADAM4053 : public ADAM4000, public IDigitalInput
{

public:

	//�����������
	ADAM4053();

	//����������
	virtual ~ADAM4053();

	static ADAM4053* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	///////////////////////////////////////////////////
	//���������� ���������� IDigitalInput

	//�������� ����� ���������� ������
	virtual int GetChannelsCountDI() const;

	//�������� ��������� ���������� ������
	bool UpdateChannelsStateDI() const;

	//��������� ��������� ������ ����������� �����
	bool GetChannelStateDI(unsigned char ch, bool update) const;

	
protected:

	//�������� �������� �� ��������� �������� ��������� 
	//������� �������
	bool GetStateDI(std::bitset<16>& state) const;

	virtual bool OnCreate();
	virtual void ControlProc();

private:

	const int  INPUT_DIGITAL_CHANNELS_COUNT;	//���������� �������� �������

	std::bitset<16> stateDI;					//c�������� �������� �������

	bool changesConfirm;		//��������� ��� ��� ��������� ��� ��������� ����

};

class ADAM4052 : public ADAM4000, public IDigitalInput
{

public:

	//�����������
	ADAM4052();

	//����������
	virtual ~ADAM4052();

	static ADAM4052* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	///////////////////////////////////////////////////
	//���������� ���������� IDigitalInput

	//�������� ����� ���������� ������
	virtual int GetChannelsCountDI() const;

	//�������� ��������� ���������� ������
	bool UpdateChannelsStateDI() const;

	//��������� ��������� ������ ����������� �����
	bool GetChannelStateDI(unsigned char ch, bool update) const;


protected:

	//�������� �������� �� ��������� �������� ��������� 
	//������� �������
	bool GetStateDI(std::bitset<8>& state) const;

	virtual bool OnCreate();
	virtual void ControlProc();

private:

	const int  INPUT_DIGITAL_CHANNELS_COUNT;	//���������� �������� �������

	std::bitset<8> stateDI;					//c�������� �������� �������

	bool changesConfirm;		//��������� ��� ��� ��������� ��� ��������� ����

};


class ADAM4069 : public ADAM4000,  public IDigitalOutput
{

public:

	//�����������
	ADAM4069();

	//����������
	virtual ~ADAM4069();

	static ADAM4069* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

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

	//��������� ��������� �������� �������
	//virtual void SaveChannelsStateDO() const; 

	//������������ ��������� �������� ������� 
	//virtual void RestoreChannelsStateDO() const; 

protected:
	
	virtual bool OnCreate();
	virtual void ControlProc();

private:

	const int  OUTPUT_DIGITAL_CHANNELS_COUNT_4069;

	mutable unsigned char stateDO;
	mutable unsigned char stateStoreDO;
};

class ADAM4068 : public ADAM4069
{

public:

	//�����������
	ADAM4068();

	//����������
	virtual ~ADAM4068();


public:

	static ADAM4068* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//��������� ������ ����� �������� ������� 
	virtual int GetChannelsCountDO() const;

protected:
	virtual bool OnCreate();

private:

	const int  OUTPUT_DIGITAL_CHANNELS_COUNT_4068;
};

class ADAM4080 : public ADAM4000, public IAnalogInput
{

public:

	//�����������
	ADAM4080();

	//����������
	virtual ~ADAM4080();


public:

	static ADAM4080* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//////////////////////////////////////////////////////////////////////////
	//���������� ������� ���������� IAnalogInput

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
	
protected:
	
	virtual bool OnCreate();

	virtual void ControlProc();

private:

	const int  INPUT_ANALOG_CHANNELS_COUNT_4080;
	std::vector<double> dataBuffer;
};

#endif // AFX_ADVANTECH_ADAM4000_SERIES_DEVICES_H_INCLUDED_