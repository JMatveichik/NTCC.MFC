#include "StdAfx.h"

#include "Converters.h"
#include "common.h"

#include "..\ah_errors.h"
#include "..\ah_project.h"
#include "..\ah_xml.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;
std::map<std::string, const BaseConverter*> BaseConverter::mapConv; 


//////////////////////////////////////////////////////////////////////////

BaseConverter::BaseConverter() : m_dMin(DBL_MIN), m_dMax(DBL_MAX) 
{
}

BaseConverter::~BaseConverter() 
{
	
}

bool BaseConverter::CreateFromXMLNode(IXMLDOMNode* pNode)
{
	if (pNode == NULL)
		return false;

	CComBSTR txt;
	pNode->get_text(&txt);

	USES_CONVERSION;
	std::string desc = W2A(txt);

	std::string val;
	
	std::strstream str;
	
	AppErrorsHelper& err = AppErrorsHelper::Instance();
	const AppXMLHelper& appXML = AppXMLHelper::Instance();

	if ( !appXML.GetNodeAttributeByName(pNode, "ID", val) )
	{
		str << "Identificator not set (tag <ID>)" << ends;
		err.SetLastError(DSCE_EMPTY_PARAMETR, str.str());
		return false;
	}
	std::string id = val;

	if ( mapConv.find(id) != mapConv.end() )
	{
		str << "Duplicate identificator [" << id.c_str() << "]" << ends;
		err.SetLastError(DSCE_DUPLICATE_NAME, str.str());
		return false;
	}
	
	m_strID		= id;
	m_strDesc	= desc;

	return true;
}

void BaseConverter::ClearAll()
{
	for (std::map<std::string, const BaseConverter*>::iterator it = mapConv.begin(); it != mapConv.end(); ++it)
		delete (*it).second;

	mapConv.clear();
}

bool BaseConverter::Create(std::string id, std::string desc, double minV, double maxV)
{
	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();

	if (minV >= maxV)
	{
		str << "Duplicate identificator [" << id.c_str() << "]" << ends;
		err.SetLastError(DSCE_DUPLICATE_NAME, str.str());
		return false;
	}

	
	m_dMin = minV;
	m_dMax = maxV;

	if ( mapConv.find(id) != mapConv.end() )
	{
		str << "Duplicate identificator [" << id.c_str() << "]" << ends;
		err.SetLastError(DSCE_DUPLICATE_NAME, str.str());
		return false;
	}

	m_strID		= id;
	m_strDesc	= desc;

	return true;
}


std::string BaseConverter::Description() const 
{ 
	return m_strDesc; 
}

bool BaseConverter::AddConverter( BaseConverter* conv )
{
	if (conv == NULL)
		return false;

	std::map<std::string, const BaseConverter*>::const_iterator it = mapConv.find(conv->m_strID);
	if ( it != mapConv.end() )
		return false;

	return mapConv.insert( std::make_pair(conv->m_strID, conv) ).second;
}

const BaseConverter* BaseConverter::GetConverter(std::string id)
{
	std::map<std::string, const BaseConverter*>::const_iterator fnd;
	fnd =  mapConv.find( id );

	if ( fnd == mapConv.end() )
		return NULL;
	else
		return (*fnd).second;		
}	


void BaseConverter::GetMinMax(double& minV, double& maxV ) const
{
	minV = m_dMin;
	maxV = m_dMax;
}



//////////////////////////////////////////////////
//				Класс LinearConverter			//
//   Преобразователь с  линейной зависимостью	//
//////////////////////////////////////////////////
LinearConverter::LinearConverter(void) :
m_dK(0.0), m_dB(0.0)
{
}

LinearConverter::~LinearConverter(void)
{
}

LinearConverter* LinearConverter::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<LinearConverter> obj ( new LinearConverter() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DSCE_OK;
	return obj.release();
}

