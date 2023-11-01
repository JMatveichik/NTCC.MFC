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


//��������� ��� ������ ���������� ������
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
//��������� ��� ������� ���� ��������� ������ ��� ������ 
const long  DS_ALL = 0x0000;	//��� ��

const long 	DS_AI  = 0x0001;	//�������� � ����������� ������� ���������
const long  DS_AO  = 0x0002;	//�������� � ����������� �������� ���������
const long 	DS_ANALOG	= DS_AI|DS_AO;	//��� ����������

const long 	DS_DI  = 0x0004;	//�������� � ����������� ������� ���������
const long 	DS_DO  = 0x0008;	//�������� � ����������� �������� ���������
const long 	DS_DISCRETE = DS_DI|DS_DO;	//��� ����������

const long 	DS_TIMERS = 0x0010;	//�������
const long  DS_COLLECTED = 0x0020; //

//����� ����� ��������� ��� ������ 
const long DSEM_TYPE = 0x0001;	//���� ���� ����������� ��� ������
const long DSEM_DEV  = 0x0002;	//���� ����� ���������� ����������� ��� ������
const long DSEM_WIRE = 0x0004;	//���� ������ ������ ����������� ��� ������
const long DSEM_ALL  = DSEM_TYPE|DSEM_DEV|DSEM_WIRE;	



/*********************************************************/
/*				����� DataSrc      						 */
/*		������� ����� ��� ���������� �������� ����		 */
/*		�������� ������ 								*/
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
	
	//�������� ��������� ������ �� XML ����
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

	//���������� � XML
	virtual bool SaveToXMLNode(IXMLDOMNode* pNode);

public:

	// �������� 
	// ��� ��������� ������ (��� ���������� � ����)
	std::string Name()	const;

	// �������� ��������� ������ (��� ���������������)
	std::string Description() const;

	// ���� ����������� �� �������� 
	COLORREF Color() const;

	//virtual bool IsValid() const = 0;
	unsigned int Permission() const;

	//
	bool IsCollected() const;

public:
	//  [10/7/2015 Johnny A. Matveichik]  
	//���������� ����������� ��������� ��������� ��������� ������
	void AddOnDataSourceChangeHandler(boost::function< void (const DataSrc*) > handler);	

protected:

	//�������� ��������� ��������� ��������� ������
	//virtual bool CheckChange() const = 0; 

public :
	
	//////////////////////////////////////////////////////////////////////////
	//����������� ������� ��� ������ �� ����� ���������� ����������� ������

	//����� ���������� ��������� ���������� ������
	static int Count();

	//����������� ��������� ������ 
	static bool AddDataSource(DataSrc* pDS);

	//�������� ������ ��������� 
	static bool DataSrc::EnableLog(const char *pszHomePath, bool bClear);
	//�������� ��������� � ������

	static bool DataSrc::LogMessage(const char *pszMessage);

	//�������� ������������������ �������� ������ �� �����  	
	static DataSrc* GetDataSource(std::string name);

	//�������� ���� ������������������ ���������� ������ 
	static void ClearAll(); 

	//����� ����������� ���������� ������ �� ����������
	static int EnumDataSources(std::vector<const DataSrc*>& ds, LPDATA_SOURCES_ENUM_PARAMS pdsep);

	//����� ������� ���������� ���������� ������ �������������� � ������ ����������
	static int FindAnalogInputDataSrc(IAnalogInput* pDev, unsigned char channel, std::vector<AnalogInputDataSrc*> &ds );

	//����� �������� ���������� ���������� ������ �������������� � ������ ����������
	static int FindAnalogOutputDataSrc(IAnalogOutput* pDev, unsigned char channel, std::vector<AnalogOutputDataSrc*> &ds );

	//����� ������� ���������� ���������� ������ �������������� � ������ ����������
	static int FindDigitalInputDataSrc(IDigitalInput* pDev, unsigned char channel, std::vector<DiscreteInputDataSrc*> &ds );

	//����� ������� ���������� ���������� ������ �������������� � ������ ����������
	static int FindDigitalOutputDataSrc(IDigitalOutput* pDev, unsigned char channel, std::vector<DiscreteOutputDataSrc*> &ds );
	
