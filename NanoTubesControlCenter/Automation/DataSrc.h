#if !defined(_DATA_SUORCE_H_INCLUDED_)
#define _DATA_SUORCE_H_INCLUDED_

#include "PhysDevice.h"
#include "Converters.h"
#include "Filters.h"
#include "MessageLog.h"
#include "common.h"


#define HIGH_STATE true
#define LOW_STATE  false

//////////////////////////////////////////////////////////////////////////
const long OBA_FN_CHECK			  = 0x0001;
const long OBA_FN_SWITCH		  = 0x0002;
const long OBA_FN_FLOW_GET		  = 0x0004;
const long OBA_FN_FLOW_SET		  = 0x0008;
const long OBA_FN_PRESSURE_GET	  = 0x0010;
const long OBA_FN_TEMPERATURE_GET = 0x0020;
const long OBA_FN_FRACTION_GET	  = 0x0040;
const long OBA_FN_CURRENT_SET	  = 0x0080;

//////////////////////////////////////////////////////////////////////////

const long OBA_FN_RESERVED_1	  = 0x0100;
const long OBA_FN_RESERVED_2	  = 0x0200;
const long OBA_FN_RESERVED_3	  = 0x0400;
const long OBA_FN_RESERVED_4	  = 0x0800;
const long OBA_FN_RESERVED_5      = 0x1000;

class AnalogInputDataSrc;
class AnalogOutputDataSrc;
class DiscreteInputDataSrc;
class DiscreteOutputDataSrc;


//структура для поиска источников данных
typedef struct tagDSEnumParams
{
	tagDSEnumParams() : mask(0), dstype(0), wire(-1) {}; 
	tagDSEnumParams(long enummask, long type, unsigned char wire, std::string devName) :
	mask(enummask), dstype(type), wire(wire), device(devName)
	{
	}

	long		  mask;
	long		  dstype;
	unsigned char wire;	
	std::string device;

} DATA_SOURCES_ENUM_PARAMS, *LPDATA_SOURCES_ENUM_PARAMS;

////////////////////////////////////////////////////////////
//константы для задания типа источника данных при поиске 
const long  DS_ALL = 0x0000;	//все ИД

const long 	DS_AI  = 0x0001;	//связаные с аналоговыми входами устройств
const long  DS_AO  = 0x0002;	//связаные с аналоговыми выходами устройств
const long 	DS_ANALOG	= DS_AI|DS_AO;	//все аналоговые

const long 	DS_DI  = 0x0004;	//связаные с дискретными входами устройств
const long 	DS_DO  = 0x0008;	//связаные с дискретными выходами устройств
const long 	DS_DISCRETE = DS_DI|DS_DO;	//все дискретные

const long 	DS_TIMERS = 0x0010;	//таймеры
const long  DS_COLLECTED = 0x0020; //

//маска полей структуры при поиске 
const long DSEM_TYPE = 0x0001;	//поле типа учитывается при поиске
const long DSEM_DEV  = 0x0002;	//поле имени устройства учитывается при поиске
const long DSEM_WIRE = 0x0004;	//поле номера канала учитывается при поиске
const long DSEM_ALL  = DSEM_TYPE|DSEM_DEV|DSEM_WIRE;	



/*********************************************************/
/*				Класс DataSrc      						 */
/*		базовый класс для реализации объектов типа		 */
/*		источник данных 								*/
/*********************************************************/
#pragma region DataSrc 

class DataSrc 
{

private:

	DataSrc(const DataSrc& rhs);
	DataSrc &operator =(const DataSrc &rhs);

public:
	
	DataSrc();
	virtual ~DataSrc();
	
	//создание источника данных из XML узла
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

	//сохранение в XML
	virtual bool SaveToXMLNode(IXMLDOMNode* pNode);

public:

	// Атрибуты 
	// Имя источника данных (для сохранения в файл)
	std::string Name()	const;

	// Описание источника данных (для информативности)
	std::string Description() const;

	// Цвет отображения на графиках 
	COLORREF Color() const;

	//virtual bool IsValid() const = 0;
	unsigned int Permission() const;

	//
	bool IsCollected() const;

public:
	//  [10/7/2015 Johnny A. Matveichik]  
	//Добавление обработчика изменения состояния источника данных
	void AddOnDataSourceChangeHandler(boost::function< void (const DataSrc*) > handler);	

protected:

	//проверка изменения состояния источника данных
	//virtual bool CheckChange() const = 0; 

public :
	
	//////////////////////////////////////////////////////////////////////////
	//Статические функции для работы со всеми созданными источниками данных

	//Общее количество созданных источников данных
	static int Count();

	//Регистрация источника данных 
	static bool AddDataSource(DataSrc* pDS);