bool LinearConverter::CreateFromXMLNode(IXMLDOMNode* pNode)
{
	if (!BaseConverter::CreateFromXMLNode(pNode) )
		return false;
	
	std::string val;
	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();
	const AppXMLHelper& appXML = AppXMLHelper::Instance();
	if ( !appXML.GetNodeAttributeByName(pNode, "MINS", val) )
	{
		str << "Lower conversion bound not set (tag <MINS>)" << ends;
		err.SetLastError(DSCE_EMPTY_PARAMETR, str.str());
		return false;
	}
	double dMinS = atof(val.c_str());
	
	if ( !appXML.GetNodeAttributeByName(pNode, "MAXS", val) )
	{
		str << "Upper conversion bound not set (tag <MAXS>)" << ends;
		err.SetLastError(DSCE_EMPTY_PARAMETR, str.str());
		return false;
	}
	double dMaxS = atof(val.c_str());

	if ( dMaxS < dMinS )
	{
		str << std::setprecision(1) <<  "Invalid argument range ["<< dMinS << ";" << dMaxS << "]" << ends;
		err.SetLastError(DSCE_EMPTY_PARAMETR, str.str());
		return false;
	}
	
	if ( !appXML.GetNodeAttributeByName(pNode, "MINV", val) )
	{
		str << "Lower conversion bound not set (tag <MINV>)" << ends;
		err.SetLastError(DSCE_EMPTY_PARAMETR, str.str());
		return false;
	}
	double dMinV = atof(val.c_str());
	
	if ( !appXML.GetNodeAttributeByName(pNode, "MAXV", val) )
	{
		str << "Upper conversion bound not set (tag <MAXV>)" << ends;
		err.SetLastError(DSCE_EMPTY_PARAMETR, str.str());
		return false;
	}
	double dMaxV = atof(val.c_str());

	m_dMin		= dMinS;
	m_dMax		= dMaxS;

	m_dK = (dMaxV - dMinV) / (dMaxS - dMinS);
	m_dB = - m_dK*dMinS + dMinV;

	return true;
}

bool LinearConverter::Create(std::string id, std::string description, double x1, double y1, double x2, double y2 )
{
	if ( x2 == x1 )
		return false;

	m_dK = (y2 - y1) / (x2 - x1);
	m_dB = - m_dK*x1 + y1;

	return BaseConverter::Create(id, description, x1, x2);
}

double LinearConverter::Convert (double x ) const
{
	// y = k*x + b
	x = (x < m_dMin) ? m_dMin : x;
	x = (x > m_dMax) ? m_dMax : x;
		
	return  m_dK * x + m_dB;
}
//Обратное преобразование
double LinearConverter::RevConvert(double y) const
{
	if ( m_dK == 0.0 )
		return DBL_MIN;
	 
	 return (y - m_dB) / m_dK;
}

//строковое представление 
std::string LinearConverter::ToString(const char x/*='x'*/, const char y/*='y'*/) const
{
	std::strstream str;
	
	str << y << "=" << m_dK << "*" << x;
	str.setf(ios::showpos);
	str << m_dB << ends;

	return str.str();
}

//////////////////////////////////////////////////
//				Класс PolynomialConverter  		//
//////////////////////////////////////////////////
PolynomialConverter::PolynomialConverter(void)
{

}
PolynomialConverter::~PolynomialConverter(void)
{
}

PolynomialConverter* PolynomialConverter::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<PolynomialConverter> obj ( new PolynomialConverter() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DSCE_OK;
	return obj.release();
}

