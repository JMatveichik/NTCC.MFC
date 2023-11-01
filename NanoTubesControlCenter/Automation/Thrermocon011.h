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

	//Получение общего числа аналоговых входов
	virtual int GetChannelsCountAI() const { return 3; };

	//Получение данных со всех аналоговых входов (выполняется запрос на чтение данных)
	virtual void	ReadChannels(double* data  = NULL) const;

	//Получение данных с одного из аналоговых входов (выполняется запрос на чтение данных)
	virtual double  ReadChannel(unsigned char channel)  const;

	// Получение данных из буфера для одного из аналоговых входов 
	// (запрос на чтение данных не выполняется)
	virtual double  GetChannelData(unsigned char channel)  const;

	
	//////////////////////////////////////////////////////////////////////////
	/// IAnalogOutput
	//Получение общего числа аналоговых выходов 
	virtual int GetChannelsCountAO() const { return 1; };

	//Записать данные в аналоговый выход 
	virtual double  WriteToChannel(unsigned char channel, double val) const;

	//Записать данные во все каналы
	virtual void WriteToAllChannels(double* vals = NULL) const; 

	//Сохранить состояние аналоговых входов
	virtual void SaveChannelsStateAO() const; 

	//Восстановить состояние аналоговых выходов 
	virtual void RestoreChannelsStateAO() const;

	//Получить последнее записанное в аналоговый выход значение 
	virtual double GetChannelStateAO (unsigned char channel) const;  

	//Получить выходной диапазон канала 
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