	//Включить журанл изменений 
	static bool DataSrc::EnableLog(const char *pszHomePath, bool bClear);
	//Добавить сообщение в журнал

	static bool DataSrc::LogMessage(const char *pszMessage);

	//Получить зарегестрированный источник данных по имени  	
	static DataSrc* GetDataSource(std::string name);

	//Удаление всех зарегестрированных источников данных 
	static void ClearAll(); 

	//поиск необходимых источников данных по параметрам
	static int EnumDataSources(std::vector<const DataSrc*>& ds, LPDATA_SOURCES_ENUM_PARAMS pdsep);

	//поиск входных аналоговых источников данных присоедененных к каналу устройства
	static int FindAnalogInputDataSrc(IAnalogInput* pDev, unsigned char channel, std::vector<AnalogInputDataSrc*> &ds );

	//поиск выходных аналоговых источников данных присоедененных к каналу устройства
	static int FindAnalogOutputDataSrc(IAnalogOutput* pDev, unsigned char channel, std::vector<AnalogOutputDataSrc*> &ds );

	//поиск входных дискретных источников данных присоедененных к каналу устройства
	static int FindDigitalInputDataSrc(IDigitalInput* pDev, unsigned char channel, std::vector<DiscreteInputDataSrc*> &ds );

	//поиск входных дискретных источников данных присоедененных к каналу устройства
	static int FindDigitalOutputDataSrc(IDigitalOutput* pDev, unsigned char channel, std::vector<DiscreteOutputDataSrc*> &ds );
	
protected:	

	//обработчики события изменения состояния источника данных
	boost::signals2::signal< void (const DataSrc*) > evOnChange;

	//Уникальный идентификатор источника данных
	std::string	m_strName;

	// Описание источника данных 
	std::string	m_strDescription;

	// Цвет отображения на графике
	COLORREF	m_outColor;				

	unsigned int m_prior;  
	static bool bUseInitState;

	//флаг восстановления состояния из реестра при создании источника данных
	bool m_bRestored;

	//флаг коллекционирования данных
	bool m_bCollected; 

	//все созданные источники данных
	static std::map<std::string, DataSrc*> m_mapDS;
	static std::vector<DataSrc*> m_vecDS;

	// Критическая секция для синхронизации данных
	mutable CRITICAL_SECTION m_dataCS;	

	// Ведение журнала состояния источников данных
	static CMessageLog logDS;
};

#pragma endregion Класс DataSrc 



/*********************************************************/
/*				Класс AnalogDataSrc						 */
/*		базовый класс для реализации объектов типа		 */
/*		аналоговый источник данных 						 */
/*********************************************************/
#pragma region AnalogDataSrc

enum AnalogValueTendency { 
		AVT_UNKNOWN		 = 0,  //Не контролируется для данного источника
		AVT_GROWTH_HIGH  = 1,   //Высокая скорость наростания величины
		AVT_GROWTH_SLOW  = 2,   //Средняя скорость наростания 
		AVT_NO_GROWTH	 = 3,   //Установившееяся состояние
		AVT_REDUCE_SLOW  = 4,   //Средняя скорость убывания
		AVT_REDUCE_HIGH  = 5,	//Высокая скорость убывания величины		
	};


class AnalogDataSrc : public DataSrc
{

private:

	AnalogDataSrc(const AnalogDataSrc& rhs);
	AnalogDataSrc &operator =(const AnalogDataSrc &rhs);

public:

	AnalogDataSrc();	
	virtual ~AnalogDataSrc();

public:

	//создание источника данных из XML узла
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);
	
// 	virtual void CheckChange() const 
// 	{
// 		double valNew = GetValue();
// 		if ( fabs(m_lastValue - valNew) > m_minChange )
// 			evOnChange(this);			
// 	}

public:

	// Единицы измерениия  (для информативности)
	std::string Units()		  const;	
	
	// Формат вывода данных
	std::string OutFormat()	  const;	
	

	//Получить значение источника в единицах пользователя
	virtual double GetValue() const  = 0;  

	//Получить значение источника в единицах устройства
	virtual double GetSignal() const  = 0;  
	
	//Возвращает минимально возможное значение аналоговой величины в единицах пользователя
	virtual double GetMin() const;

	//Возвращает максимально возможное значение аналоговой величины в единицах пользователя
	virtual double GetMax() const;

	//Получить диапазон значений управляемой величины
	virtual void GetRange(double& minV, double& maxV) const;

protected:

	// Единицы измерения (для информативности)
	std::string	m_strUnits;

	// Формат вывода данных	
	std::string m_strOutFormat;			

	//Минимальное значение изменения величины для генерации события OnChange
	double m_minChange;

	// Последнее значение 
	mutable double	 m_lastValue;	
	
	//Набор конвертеров для прямого преобразования 
	std::vector<const BaseConverter*> m_vConverters;

	//Минимальное значение и максимальное значение
	double m_minValue;
	double m_maxValue;
};

