#if !defined(_LNP_BASE_FLOW_CONTROLLER_H_INCLUDED_)
#define _LNP_BASE_FLOW_CONTROLLER_H_INCLUDED_

#include "PhysDevice.h"
#include "common.h"

//////////////////////////////////////////////////////////////////////////
//  [3/22/2011 Johnny A. Matveichik]
//  Все наследники BaseFlowController должны реализовать интерфейсы 
//	IAnalogInput для измерительной части
//  IAnalogOutput для управляющей части
//  
//	для регулярного обновления информации о текущем состоянии реализовать функцию
//  virtual void ControlProc()
class BaseFlowController :	public IdentifiedPhysDevice, public IAnalogOutput //, public IAnalogInput, 
{

public:
	
	BaseFlowController();
	virtual ~BaseFlowController();

	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);
		

public: 

	//возвращает текущее значение  расхода в единицах устройства
 	virtual double FlowGet() const = 0;

	//задает текущий расход в единицах устройства
 	virtual void FlowSet(double newFlow) = 0;

	//возвращает максимальный расход  в единицах устройства
	double FlowMaxGet() const 
	{ 
		return m_maxFlow; 
	};

	//получить имя контролируемого газа
	std::string ControledGas() const 
	{ 
		return m_strGas; 
	} 

	
	//!!! Для любых контроллеров номер выходного аналогового
	// канала для задания расхода должен быть равен 0
	//const unsigned char FC_AO_CHANNEL_FLOW;


public:
	  

protected:

	BaseFlowController(std::string manufact, std::string model);
	//************************************
	// Method:    Create
	// FullName:  BaseFlowController::Create
	// Access:    public 
	// Returns:   bool
	// Qualifier:
	// Parameter: std::string name - уникальное имя для идентификации устройства
	// Parameter: std::string controledGas - контролируемый газ
	// Parameter: double maxFlow - максимальный расход в единицах устройства
	// Parameter: bool bIsVolFlow - флаг контроля объемного расхода расхода
	//************************************
	virtual bool Create(std::string name, unsigned long updateInterval, std::string controledGas, double maxFlow);

	//максимальный расход
	double m_maxFlow;	
	

	//контролируемый газ 
	std::string m_strGas;	

private:
	

};


class TestFlowController : public BaseFlowController
{

public:
	TestFlowController();
	virtual ~TestFlowController();

	//возвращает текущее значение  расхода в единицах устройства
	virtual double FlowGet() const { return m_dCurFlow; } ;

	//задает текущий расход в единицах устройства
	virtual void FlowSet(double newFlow) { m_dCurFlow = newFlow; };

private:
	virtual void ControlProc() { };
	double m_dCurFlow;
};

#endif //_LNP_BASE_FLOW_CONTROLLER_H_INCLUDED_