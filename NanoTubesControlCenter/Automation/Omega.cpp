#include "StdAfx.h"
#include "Omega.h"
#include "CommandManager.h"

#include "..\ah_errors.h"
#include "..\ah_xml.h"
#include "..\ah_msgprovider.h"


#ifdef _DEBUG
#define new DEBUG_NEW	
#endif

//  0 1       2       3       4       5     6     
//  A +014.70 +025.00 +02.004 +02.004 2.004 Air
const unsigned int MASS_FLOW_DATA_COLUMNS_COUNT = 7;


std::map< std::string, double> OmegaMFC::omegaMaxFlowByModel(dataOmega, dataOmega + MFC_MODELS_COUNT);


OmegaMFC::OmegaMFC() :
	BaseFlowController("Omega Engineering, Inc.", "MFC"),
	INPUT_ANALOG_CHANNELS_COUNT(6),
	OUTPUT_ANALOG_CHANNELS_COUNT(1),
	INPUT_DIGITAL_CHANNELS_COUNT(3),
	curGasIndex(GAS_UNKNOWN),
	mfcAdress('A'),	
	critDev(5.0),
	lpGas(NULL),
	pDataProvider(NULL)
{
	pLastData = new double[INPUT_ANALOG_CHANNELS_COUNT];
	memset(pLastData, 0, sizeof(double)*INPUT_ANALOG_CHANNELS_COUNT);

	rangesAI.push_back(MEASSURE_RANGE_INFO(0, "Давление газа", "PSIA", 0, 150	));
	rangesAI.push_back(MEASSURE_RANGE_INFO(1, "Температура", "С", 0, 70));
	rangesAI.push_back(MEASSURE_RANGE_INFO(2, "Расход измеренный", "л/мин", 0, 500));
	rangesAI.push_back(MEASSURE_RANGE_INFO(3, "Расход измеренный", "л/мин", 0, 500));
	rangesAI.push_back(MEASSURE_RANGE_INFO(4, "Расход заданный", "л/мин", 0, 500));
}		

OmegaMFC::~OmegaMFC(void)
{
	delete [] pLastData;
}

LPC_OMEGA_MFC_GAS_INFO OmegaMFC::GetGasInfo(std::string name)
{
	int count = sizeof(gases) / sizeof(gases[0]);
	std::string nameFnd = name;
	std::transform(nameFnd.begin(), nameFnd.end(), nameFnd.begin(), tolower);
	
	for (int i= 0; i < count; i++)
	{
		std::string nameCur = gases[i].szShortName;		
		std::transform(nameCur.begin(), nameCur.end(), nameCur.begin(), tolower);

		if ( nameCur == nameFnd )
			return &gases[i];
	}
	return NULL;
}