#pragma endregion AnalogDataSrc

/*********************************************************/
/*				Класс AnalogTimeAverageDataSrc			 */
/*		класс для реализации объектов типа				 */
/*		аналоговый источник данных						 */
/*			с усреднением по времени					 */
/*********************************************************/
class AnalogTimeAverageDataSrc : public AnalogDataSrc
{
private:

	AnalogTimeAverageDataSrc(const AnalogTimeAverageDataSrc& rhs);
	AnalogTimeAverageDataSrc &operator =(const AnalogTimeAverageDataSrc &rhs);
	
public:

	AnalogTimeAverageDataSrc();	
	virtual ~AnalogTimeAverageDataSrc();

public:
	
	//создание источника данных из XML узла
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

	//создание источника данных из XML узла
	static AnalogTimeAverageDataSrc* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	
public:
	
	//Получить значение источника в единицах пользователя
	virtual double GetValue() const;  

	//Получить значение источника в единицах устройства
	virtual double GetSignal() const;  

protected:

	int timeWndSize;  //время усреднения
	std::string dsid; //имя источника данных для усреднения
};

#pragma endregion AnalogDataSrc

/*************************************************************/
/*		Класс AnalogInputDataSrc						     */
/* Источник данных получающий значение по одному из каналов  */
/* от устройства с несколькими аналоговыми входами			 */  
/*************************************************************/
#pragma region AnalogInputDataSrc

class AnalogInputDataSrc : public AnalogDataSrc
{
private:

	AnalogInputDataSrc(const AnalogInputDataSrc& rhs);
	AnalogInputDataSrc &operator =(const AnalogInputDataSrc &rhs);

public:

	AnalogInputDataSrc();
	virtual ~AnalogInputDataSrc();

	//создание источника данных из XML узла
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

	//создание источника данных из XML узла
	static AnalogInputDataSrc* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	double GetClearValue() const;

protected:

	// Устройство с аналоговыми входами
	IAnalogInput* pAnalogInputDevice;

	// Номер аналогового входа для чтения данных
	unsigned char		  channel;

		
public:

	//Получить значение с аналогового входа в единицах пользователя
	virtual double GetValue() const;


	//Получить значение с аналогового входа в единицах устройства
	virtual double GetSignal() const;
	
	//Получить диапазон значений управляемой величины
	//virtual void GetRange(double& minV, double& maxV) const;
	
	//Получить номер канала c которым связан источник 
	unsigned char GetChannelNo() const;

	//Получить устройство с которым связан источник  данных
	IAnalogInput* GetParentDevice() const;

private:

	const BaseFilter* pFilter;

};
#pragma endregion AnalogInputDataSrc

/************************************************************************/
/*		Класс AnalogOutputDataSrc										*/
/* Класс - управляемый источник данных	позволяющий получать и задавать */
/* текущее состояние контролируемой величины по одному из каналов		*/
/* от устройства с несколькими аналоговыми выходами						*/
/************************************************************************/
#pragma region AnalogOutputDataSrc
class AnalogOutputDataSrc : public AnalogDataSrc
{
private:

	AnalogOutputDataSrc(const AnalogOutputDataSrc& rhs);
	AnalogOutputDataSrc &operator =(const AnalogOutputDataSrc &rhs);

public:

	AnalogOutputDataSrc();
	virtual ~AnalogOutputDataSrc();

	//создание источника данных из XML узла
	static AnalogOutputDataSrc* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//создание источника данных из XML узла
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

	
public:

	//Получить текущее значение управляемой величины в единицах пользователя
	virtual double GetValue() const;

	//Получить текущее значение управляемой величины в единицах устройства	
	virtual double GetSignal() const;

	//Задать новое значение управляемой величины
	virtual double SetValue(double val) const;

	//Получить диапазон значений управляемой величины
	virtual void GetRange(double& minV, double& maxV) const;

	//Получить номер канала c которым связан источник 
	virtual unsigned char GetChannelNo() const ;

	//Получить устройство с которым связан источник  данных
	virtual IAnalogOutput* GetParentDevice() const;

	//  [5/15/2010 Johnny A. Matveichik]
	//Добавление обработчика 
	void AddOnOutputValueChange(boost::function< void (const AnalogOutputDataSrc*) > handler);

	bool AddUpdateWnd(HWND hWnd);

protected:

	bool saveState() const;
	void restoreState();

	// Устройство с аналоговыми выходами
	IAnalogOutput* pAnalogOutputDevice;	 
	
	// Номер аналогового выхода для записи данных
	unsigned char		   channel;				

	//Набор конвертеров для обратного преобразования 
	std::vector<const BaseConverter*> m_vRevConverters;

