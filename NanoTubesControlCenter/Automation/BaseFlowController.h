#if !defined(_LNP_BASE_FLOW_CONTROLLER_H_INCLUDED_)
#define _LNP_BASE_FLOW_CONTROLLER_H_INCLUDED_

#include "PhysDevice.h"
#include "common.h"

//////////////////////////////////////////////////////////////////////////
//  [3/22/2011 Johnny A. Matveichik]
//  ��� ���������� BaseFlowController ������ ����������� ���������� 
//	IAnalogInput ��� ������������� �����
//  IAnalogOutput ��� ����������� �����
//  
//	��� ����������� ���������� ���������� � ������� ��������� ����������� �������
//  virtual void ControlProc()
class BaseFlowController :	public IdentifiedPhysDevice, public IAnalogOutput //, public IAnalogInput, 
{

public:
	
	BaseFlowController();
	virtual ~BaseFlowController();

	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);
		

public: 

	//���������� ������� ��������  ������� � �������� ����������
 	virtual double FlowGet() const = 0;

	//������ ������� ������ � �������� ����������
 	virtual void FlowSet(double newFlow) = 0;

	//���������� ������������ ������  � �������� ����������
	double FlowMaxGet() const 
	{ 
		return m_maxFlow; 
	};

	//�������� ��� ��������������� ����
	std::string ControledGas() const 
	{ 
		return m_strGas; 
	} 

	
	//!!! ��� ����� ������������ ����� ��������� �����������
	// ������ ��� ������� ������� ������ ���� ����� 0
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
	// Parameter: std::string name - ���������� ��� ��� ������������� ����������
	// Parameter: std::string controledGas - �������������� ���
	// Parameter: double maxFlow - ������������ ������ � �������� ����������
	// Parameter: bool bIsVolFlow - ���� �������� ��������� ������� �������
	//************************************
	virtual bool Create(std::string name, unsigned long updateInterval, std::string controledGas, double maxFlow);

	//������������ ������
	double m_maxFlow;	
	

	//�������������� ��� 
	std::string m_strGas;	

private:
	

};


class TestFlowController : public BaseFlowController
{

public:
	TestFlowController();
	virtual ~TestFlowController();

	//���������� ������� ��������  ������� � �������� ����������
	virtual double FlowGet() const { return m_dCurFlow; } ;

	//������ ������� ������ � �������� ����������
	virtual void FlowSet(double newFlow) { m_dCurFlow = newFlow; };

private:
	virtual void ControlProc() { };
	double m_dCurFlow;
};

#endif //_LNP_BASE_FLOW_CONTROLLER_H_INCLUDED_