bool PolynomialConverter::CreateFromXMLNode(IXMLDOMNode* pNode)
{
	if (!BaseConverter::CreateFromXMLNode(pNode) )
		return false;
	
	std::string val;
	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();
	const AppXMLHelper& appXML = AppXMLHelper::Instance();

	if ( !appXML.GetNodeAttributeByName(pNode, "POLYNOM", val))
	{
		str << "Conversion polynom not set(tag <POLYNOM>)" << ends;
		err.SetLastError(DSCE_EMPTY_PARAMETR, str.str());
		return false;
	}
	if ( !SetPolynom(val.c_str() ) )
	{
		str << "Invalid conversion polynom (tag <POLYNOM>)" << ends;
		err.SetLastError(DSCE_INVALID_PARAMETR, str.str());
		return false;
	}
		
	if ( !appXML.GetNodeAttributeByName(pNode, "MIN", val) )
	{
		str << "Lower conversion bound not set (tag <MIN>)" << ends;
		err.SetLastError(DSCE_EMPTY_PARAMETR, str.str());
		return false;
	}
	double dMin = atof(val.c_str());
	
	if ( !appXML.GetNodeAttributeByName(pNode, "MAX", val) )
	{
		str << "Upper conversion bound not set (tag <MAX>)" << ends;
		err.SetLastError(DSCE_EMPTY_PARAMETR, str.str());
		return false;
	}
	double dMax = atof(val.c_str());

	if ( dMax < dMin )
	{
		str << "Invalid conversion range ["<< dMin << ";" << dMax << "]" << ends;
		err.SetLastError(DSCE_EMPTY_PARAMETR, str.str());
		return false;
	}
	m_dMin		= dMin;
	m_dMax		= dMax;

	for (double d = m_dMin; d < m_dMax; d += (m_dMax - m_dMin)/ 1000.0 )
		rtable.insert( std::make_pair( Convert(d), d) ); 

	rtable.insert( std::make_pair( Convert(m_dMax), m_dMax) );

	return true;
}

//Задание полинома при помощи массива полиномиальных коэффициентов
bool PolynomialConverter::Create(std::string id, std::string description, const double* pdPoly, int nOrder)
{
	if ( !BaseConverter::Create(id, description) )
		return false;

	return SetPolynom(pdPoly, nOrder);
}

//Задание полинома при помощи вектора полиномиальных коэффициентов
bool PolynomialConverter::Create(std::string id, std::string description, std::vector<double> vdPoly)
{
	if ( !SetPolynom(vdPoly) )
		return false;

	return BaseConverter::Create(id, description);
}


//Задание полинома при помощи строки с разделителями
//строка должна содержать коэффициенты полинома по порядку начиная 
//с нулевой степени разделенные одним из следующих разделителей  табуляция, пробел,
//или  ;:,!|
bool PolynomialConverter::Create(std::string id, std::string description, std::string strPolynom)
{
	if ( !SetPolynom(strPolynom) )
		return false;

	return BaseConverter::Create(id, description);
}

double PolynomialConverter::Convert (double inpValue) const
{
	int		nOrder = (int)m_vPolynom.size();
	double	dRes = 0.0;

	for (int i = 0; i < nOrder; i++)	
		dRes += m_vPolynom[i] * pow(inpValue, i);
	
	return dRes;
}

//Обратное преобразование
double PolynomialConverter::RevConvert(double y) const
{
	ASSERT(m_dMin != DBL_MIN);
	ASSERT(m_dMax != DBL_MAX);
	ASSERT(m_dMin < y);
	ASSERT(m_dMax > y);
	
	std::map<double, double>::const_iterator it = rtable.lower_bound(y);
	return (*it).second;
}


//Задание полинома при помощи массива полиномиальных коэффициентов
bool PolynomialConverter::SetPolynom(const double* pdPoly, int nOrder)
{	

	if ( pdPoly == NULL )
		return false;

	m_vPolynom.assign(pdPoly, pdPoly + nOrder);
	return ( m_vPolynom.size() > 0 );
}
//Задание полинома при помощи вектора полиномиальных коэффициентов
bool PolynomialConverter::SetPolynom(std::vector<double> vPoly)
{
	m_vPolynom = vPoly;
	return ( m_vPolynom.size() > 0 );
}