	//обработка события изменения выходной величины
	boost::signals2::signal< void (const AnalogOutputDataSrc*) > evOnValueChange;

	//описатели окон для обновления при изменении величины
	std::vector<HWND> updateWnds;

	//значение записываемое при уничтожении объекта
	double dSafeState;

	//нужно ли записывать безопасное значение при уничтожении объекта
	bool useSafe;
};

#pragma endregion AnalogOutputDataSrc



#pragma region    AnalogOutputListDataSrc

typedef struct  tagAnalogValueInfo
{
	double Value;
	std::string Info;
}ANALOG_VALUE_INFO;

struct double_cmp {

	double_cmp(double v, unsigned short p) : val(v), presition(p) 
	{ }

	inline bool operator()(const ANALOG_VALUE_INFO& x) const 
	{
		int v1 = (int)(val*pow(10.0, presition));
		int v2 = (int)(x.Value*pow(10.0, presition));

		return v1 == v2;
	}
private:
	double val;
	unsigned short presition;
};

struct double_in_range {

	double_in_range(double v) : val(v) 
	{ }

	inline bool operator()(const std::pair< std::pair<double, double>, Gdiplus::Color>& x) const 
	{
		std::pair<double, double> rng = x.first;
		if (val >= rng.first && val < rng.second)
			return true; 
		return false;
	}
private:
	double val;	
};	

bool analog_value_comparer (ANALOG_VALUE_INFO v1, ANALOG_VALUE_INFO v2);

class AnalogOutputListDataSrc : public AnalogOutputDataSrc
{
private:

	AnalogOutputListDataSrc(const AnalogOutputListDataSrc& rhs);
	AnalogOutputListDataSrc &operator =(const AnalogOutputListDataSrc &rhs);

public:

	AnalogOutputListDataSrc();
	virtual ~AnalogOutputListDataSrc();

	//создание источника данных из XML узла
	static AnalogOutputListDataSrc* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//создание источника данных из XML узла
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);


public:
	
	


	//Задать новое значение управляемой величины
	virtual double SetValue(double val) const;

	const std::vector<ANALOG_VALUE_INFO>& GetValues() const;
		
protected:

	

	mutable std::vector<ANALOG_VALUE_INFO> presetValues;

	//число знаков после запятой
	unsigned short valuesPresition;
	
};

#pragma endregion AnalogOutputListDataSrc


//////////////////////////////////////////////////////////////////////////
//Базовый класс  для составных источников данных
//Составные источники данных включают 
class IComposite
{
public :

	//деструктор
	virtual  ~IComposite();

	//количество подисточников данных
	virtual int Size() const = 0;

	//получить подисточник данных 
	virtual const DataSrc* GetSubDataSrc(int ind) const = 0;	

};

//  [3/25/2010 Johnny A. Matveichik]
class CompositeAnalogDataSrc : public AnalogDataSrc , public IComposite
{

private:

	CompositeAnalogDataSrc(const CompositeAnalogDataSrc& rhs);
	CompositeAnalogDataSrc &operator =(const CompositeAnalogDataSrc &rhs);


public:

	//конструктор
	CompositeAnalogDataSrc();

	//деструктор
	virtual ~CompositeAnalogDataSrc();

	//создание источника данных из XML узла
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

public:
	
	//количество подисточников данных
	virtual int Size() const;

	//получить подисточник данных 
	virtual const DataSrc* GetSubDataSrc(int ind) const;

	//Получить текущее значение величины в единицах устройства	
	virtual double GetSignal() const;

protected :
	
	// источники входящие в состав CompositeDataSrc
	std::vector <const DataSrc* > vSubDS;
	
};
//  [3/25/2010 Johnny A. Matveichik]


/************************************************************************/
/*		Класс CalcSummDataSrc											*/
/*	Класс - источник данных	вычисляющий сумму значений от				*/
/*	нескольких источников												*/
/************************************************************************/
#pragma region CalcSummDataSrc

class CalcSummDataSrc : public CompositeAnalogDataSrc
{

private:

	CalcSummDataSrc(const CalcSummDataSrc& rhs);
	CalcSummDataSrc &operator =(const CalcSummDataSrc &rhs);

public :
	//конструктор
	CalcSummDataSrc();

	//деструктор
	virtual ~CalcSummDataSrc();

	static CalcSummDataSrc* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//вычислить сумму от источников данных
	virtual double GetValue() const;
	
};

#pragma endregion CalcSummDataSrc

/************************************************************************/
/*		Класс CalcAverageDataSrc										*/
/*  Класс - Источник данных вычисляющий среднее значение				*/
/*	от нескольких источников											*/
/************************************************************************/
#pragma region CalcAverageDataSrc 

class  CalcAverageDataSrc : public CompositeAnalogDataSrc
{
private:

	CalcAverageDataSrc(const CalcAverageDataSrc& rhs);
	CalcAverageDataSrc &operator =(const CalcAverageDataSrc &rhs);

public :

	//конструктор
	CalcAverageDataSrc();

	//деструктор
	virtual ~CalcAverageDataSrc();

	static CalcAverageDataSrc* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//Вычислить среднее значение от источников данных
	virtual double GetValue() const;
	
};

#pragma endregion CalcAverageDataSrc 

/********************************************************************/
/*		Класс CalcMulDataSrc										*/
/*  Класс - Источник данных вычисляющий значение от перемножения	*/
/*	от нескольких источников										*/
/********************************************************************/
#pragma region CalcMulDataSrc 

class CalcMulDataSrc : public CompositeAnalogDataSrc
{
private:

	CalcMulDataSrc(const CalcMulDataSrc& rhs);
	CalcMulDataSrc &operator =(const CalcMulDataSrc &rhs);

public:
	//конструктор
	CalcMulDataSrc();

	//деструктор
	virtual ~CalcMulDataSrc();

	static CalcMulDataSrc* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//Вычислить значение от источников данных
	virtual double GetValue() const ;

};

#pragma endregion CalcMulDataSrc 

/********************************************************************/
/*		Класс CalcMinDataSrc										*/
/*  Класс - Источник данных вычисляющий минимальное значение        */
/*	от нескольких источников										*/
/********************************************************************/
#pragma region CalcMinDataSrc 

class CalcMinDataSrc : public CompositeAnalogDataSrc
{
private:

	CalcMinDataSrc(const CalcMinDataSrc& rhs);
	CalcMinDataSrc &operator =(const CalcMinDataSrc &rhs);

public:

	//конструктор
	CalcMinDataSrc();

	//деструктор
	virtual ~CalcMinDataSrc();

	static CalcMinDataSrc* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//Вычислить значение от источников данных
	virtual double GetValue() const ;	
	
};
#pragma endregion CalcMinDataSrc 

/********************************************************************/
/*		Класс CalcMaxDataSrc										*/
/*  Класс - Источник данных вычисляющий минимальное значение        */
/*	от нескольких источников										*/
/********************************************************************/
#pragma region CalcMaxDataSrc 

class CalcMaxDataSrc : public CompositeAnalogDataSrc
{

private:

	CalcMaxDataSrc(const CalcMaxDataSrc& rhs);
	CalcMaxDataSrc &operator =(const CalcMaxDataSrc &rhs);

public:
	
	//конструктор
	CalcMaxDataSrc();

	//деструктор
	virtual ~CalcMaxDataSrc();

	static CalcMaxDataSrc* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//Вычислить значение от источников данных
	virtual double GetValue() const ;

	
};
#pragma endregion CalcMaxDataSrc 


/************************************************************************/
/*		Класс CalcSummDataSrc											*/
/*	Класс - источник данных	вычисляющий сумму значений от				*/
/*	нескольких источников												*/
/************************************************************************/
#pragma region CalcDataSrc

class CalcDataSrc : public CompositeAnalogDataSrc
{
private:

	CalcDataSrc(const CalcDataSrc& rhs);
	CalcDataSrc &operator =(const CalcDataSrc &rhs);

public :

	//конструктор
	CalcDataSrc();

	//деструктор
	virtual ~CalcDataSrc();

	//создание источника данных из XML узла
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

	//создание источника данных из XML узла
	static CalcDataSrc* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//вычислить значение на основании значений источников данных
	virtual double GetValue() const;
	
private :

	mutable vector< double > vars;
	mu::Parser parser;
};

#pragma endregion CalcDataSrc

/************************************************************************/
/*		Класс DiscreteDataSrc											*/
/*  Класс - Дикретный источник данных - отбражающий состояние			*/
/*	включен/выключен													*/
/************************************************************************/
#pragma region DiscreteDataSrc
class DiscreteDataSrc : public DataSrc
{
private:

	DiscreteDataSrc(const DiscreteDataSrc& rhs);
	DiscreteDataSrc &operator =(const DiscreteDataSrc &rhs);

public:

	DiscreteDataSrc();
	virtual ~DiscreteDataSrc();

	//создание источника данных из XML узла
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

public:

	void AddOnChangeState(boost::function< void (const DiscreteDataSrc*) > handler )
	{
		evOnChangeState.connect(handler);
	}

	std::string ToString() const
	{
		return enabled ? strHighState : strLowState;
	}

	std::string ToString(bool st) const
	{
		return st ? strHighState : strLowState;
	}

	//Проверка состояния сигнала
	virtual bool IsEnabled() const = 0; 

protected:

	boost::signals2::signal< void (const DiscreteDataSrc*) > evOnChangeState;
	