protected:	

	//����������� ������� ��������� ��������� ��������� ������
	boost::signals2::signal< void (const DataSrc*) > evOnChange;

	//���������� ������������� ��������� ������
	std::string	m_strName;

	// �������� ��������� ������ 
	std::string	m_strDescription;

	// ���� ����������� �� �������
	COLORREF	m_outColor;				

	unsigned int m_prior;  
	static bool bUseInitState;

	//���� �������������� ��������� �� ������� ��� �������� ��������� ������
	bool m_bRestored;

	//���� ������������������ ������
	bool m_bCollected; 

	//��� ��������� ��������� ������
	static std::map<std::string, DataSrc*> m_mapDS;
	static std::vector<DataSrc*> m_vecDS;

	// ����������� ������ ��� ������������� ������
	mutable CRITICAL_SECTION m_dataCS;	

	// ������� ������� ��������� ���������� ������
	static CMessageLog logDS;
};

#pragma endregion ����� DataSrc 



/*********************************************************/
/*				����� AnalogDataSrc						 */
/*		������� ����� ��� ���������� �������� ����		 */
/*		���������� �������� ������ 						 */
/*********************************************************/
#pragma region AnalogDataSrc

enum AnalogValueTendency { 
		AVT_UNKNOWN		 = 0,  //�� �������������� ��� ������� ���������
		AVT_GROWTH_HIGH  = 1,   //������� �������� ���������� ��������
		AVT_GROWTH_SLOW  = 2,   //������� �������� ���������� 
		AVT_NO_GROWTH	 = 3,   //��������������� ���������
		AVT_REDUCE_SLOW  = 4,   //������� �������� ��������
		AVT_REDUCE_HIGH  = 5,	//������� �������� �������� ��������		
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

	//�������� ��������� ������ �� XML ����
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);
	
// 	virtual void CheckChange() const 
// 	{
// 		double valNew = GetValue();
// 		if ( fabs(m_lastValue - valNew) > m_minChange )
// 			evOnChange(this);			
// 	}

public:

	// ������� ����������  (��� ���������������)
	std::string Units()		  const;	
	
	// ������ ������ ������
	std::string OutFormat()	  const;	
	

	//�������� �������� ��������� � �������� ������������
	virtual double GetValue() const  = 0;  

	//�������� �������� ��������� � �������� ����������
	virtual double GetSignal() const  = 0;  
	
	//���������� ���������� ��������� �������� ���������� �������� � �������� ������������
	virtual double GetMin() const;

	//���������� ����������� ��������� �������� ���������� �������� � �������� ������������
	virtual double GetMax() const;

	//�������� �������� �������� ����������� ��������
	virtual void GetRange(double& minV, double& maxV) const;

protected:

	// ������� ��������� (��� ���������������)
	std::string	m_strUnits;

	// ������ ������ ������	
	std::string m_strOutFormat;			

	//����������� �������� ��������� �������� ��� ��������� ������� OnChange
	double m_minChange;

	// ��������� �������� 
	mutable double	 m_lastValue;	
	
	//����� ����������� ��� ������� �������������� 
	std::vector<const BaseConverter*> m_vConverters;

	//����������� �������� � ������������ ��������
	double m_minValue;
	double m_maxValue;
};

#pragma endregion AnalogDataSrc

/*********************************************************/
/*				����� AnalogTimeAverageDataSrc			 */
/*		����� ��� ���������� �������� ����				 */
/*		���������� �������� ������						 */
/*			� ����������� �� �������					 */
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
	
	//�������� ��������� ������ �� XML ����
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

	//�������� ��������� ������ �� XML ����
	static AnalogTimeAverageDataSrc* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	
public:
	
	//�������� �������� ��������� � �������� ������������
	virtual double GetValue() const;  

	//�������� �������� ��������� � �������� ����������
	virtual double GetSignal() const;  

protected:

	int timeWndSize;  //����� ����������
	std::string dsid; //��� ��������� ������ ��� ����������
};

#pragma endregion AnalogDataSrc

/*************************************************************/
/*		����� AnalogInputDataSrc						     */
/* �������� ������ ���������� �������� �� ������ �� �������  */
/* �� ���������� � ����������� ����������� �������			 */  
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

	//�������� ��������� ������ �� XML ����
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

	//�������� ��������� ������ �� XML ����
	static AnalogInputDataSrc* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	double GetClearValue() const;

protected:

	// ���������� � ����������� �������
	IAnalogInput* pAnalogInputDevice;

	// ����� ����������� ����� ��� ������ ������
	unsigned char		  channel;

		
