#include "StdAfx.h"
#include "InfracarGasAnalyser.h"
#include "CommandManager.h"

#include "..\ah_errors.h"
#include "..\ah_msgprovider.h"
#include "..\ah_xml.h"

char crc(LPASCII_COMMAND pCmd, int count)
{
	char cr = 0x00;
	for (int i = 1; i < 10; i++)
		cr += pCmd->cmd[i];

	cr = cr & 0xFF;
	return cr;
}

InfracarGasAnalyser::InfracarGasAnalyser(void) :
IdentifiedPhysDevice("Инфракар", "INFRACAR-M"),
INPUT_ANALOG_CHANNELS_COUNT(7)
{
	ranges.push_back(MEASSURE_RANGE_INFO(0, "Концентрация угарного газа (CO)", "%", 0, 30));
	ranges.push_back(MEASSURE_RANGE_INFO(1, "Концентрация углеводородов (CH)", "ppm", 0, 30));
	ranges.push_back(MEASSURE_RANGE_INFO(2, "Концентрация углекислого газа (CO2)", "%", 0, 30));
	ranges.push_back(MEASSURE_RANGE_INFO(3, "Концентрация кислорода (O2)", "%", 0, 30));
	ranges.push_back(MEASSURE_RANGE_INFO(4, "Частота вращения вала", "об/мин", 0, 30));
	ranges.push_back(MEASSURE_RANGE_INFO(5, "Лямбда", "об/мин", 0, 30));
	ranges.push_back(MEASSURE_RANGE_INFO(6, "Концентрация окислов азота (NOx)", "ppm", 0, 30));

}


InfracarGasAnalyser::~InfracarGasAnalyser(void)
{
	//ranges.clear();
}

bool InfracarGasAnalyser::CreateFromXMLNode(IXMLDOMNode* pNode)
{
	if ( !IdentifiedPhysDevice::CreateFromXMLNode(pNode) )
		return false;

	std::strstream str;
	AppErrorsHelper& err = AppErrorsHelper::Instance();
	const AppXMLHelper& appXML = appXML.Instance();

	std::string sAtr;
	if (!appXML.GetNodeAttributeByName(pNode, "PORT", sAtr))
	{
		str << "Port number for device not set (tag <PORT>)" << std::ends;
		err.SetLastError(DCE_PORT_NUMBER_NOT_SET, str.str() );
		return false;	
	}

	int port =  atoi(sAtr.c_str());
	CommandManager* pMan = CommandManager::GetCommandManager(port);
	if ( pMan == NULL )
	{
		str << "Command manager assigned with port COM" << (int)port << " not found" << std::ends;
		err.SetLastError( DCE_PORT_NOT_AVAILABLE, str.str() );
		return false;
	}
	comPortNo = port;
	
	/*
	0xCA,0x00,0x05,0x07,0x00,0x00,0x03,0x05,0x00,0xFF,0x13
	
		Ответ газоанализатора:
	0xFA,0xFF,0x05,0x07,0x02,0x00,0x11,0x05,0xC3,0xC0,0xC7,0xCE,0xC0,0xCD,0xC0,0xCB,
		0xC8,0xC7,0xC0,0xD2,0xCE,0xD0,0x00,0x02,0x14
		В поле результат – тип устройства 0x02, в поле данных наименование устройства ‘ГАЗОАНАЛИЗАТОР ИНФРАКАР-М’.
	*/

	ASCII_COMMAND aCmd;
	aCmd.pSender = this;
	
	aCmd.cmd[0] = char(0xCA); //0xCA – идентификатор запроса
	aCmd.cmd[1] = char(0x00);	//0x00 – Неизвестное устройство
	aCmd.cmd[2] = char(0x05); //0x05 – Модуль системных данных
	aCmd.cmd[3] = char(0x07); //0x07 – идентификатор функции вывода значения переменной
	aCmd.cmd[4] = char(0x00); //0x00 – поле результата функции
	aCmd.cmd[5] = char(0x00); //0x00 – поле результата функции
	aCmd.cmd[6] = char(0x03); //0x03 – Количество байт поля данных
	aCmd.cmd[7] = char(0x05); //0x05 – Индекс типа устройства	
	aCmd.cmd[8] = char(0x00); //0x00 – Разделитель индексов
	aCmd.cmd[9] = char(0xFF); //0xFF – Идентификатор устройства выдающего запрос (ПК)
	aCmd.cmd[10] = crc(&aCmd, 10); // – Контрольная сумма
	
	aCmd.length = 11;

	if ( pMan->SendCommand(&aCmd, false) )
	{
		int len = aCmd.length;
		TRACE("\nОТВЕТ %d байт\n", len );
	}
	/*
	0xFA - ъ			//передается ответ от устройства. контрольный пакет
	0x05 - 			//ДРУГИЕ УСТРОЙСТВА
	0x07 - 			//ТИП ЗАПРАШИВАЕМОГО МОДУЛЯ
	0x02 - 			//Идентификатор ФУНКЦИИ
	0x00 -			//младший байт результата функции
	0x23 - #			//старший байт результата функции
	0x05 - 			//размер раздела данных
	0xC3 - Г
	0xC0 - А
	0xC7 - З
	0xCE - О
	0xC0 - А
	0xCD - Н
	0xC0 - А
	0xCB - Л
	0xC8 - И
	0xC7 - З
	0xC0 - А
	0xD2 - Т
	0xCE - О
	0xD0 - Р
	0x20 -  
	0xCC - М
	0xCD - Н
	0xCE - О
	0xC3 - Г
	0xCE - О
	0xCA - К
	0xCE - О
	0xCC - М
	0xCF - П
	0xCE - О
	0xCD - Н
	0xC5 - Е
	0xCD - Н
	0xD2 - Т
	0xCD - Н
	0xDB - Ы
	0xC9 - Й
	0x00 - 
	0x02 - 
	0xE1 - б
	*/
	unsigned char res[1024];
	memcpy(res, aCmd.response, aCmd.length);

	for(int i =0; i < aCmd.length; i++)
	{
		TRACE("\n0x%02X - %c", res[i], res[i]);
	}
	return true;

}

