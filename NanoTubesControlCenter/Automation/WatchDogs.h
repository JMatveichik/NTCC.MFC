#if !defined(_WATCH_DOGC_H_INCLUDED_)
#define _WATCH_DOGC_H_INCLUDED_

#include "TimedControl.h"
#include "DataSrc.h"
#include "..\OutMessage.h"

#pragma region WatchDog

//������� ����� ��� ������������� ��������� ����������� 
//�� � �������� �� ��� ���������

class BaseWatchDog : public TimedControl
{
public:

	//�����������
	BaseWatchDog();	

	//����������
	virtual ~BaseWatchDog();

	//�������� ��������� ������ �� XML ����
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

	//************************************
	// Parameter: std::string id - ������������� 
	// Parameter: std::string dsID - ������������� ��������������� ��
	// Parameter: bool changeStateTo - �������������� ���������  false - ������������ 1->0; true  - ������������ 0->1
	// Parameter: bool stopOnSet - ���� ��������� ��� ������������
	// Parameter: unsigned int updateInterval - ���� ����� �������� >= MINDELAY=50 ms ��������� ��� ��������, ����� ������ � ������ 
	// Returns:   true - ���� ������ �������, ����� false 
	//************************************	
	bool Create(std::string sID, std::string sListenDSID, bool bListenState,  int nWaitOnSet = 5000, unsigned int updateInterval = MINDELAY);


	/////////////////////////////////////////////////////////////
	///����������� ������� ��� ������ � ����������� ���������� ��������

	//����������������  ���������� ������
	static bool AddWatchDog(BaseWatchDog* pWD);

	//��������  ���������� ������
	static const BaseWatchDog* GetWatchDog(std::string id);

	//������� ��� ���������� �������
	static void ClearAll();
	

	std::string Name() const;


	//���������� �������� ����������� � ������ ������������ 
	//��������������� �� � �������� ���������
	void AddOnWatchDogSetHandler(boost::function< void (const DiscreteDataSrc*) > handler)
	{
		evOnWatchDogSet.connect(handler);
	}


	//������ ������ �������������
	virtual bool Start(unsigned int nInterval = BASEDELAY);



protected:	

	void ControlState();
	

	//���������� � ������ ������������ ��������������� 
	//�� � �������� ���������
	virtual void OnListenStateSet() = 0;

	virtual void LogEvent(std::strstream& out);

	//��� ������������ ���������� ��������� ���������
	boost::signals2::signal< void (const DiscreteDataSrc*) > evOnWatchDogSet;

	//�������������� ���������� �������� ������
	const DiscreteDataSrc* m_pListenDS;	
	
	//�������������� ��������� 
	//false - ������������ 1->0
	//true  - ������������ 0->1
	bool m_bListenState;

	//��������� ������������� 
	std::string m_sID;

	//���������� ��������� ���������
	bool m_bPrevState;

	//�������� ��� ������������
	int m_nWaitOnSet;

	int m_nCounter;

	bool m_bStopOnSet;

	int m_nConfirmTime;

	//��� ��������� ���������� ������ 
	static std::map<std::string, BaseWatchDog*> watchDogs; 

	bool ConfirmStateChanges() const;

	//��������� �������������
	virtual void ControlProc();
};

#pragma endregion WatchDog



#pragma region WatchDogDO

//����� ��� ������������� ��������� ����������� 
//�� � ������������� �������� ���������� �� � �������� ���������
class WatchDogDO : public BaseWatchDog
{
public:

	//�����������
	WatchDogDO();	

	//����������
	virtual ~WatchDogDO();

	
	//�������� ��������� ������ �� XML ����
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

	//************************************
	// Parameter: std::string sID - ������������� 
	// Parameter: std::string sListenDSID - ������������� ��������������� ��
	// Parameter: bool changeStateTo - �������������� ���������  false - ������������ 1->0; true  - ������������ 0->1
	// Parameter: bool stopOnSet - ���� ��������� ��� ������������
	// Parameter: unsigned int updateInterval - ���� ����� �������� >= MINDELAY=50 ms ��������� ��� ��������, ����� ������ � ������ 
	// Parameter: std::string dsIDOUT - ������������� ��������� ��
	// Parameter: bool outState	- ��������� ��������� ��, � ������� �� ������������ ��� ������������ ������� 
	// Returns:   true - ���� ������ �������, ����� false 
	//************************************	
	bool Create(std::string sID, std::string sListenDSID, bool bListenState, std::string sOutputDSID, bool bOutState, int nWaitOnSet = 5000, unsigned int updateInterval = MINDELAY);


	static WatchDogDO* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//���������� � ������ ������������ ��������������� 
	//�� � �������� ���������
	virtual void OnListenStateSet();

	virtual void LogEvent(std::strstream& out);

protected:

// 	const DiscreteOutputDataSrc* m_pOutDS;
// 	bool m_bOutState;

	//�������� ���������� �� � ��������� � ������� ����� ����������� ��� ������������ 
	std::vector< std::pair<DiscreteOutputDataSrc*, bool> > m_vDSOut;
	
};


//����� ��� ������������� ��������� ����������� ��
//� ��������/���������� ������������ �������
class WatchDogScriptControl : public BaseWatchDog
{
public:

	//�����������
	WatchDogScriptControl();	

	//����������
	virtual ~WatchDogScriptControl();

	
	//�������� ��������� ������ �� XML ����
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

	//************************************
	// Parameter: std::string sID - ������������� 
	// Parameter: std::string sListenDSID - ������������� ��������������� ��
	// Parameter: bool changeStateTo - �������������� ���������  false - ������������ 1->0; true  - ������������ 0->1
	// Parameter: bool stopOnSet - ���� ��������� ��� ������������
	// Parameter: unsigned int updateInterval - ���� ����� �������� >= MINDELAY=100 ms ��������� ��� ��������, ����� ������ � ������ 
	// Parameter: std::string sScriptName - ������������� �������
	// Parameter: bool bRun	- �������� ��� ��������  ��� ������������ true - ���������; false - ����������; 
	// Returns:   true - ���� ������ �������, ����� false 
	//************************************	
	bool Create(std::string sID, std::string sListenDSID, bool bListenState, std::string sScriptName, bool bRun = true, int nWaitOnSet = 5000, unsigned int updateInterval = MINDELAY);


	static WatchDogScriptControl* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//���������� � ������ ������������ ��������������� 
	//�� � �������� ���������
	virtual void OnListenStateSet();


protected:

	std::string  m_sScriptName;
	bool m_bRun;
	
};
#pragma endregion WatchDogDO

//����� ��� ������������� ��������� ����������� ��
//� ���������� ��������� ��� ��������� �������
class WatchDogSystemShutdown : public BaseWatchDog
{
public:

	//�����������
	WatchDogSystemShutdown ();	

	//����������
	virtual ~WatchDogSystemShutdown ();

	
	//�������� ��������� ������ �� XML ����
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

	
	static WatchDogSystemShutdown* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//���������� � ������ ������������ ��������������� 
	//�� � �������� ���������
	virtual void OnListenStateSet();
	
protected:

	bool m_bSystemShutDown;	
};

#endif