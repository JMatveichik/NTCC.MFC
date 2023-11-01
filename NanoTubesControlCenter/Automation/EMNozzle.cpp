
#include "StdAfx.h"

#include "EMNozzle.h"
#include "..\AppHelpers.h"
#include "ADAM6000Series.h"

#ifdef _DEBUG
#define new DEBUG_NEW	
#endif

//максимальный расход по моделям контроллеров расхода
std::map< std::string, double > EMNozzle::emnMaxFlowByModel(dataEMN, dataEMN + EMN_MODELS_COUNT);

EMNozzle::EMNozzle(void) 
: BaseFlowController("Electomagnetic nozzle","EM-001"), hiWidth(50), curFlow(0)
{
}


EMNozzle::~EMNozzle(void)
{
}

EMNozzle* EMNozzle::CreateFromXML( IXMLDOMNode* pNode, UINT& error )
{
	auto_ptr<EMNozzle> obj ( new EMNozzle() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DCE_OK;
	return obj.release();
}

bool EMNozzle::CreateFromXMLNode( IXMLDOMNode* pNode )
{
	if ( !BaseFlowController::CreateFromXMLNode(pNode) )
		return false;

	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();
	
	CString sAtr;
	if (! AppXMLHelper::GetNodeAttributeByName(pNode, "PULSEGEN", sAtr) )
	{
		str << "Address for pulse generator not set" << std::ends;
		err.SetLastError( DCE_DEVICE_ADDRESS_NOT_SET, str.str() );
		return false;
	}

	int curPos = 0;
	std::string genID = (LPCTSTR)sAtr.Tokenize(":", curPos);
	unsigned char  genCahnnel = (unsigned char) (atoi( (LPCTSTR)sAtr.Tokenize(":", curPos) ) );

	if (! AppXMLHelper::GetNodeAttributeByName(pNode, "POLYNOM", sAtr) )
	{
		str << "Polynom not set for nozzle frequency conversion" << std::ends;
		err.SetLastError( DCE_INVALID_DATA, str.str() );
		return false;
	}

	std::string polynom = (LPCTSTR)sAtr;

	unsigned long hiWidth = 50;
	if ( AppXMLHelper::GetNodeAttributeByName(pNode, "HIWIDTH", sAtr) )
		hiWidth = atoi((LPCTSTR)sAtr);

	return true;
}


//************************************
bool EMNozzle::Create(std::string name, std::string model,  std::string controledGas, std::string generatorID, unsigned char generatorChannel, std::string polynom, unsigned long hiWidth)
{
	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();

	std::map< std::string, double >::iterator fnd = emnMaxFlowByModel.find(model);
	if (  fnd ==  emnMaxFlowByModel.end() )
	{
		str << "Unknown nozzle model (" << model.c_str() << ")" << std::ends;
		err.SetLastError(DCE_UNKNOWN_DEVICE_TYPE, str.str() );
		return false;	
	}

	if( !BaseFlowController::Create(name, -1L, controledGas, (*fnd).second) )
		return false;

	pGenerator = dynamic_cast<ADAM6000DIDO*>( IdentifiedPhysDevice::GetDevice(generatorID) );

	if ( !pGenerator )
	{
		str << "Pulse generator (" << generatorID.c_str() << ") not found" <<  std::ends;
		err.SetLastError(DCE_DATA_PROVIDER_NOT_FOUND, str.str() );
		return false;	
	}

	if ( !pGenerator->SetupGenerator(generatorChannel, 0, 0) )
	{
		str << "Pulse generator (" << generatorID.c_str() << ") not response" <<  std::ends;
		err.SetLastError(DCE_MODULE_NOT_RESPOND, str.str() );
		return false;
	}

	if ( !GtoF.Create(name + "_gtf_","Конвертирование расхода газа в частоту срабатывания форсунки", polynom ) )
	{
		str << "Invalid conversion polynom" <<  std::ends;
		err.SetLastError(DCE_INVALID_DATA, str.str() );
		return false;
	}

	this->hiWidth = hiWidth;
	this->generatorChannel = generatorChannel;

	return true;
}

//возвращает текущее значение  расхода в единицах устройства
double EMNozzle::FlowGet() const
{
	return curFlow;
}

//задает текущий расход в единицах устройства
void EMNozzle::FlowSet(double newFlow)
{
	if (newFlow == 0)
	{
		pGenerator->SetupGenerator(generatorChannel, 0, 0);
		curFlow = 0;
		return;
	}
	
	double f = GtoF.Convert( newFlow );
	
	if ( f <= 0.0 )
	{
		pGenerator->SetupGenerator(generatorChannel, 0, 0);
		curFlow = 0;
		return;
	}
	
	unsigned long loWidth = unsigned long(1000.0 / f) - hiWidth;

	if ( pGenerator->SetupGenerator(generatorChannel, hiWidth, loWidth) )
		curFlow = newFlow;
}


//Интерфейс для устройств включающих в себя один или несколько 
//аналоговых выходов в которые записываются данные

//Получение общего числа аналоговых выходов 
int EMNozzle::GetChannelsCountAO() const
{
	return 1;
}

//Записать данные в аналоговый выход 
double  EMNozzle::WriteToChannel(unsigned char channel, double val)
{
	FlowSet(val);
	return curFlow;
}

//Записать данные во все каналы
void EMNozzle::WriteToAllChannels(const std::vector<double>& vals) 
{
	//для данного устройства функция ничего не делает
}

//Получить последнее записанное в аналоговый выход значение 
double EMNozzle::GetChannelStateAO (unsigned char channel) const
{
	return curFlow;
}

//Получить выходной диапазон канала 
void EMNozzle::GetMinMax(unsigned char channel, double& minOut, double& maxOut) const
{
	minOut = 0.0;
	maxOut = m_maxFlow;
}

