#include "StdAfx.h"
#include "filters.h"

#include "DataSrc.h"
#include "..\ah_errors.h"
#include "..\ah_xml.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

std::map<std::string, const BaseFilter*> BaseFilter::mapFilters; 

BaseFilter::BaseFilter()
{
}

BaseFilter::~BaseFilter() 
{

}

bool BaseFilter::CreateFromXMLNode(IXMLDOMNode* pNode)
{
	if (pNode == NULL)
		return false;

	CComBSTR txt;
	pNode->get_text(&txt);

	USES_CONVERSION;
	std::string desc = W2A(txt);

	/*CString val;

	
	std::strstream str;

	AppErrorsHelper& err = AppErrorsHelper::Instance();
	const AppXMLHelper& appXML = AppXMLHelper::Instance();

	if ( !appXML.GetNodeAttributeByName(pNode, "ID", val) )
	{
		str << "Identificator not set (tag <ID>)" << ends;
		err.SetLastError(DSCE_EMPTY_PARAMETR, str.str());
		return false;
	}
	std::string id = (LPCTSTR)(val);

	if ( mapFilters.find(id) != mapFilters.end() )
	{
		str << "Duplicate identificator [" << id.c_str() << "]" << ends;
		err.SetLastError(DSCE_DUPLICATE_NAME, str.str());
		return false;
	}
	
	m_strID		= id;*/
	m_strDesc	= desc;

	return true;
}

void BaseFilter::ClearAll()
{
	for (std::map<std::string, const BaseFilter*>::iterator it = mapFilters.begin(); it != mapFilters.end(); ++it)
		delete (*it).second;

	mapFilters.clear();
}


//идентификатор
std::string BaseFilter::Name() const
{
	return m_strID; 
}

std::string BaseFilter::Description() const 
{ 
	return m_strDesc; 
}

bool BaseFilter::AddFilter( BaseFilter* conv )
{
	if (conv == NULL)
		return false;

	std::map<std::string, const BaseFilter*>::const_iterator it = mapFilters.find(conv->m_strID);
	if ( it != mapFilters.end() )
		return false;

	return mapFilters.insert( std::make_pair(conv->m_strID, conv) ).second;
}

const BaseFilter* BaseFilter::GetFilter(std::string id)
{
	std::map<std::string, const BaseFilter*>::const_iterator fnd;
	fnd =  mapFilters.find( id );

	if ( fnd == mapFilters.end() )
		return NULL;
	else
		return (*fnd).second;		
}	

//////////////////////////////////////////////////////////////////////////
KalmanFilter::KalmanFilter() : Q(0.0), R(15), F(1.0), H(1.0)
{

}

KalmanFilter::KalmanFilter(double q, double r, double f/* = 1.0*/, double h/* = 1.0*/) :
Q(q), R(r), F(f), H(h)
{	
}

KalmanFilter::~KalmanFilter()
{

}

//создание фильтра данных из XML узла
bool KalmanFilter::CreateFromXMLNode(IXMLDOMNode* pNode)
{
	if ( !BaseFilter::CreateFromXMLNode(pNode) )
		return false;


	std::string val;
	std::strstream str;

	AppErrorsHelper& err = AppErrorsHelper::Instance();
	const AppXMLHelper& appXML = AppXMLHelper::Instance();

	Q = 2.0;
	if ( appXML.GetNodeAttributeByName(pNode, "Q", val) )
		Q = atof(val.c_str());

	R = 4.0;
	if ( appXML.GetNodeAttributeByName(pNode, "R", val) )
		R = atof(val.c_str());

	H = 1.0;
	if ( appXML.GetNodeAttributeByName(pNode, "H", val) )
		H = atof(val.c_str());
	
	F = 1.0;
	if ( appXML.GetNodeAttributeByName(pNode, "F", val) )
		F = atof(val.c_str());

	return true;
}

//создание фильтра данных из XML узла
KalmanFilter* KalmanFilter::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<KalmanFilter> obj ( new KalmanFilter() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DSCE_OK;
	return obj.release();
}

bool KalmanFilter::InitFilter(const AnalogInputDataSrc* pDS)
{
	S = pDS->GetClearValue();
	C = 0.1;

	return true;
}

double KalmanFilter::Correct(double V) const
{
	//time update - prediction
	X0 = F*S;
	P0 = F*C*F + Q;

	//measurement update - correction
	double K = H*P0/(H*P0*H + R);

	S = X0 + K*(V - H*X0);
	C = (1.0 - K*H)*P0;

	return S;
}

//////////////////////////////////////////////////////////////////////////
AverageFilter::AverageFilter()
{

}

AverageFilter::~AverageFilter()
{

}

//создание фильтра данных из XML узла
bool AverageFilter::CreateFromXMLNode(IXMLDOMNode* pNode)
{
	if ( !BaseFilter::CreateFromXMLNode(pNode) )
		return false;

	std::string val;
	std::strstream str;

	AppErrorsHelper& err = AppErrorsHelper::Instance();
	const AppXMLHelper& appXML = AppXMLHelper::Instance();

	dbcapacity = 5;
	if ( appXML.GetNodeAttributeByName(pNode, "BUFFSIZE", val) )
		dbcapacity = atoi(val.c_str());
	
	return true;
}

//создание фильтра данных из XML узла
AverageFilter* AverageFilter::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<AverageFilter> obj ( new AverageFilter() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DSCE_OK;
	return obj.release();
}

bool AverageFilter::InitFilter(const AnalogInputDataSrc* pDS)
{
	return true;
}

double AverageFilter::Correct(double V) const
{
	if ((int)databuffer.size() >= dbcapacity)
		databuffer.pop_front();

	databuffer.push_back(V);
	double val = accumulate(databuffer.begin(), databuffer.end(), 0.0);
	val /= databuffer.size();

	return val;
}