//Задание полинома при помощи строки
bool PolynomialConverter::SetPolynom(std::string strPolynom)
{
	m_vPolynom.clear();

	if ( strPolynom.empty() )
		return false;

	//Доступные разделители ",;|\t "
	boost::char_separator<char> sep(" \t;:,!|");

	//
	boost::tokenizer< boost::char_separator<char> > tokens(strPolynom, sep);

	for ( boost::tokenizer< boost::char_separator<char> >::iterator it = tokens.begin(); it != tokens.end(); ++it)
	{
		try
		{
			double val = boost::lexical_cast<double>(*it);			
			m_vPolynom.push_back( val );
		}
		catch(boost::bad_lexical_cast &)
		{
			return false;
		}
	}

	return ( m_vPolynom.size() > 0 );
}

std::string PolynomialConverter::ToString(const char x/*='x'*/, const char y/*='y'*/) const
{
	int	nOrder = (int)m_vPolynom.size();
	
	if ( nOrder == 0 )
		return "invalid";

	std::strstream str;
	
	str << y << "=" << m_vPolynom[0];
	for (int i = 1; i < nOrder; i++)
	{
		if ( m_vPolynom[i] > 0)
			str << '+';

		str << m_vPolynom[i] << "*" << x;

		if ( i > 1)
			str << "^" << i;
	}
	str << ends;
	return str.str();
}

//////////////////////////////////////////////////
//				Класс PolynomialNegativeConverter  		//
//////////////////////////////////////////////////
PolynomialNegativeConverter::PolynomialNegativeConverter(void)
{

}
PolynomialNegativeConverter::~PolynomialNegativeConverter(void)
{
}

PolynomialNegativeConverter* PolynomialNegativeConverter::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<PolynomialNegativeConverter> obj ( new PolynomialNegativeConverter() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DSCE_OK;
	return obj.release();
}

double PolynomialNegativeConverter::Convert (double x) const
{
	int		nOrder = (int)m_vPolynom.size();
	double	dRes = 0.0;

	for (int i = 0; i < nOrder; i++)	
		dRes += m_vPolynom[i] * pow(x, -i);
	
	return dRes;
}

std::string PolynomialNegativeConverter::ToString(const char x/*='x'*/, const char y/*='y'*/) const
{
	int	nOrder = (int)m_vPolynom.size();
	
	if ( nOrder == 0 )
		return "invalid";

	std::strstream str;
	
	str << y << "=" << m_vPolynom[0];
	for (int i = 1; i < nOrder; i++)
	{
		if ( m_vPolynom[i] > 0)
			str << '+';

		str << m_vPolynom[i] << "*" << x;

		if ( i > 1)
			str << "^-" << i;
	}
	str << ends;
	return str.str();
}

//////////////////////////////////////////////////
//				Класс MultiRangeConverter  		//
//////////////////////////////////////////////////
MultiRangeConverter::MultiRangeConverter(void)
{

}
MultiRangeConverter::~MultiRangeConverter(void)
{
}

MultiRangeConverter* MultiRangeConverter::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<MultiRangeConverter> obj ( new MultiRangeConverter() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DSCE_OK;
	return obj.release();
}

