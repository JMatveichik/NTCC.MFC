#if !defined(_WATCH_DOGC_H_INCLUDED_)
#define _WATCH_DOGC_H_INCLUDED_

#include "TimedControl.h"
#include "DataSrc.h"
#include "..\OutMessage.h"

#pragma region WatchDog

//Базовый класс для прослушивания состояния дискретного 
//ИД с реакцией на его изменение

class BaseWatchDog : public TimedControl
{
public:

	//Конструктор
	BaseWatchDog();	

	//Деструктор
	virtual ~BaseWatchDog();

	//создание источника данных из XML узла
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

	//************************************
	// Parameter: std::string id - идентификатор 
	// Parameter: std::string dsID - идентификатор прослушиваемого ИД
	// Parameter: bool changeStateTo - прослушиваемое состояние  false - переключение 1->0; true  - переключение 0->1
	// Parameter: bool stopOnSet - флаг остановки при срабатывании
	// Parameter: unsigned int updateInterval - если задан интервал >= MINDELAY=50 ms запускаем при создании, иначе запуск в ручную 
	// Returns:   true - если создан успешно, иначе false 
	//************************************	
	bool Create(std::string sID, std::string sListenDSID, bool bListenState,  int nWaitOnSet = 5000, unsigned int updateInterval = MINDELAY);


	/////////////////////////////////////////////////////////////
	///Статические функции для работы с контейнером сторожевых объектов

	//Зарегестрировать  сторожевой объект
	static bool AddWatchDog(BaseWatchDog* pWD);

	//Получить  сторожевой объект
	static const BaseWatchDog* GetWatchDog(std::string id);

	//Удалить все сторожевые объекты
	static void ClearAll();
	

	std::string Name() const;


	//Добавление внешнего обработчика в случае переключения 
	//прослушиваемого ИД в заданное состояние
	void AddOnWatchDogSetHandler(boost::function< void (const DiscreteDataSrc*) > handler)
	{
		evOnWatchDogSet.connect(handler);
	}


	//запуск начала прослушивания
	virtual bool Start(unsigned int nInterval = BASEDELAY);



protected:	

	void ControlState();
	

	//Вызывается в случае переключения прослушиваемого 
	//ИД в заданное состояние
	virtual void OnListenStateSet() = 0;

	virtual void LogEvent(std::strstream& out);

	//все подключенные обаботчики изменения состояния
	boost::signals2::signal< void (const DiscreteDataSrc*) > evOnWatchDogSet;

	//прослушиваемый дискретный источник данных
	const DiscreteDataSrc* m_pListenDS;	
	
	//прослушиваемое состояние 
	//false - переключение 1->0
	//true  - переключение 0->1
	bool m_bListenState;

	//строковый идентификатор 
	std::string m_sID;

	//предыдущее состояние источника
	bool m_bPrevState;

	//ожидание при срабвтывании
	int m_nWaitOnSet;

	int m_nCounter;

	bool m_bStopOnSet;

	int m_nConfirmTime;

	//все созданные экземпляры класса 
	static std::map<std::string, BaseWatchDog*> watchDogs; 

	bool ConfirmStateChanges() const;

	//процедура прослушивания
	virtual void ControlProc();
};

#pragma endregion WatchDog



#pragma region WatchDogDO

//Класс для прослушивания состояния дискретного 
//ИД с переключением выходных дискретных ИД в заданное состояние
class WatchDogDO : public BaseWatchDog
{
public:

	//Конструктор
	WatchDogDO();	

	//Деструктор
	virtual ~WatchDogDO();

	
	//создание источника данных из XML узла
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

	//************************************
	// Parameter: std::string sID - идентификатор 
	// Parameter: std::string sListenDSID - идентификатор прослушиваемого ИД
	// Parameter: bool changeStateTo - прослушиваемое состояние  false - переключение 1->0; true  - переключение 0->1
	// Parameter: bool stopOnSet - флаг остановки при срабатывании
	// Parameter: unsigned int updateInterval - если задан интервал >= MINDELAY=50 ms запускаем при создании, иначе запуск в ручную 
	// Parameter: std::string dsIDOUT - идентификатор выходного ИД
	// Parameter: bool outState	- состояние выходного ИД, в которое он переключится при сробатывании объекта 
	// Returns:   true - если создан успешно, иначе false 
	//************************************	
	bool Create(std::string sID, std::string sListenDSID, bool bListenState, std::string sOutputDSID, bool bOutState, int nWaitOnSet = 5000, unsigned int updateInterval = MINDELAY);


	static WatchDogDO* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//Вызывается в случае переключения прослушиваемого 
	//ИД в заданное состояние
	virtual void OnListenStateSet();

	virtual void LogEvent(std::strstream& out);

protected:

// 	const DiscreteOutputDataSrc* m_pOutDS;
// 	bool m_bOutState;

	//выходные дискретные ИД и состояние в которые нужно переключить при срабатывании 
	std::vector< std::pair<DiscreteOutputDataSrc*, bool> > m_vDSOut;
	
};


//Класс для прослушивания состояния дискретного ИД
//с запуском/остановкой управляющего скрипта
class WatchDogScriptControl : public BaseWatchDog
{
public:

	//Конструктор
	WatchDogScriptControl();	

	//Деструктор
	virtual ~WatchDogScriptControl();

	
	//создание источника данных из XML узла
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

	//************************************
	// Parameter: std::string sID - идентификатор 
	// Parameter: std::string sListenDSID - идентификатор прослушиваемого ИД
	// Parameter: bool changeStateTo - прослушиваемое состояние  false - переключение 1->0; true  - переключение 0->1
	// Parameter: bool stopOnSet - флаг остановки при срабатывании
	// Parameter: unsigned int updateInterval - если задан интервал >= MINDELAY=100 ms запускаем при создании, иначе запуск в ручную 
	// Parameter: std::string sScriptName - идентификатор скрипта
	// Parameter: bool bRun	- действие над скриптом  при срабатывании true - запустить; false - остановить; 
	// Returns:   true - если создан успешно, иначе false 
	//************************************	
	bool Create(std::string sID, std::string sListenDSID, bool bListenState, std::string sScriptName, bool bRun = true, int nWaitOnSet = 5000, unsigned int updateInterval = MINDELAY);


	static WatchDogScriptControl* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//Вызывается в случае переключения прослушиваемого 
	//ИД в заданное состояние
	virtual void OnListenStateSet();


protected:

	std::string  m_sScriptName;
	bool m_bRun;
	
};
#pragma endregion WatchDogDO

//Класс для прослушивания состояния дискретного ИД
//с остановкой программы или остановки системы
class WatchDogSystemShutdown : public BaseWatchDog
{
public:

	//Конструктор
	WatchDogSystemShutdown ();	

	//Деструктор
	virtual ~WatchDogSystemShutdown ();

	
	//создание источника данных из XML узла
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

	
	static WatchDogSystemShutdown* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//Вызывается в случае переключения прослушиваемого 
	//ИД в заданное состояние
	virtual void OnListenStateSet();
	
protected:

	bool m_bSystemShutDown;	
};

#endif