#pragma once

#include "automation\physdevice.h"
#include "automation\Serial.h"
#include "Automation\common.h"

class Thrermocon011 : public IdentifiedPhysDevice, public IAnalogInput, public IAnalogOutput
{
public:
	Thrermocon011(void);
	virtual ~Thrermocon011(void);

	virtual bool Create(std::string name, unsigned char portNo);


	//////////////////////////////////////////////////////////////////////////
	///IAnalogInput

	//��������� ������ ����� ���������� ������
	virtual int GetChannelsCountAI() const { return 3; };

	//��������� ������ �� ���� ���������� ������ (����������� ������ �� ������ ������)
	virtual void	ReadChannels(double* data  = NULL) const;

	//��������� ������ � ������ �� ���������� ������ (����������� ������ �� ������ ������)
	virtual double  ReadChannel(unsigned char channel)  const;

	// ��������� ������ �� ������ ��� ������ �� ���������� ������ 
	// (������ �� ������ ������ �� �����������)
	virtual double  GetChannelData(unsigned char channel)  const;

	
	//////////////////////////////////////////////////////////////////////////
	/// IAnalogOutput
	//��������� ������ ����� ���������� ������� 
	virtual int GetChannelsCountAO() const { return 1; };

	//�������� ������ � ���������� ����� 
	virtual double  WriteToChannel(unsigned char channel, double val) const;

	//�������� ������ �� ��� ������
	virtual void WriteToAllChannels(double* vals = NULL) const; 

	//��������� ��������� ���������� ������
	virtual void SaveChannelsStateAO() const; 

	//������������ ��������� ���������� ������� 
	virtual void RestoreChannelsStateAO() const;

	//�������� ��������� ���������� � ���������� ����� �������� 
	virtual double GetChannelStateAO (unsigned char channel) const;  

	//�������� �������� �������� ������ 
	virtual void GetMinMax(unsigned char channel, double& minOut, double& maxOut) const;  

private:
	
	bool Write(unsigned short par, unsigned char data);
	bool Read(unsigned short par, unsigned char& data);

	bool SetTemperature(double T);

	bool SwitchMode(bool bEditMode);

	virtual void ControlProc();

	mutable SyncSerialPort comPort;

	DCB dcb0;
	DCB dcb1;

	mutable double m_values[3];
	
	mutable double lastWritenT; 
	mutable double storeT;

	mutable CRITICAL_SECTION m_writeCS;
};