bool MultiRangeConverter::CreateFromXMLNode(IXMLDOMNode* pNode)
{
	if (!BaseConverter::CreateFromXMLNode(pNode) )
		return false;
	
	std::string val;
	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();
	const AppXMLHelper& appXML = AppXMLHelper::Instance();
	if ( !appXML.GetNodeAttributeByName(pNode, "MIN", val) )
	{
		str << "Lower conversion bound not set (tag <MIN>)" << ends;
		err.SetLastError(DSCE_EMPTY_PARAMETR, str.str());
		return false;
	}
	double dMin = atof(val.c_str());
	
	if ( !appXML.GetNodeAttributeByName(pNode, "MAX", val) )
	{
		str << "Upper conversion bound not set (tag <MAX>)" << ends;
		err.SetLastError(DSCE_EMPTY_PARAMETR, str.str());
		return false;
	}
	double dMax = atof(val.c_str());

	if ( dMax <= dMin )
	{
		str << "Invalid conversion range ["<< dMin << ";" << dMax << "]" << ends;
		err.SetLastError(DSCE_EMPTY_PARAMETR, str.str());
		return false;
	}
	m_dMin		= dMin;
	m_dMax		= dMax;

	CComPtr<IXMLDOMNodeList> pSubItemsList;
	HRESULT hr = pNode->selectNodes( CComBSTR("ITEM"), &pSubItemsList);


	if ( FAILED(hr) || pSubItemsList == NULL )
	{
		str << "Empty converters list (tags <ITEM>)" << ends;
		err.SetLastError(DSCE_EMPTY_PARAMETR, str.str());		
		return false;
	}

	long subItems;
	pSubItemsList->get_length(&subItems);
	pSubItemsList->reset();

	for (long n = 0; n < subItems; n++)
	{
		CComPtr<IXMLDOMNode> pSubItem;
		pSubItemsList->get_item(n, &pSubItem);

		std::string sVal;
		
		if ( !appXML.GetNodeAttributeByName(pSubItem, "CONVID", sVal) )
		{
			str << "Converter id not set (tags <CONVID>)" << ends;
			err.SetLastError(DSCE_EMPTY_PARAMETR, str.str());		
			return false;
		}

		const BaseConverter* conv =  BaseConverter::GetConverter( sVal );
		if ( conv == NULL )
		{
			str << "Converter ["<< sVal.c_str() << "]  not found" << ends;
			err.SetLastError(DSCE_INVALID_PARAMETR, str.str());		
			return false;
		}
		m_vSubConv.push_back(conv);
	}
	
	/*
	if ( !appXML.GetNodeAttributeByName(pNode, "POLYNOM", val))
	{
		str << "Conversion polynom not set(tag <POLYNOM>)" << ends;
		err.SetLastError(DSCE_EMPTY_PARAMETR, str.str());
		return false;
	}
	if ( !SetPolynom(val.c_str() ) )
	{
		str << "Invalid conversion polynom (tag <POLYNOM>)" << ends;
		err.SetLastError(DSCE_INVALID_PARAMETR, str.str());
		return false;
	}
		
	if ( !appXML.GetNodeAttributeByName(pNode, "MIN", val) )
	{
		str << "Lower conversion bound not set (tag <MIN>)" << ends;
		err.SetLastError(DSCE_EMPTY_PARAMETR, str.str());
		return false;
	}
	double dMin = atof(val.c_str());
	
	if ( !appXML.GetNodeAttributeByName(pNode, "MAX", val) )
	{
		str << "Upper conversion bound not set (tag <MAX>)" << ends;
		err.SetLastError(DSCE_EMPTY_PARAMETR, str.str());
		return false;
	}
	double dMax = atof(val.c_str());

	if ( dMax <= dMin )
	{
		str << "Invalid conversion range ["<< dMin << ";" << dMax << "]" << ends;
		err.SetLastError(DSCE_EMPTY_PARAMETR, str.str());
		return false;
	}
	m_dMin		= dMin;
	m_dMax		= dMax;

	for (double d = m_dMin; d < m_dMax; d += (m_dMax - m_dMin)/ 1000.0 )
		rtable.insert( std::make_pair( Convert(d), d) ); 

	rtable.insert( std::make_pair( Convert(m_dMax), m_dMax) );
	*/
	return true;
}
const BaseConverter* MultiRangeConverter::Find(double x) const
{
	std::vector <const BaseConverter*>::const_iterator it = m_vSubConv.begin();
	for( ;it != m_vSubConv.end(); ++it)
	{
		double xMin, xMax;
		(*it)->GetMinMax(xMin, xMax);
		
		ASSERT(xMin != DBL_MIN);
		ASSERT(xMax != DBL_MAX);
	
		if (x < xMin)
			return (*it);

		if ( x <= xMax )
			return (*it);
	}
	return NULL;
}