public:

	//�������� �������� � ����������� ����� � �������� ������������
	virtual double GetValue() const;


	//�������� �������� � ����������� ����� � �������� ����������
	virtual double GetSignal() const;
	
	//�������� �������� �������� ����������� ��������
	//virtual void GetRange(double& minV, double& maxV) const;
	
	//�������� ����� ������ c ������� ������ �������� 
	unsigned char GetChannelNo() const;

	//�������� ���������� � ������� ������ ��������  ������
	IAnalogInput* GetParentDevice() const;

private:

	const BaseFilter* pFilter;

};
#pragma endregion AnalogInputDataSrc

/************************************************************************/
/*		����� AnalogOutputDataSrc										*/
/* ����� - ����������� �������� ������	����������� �������� � �������� */
/* ������� ��������� �������������� �������� �� ������ �� �������		*/
/* �� ���������� � ����������� ����������� ��������						*/
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

	//�������� ��������� ������ �� XML ����
	static AnalogOutputDataSrc* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//�������� ��������� ������ �� XML ����
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

	
public:

	//�������� ������� �������� ����������� �������� � �������� ������������
	virtual double GetValue() const;

	//�������� ������� �������� ����������� �������� � �������� ����������	
	virtual double GetSignal() const;

	//������ ����� �������� ����������� ��������
	virtual double SetValue(double val) const;

	//�������� �������� �������� ����������� ��������
	virtual void GetRange(double& minV, double& maxV) const;

	//�������� ����� ������ c ������� ������ �������� 
	virtual unsigned char GetChannelNo() const ;

	//�������� ���������� � ������� ������ ��������  ������
	virtual IAnalogOutput* GetParentDevice() const;

	//  [5/15/2010 Johnny A. Matveichik]
	//���������� ����������� 
	void AddOnOutputValueChange(boost::function< void (const AnalogOutputDataSrc*) > handler);

	bool AddUpdateWnd(HWND hWnd);

protected:

	bool saveState() const;
	void restoreState();

	// ���������� � ����������� ��������
	IAnalogOutput* pAnalogOutputDevice;	 
	
	// ����� ����������� ������ ��� ������ ������
	unsigned char		   channel;				

	//����� ����������� ��� ��������� �������������� 
	std::vector<const BaseConverter*> m_vRevConverters;

	//��������� ������� ��������� �������� ��������
	boost::signals2::signal< void (const AnalogOutputDataSrc*) > evOnValueChange;

	//��������� ���� ��� ���������� ��� ��������� ��������
	std::vector<HWND> updateWnds;

	//�������� ������������ ��� ����������� �������
	double dSafeState;

	//����� �� ���������� ���������� �������� ��� ����������� �������
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

	//�������� ��������� ������ �� XML ����
	static AnalogOutputListDataSrc* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//�������� ��������� ������ �� XML ����
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);


public:
	
	


	//������ ����� �������� ����������� ��������
	virtual double SetValue(double val) const;

	const std::vector<ANALOG_VALUE_INFO>& GetValues() const;
		
protected:

	

	mutable std::vector<ANALOG_VALUE_INFO> presetValues;

	//����� ������ ����� �������
	unsigned short valuesPresition;
	
};

#pragma endregion AnalogOutputListDataSrc


//////////////////////////////////////////////////////////////////////////
//������� �����  ��� ��������� ���������� ������
//��������� ��������� ������ �������� 
class IComposite
{
public :

	//����������
	virtual  ~IComposite();

	//���������� ������������� ������
	virtual int Size() const = 0;

	//�������� ����������� ������ 
	virtual const DataSrc* GetSubDataSrc(int ind) const = 0;	

};

//  [3/25/2010 Johnny A. Matveichik]
class CompositeAnalogDataSrc : public AnalogDataSrc , public IComposite
{

private:

	CompositeAnalogDataSrc(const CompositeAnalogDataSrc& rhs);
	CompositeAnalogDataSrc &operator =(const CompositeAnalogDataSrc &rhs);


public:

	//�����������
	CompositeAnalogDataSrc();

	//����������
	virtual ~CompositeAnalogDataSrc();

	//�������� ��������� ������ �� XML ����
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

public:
	
	//���������� ������������� ������
	virtual int Size() const;

	//�������� ����������� ������ 
	virtual const DataSrc* GetSubDataSrc(int ind) const;

	//�������� ������� �������� �������� � �������� ����������	
	virtual double GetSignal() const;

protected :
	
	// ��������� �������� � ������ CompositeDataSrc
	std::vector <const DataSrc* > vSubDS;
	
};
//  [3/25/2010 Johnny A. Matveichik]


