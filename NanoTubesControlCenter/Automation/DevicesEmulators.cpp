#include "StdAfx.h"
#include "DevicesEmulators.h"

#include "..\ah_project.h"
#include "..\ah_errors.h"
#include "..\ah_xml.h"

#ifdef _DEBUG
#define new DEBUG_NEW	
#endif

using namespace std;

double drand(double x1, double x2)
{ return x1 + double( rand() ) / RAND_MAX * (x2 - x1); }

VirtualDevice::VirtualDevice() : IdentifiedPhysDevice("ART Software", "Virtual Device")
{
	m_dt = 0.0;
	srand( (unsigned int)time(NULL) );
}

VirtualDevice::~VirtualDevice()
{

}

bool VirtualDevice::SignalsFromFile(std::string file)
{
	if ( inStream.is_open() )
		inStream.close();

	inStream.open(file);

	if (inStream.bad())
		return false;

	srteamSignals.clear();

	if (!inStream.good())
		return false;

	std::string line;	
	if ( getline(inStream, line).bad() )
		return false;
	

	return true;
}

void VirtualDevice::UpdateSignalsFromFile()
{
	if ( !inStream.is_open())
		return;

	boost::char_separator<char> sep(" \t;");

	std::string line;	
	if ( inStream.eof() )
	{
		//TRACE("\n\tEND OF SIGNAL DATA FILE... REWIND");
		inStream.clear();
		inStream.seekg(0);		
		getline(inStream, line);
	}

	getline(inStream, line);
	vector<double> data;

	boost::tokenizer< boost::char_separator<char> > tokens(line, sep);

	for ( boost::tokenizer< boost::char_separator<char> >::iterator it = tokens.begin(); it != tokens.end(); ++it)
	{
		try
		{
			double val = boost::lexical_cast<double>(*it);			
			data.push_back( val );
		}
		catch(boost::bad_lexical_cast &)
		{
			continue;
		}		
	}

	if (data.empty())
		return ;

	int start = 0;
	for(int i = start; i < GetChannelsCountAI(); i ++ )
	{
		channelsDataAI[i] = data[i];
		start = i;
	}
	
	for(int i = start; i < GetChannelsCountDI(); i ++ )
		channelsStateDI[i] = data[i] > 0;
		
}