	bool		 bInvert;	//	инвертировать реальное значение 
	mutable bool enabled;	//	последнее значение сигнала

	std::string strLowState;
	std::string strHighState;
};
#pragma endregion DiscreteDataSrc


/************************************************************************/
/*		Класс DiscreteInputDataSrc										*/
/* Источник данных получающий значение по одному из каналов				*/
/* от устройства с несколькими дискретными входами						*/
/************************************************************************/
#pragma region DiscreteInputDataSrc
class DiscreteInputDataSrc : public DiscreteDataSrc
{
private:

	DiscreteInputDataSrc(const DiscreteInputDataSrc& rhs);
	DiscreteInputDataSrc &operator =(const DiscreteInputDataSrc &rhs);

public:

	DiscreteInputDataSrc();
	virtual ~DiscreteInputDataSrc();

	//создание источника данных из XML узла
	static DiscreteInputDataSrc* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//создание источника данных из XML узла
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

public:

	//Проверка состояния сигнала
	virtual bool IsEnabled() const;

	//Получить номер канала c которым связан источник 
	unsigned char GetChannelNo() const;

	//Получить устройство с которым связан источник  данных
	IDigitalInput* GetParentDevice() const;

	//  [5/15/2010 Johnny A. Matveichik]

protected:

	IDigitalInput* pDigitalInputDevice;
	unsigned char channel;
};

#pragma endregion DiscreteInputDataSrc



/************************************************************************/
/*		Класс DiscreteOutputDataSrc										*/
/* Класс - управляемый источник данных	позволяющий получать и задавать */
/* текущее состояние контролируемой величины по одному из каналов		*/
/* от устройства с несколькими дискретными выходами						*/
/************************************************************************/
#pragma region DiscreteOutputDataSrc
class DiscreteOutputDataSrc : public DiscreteDataSrc
{
private:

	DiscreteOutputDataSrc(const DiscreteOutputDataSrc& rhs);
	DiscreteOutputDataSrc &operator =(const DiscreteOutputDataSrc &rhs);

public:

	DiscreteOutputDataSrc();
	virtual ~DiscreteOutputDataSrc();

	//создание источника данных из XML узла
	static DiscreteOutputDataSrc* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//создание источника данных из XML узла
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);


public:

	//Установка состояния сигнала
	virtual bool Enable(bool enable = true);

	//Проверка состояния сигнала
	virtual bool IsEnabled() const;

	//Переключить состояние
	virtual bool ToggleState();
	
	//возможность изменения состояния
	virtual bool CanChangeStateTo(bool state) const;

	//Получить номер канала c которым связан источник 
	unsigned char GetChannelNo() const;
	
	//Получить устройство с которым связан источник  данных
	IDigitalOutput* GetParentDevice() const;

	//  [5/15/2010 Johnny A. Matveichik]

protected:

	bool saveState() const;
	void restoreState();

	IDigitalOutput* pDigitalOutputDevice;
	unsigned char channel;	

	bool bSafeState;

	///нужно ли записывать безопасное значение при уничтожении объекта
	bool useSafe;
};

#pragma endregion DiscreteOutputDataSrc


/************************************************************************/
/*		Класс ControledDiscreteOutputDataSrc							*/
/* Класс - управляемый дискретный источник данных						*/
/* позволяющий получать и задавать текущее состояние контролируемой		*/
/* величины по одному из каналов										*/
/* от устройства с несколькими дискретными выходами						*/
/* с проверкой возможности изменения текущего состояния					*/
/************************************************************************/
#pragma region ControledDiscreteOutputDataSrc

class ControledDiscreteOutputDataSrc : public DiscreteOutputDataSrc, public IComposite
{
private:

	ControledDiscreteOutputDataSrc(const ControledDiscreteOutputDataSrc& rhs);
	ControledDiscreteOutputDataSrc &operator =(const ControledDiscreteOutputDataSrc &rhs);

	
public:

	ControledDiscreteOutputDataSrc();
	virtual ~ControledDiscreteOutputDataSrc();

	//создание источника данных из XML узла
	static ControledDiscreteOutputDataSrc* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//создание источника данных из XML узла
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

public:

	virtual bool CanChangeStateTo(bool newState) const;

	enum tagDSInd
	{
		SUB_DS_IND_TOLOWSTATE = 0,
		SUB_DS_IND_TOHIGHSTATE = 1	
	};

	//////////////////////////////////////////////////////////////////////////
	//интерфейс IComposite

	//количество подисточников данных
	virtual int Size() const;

	//получить подисточник данных 
	virtual const DataSrc* GetSubDataSrc(int ind) const;

protected:
	
	std::pair<std::string, bool> extractDSControl(std::string str);

private:

	typedef std::pair<const DiscreteDataSrc*, bool> DSCONTROLSTATE;