/************************************************************************/
/*		����� CalcSummDataSrc											*/
/*	����� - �������� ������	����������� ����� �������� ��				*/
/*	���������� ����������												*/
/************************************************************************/
#pragma region CalcSummDataSrc

class CalcSummDataSrc : public CompositeAnalogDataSrc
{

private:

	CalcSummDataSrc(const CalcSummDataSrc& rhs);
	CalcSummDataSrc &operator =(const CalcSummDataSrc &rhs);

public :
	//�����������
	CalcSummDataSrc();

	//����������
	virtual ~CalcSummDataSrc();

	static CalcSummDataSrc* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//��������� ����� �� ���������� ������
	virtual double GetValue() const;
	
};

#pragma endregion CalcSummDataSrc

/************************************************************************/
/*		����� CalcAverageDataSrc										*/
/*  ����� - �������� ������ ����������� ������� ��������				*/
/*	�� ���������� ����������											*/
/************************************************************************/
#pragma region CalcAverageDataSrc 

class  CalcAverageDataSrc : public CompositeAnalogDataSrc
{
private:

	CalcAverageDataSrc(const CalcAverageDataSrc& rhs);
	CalcAverageDataSrc &operator =(const CalcAverageDataSrc &rhs);

public :

	//�����������
	CalcAverageDataSrc();

	//����������
	virtual ~CalcAverageDataSrc();

	static CalcAverageDataSrc* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//��������� ������� �������� �� ���������� ������
	virtual double GetValue() const;
	
};

#pragma endregion CalcAverageDataSrc 

/********************************************************************/
/*		����� CalcMulDataSrc										*/
/*  ����� - �������� ������ ����������� �������� �� ������������	*/
/*	�� ���������� ����������										*/
/********************************************************************/
#pragma region CalcMulDataSrc 

class CalcMulDataSrc : public CompositeAnalogDataSrc
{
private:

	CalcMulDataSrc(const CalcMulDataSrc& rhs);
	CalcMulDataSrc &operator =(const CalcMulDataSrc &rhs);

public:
	//�����������
	CalcMulDataSrc();

	//����������
	virtual ~CalcMulDataSrc();

	static CalcMulDataSrc* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//��������� �������� �� ���������� ������
	virtual double GetValue() const ;

};

#pragma endregion CalcMulDataSrc 

/********************************************************************/
/*		����� CalcMinDataSrc										*/
/*  ����� - �������� ������ ����������� ����������� ��������        */
/*	�� ���������� ����������										*/
/********************************************************************/
#pragma region CalcMinDataSrc 

class CalcMinDataSrc : public CompositeAnalogDataSrc
{
private:

	CalcMinDataSrc(const CalcMinDataSrc& rhs);
	CalcMinDataSrc &operator =(const CalcMinDataSrc &rhs);

public:

	//�����������
	CalcMinDataSrc();

	//����������
	virtual ~CalcMinDataSrc();

	static CalcMinDataSrc* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//��������� �������� �� ���������� ������
	virtual double GetValue() const ;	
	
};
#pragma endregion CalcMinDataSrc 

/********************************************************************/
/*		����� CalcMaxDataSrc										*/
/*  ����� - �������� ������ ����������� ����������� ��������        */
/*	�� ���������� ����������										*/
/********************************************************************/
#pragma region CalcMaxDataSrc 

class CalcMaxDataSrc : public CompositeAnalogDataSrc
{

private:

	CalcMaxDataSrc(const CalcMaxDataSrc& rhs);
	CalcMaxDataSrc &operator =(const CalcMaxDataSrc &rhs);

public:
	
	//�����������
	CalcMaxDataSrc();

	//����������
	virtual ~CalcMaxDataSrc();

	static CalcMaxDataSrc* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//��������� �������� �� ���������� ������
	virtual double GetValue() const ;

	
};
#pragma endregion CalcMaxDataSrc 


/************************************************************************/
/*		����� CalcSummDataSrc											*/
/*	����� - �������� ������	����������� ����� �������� ��				*/
/*	���������� ����������												*/
/************************************************************************/
#pragma region CalcDataSrc

class CalcDataSrc : public CompositeAnalogDataSrc
{
private:

	CalcDataSrc(const CalcDataSrc& rhs);
	CalcDataSrc &operator =(const CalcDataSrc &rhs);

public :

	//�����������
	CalcDataSrc();

	//����������
	virtual ~CalcDataSrc();

	//�������� ��������� ������ �� XML ����
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

	//�������� ��������� ������ �� XML ����
	static CalcDataSrc* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//��������� �������� �� ��������� �������� ���������� ������
	virtual double GetValue() const;
	
private :