double MultiRangeConverter::Convert (double x) const
{
	const BaseConverter* pConv = Find(x);
	
	if ( pConv != NULL)
		return pConv->Convert(x);

	return UNKNONW_VALUE;
}

//Обратное преобразование
double MultiRangeConverter::RevConvert(double y) const
{
	ASSERT(m_dMin != DBL_MIN);
	ASSERT(m_dMax != DBL_MAX);
	ASSERT(m_dMin < y);
	ASSERT(m_dMax > y);
	
	std::map<double, double>::const_iterator it = rtable.lower_bound(y);
	return (*it).second;
}


std::string MultiRangeConverter::ToString(const char x/*='x'*/, const char y/*='y'*/) const
{
// 	int	nOrder = (int)m_vPolynom.size();
// 	
// 	if ( nOrder == 0 )
// 		return "invalid";

	std::strstream str;
	
// 	str << y << "=" << m_vPolynom[0];
// 	for (int i = 1; i < nOrder; i++)
// 	{
// 		if ( m_vPolynom[i] > 0)
// 			str << '+';
// 
// 		str << m_vPolynom[i] << "*" << x;
// 
// 		if ( i > 1)
// 			str << "^" << i;
// 	}
	str << ends;
	return str.str();
}


TableConverter::TableConverter() : defEps(0.0)
{

}
TableConverter::~TableConverter()
{

}

TableConverter* TableConverter::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<TableConverter> obj ( new TableConverter() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DSCE_OK;
	return obj.release();
}

bool TableConverter::CreateFromXMLNode(IXMLDOMNode* pNode)
{
	if (!BaseConverter::CreateFromXMLNode(pNode) )
		return false;

	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();
	const AppXMLHelper& appXML = AppXMLHelper::Instance();

	std::string val;

	if ( appXML.GetNodeAttributeByName(pNode, "EPS", val) )
		defEps = atof(val.c_str());		

	if ( appXML.GetNodeAttributeByName(pNode, "FILE", val) )
	{
		val = NTCCProject::Instance().GetPath(NTCCProject::PATH_CONFIG, val);
		return LoadData(val.c_str());
	}
	else
	{
		CComPtr<IXMLDOMNodeList> pPointsList;
		HRESULT hr = pNode->selectNodes( CComBSTR("POINT"), &pPointsList);


		if ( FAILED(hr) || pPointsList == NULL )
		{
			str << "Empty points list (tags <POINTS>)" << ends;
			err.SetLastError(DSCE_EMPTY_PARAMETR, str.str());		
			return false;
		}

		long subItems;
		pPointsList->get_length(&subItems);
		pPointsList->reset();

		for (long n = 0; n < subItems; n++)
		{
			CComPtr<IXMLDOMNode> pPoint;
			pPointsList->get_item(n, &pPoint);

			std::string sVal;
			double x;
			
			if ( !appXML.GetNodeAttributeByName(pPoint, "X", sVal) )
			{
				str << "Point parameter x not set (attribute X)" << ends;
				err.SetLastError(DSCE_EMPTY_PARAMETR, str.str());		
				return false;
			}

			double y;

			if ( !appXML.GetNodeAttributeByName(pPoint, "Y", sVal) )
			{
				str << "Point parameter y not set (attribute Y)" << ends;
				err.SetLastError(DSCE_EMPTY_PARAMETR, str.str());		
				return false;
			}

			tbl.insert(std::make_pair(x, y));			
		}
	}


	 	

	return true;	
}

bool TableConverter::LoadData(const char* pszFileName)
{
	std::ifstream inp(pszFileName);
	
	if (!inp.good())
		return false;

	while (inp.good())
	{
		string line;
		getline(inp, line);

		vector<double> point;
		
		try {
			ExtractData<double>(line, ";", point);			
		}
		catch(...)
		{
			return false;
		}
		
		if (point.size() != 2)
			return false;

		tbl.insert(std::make_pair(point[0], point[1]));
		tblr.insert(std::make_pair(point[1], point[0]));

	}

	return true;
}