	bool controlCanChange;
	std::vector <const DiscreteDataSrc* > vSubDS;
	std::vector <DSCONTROLSTATE> vDSCS;
};

#pragma endregion ControledDiscreteOutputDataSrc


#pragma region CompositeDiscreteDataSrc

class CompositeDiscreteDataSrc : public DiscreteDataSrc, public IComposite
{
private:

	CompositeDiscreteDataSrc(const CompositeDiscreteDataSrc& rhs);
	CompositeDiscreteDataSrc &operator =(const CompositeDiscreteDataSrc &rhs);

public:
	
	CompositeDiscreteDataSrc();
	virtual ~CompositeDiscreteDataSrc();

	//создание источника данных из XML узла
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);
	
	
public:
	//////////////////////////////////////////////////////////////////////////
	// интерфейс IComposite

	//количество подисточников данных
	virtual int Size() const;

	//получить подисточник данных 
	virtual const DataSrc* GetSubDataSrc(int ind) const;

protected:

	// источники входящие в состав CompositeDataSrc
	std::vector <const DiscreteDataSrc* > vSubDS;
	
};

#pragma endregion CompositeDiscreteDataSrc




/*************************************************************************/
/*		Класс DiscreteANDDataSrc										 */
/* Класс - дискретный источник данных	позволяющий получать сотояние	 */
/* от нескольких дискретных источников данных с логической операцией И   */
/*************************************************************************/
#pragma region DiscreteANDDataSrc

class DiscreteANDDataSrc : public CompositeDiscreteDataSrc
{
private:

	DiscreteANDDataSrc(const DiscreteANDDataSrc& rhs);
	DiscreteANDDataSrc &operator =(const DiscreteANDDataSrc &rhs);

public:
	
	DiscreteANDDataSrc();
	virtual ~DiscreteANDDataSrc();

	//создание источника данных из XML узла
	static DiscreteANDDataSrc* CreateFromXML(IXMLDOMNode* pNode, UINT& error);
	
	virtual bool IsEnabled() const;

};

#pragma endregion DiscreteANDDataSrc

/*************************************************************************/
/*		Класс DiscreteORDataSrc										     */
/* Класс - дискретный источник данных	позволяющий получать сотояние	 */
/* от нескольких дискретных источников данных с логической операцией ИЛИ */
/*************************************************************************/
#pragma region DiscreteORDataSrc
class DiscreteORDataSrc : public CompositeDiscreteDataSrc
{
private:

	DiscreteORDataSrc(const DiscreteORDataSrc& rhs);
	DiscreteORDataSrc &operator =(const DiscreteORDataSrc &rhs);

public:
	
	DiscreteORDataSrc();
	virtual ~DiscreteORDataSrc();

	//создание источника данных из XML узла
	static DiscreteORDataSrc* CreateFromXML(IXMLDOMNode* pNode, UINT& error);
	
	virtual bool IsEnabled() const;
};

#pragma endregion DiscreteORDataSrc

/************************************************************************/
/*		Класс AnalogLevelAsDiscreteDataSrc								*/
/* Класс - дискретный источник данных позволяющий устанавливающийся		*/
/* в состояние включено при привышении	присоединенным аналоговым		*/
/* источником данных определенного уровня								*/
/************************************************************************/
#pragma region AnalogLevelAsDiscreteDataSrc
class AnalogLevelAsDiscreteDataSrc : public DiscreteDataSrc, public IComposite 
{
private:

	AnalogLevelAsDiscreteDataSrc(const AnalogLevelAsDiscreteDataSrc& rhs);
	AnalogLevelAsDiscreteDataSrc &operator =(const AnalogLevelAsDiscreteDataSrc &rhs);

public:
	
	AnalogLevelAsDiscreteDataSrc();	

	virtual ~AnalogLevelAsDiscreteDataSrc();	 

	//создание источника данных из XML узла
	static AnalogLevelAsDiscreteDataSrc* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//создание источника данных из XML узла
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

		
public:

	virtual  bool IsEnabled() const;
	
	//Получить контролируемый уровень сигнала
	double GetLevel() const;

	//Задать  контролируемый уровень сигнала
	void SetLevel(double level);

	//////////////////////////////////////////////////////////////////////////
	// интерфейс IComposite
	//количество подисточников данных
	virtual int Size() const;

	//получить подисточник данных 
	virtual const DataSrc* GetSubDataSrc(int ind) const;
		

protected:

	const AnalogDataSrc* pControledADS;	//контролируемый аналоговый источник данных
	const AnalogDataSrc* pBaseADS;		//опорный аналоговый источник данных для получения уровня

	double lvl;							//заданный уровень сигнала
	bool   bForGrowth;

};

#pragma endregion AnalogLevelAsDiscreteDataSrc