	mutable vector< double > vars;
	mu::Parser parser;
};

#pragma endregion CalcDataSrc

/************************************************************************/
/*		����� DiscreteDataSrc											*/
/*  ����� - ��������� �������� ������ - ����������� ���������			*/
/*	�������/��������													*/
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

	//�������� ��������� ������ �� XML ����
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

	//�������� ��������� �������
	virtual bool IsEnabled() const = 0; 

protected:

	boost::signals2::signal< void (const DiscreteDataSrc*) > evOnChangeState;
	
	bool		 bInvert;	//	������������� �������� �������� 
	mutable bool enabled;	//	��������� �������� �������

	std::string strLowState;
	std::string strHighState;
};
#pragma endregion DiscreteDataSrc


/************************************************************************/
/*		����� DiscreteInputDataSrc										*/
/* �������� ������ ���������� �������� �� ������ �� �������				*/
/* �� ���������� � ����������� ����������� �������						*/
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

	//�������� ��������� ������ �� XML ����
	static DiscreteInputDataSrc* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//�������� ��������� ������ �� XML ����
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

public:

	//�������� ��������� �������
	virtual bool IsEnabled() const;

	//�������� ����� ������ c ������� ������ �������� 
	unsigned char GetChannelNo() const;

	//�������� ���������� � ������� ������ ��������  ������
	IDigitalInput* GetParentDevice() const;

	//  [5/15/2010 Johnny A. Matveichik]

protected:

	IDigitalInput* pDigitalInputDevice;
	unsigned char channel;
};

#pragma endregion DiscreteInputDataSrc



/************************************************************************/
/*		����� DiscreteOutputDataSrc										*/
/* ����� - ����������� �������� ������	����������� �������� � �������� */
/* ������� ��������� �������������� �������� �� ������ �� �������		*/
/* �� ���������� � ����������� ����������� ��������						*/
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

	//�������� ��������� ������ �� XML ����
	static DiscreteOutputDataSrc* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//�������� ��������� ������ �� XML ����
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);


public:

	//��������� ��������� �������
	virtual bool Enable(bool enable = true);

	//�������� ��������� �������
	virtual bool IsEnabled() const;

	//����������� ���������
	virtual bool ToggleState();
	
	//����������� ��������� ���������
	virtual bool CanChangeStateTo(bool state) const;

	//�������� ����� ������ c ������� ������ �������� 
	unsigned char GetChannelNo() const;
	
	//�������� ���������� � ������� ������ ��������  ������
	IDigitalOutput* GetParentDevice() const;

	//  [5/15/2010 Johnny A. Matveichik]

protected:

	bool saveState() const;
	void restoreState();

	IDigitalOutput* pDigitalOutputDevice;
	unsigned char channel;	

	bool bSafeState;

	///����� �� ���������� ���������� �������� ��� ����������� �������
	bool useSafe;
};

#pragma endregion DiscreteOutputDataSrc


/************************************************************************/
/*		����� ControledDiscreteOutputDataSrc							*/
/* ����� - ����������� ���������� �������� ������						*/
/* ����������� �������� � �������� ������� ��������� ��������������		*/
/* �������� �� ������ �� �������										*/
/* �� ���������� � ����������� ����������� ��������						*/
/* � ��������� ����������� ��������� �������� ���������					*/
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

	//�������� ��������� ������ �� XML ����
	static ControledDiscreteOutputDataSrc* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//�������� ��������� ������ �� XML ����
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

public:

	virtual bool CanChangeStateTo(bool newState) const;

	enum tagDSInd
	{
		SUB_DS_IND_TOLOWSTATE = 0,
		SUB_DS_IND_TOHIGHSTATE = 1	
	};

	//////////////////////////////////////////////////////////////////////////
	//��������� IComposite

	//���������� ������������� ������
	virtual int Size() const;

	//�������� ����������� ������ 
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

	//�������� ��������� ������ �� XML ����
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);
	
	
public:
	//////////////////////////////////////////////////////////////////////////
	// ��������� IComposite

	//���������� ������������� ������
	virtual int Size() const;

	//�������� ����������� ������ 
	virtual const DataSrc* GetSubDataSrc(int ind) const;

protected:

	// ��������� �������� � ������ CompositeDataSrc
	std::vector <const DiscreteDataSrc* > vSubDS;
	
};

#pragma endregion CompositeDiscreteDataSrc