bool VirtualDevice::CreateFromXMLNode(IXMLDOMNode* pNode)
{
	if ( !IdentifiedPhysDevice::CreateFromXMLNode(pNode) )
		return false;

	std::string sAtr;
	
	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();
	const AppXMLHelper& appXML = appXML.Instance();

	int countAI = 0;
	if (appXML.GetNodeAttributeByName(pNode, "CHANNELSAI", sAtr) || sAtr.length() == 0 )
		countAI = atoi( sAtr.c_str() );

	int countAO = 0;
	if (appXML.GetNodeAttributeByName(pNode, "CHANNELSAO", sAtr) || sAtr.length() == 0 )
		countAO = atoi( sAtr.c_str() );

	int countDI = 0;
	if (appXML.GetNodeAttributeByName(pNode, "CHANNELSDI", sAtr) || sAtr.length() == 0 )
		countDI = atoi( sAtr.c_str() );

	int countDO = 0;
	if (appXML.GetNodeAttributeByName(pNode, "CHANNELSDO", sAtr) || sAtr.length() == 0 )
		countDO = atoi( sAtr.c_str() );

	if (appXML.GetNodeAttributeByName(pNode, "SIGNALSSRC", sAtr) )
	{
		std::string path = NTCCProject::Instance().GetPath(NTCCProject::PATH_CONFIG, sAtr);
		SignalsFromFile(path);
	}


	CComPtr<IXMLDOMNodeList>	 pItemsList;
	pNode->selectNodes(CComBSTR("AO"), &pItemsList);

	long subItems = 0;

	pItemsList->get_length(&subItems); 
	pItemsList->reset();

	std::map<unsigned char, MEASSURE_RANGE_INFO> rangsAO;

	for (int n = 0; n < subItems; n++)
	{
		CComPtr<IXMLDOMNode> pSubItem;
		pItemsList->get_item(n, &pSubItem);

		CComPtr<IXMLDOMNamedNodeMap> pItemAtr;
		pSubItem->get_attributes(&pItemAtr);

		std::string sAtr;		
		if ( !appXML.GetAttributeByName(pItemAtr, "WIRE", sAtr))
			continue;

		int channel = atoi(sAtr.c_str());
		MEASSURE_RANGE_INFO msri;
		
		if ( !appXML.GetAttributeByName(pItemAtr, "MIN", sAtr))
			continue;

		msri.mins = atof(sAtr.c_str());


		if ( !appXML.GetAttributeByName(pItemAtr, "MAX", sAtr))
			continue;

		msri.maxs = atof(sAtr.c_str());

		msri.desc = "Unknown";
		if ( appXML.GetAttributeByName(pItemAtr, "DESC", sAtr))
			msri.desc = sAtr.c_str();
		
		msri.units = "";
		if ( appXML.GetAttributeByName(pItemAtr, "UNITS", sAtr))
			msri.units = sAtr.c_str();

		msri.rngid = outRanges.size();
		
		rangsAO.insert(std::make_pair(channel, msri ) );

		outRanges.insert(std::make_pair(msri.rngid, msri ) );

	}

	CComPtr<IXMLDOMNodeList>	 pItemsList1;
	pNode->selectNodes(CComBSTR("AI"), &pItemsList1);
	
	pItemsList1->get_length(&subItems); 
	pItemsList1->reset();

	std::map<unsigned char, MEASSURE_RANGE_INFO> rangsAI;

	for (int n = 0; n < subItems; n++)
	{
		CComPtr<IXMLDOMNode> pSubItem;
		pItemsList1->get_item(n, &pSubItem);

		CComPtr<IXMLDOMNamedNodeMap> pItemAtr;
		pSubItem->get_attributes(&pItemAtr);

		std::string sAtr;		
		if ( !appXML.GetAttributeByName(pItemAtr, "WIRE", sAtr))
			continue;

		int channel = atoi(sAtr.c_str());
		MEASSURE_RANGE_INFO msri;
		
		if ( !appXML.GetAttributeByName(pItemAtr, "MIN", sAtr))
			continue;

		msri.mins = atof(sAtr.c_str());


		if ( !appXML.GetAttributeByName(pItemAtr, "MAX", sAtr))
			continue;

		msri.maxs = atof(sAtr.c_str());

		msri.desc = "Unknown";
		if ( appXML.GetAttributeByName(pItemAtr, "DESC", sAtr))
			msri.desc = sAtr.c_str();
		
		msri.units = "";
		if ( appXML.GetAttributeByName(pItemAtr, "UNITS", sAtr))
			msri.units = sAtr.c_str();

		msri.rngid = inpRanges.size();
		
		rangsAI.insert(std::make_pair(channel, msri ) );

		inpRanges.insert(std::make_pair(msri.rngid, msri ) );

	}

	channelsDataAI.insert(channelsDataAI.begin(), countAI, 0.0);
	channelsRangesAI.insert(channelsRangesAI.begin(), countAI, MEASSURE_RANGE_INFO() );

	for( std::map<unsigned char, MEASSURE_RANGE_INFO>::iterator it = rangsAI.begin(); it != rangsAI.end(); it++)
	{
		if ( it->first >= channelsRangesAI.size() )
			continue;

		channelsRangesAI.at(it->first) = (*it).second;		
	}


	channelsDataAO.insert(channelsDataAO.begin(), countAO, 0.0);
	channelsRangesAO.insert(channelsRangesAO.begin(), countAO, MEASSURE_RANGE_INFO());

	for( std::map<unsigned char, MEASSURE_RANGE_INFO>::iterator it = rangsAO.begin(); it != rangsAO.end(); it++)
	{
		if ( it->first >= channelsRangesAO.size() )
			continue;

		channelsRangesAO.at(it->first)  = (*it).second;		
	}

	channelsStateDI.insert(channelsStateDI.begin(), countDI, false);

	channelsStateDO.insert(channelsStateDO.begin(), countDO, false);


	//////////////////////////////////////////////////////////////////////////
	m_dt += 0.0005;

	for(int ch = 0; ch < GetChannelsCountAI(); ch++)
	{
		double fi = 0.025 * ch;
		double A = channelsRangesAI[ch].maxs;

		double val = channelsRangesAI[ch].mins + A * fabs(sin( m_dt + fi) ) ;
		double r = drand(channelsRangesAI[ch].mins/drand(0,ch + 50.0), channelsRangesAI[ch].maxs/drand(0, ch + 50.0));
		channelsDataAI[ch] = val + r;

	}
	//////////////////////////////////////////////////////////////////////////
	return true;
}

