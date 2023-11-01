#pragma once

#include "BaseFlowController.h"
#include "Converters.h"

class ADAM6000DIDO;
//������������ ������� �� �������� ��������
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

	//���������� ������� ��������  ������� � �������� ����������
 	virtual double FlowGet() const;

	//������ ������� ������ � �������� ����������
	virtual void FlowSet(double newFlow);

	
	
	//��������� ��� ��������� ���������� � ���� ���� ��� ��������� 
	//���������� ������� � ������� ������������ ������

	//��������� ������ ����� ���������� ������� 
	virtual int GetChannelsCountAO() const;

	//�������� ������ � ���������� ����� 
	virtual double  WriteToChannel(unsigned char channel, double val);

	//�������� ������ �� ��� ������
	virtual void WriteToAllChannels(const std::vector<double>& vals) ; 

	//�������� ��������� ���������� � ���������� ����� �������� 
	virtual double GetChannelStateAO (unsigned char channel) const;

	//�������� �������� �������� ������ 
	virtual void GetMinMax(unsigned char channel, double& minOut, double& maxOut) const;



protected:

	
	//************************************
	virtual bool EMNozzle::Create(std::string name, std::string model,  std::string controledGas, std::string generatorID, unsigned char generatorChannel, std::string polynom, unsigned long hiWidth);


	//������� �������� ���������� ���������� ����� �������� �������� �������
	virtual void ControlProc(){};

protected :

	//������������ ������ �� ������� ������������ �������
	static std::map< std::string, double > emnMaxFlowByModel;

	//��������� ���������
	ADAM6000DIDO* pGenerator;

	//����� ���������� ���������
	unsigned char generatorChannel;
	
	//��������������� ������� � �������
	PolynomialConverter GtoF; 

	//������� ������
	double curFlow;

	//������������ ��������� ��������� ��������
	unsigned long hiWidth; 
};