/*************************************************************************/
/*		����� DiscreteANDDataSrc										 */
/* ����� - ���������� �������� ������	����������� �������� ��������	 */
/* �� ���������� ���������� ���������� ������ � ���������� ��������� �   */
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

	//�������� ��������� ������ �� XML ����
	static DiscreteANDDataSrc* CreateFromXML(IXMLDOMNode* pNode, UINT& error);
	
	virtual bool IsEnabled() const;

};

#pragma endregion DiscreteANDDataSrc

/*************************************************************************/
/*		����� DiscreteORDataSrc										     */
/* ����� - ���������� �������� ������	����������� �������� ��������	 */
/* �� ���������� ���������� ���������� ������ � ���������� ��������� ��� */
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

	//�������� ��������� ������ �� XML ����
	static DiscreteORDataSrc* CreateFromXML(IXMLDOMNode* pNode, UINT& error);
	
	virtual bool IsEnabled() const;
};

#pragma endregion DiscreteORDataSrc

/************************************************************************/
/*		����� AnalogLevelAsDiscreteDataSrc								*/
/* ����� - ���������� �������� ������ ����������� �����������������		*/
/* � ��������� �������� ��� ����������	�������������� ����������		*/
/* ���������� ������ ������������� ������								*/
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

	//�������� ��������� ������ �� XML ����
	static AnalogLevelAsDiscreteDataSrc* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//�������� ��������� ������ �� XML ����
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

		
public:

	virtual  bool IsEnabled() const;
	
	//�������� �������������� ������� �������
	double GetLevel() const;

	//������  �������������� ������� �������
	void SetLevel(double level);

	//////////////////////////////////////////////////////////////////////////
	// ��������� IComposite
	//���������� ������������� ������
	virtual int Size() const;

	//�������� ����������� ������ 
	virtual const DataSrc* GetSubDataSrc(int ind) const;
		

protected:

	const AnalogDataSrc* pControledADS;	//�������������� ���������� �������� ������
	const AnalogDataSrc* pBaseADS;		//������� ���������� �������� ������ ��� ��������� ������

	double lvl;							//�������� ������� �������
	bool   bForGrowth;

};

#pragma endregion AnalogLevelAsDiscreteDataSrc

/************************************************************************/
/*		����� AnalogRangeAsDiscreteDataSrc								*/
/* ����� - ���������� �������� ������ ����������� �����������������		*/
/* � ��������� �������� ��� ������	�������� ��������������� �����������*/
/* ��������� ������ �� ������� �������� 								*/
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

	//�������� ��������� ������ �� XML ����
	static AnalogRangeAsDiscreteDataSrc* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//�������� ��������� ������ �� XML ����
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

public:

	virtual  bool IsEnabled() const;
	
	//�������� ������������� �������� �������
	void GetRange(double& dLow, double& dHigh);

	//////////////////////////////////////////////////////////////////////////
	// ��������� IComposite
	//���������� ������������� ������
	virtual int Size() const;

	//�������� ����������� ������ 
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
/*		����� DeviceStateDataSrc										*/
/* �������� ������ ������������� � ������� ��������� ����������			*/
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

	//�������� ��������� ������ �� XML ����
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

	//�������� ��������� ������ �� XML ����
	static DeviceStateDataSrc* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//�������� ��������� �������
	virtual bool IsEnabled() const;

	//�������� ���������� � ������� ������ ��������  ������
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

	//�������� ��������� ������ �� XML ����
	static AnalogToBitsetDataSrc* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//�������� ��������� ������ �� XML ����
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);


public:

	//�������� ������� �������� ����������� �������� � �������� ������������
	virtual double GetValue() const;

	//�������� ������� �������� ����������� �������� � �������� ����������	
	virtual double GetSignal() const;

	//������ ����� �������� ����������� ��������
	virtual double SetValue(double val) const;

	//�������� �������� �������� ����������� ��������
	virtual void GetRange(double& minV, double& maxV) const;

	//�������� ����� ������ c ������� ������ �������� 
	virtual unsigned char GetChannelNo() const;

	//�������� ���������� � ������� ������ ��������  ������
	virtual IAnalogOutput* GetParentDevice() const;
	
	//���������� ������������� ������
	virtual int Size() const;

	//�������� ����������� ������ 
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

	//�������� ��������� ������ �� XML ����
	static StringDataSrc* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	//�������� ��������� ������ �� XML ����
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);
	

public:

	std::string GetString() const;

	void SetString(std::string text);

private:

	std::string str;
};

#endif