VirtualDevice* VirtualDevice::CreateFromXML(IXMLDOMNode* pNode, UINT& error )
{
	
	auto_ptr<VirtualDevice> obj ( new VirtualDevice() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DCE_OK;
	return obj.release();
}

bool VirtualDevice::Create(std::string name, unsigned long updateInterval, int nCountAI, std::map<unsigned char, MEASSURE_RANGE_INFO> rangesAI, int nCountAO, std::map<unsigned char, MEASSURE_RANGE_INFO> rangesAO, int nCountDI, int nCountDO)
{
	if( !IdentifiedPhysDevice::Create(name, updateInterval))
		return false;

	channelsDataAI.insert(channelsDataAI.begin(), nCountAI, 0.0);
	channelsRangesAI.insert(channelsRangesAI.begin(), nCountAI, MEASSURE_RANGE_INFO());
	for( std::map<unsigned char, MEASSURE_RANGE_INFO>::iterator it = rangesAI.begin(); it != rangesAI.end(); it++)
	{
		if ( it->first >= channelsRangesAI.size() )
			continue;

		channelsRangesAI.at(it->first)  = (*it).second;
	}


	channelsDataAO.insert(channelsDataAO.begin(), nCountAO, 0.0);
	channelsRangesAO.insert(channelsRangesAO.begin(), nCountAO, MEASSURE_RANGE_INFO());

	for( std::map<unsigned char, MEASSURE_RANGE_INFO>::iterator it = rangesAO.begin(); it != rangesAO.end(); it++)
	{
		if ( it->first >= channelsRangesAO.size() )
			continue;

		channelsRangesAO.at(it->first)  = (*it).second;		
	}

	channelsStateDI.insert(channelsStateDI.begin(), nCountDI, false);

	channelsStateDO.insert(channelsStateDO.begin(), nCountDO, false);

	return true;
}

//Получение общего числа аналоговых входов
int VirtualDevice::GetChannelsCountAI() const 
{ 
	return (int)channelsDataAI.size(); 
}

double  VirtualDevice::GetChannelData(unsigned char channel)  const
{ 
	return channelsDataAI[channel];
}

//Получение данных с одного из аналоговых входов (выполняется запрос на чтение данных)
double  VirtualDevice::ReadChannel(unsigned char channel)  const 
{ 
	return GetChannelData(channel); 
}

//Получение данных со всех аналоговых входов (выполняется запрос на чтение данных)
void VirtualDevice::ReadChannels(double* data/* = NULL*/) const 
{
	if ( data != NULL )
	{
		int ind = 0;
		for(std::vector<double>::const_iterator it = channelsDataAI.begin(); it != channelsDataAI.end(); ++it, ind++)
			data[ind] = (*it);
	}
}

//Получить  информацию об измеряемом диапазоне канала
void VirtualDevice::GetInputRangeInfo(unsigned char ch, LPMEASSURE_RANGE_INFO lpmci) const
{
	*lpmci = channelsRangesAI.at(ch);
}

//Установить  диапазон  аналогового входа
bool VirtualDevice::SetInputRange(unsigned char ch, unsigned char range)
{
	channelsRangesAI.at(ch) = outRanges.at(range);
	return true;	
}

//Получить все возможные  диапазоны  измерения
void VirtualDevice::EnumInputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const
{
	rngs.clear();
	for (std::map< long, MEASSURE_RANGE_INFO >::const_iterator it = inpRanges.begin(); it != inpRanges.end(); ++it)
		rngs.push_back((*it).second);
}


///IAnalogOutput
//Получение общего числа аналоговых выходов 
int VirtualDevice::GetChannelsCountAO() const
{
	return (int)channelsDataAO.size();
}

//Записать данные в аналоговый выход 
double  VirtualDevice::WriteToChannel(unsigned char channel, double val) 
{
	double oldVal = channelsDataAO[channel];

	channelsDataAO[channel] = val;

	return oldVal;
}

//Записать данные во все каналы
void VirtualDevice::WriteToAllChannels(const std::vector<double>& vals)  
{
	for(size_t i = 0; i < channelsDataAO.size(); i++)
		channelsDataAO[i] = vals[i];
}

//Получить последнее записанное в аналоговый выход значение 
double VirtualDevice::GetChannelStateAO (unsigned char channel) const
{
	return channelsDataAO[channel];
}

//Получить выходной диапазон канала 
void VirtualDevice::GetMinMax(unsigned char channel, double& minOut, double& maxOut) const
{
	MEASSURE_RANGE_INFO rng = channelsRangesAO.at(channel);
	minOut = rng.mins;
	maxOut = rng.maxs;
}

//Получить  информацию об диапазоне канала
void VirtualDevice::GetOutputRangeInfo(unsigned char ch, LPMEASSURE_RANGE_INFO lpmci) const
{
	*lpmci = channelsRangesAO.at(ch);
}

//Установить  диапазон  аналогового выхода
bool VirtualDevice::SetOutputRange(unsigned char ch, unsigned char range)
{
	channelsRangesAO.at(ch) = outRanges.at(range);
	return true;
}

//Получить все возможные  диапазоны  
void VirtualDevice::EnumOutputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const
{
	rngs.clear();
	for (std::map< long, MEASSURE_RANGE_INFO >::const_iterator it = outRanges.begin(); it != outRanges.end(); ++it)
		rngs.push_back((*it).second);
}



//Получение общего числа дискретных входов
int VirtualDevice::GetChannelsCountDI() const
{
	return (int)channelsStateDI.size();
}

//Обновить состояния дискретных входов	
bool VirtualDevice::UpdateChannelsStateDI() const
{
	return true;
}

//Получение состояния одного из дискретных входов	
bool  VirtualDevice::GetChannelStateDI(unsigned char channel, bool update) const 
{
	if ( update )
		UpdateChannelsStateDI();

	return channelsStateDI[channel];
}

//Получение общего числа цифровых выходов 
int VirtualDevice::GetChannelsCountDO() const
{
	return (int)channelsStateDO.size();
}

//Обновить текущее состояния дискретных выходов	
bool VirtualDevice::UpdateChannelsStateDO() const
{
	return true;
}

//Получить состояние цифрового выхода
bool VirtualDevice::GetChannelStateDO(unsigned char channel, bool update) const
{
	if ( update )
		UpdateChannelsStateDO();

	return channelsStateDO[channel];
}

//Установить состояние цифрового выхода 
bool  VirtualDevice::SetChannelStateDO(unsigned char channel, bool enable)
{
	channelsStateDO[channel] = enable;
	return true;
}

//Записать данные во все каналы
void VirtualDevice::SetChannelsStateDO(unsigned char val)
{
	
}


void VirtualDevice::ControlProc() 
{
	m_dt += 0.0005;
	
	for(int ch = 0; ch < GetChannelsCountAI(); ch++)
	{
		double fi = 0.025 * ch;
		double A = channelsRangesAI[ch].maxs;

		double val = channelsRangesAI[ch].mins + A * fabs(sin( m_dt + fi) ) ;
		double r = drand(channelsRangesAI[ch].mins/50.0, channelsRangesAI[ch].maxs/50.0);
		channelsDataAI[ch] = val + r;

	}

	
	
	UpdateSignalsFromFile();

// 	static long counter = 0;
// 	counter++;
// 	bool bState = (counter % 20 != 0);
// 	for(int ch = 0; ch < GetChannelsCountDI(); ch++)
// 		channelsStateDI[ch] = bState;
	
};

//Реализация интерфейса  IPulseDigitalOutput

bool VirtualDevice::SetupGenerator(unsigned char channel, unsigned long highWidth, unsigned long lowWidth)
{
	channelsPulseWidth.first  = highWidth;
	channelsPulseWidth.second = lowWidth;
	return true;
}