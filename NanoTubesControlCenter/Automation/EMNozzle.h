#pragma once

#include "BaseFlowController.h"
#include "Converters.h"

class ADAM6000DIDO;
//максимальные расходы по моделями форсунки
const std::map< std::string, double >::value_type dataEMN[] = 
{
	std::pair<std::string, double>("EMN-001", 3.7),
	std::pair<std::string, double>("EMN-002", 4.0)
};
const unsigned int EMN_MODELS_COUNT = sizeof(dataEMN)/sizeof(dataEMN[0]);

class EMNozzle : public BaseFlowController
{

public:

	EMNozzle(void);
	virtual ~EMNozzle(void);

	bool CreateFromXMLNode( IXMLDOMNode* pNode );

	static EMNozzle* CreateFromXML(IXMLDOMNode* pNode, UINT& error );

public:

	//возвращает текущее значение  расхода в единицах устройства
 	virtual double FlowGet() const;

	//задает текущий расход в единицах устройства
	virtual void FlowSet(double newFlow);

	
	
	//Интерфейс для устройств включающих в себя один или несколько 
	//аналоговых выходов в которые записываются данные

	//Получение общего числа аналоговых выходов 
	virtual int GetChannelsCountAO() const;

	//Записать данные в аналоговый выход 
	virtual double  WriteToChannel(unsigned char channel, double val);

	//Записать данные во все каналы
	virtual void WriteToAllChannels(const std::vector<double>& vals) ; 

	//Получить последнее записанное в аналоговый выход значение 
	virtual double GetChannelStateAO (unsigned char channel) const;

	//Получить выходной диапазон канала 
	virtual void GetMinMax(unsigned char channel, double& minOut, double& maxOut) const;



protected:

	
	//************************************
	virtual bool EMNozzle::Create(std::string name, std::string model,  std::string controledGas, std::string generatorID, unsigned char generatorChannel, std::string polynom, unsigned long hiWidth);


	//Функция процесса выполяется циклически через заданный интервал времени
	virtual void ControlProc(){};

protected :

	//максимальный расход по моделям контроллеров расхода
	static std::map< std::string, double > emnMaxFlowByModel;

	//генератор импульсов
	ADAM6000DIDO* pGenerator;

	//канал генератора импульсов
	unsigned char generatorChannel;
	
	//преобразователь расхода в частоту
	PolynomialConverter GtoF; 

	//текущий расход
	double curFlow;

	//длительность закрытого состояния форсунки
	unsigned long hiWidth; 
};