//Преобразование
double TableConverter::Convert(double inpValue) const
{
	
	if (defEps != 0.0) 
	{
		auto pt = find_if(tbl.begin(), tbl.end(), double_equal(inpValue, defEps) );
		if ( pt != tbl.end() )
			return pt->second;	
		else
		
			throw std::invalid_argument("Не найдено соответствие в таблице преобразования!");
	
	}
	else {

		std::map<double, double>::const_iterator ptl = tbl.lower_bound(inpValue);
		if (ptl != tbl.begin())
			ptl--;

		if (ptl != tbl.end())
		{
			std::map<double, double>::const_iterator pth = ptl;
			pth++;

			double k  = (pth->second -  ptl->second) / (pth->first - ptl->first);
			double b =  -k*ptl->first + ptl->second;

			double y = k*inpValue + b;
			return y;

		}
		else
		{
			throw std::invalid_argument("Не найдено соответствие в таблице преобразования!");
		}
	}
	
	return inpValue;
}

bool double_equal::operator() (std::pair<const double, double> elem)
{	
	double d = fabs(elem.first - v);
	return d < eps;
}

//Обратное преобразование
double TableConverter::RevConvert(double inpValue) const
{
	auto pt = find_if(tblr.begin(), tblr.end(), double_equal(inpValue, defEps ) );
	if ( pt != tblr.end() )
	{	
		return pt->second;
	}
	else {
		
		throw std::invalid_argument("Не найдено соответствие в таблице преобразования!");
	}
		
	return inpValue;
}

//Действительный диапазон
void TableConverter::GetMinMax(double& minV, double& maxV ) const
{
	minV = tbl.begin()->first;
	maxV = tbl.rbegin()->first;
}

std::string TableConverter::ToString(const char x /*='x'*/, const char y/*='y'*/) const
{
	std::strstream str;

	str << ends;
	return str.str();
}


//  [11/25/2014 Johnny A. Matveichik]

FormulaConverter::FormulaConverter()
{

}
FormulaConverter::~FormulaConverter()
{

}

FormulaConverter* FormulaConverter::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<FormulaConverter> obj ( new FormulaConverter() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DSCE_OK;
	return obj.release();
}

bool FormulaConverter::CreateFromXMLNode(IXMLDOMNode* pNode)
{
	if (!BaseConverter::CreateFromXMLNode(pNode) )
		return false;

	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();
	const AppXMLHelper& appXML = AppXMLHelper::Instance();

	std::string val;
	if ( !appXML.GetNodeAttributeByName(pNode, "FORMULA", val) || val.length() == 0 )
	{
		str << "Formula not set" << ends;
		err.SetLastError(DSCE_EMPTY_PARAMETR, str.str());		
		return false;
	}

	boost::algorithm::to_lower(val);
	parser.SetExpr(val.c_str());
	parser.DefineVar("x", &x);

	return true;
}

//Преобразование
double FormulaConverter::Convert(double inpValue) const
{
	//устанавливаем значение переменной x
	x = inpValue;
	double y = DBL_MIN;
	try
	{
		y = parser.Eval();
		
	}
	catch(mu::Parser::exception_type &e)
	{
		TRACE("\nMessage:  %s\n", e.GetMsg());
		TRACE("\nFormula:  %s\n", e.GetExpr());
		TRACE("\nToken:    %s\n", e.GetToken());
		TRACE("\nPosition: %d\n", e.GetPos());
		TRACE("\nErrc:     %s\n", e.GetCode());
	}
	return y;
}

//Обратное преобразование
double FormulaConverter::RevConvert(double y) const
{
	return 0.0;
}

//строковое представление 
std::string FormulaConverter::ToString(const char x/*='x'*/, const char y/*='y'*/) const
{
	std::strstream formula;
	formula << "y = " << parser.GetExpr() << ends;
	return formula.str();
}

//Действительный диапазон
void FormulaConverter::GetMinMax(double& minV, double& maxV ) const
{

}