/************************************************************************/
/*		Класс AnalogRangeAsDiscreteDataSrc								*/
/* Класс - дискретный источник данных позволяющий устанавливающийся		*/
/* в состояние включено при выходе	значения присоединенного аналогового*/
/* источника данных за границы дипазона 								*/
/************************************************************************/
#pragma region AnalogRangeAsDiscreteDataSrc

class AnalogRangeAsDiscreteDataSrc : public DiscreteDataSrc, public IComposite 
{
private:

	AnalogRangeAsDiscreteDataSrc(const AnalogRangeAsDiscreteDataSrc  &rhs);
	AnalogRangeAsDiscreteDataSrc &operator =(const AnalogRangeAsDiscreteDataSrc &rhs);

public:

	AnalogRangeAsDiscreteDataSrc();
	virtual ~AnalogRangeAsDiscreteDataSrc();

	//создание источника данных из XML узла
	static AnalogRangeAsDiscreteDataSrc* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//создание источника данных из XML узла
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

public:

	virtual  bool IsEnabled() const;
	
	//Получить контролируемы диапазон сигнала
	void GetRange(double& dLow, double& dHigh);

	//////////////////////////////////////////////////////////////////////////
	// интерфейс IComposite
	//количество подисточников данных
	virtual int Size() const;

	//получить подисточник данных 
	virtual const DataSrc* GetSubDataSrc(int ind) const;

protected:

	enum tagDSINDEX { DSBASE = 0, DSLOW = 1, DSHIGH = 2 };

	std::vector<const AnalogDataSrc*> vSubBuf;

	mutable double lvlLow;	
	mutable double lvlUp;

	bool bInRange;

private:
	void UpdateRange() const;
};

#pragma endregion AnalogRangeAsDiscreteDataSrc


/************************************************************************/
/*		Класс DeviceStateDataSrc										*/
/* Источник данных информирующий о рабочем состоянии устройства			*/
/************************************************************************/

#pragma region DeviceStateDataSrc
class DeviceStateDataSrc : public DiscreteDataSrc
{
private:

	DeviceStateDataSrc(const DeviceStateDataSrc& rhs);
	DeviceStateDataSrc &operator =(const DeviceStateDataSrc &rhs);

public:

	DeviceStateDataSrc();
	virtual ~DeviceStateDataSrc();

	//создание источника данных из XML узла
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

	//создание источника данных из XML узла
	static DeviceStateDataSrc* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//Проверка состояния сигнала
	virtual bool IsEnabled() const;

	//Получить устройство с которым связан источник  данных
	//  [9/25/2011 Johnny A. Matveichik]
	IdentifiedPhysDevice* GetParentDevice() const;
	
protected:

	IdentifiedPhysDevice* pDevice;	
};

#pragma endregion DeviceStateDataSrc



class AnalogToBitsetDataSrc : public AnalogOutputDataSrc, public IComposite
{
private:

	AnalogToBitsetDataSrc(const AnalogToBitsetDataSrc& rhs);
	AnalogToBitsetDataSrc &operator =(const AnalogToBitsetDataSrc &rhs);

public:

	AnalogToBitsetDataSrc();
	virtual ~AnalogToBitsetDataSrc();

	//создание источника данных из XML узла
	static AnalogToBitsetDataSrc* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//создание источника данных из XML узла
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);


public:

	//Получить текущее значение управляемой величины в единицах пользователя
	virtual double GetValue() const;

	//Получить текущее значение управляемой величины в единицах устройства	
	virtual double GetSignal() const;

	//Задать новое значение управляемой величины
	virtual double SetValue(double val) const;

	//Получить диапазон значений управляемой величины
	virtual void GetRange(double& minV, double& maxV) const;

	//Получить номер канала c которым связан источник 
	virtual unsigned char GetChannelNo() const;

	//Получить устройство с которым связан источник  данных
	virtual IAnalogOutput* GetParentDevice() const;
	
	//количество подисточников данных
	virtual int Size() const;

	//получить подисточник данных 
	const DataSrc* GetSubDataSrc(int ind) const;

protected:

	void OnDiscreteDSChanged(const DiscreteDataSrc* pDDS);

// 	bool saveState() const;
// 	void restoreState();

	mutable double lastWriten;
	std::vector<DiscreteOutputDataSrc*> bits;
};


class StringDataSrc : public DataSrc
{
private:

	StringDataSrc(const StringDataSrc& rhs);
	StringDataSrc &operator =(const StringDataSrc &rhs);

public:

	StringDataSrc();
	virtual ~StringDataSrc();

	//создание источника данных из XML узла
	static StringDataSrc* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//создание источника данных из XML узла
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);
	

public:

	std::string GetString() const;

	void SetString(std::string text);

private:

	std::string str;
};

#endif