OmegaMFC* OmegaMFC::CreateFromXML(IXMLDOMNode* pNode, UINT& error )
{
	auto_ptr<OmegaMFC> obj ( new OmegaMFC() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DCE_OK;
	return obj.release();
}



bool OmegaMFC::CreateFromXMLNode(IXMLDOMNode* pNode)
{
	if (!IdentifiedPhysDevice::CreateFromXMLNode(pNode) )
		return false;

	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();
	const AppXMLHelper& appXML = appXML.Instance();

	std::string sAtr;
	if (! appXML.GetNodeAttributeByName(pNode, "GAS", sAtr) )
	{
		str << "Controlled gas not select" <<  std::ends;
		err.SetLastError(DCE_CONTROLED_GAS_NOT_SET, str.str());
		return false;
	}

	std::string gas = sAtr;

	if (! appXML.GetNodeAttributeByName(pNode, "ADDRESS", sAtr) )
	{
		str << "Address for device not set" << std::ends;
		err.SetLastError(DCE_DEVICE_ADDRESS_NOT_SET, str.str());
		return false;
	}

	int curPos = 0;
	CString strAtr = sAtr.c_str();
	std::string dataProvider = (LPCTSTR)strAtr.Tokenize(":", curPos);
	std::string address = (LPCTSTR)strAtr.Tokenize(":", curPos);

	if (appXML.GetNodeAttributeByName(pNode, "TYPE", sAtr) )
		m_strModel = sAtr;

	std::map< std::string, double >::iterator fnd = omegaMaxFlowByModel.find(m_strModel);
	if (  fnd ==  omegaMaxFlowByModel.end() )
	{
		str << "Unknown device type (" << m_strModel.c_str() << ")" << std::ends;
		err.SetLastError(DCE_UNKNOWN_DEVICE_TYPE, str.str() );
		return false;	
	}
	
	return Create(m_strName, m_strModel, m_nInterval, dataProvider, address[0], gas);
}

bool OmegaMFC::Create(std::string name, std::string model, unsigned long updateInterval, std::string dataProviderId, char address, std::string gas)
{
	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();

	std::map< std::string, double >::iterator fnd = omegaMaxFlowByModel.find(m_strModel);
	if (  fnd ==  omegaMaxFlowByModel.end() )
	{
		str << "Unknown device type (" << model.c_str() << ")" << std::ends;
		err.SetLastError(DCE_UNKNOWN_DEVICE_TYPE, str.str() );
		return false;	
	}
	
	if ( !BaseFlowController::Create(name, updateInterval, gas, (*fnd).second)  )
		return false;

	//максимальный расход
	pLastData[OMFC_AIC_NOMINAL_FLOW] = (*fnd).second;

	//Получение устройства сопряжения с сетью
	pDataProvider = dynamic_cast<IDataProvider*>( IdentifiedPhysDevice::GetDevice(dataProviderId) );
	if ( pDataProvider == NULL )
	{
		//номер COM порта (при непосредствернной связи с конторллером расхода)
		unsigned char port = (unsigned char)atoi( dataProviderId.c_str() );

		CommandManager *pMan = CommandManager::GetCommandManager(port);
		if ( pMan != NULL )
		{
			DCB dcb;
			pMan->GetComState(dcb);

			dcb.BaudRate = 38400;
			dcb.ByteSize = 8;		//длина байта данных
			dcb.fParity = FALSE;	//контроль четности
			dcb.StopBits = ONESTOPBIT; // число стоповых битов

			if ( !pMan->SetComState(dcb) )
			{
				str << "COM" << (int)port<< " initialization failed" << std::ends;
				err.SetLastError(DCE_PORT_NOT_AVAILABLE, str.str() );
				return false;
			}

		}

		pDataProvider = dynamic_cast<IDataProvider*>( pMan );

		if ( pDataProvider == NULL )
		{
			str << "Data provider (" << dataProviderId.c_str() << ") not found" <<  std::ends;
			err.SetLastError(DCE_DATA_PROVIDER_NOT_FOUND, str.str() );
			return false;
		}
	}

	if (address < 'A' || address > 'Z')
	{
		str << "Invalid address (" << (int)address << ")" << std::ends;
		err.SetLastError(DCE_INVALID_DEVICE_ADDRESS, str.str() );
		return false;
	}

	mfcAdress = address;
	lpGas = GetGasInfo(gas);

	if ( lpGas == NULL )
	{
		str << "Unknown controlled gas (" << gas.c_str() << ")"<< std::ends;
		err.SetLastError(DCE_INVALID_CONTROLED_GAS, str.str() );
		return false;
	}

	//Задание контролируемого газа
	if ( !SelectGas(lpGas->id) )
	{
		str << "Impossible to change controlled gas" <<  std::ends;
		err.SetLastError(DCE_MODULE_NOT_RESPOND, str.str() );
		return false;
	}

	ReadChannels();

	return true;
}


bool OmegaMFC::OnFailure()
{
	IdentifiedPhysDevice::OnFailure();

	if ( m_nFailures == 1 )
	{
		std::string msg;
		msg = "Device " + m_strName + " does not respond";
		
		AppMessagesProviderHelper::Instance().ShowPopupMessage(msg.c_str(), m_strModel.c_str(), COutMessage::MSG_WARNING );
	}
	if ( m_nFailures == 10 )
	{
		std::string msg;
		msg = "Device " + m_strName + " did not respond more then ten times. Check device power or connection...";
		
		m_bStatusOk = false;
		//BOOL b = ::SetEvent(m_hStopEvent);

		AppMessagesProviderHelper::Instance().ShowPopupMessage(msg.c_str(), m_strModel.c_str(), COutMessage::MSG_ERROR );
	}

	Sleep(1000);

	return false; 
}

void OmegaMFC::ControlProc()
{
	ReadChannels(NULL);
}

void OmegaMFC::OnExit()
{
	//TRACE("\n\t%s - OnExit", m_strName.c_str() );
}

//////////////////////////////////////////////////////////////////////////
// Реализация интерфейса IAnalogInput

//Получение данных со всех аналоговых входов (выполняется запрос на чтение данных)
void OmegaMFC::ReadChannels(double* data/*  = NULL*/) const
{

	//TODO:Оптимизировать разбор строки

	char szCmd[16];
	sprintf_s(szCmd, 16, "%c\r", mfcAdress);

	std::string respond;
	if ( pDataProvider != NULL )
		respond = pDataProvider->DataPass(this, szCmd, false);

	size_t endpos = respond.find(0x0D);
	if (endpos != string::npos)
		respond[endpos] = 0;

	if ( respond.empty() || respond == "FAILURE")
	{
		Lock();

		memset(pLastData, 0, sizeof(double)*(INPUT_ANALOG_CHANNELS_COUNT - 1));
		strActualGas = "";
		
		Unlock();
		
		//const_cast<OmegaMFC*>(this)->FailureCallback((void*)this);

		return;
	}
	
	//  [12/14/2015 Johnny A. Matveichik]
	//  A +014.70 +025.00 +02.004 +02.004 2.004 Air

	boost::char_separator<char> sep(" \t\n");
	boost::tokenizer< boost::char_separator<char> > tokens(respond, sep);
	std::vector< string > subs;

	

	for ( boost::tokenizer< boost::char_separator<char> >::iterator it = tokens.begin(); it != tokens.end(); ++it)
		subs.push_back( std::string(*it) );


	//неполный ответ
	if (subs.size() != MASS_FLOW_DATA_COLUMNS_COUNT ) 
	{
		//const_cast<OmegaMFC*>(this)->FailureCallback((void*)this);
		Unlock();
		return;
	}

	//несоответствие адреса контроллера
	if (subs[0][0] != mfcAdress) 
	{
		//const_cast<OmegaMFC*>(this)->FailureCallback((void*)this);
		Unlock();
		return;
	}

	Lock();
	
	for( int i = 0; i < INPUT_ANALOG_CHANNELS_COUNT - 1; i++ )
		pLastData[i] = atof ( subs[ i + 1 ].c_str() );
	
	//последняя колонка актуальный газ 
	strActualGas = subs[MASS_FLOW_DATA_COLUMNS_COUNT - 1];

	Unlock();
	//  [12/14/2015 Johnny A. Matveichik]

	//////////////////////////////////////////////////////////////////////////

	//Удаляем cимвол адреса  контроллера из ответа
	/*respond.erase(0, 1);

	const std::string delims = " \t\n";

	std::string::size_type begInd, endInd;
	int count = 0;

	begInd = respond.find_first_not_of(delims);
	while (begInd != std::string::npos )
	{
		endInd = respond.find_first_of(delims, begInd);
		
		if (endInd == std::string::npos)
			endInd = respond.length();

		if ( count == INPUT_ANALOG_CHANNELS_COUNT - 1)
		{
			Lock();
			strActualGas.assign(respond, begInd, endInd - begInd);
			Unlock();

			break;
		}
		else
		{

			std::string str;
			str.assign(respond, begInd, endInd - begInd);
			
			Lock();

			pLastData[count] = atof ( str.c_str() );

			Unlock();
		}

		count++;
		begInd = respond.find_first_not_of(delims, endInd);
	}
	*/
	
	if (data != NULL)
	{
		Lock();
		memcpy(data, pLastData, sizeof(double)*INPUT_ANALOG_CHANNELS_COUNT);
		Unlock();
	}
	


}

//Получение данных с одного из аналоговых входов (выполняется запрос на чтение данных)
double  OmegaMFC::ReadChannel(unsigned char channel)  const
{
	//ReadChannels(NULL);
	return GetChannelData(channel);
}

// Получение данных из буфера для одного из аналоговых входов 
// (запрос на чтение данных не выполняется)
double  OmegaMFC::GetChannelData(unsigned char channel)  const 
{
	double val; 
	Lock();

	val = pLastData[channel]; 

	Unlock();

	return val; 
};


//Получить  информацию об измеряемом диапазоне канала
void OmegaMFC::GetInputRangeInfo(unsigned char channel, LPMEASSURE_RANGE_INFO lpmci) const
{
	*lpmci =  rangesAI[channel];
}

//Установить  диапазон  аналогового входа
bool OmegaMFC::SetInputRange(unsigned char ch, unsigned char range)
{
	///ASSERT(FALSE);
	return false;
}

//Получить все возможные  диапазоны  измерения
void OmegaMFC::EnumInputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const
{
	rngs.push_back(rangesAI[ch]);
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//Реализация интерфейса IAnalogOutput

//Записать данные в аналоговый выход 
//Устанавливает новое значение массового расхода 
// channel - номер канала (для контроллера массовых расходов единственный 
//			 поэтому в функции не используется)
// val - значение нового расхода должно быть в л/мин
// Функция возвращает последнее записанное значение
double  OmegaMFC::WriteToChannel(unsigned char channel, double val) 
{
	double oldVal = GetChannelData(OMFC_AIC_SET_POINT);

	if (oldVal == val)
		return oldVal;

	
	std::size_t discrete = std::size_t(val * MAX_VAL_IN_DISCRETE / pLastData[OMFC_AIC_NOMINAL_FLOW]);

	//  [5/25/2010 Johnny A. Matveichik]
	if (discrete > MAX_VAL_IN_DISCRETE)
		discrete = MAX_VAL_IN_DISCRETE;

	if (discrete < MIN_VAL_IN_DISCRETE)
		discrete = MIN_VAL_IN_DISCRETE;

	char szCmd[16];
	sprintf_s(szCmd, 16, "%c%d\r", mfcAdress, discrete);

	std::string respond;

	if (pDataProvider != NULL )
		respond = pDataProvider->DataPass(this, szCmd, true);		

	if ( respond.empty() || respond == "FAILURE")
	{
		//FailureCallback(this);
		return oldVal;
	}
	Lock();
	pLastData[OMFC_AIC_SET_POINT] = val;
	Unlock();
	return oldVal;
}

//Записать данные во все каналы
void OmegaMFC::WriteToAllChannels(const std::vector<double>& vals) 
{
	if ( !vals.empty() )
		WriteToChannel(0, vals[0]);
	else
		WriteToChannel(0, 0.0);
}

//Получить последнее записанное в аналоговый выход значение 
double OmegaMFC::GetChannelStateAO (unsigned char channel) const
{
	//ReadChannels(NULL);
	return GetChannelData(OMFC_AIC_SET_POINT);
}

//--------------------------------------------------

//Обновить состояния дискретных входов	
bool OmegaMFC::UpdateChannelsStateDI() const
{
	//ReadChannels(NULL);
	return true;
}

//Получение состояния одного из дискретных входов	
bool  OmegaMFC::GetChannelStateDI(unsigned char channel, bool update) const
{
	if (update)
		UpdateChannelsStateDI();

	bool enable = false;
	switch (channel)
	{
		//Цифровой вход - расход газа задан 
		case OMFC_DI_CHANNEL_SET_POINT_NOT_NULL:
			enable = GetChannelData(OMFC_AIC_SET_POINT) != 0.0;
			break;

		//Цифровой вход - зафиксирован расход газа 
		case OMFC_DI_CHANNEL_FLOW_NOT_NULL:
			enable = GetChannelData(OMFC_AIC_MASS_FLOWRATE) != 0.0;
			break;

		//Цифровой вход - критичесакое отклонение заданного и зафиксированного расхода газа 
		case OMFC_DI_CHANNEL_FLOW_DEVIATION:
			{
				double setPt   = GetChannelData(OMFC_AIC_SET_POINT);
				
				if ( setPt == 0.0 )
				{
					enable = false;
					break;
				}

				double curFlow = GetChannelData(OMFC_AIC_MASS_FLOWRATE);
				double dev  = fabs(setPt - curFlow) * 100.0 / setPt;
				enable = ( dev > critDev );
			}		
			break;

		default:
			enable = false;

	}
	return enable;
}


//Получить  информацию об диапазоне канала
void OmegaMFC::GetOutputRangeInfo(unsigned char channel, LPMEASSURE_RANGE_INFO lpmci) const
{
	lpmci->desc = lpGas->szFullName;
	lpmci->units = "л/мин";
	lpmci->mins = 0.0;
	lpmci->maxs =  omegaMaxFlowByModel[m_strModel];
	lpmci->rngid = curGasIndex;

}

//Установить  диапазон  аналогового выхода
bool OmegaMFC::SetOutputRange(unsigned char ch, unsigned char range)
{
	if ( !SelectGas(range) )
		return false;
	
	return true;
}

//Получить все возможные  диапазоны  
void OmegaMFC::EnumOutputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const
{
	rngs.clear();
	for(int i = 0; i < sizeof(gases) / sizeof( OMEGA_MFC_GAS_INFO ); i++)
		rngs.push_back( MEASSURE_RANGE_INFO (gases[i].id, gases[i].szFullName, "л/мин", 0.0, omegaMaxFlowByModel[m_strModel]) );
}

//////////////////////////////////////////////////////////////////////////
/// Реализация специфичных функций

//Задать расход в nl/min
void OmegaMFC::FlowSet(double newFlow)
{
	WriteToChannel(0, newFlow);	
}


//Задать контролируемый газ (gasIndex - номер из таблицы газов)
bool OmegaMFC::SelectGas(int gasIndex)
{
	char szCmd[16];
	sprintf_s(szCmd, 16, "%c$$%d\r", mfcAdress, gasIndex);

	std::string respond;
	if ( pDataProvider != NULL )
		respond = pDataProvider->DataPass(this, szCmd, false);

	Lock();
	if ( respond.empty() || respond == "FAILURE")
	{		
		curGasIndex = GAS_UNKNOWN;
		Unlock();
		
		FailureCallback(this);

		return false;
	}	

	curGasIndex = gasIndex;
	
	Unlock();
	return true;
}

void OmegaMFC::ControledGas(OMEGA_MFC_GAS_INFO& gasInfo) const
{
	ReadChannels(NULL);
	gasInfo = gases[curGasIndex];
}