InfracarGasAnalyser* InfracarGasAnalyser::CreateFromXML(IXMLDOMNode* pNode, UINT& error)
{
	auto_ptr<InfracarGasAnalyser> obj ( new InfracarGasAnalyser() );

	if( !obj->CreateFromXMLNode(pNode) )
	{
		AppErrorsHelper::Instance().GetLastError(error);
		return NULL;
	}

	error = DCE_OK;
	return obj.release();
}

//Получение данных со всех аналоговых входов (выполняется запрос на чтение данных)
void	InfracarGasAnalyser::ReadChannels(double* data /* = NULL*/) const
{
	for (int i = 0; i < INPUT_ANALOG_CHANNELS_COUNT; i++)
	{
		data[i] = ReadChannel(i);		
	}
	
}

//Получение данных с одного из аналоговых входов (выполняется запрос на чтение данных)
double  InfracarGasAnalyser::ReadChannel(unsigned char channel)  const
{
	/*
	Модуль ИЗМЕРИТЕЛЬ (0x2A) содержит подмодули: 
	CO (1),CH(2),CO2(3), O2(4), n(5), tt(6), Nox(7).
	Пример:
	Запрос переменной содержащей результат измерения CO
	0xCA,0x02,0x2A,0x07,0x00,0x00,0x04,0x01,0x01,0x00,0xFF,0x38
	
	Ответ устройства
	0xFA,0xFF,0x2A,0x07,0x2C,0x01,0x09,0x01,0x01,0x33,0x2E,0x30,0x30,0x31,0x00,0x02, 0x5C
	*/
	
// 		26355 30/09/2014 09:25:42 IRP_MJ_WRITE UP STATUS_SUCCESS ca 00 2a 07 45 00 03 01 01 ff 7a  К.*.E....яz 11 
// 		26369 30/09/2014 09:25:42 IRP_MJ_WRITE UP STATUS_SUCCESS ca 00 2a 07 45 00 03 02 01 ff 7b  К.*.E....я{ 11 
// 		26387 30/09/2014 09:25:42 IRP_MJ_WRITE UP STATUS_SUCCESS ca 00 2a 07 45 00 03 03 01 ff 7c  К.*.E....я| 11 

	CommandManager* pMan = CommandManager::GetCommandManager(comPortNo);
	ASCII_COMMAND aCmd;
	aCmd.pSender = this;
	aCmd.cmd[0] = char(0xCA); //0xCA – идентификатор запроса
	aCmd.cmd[1] = char(0x00);	//0x02 – идентификатор газоанализатора ИНФРАКАР-М
	aCmd.cmd[2] = char(0x2A); //0x2A – идентификатор модуля измерителя
	aCmd.cmd[3] = char(0x07); //0x07 – идентификатор функции вывода значения переменной
	aCmd.cmd[4] = char(0x45); //0x00 – поле результата функции
	aCmd.cmd[5] = char(0x00); //0x00 – поле результата функции
	aCmd.cmd[6] = char(0x03); //0x04 – Количество байт поля данных
	aCmd.cmd[7] = char(channel + 1); //0x01 – Индекс подмодуля канала (channel)
	aCmd.cmd[8] = char(0x01); //0x01 – Индекс поля результат измерения
	aCmd.cmd[9] = char(0xFF); //0xFF – Идентификатор устройства выдающего запрос (ПК)
	aCmd.cmd[10] = crc(&aCmd, 10); // – Контрольная сумма
						
	aCmd.length = 11;
	
	unsigned char res[32];

	if ( pMan->SendCommand(&aCmd, false) )
	{
		int byteNo = 0;
		int dataStartByteNo = 8;
		for(int i = dataStartByteNo; i < aCmd.length; i++)
		{
			res[byteNo++] = aCmd.response[i];
			if ( aCmd.response[i] == 0)
				break;
		}
	}
	return atof((const char*)res);
}

// Получение данных из буфера для одного из аналоговых входов 
// (запрос на чтение данных не выполняется)
double  InfracarGasAnalyser::GetChannelData(unsigned char channel)  const
{
	return data[channel];
}

//Получить  информацию об измеряемом диапазоне канала
void InfracarGasAnalyser::GetInputRangeInfo(unsigned char channel, LPMEASSURE_RANGE_INFO lpmci) const
{
	*lpmci =  ranges[channel];
}

//Установить  диапазон  аналогового входа
bool InfracarGasAnalyser::SetInputRange(unsigned char ch, unsigned char range)
{
	return false;
}

//Получить все возможные  диапазоны  измерения
void InfracarGasAnalyser::EnumInputRanges(unsigned char ch, std::vector<MEASSURE_RANGE_INFO>& rngs) const
{
	rngs.push_back(ranges[ch]);
}

//Функция процесса выполяется циклически через заданный интервал времени
void InfracarGasAnalyser::ControlProc()
{
	ReadChannels(data);
}