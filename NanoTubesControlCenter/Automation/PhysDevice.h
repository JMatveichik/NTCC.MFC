#if !defined(_LNP_PHYS_DEVICES_H_INCLUDED_)
#define _LNP_PHYS_DEVICES_H_INCLUDED_

#include "TimedControl.h"

class IdentifiedPhysDevice : public TimedControl
{

public:

	IdentifiedPhysDevice();
	virtual ~IdentifiedPhysDevice();

	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

public:

	std::string Manufacturer() const;
	std::string Model() const;
	std::string Name()	const;
	std::string Description()	const;

	virtual bool IsAlive() const;

	static bool FailureCallback(void* p);
	
	static bool AddDevice(IdentifiedPhysDevice* dev);
	static IdentifiedPhysDevice* GetDevice(std::string name);
	static void ClearAll();
	static void StopAll();

	static void EnumDevices(std::vector <const IdentifiedPhysDevice*>& devs );
	bool LogEnable(const char* pszPath);
	
	//повторная обработка узлов устройств после загрузки конфигурации
	virtual bool PostConfigProcessing(IXMLDOMNode* pNode);

protected:

	virtual bool Create(std::string name, unsigned long interval);
	IdentifiedPhysDevice(std::string manufacturer, std::string mode);

	virtual bool OnFailure();
	

	std::string m_strManufacturer;	//производитель 
	std::string m_strModel;			//модель
	std::string m_strName;			//идентификатор
	std::string m_strDesc;			//описание

	bool m_bStatusOk;
	unsigned int m_nFailures;
	
	mutable std::ofstream m_logDevs;
	
	static std::map<std::string, IdentifiedPhysDevice*> m_allDevices;

};
#endif // !defined(AFX_TIMEDCONTROL_H__F3FE3814_F5C2_4D12_886F_A89B055A5F5A__INCLUDED_)