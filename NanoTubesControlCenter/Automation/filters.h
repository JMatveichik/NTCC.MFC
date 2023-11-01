#if !defined(_DATA_FILTERS_H_INCLUDED_)
#define _DATA_FILTERS_H_INCLUDED_

class AnalogInputDataSrc;
class BaseFilter
{
public:

	BaseFilter();
	virtual ~BaseFilter();

	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);
	
	virtual bool InitFilter(const AnalogInputDataSrc* pDS) { return true; };

public:

	//��������������
	virtual double Correct(double val) const = 0;

	//��������
	std::string Description() const;

	//�������������
	std::string Name() const;
public:

	//����������� �������
	//�������� ������
	static bool AddFilter( BaseFilter* filter );

	//�������� ������ �� ��������������
	static const BaseFilter* GetFilter(std::string id);

	//������� ��� �������
	static void ClearAll();


protected:

	static std::map<std::string, const BaseFilter*> mapFilters; 

	std::string m_strDesc;
	std::string m_strID;
};


/*
F � ���������� ����������� �������� �������, � ������ � �������� � ��� ����� ���� 
����������� ������������ ������ ������� �� �������� �������� �� ����� ������������� 
(����� ����� ������ ���������). ������ ������ ������� �������, ���������� ��� � �������� 
������� ��� �������� ������ ��� ���������� ������ 1 (�� ���� �� ���������, ��� ��������������� 
�������� ����� ����� ����������� ���������).

B � ���������� ������������ ���������� ������������ �����������. ���� �� � ��� ���� 
�������������� ���������� �� �������� ��������� ��� ������� ������� �� ������ ������������, 
�� ���� �������� �� ��������� ��� ��������� ������ ������� �� ����� �������������. 
��� ��� ����������� ����������� � ����� ������ ��� (��� ���������� � ���), �� ��������� B = 0.

H � ������� ������������ ��������� ����� ����������� � ���������� �������,
���� ��� ���������� ������ ��� ���������� ����� ������ 1.

����������� ������������ �������

R � ������ ��������� ����� ���� ���������� ���������� ������������� �������� � 
������������ ����������� �� ���������.

Q � ����������� ���� �������� �������� ����� ������� �������, ��� ��� ��������� 
���������� ��������� ��������, ��� �� ������ ��������. � ����� ������, ����� ��������� 
���� �������� ��� ����������� ���������� ������ ����������.
*/

class KalmanFilter : public BaseFilter
{	

public: 	

	KalmanFilter();

	KalmanFilter(double q, double r, double f = 1.0, double h = 1.0);
	virtual ~KalmanFilter();

	//�������� ������� ������ �� XML ����
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

	//�������� ������� ������ �� XML ����
	static KalmanFilter* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	///
	virtual bool InitFilter(const AnalogInputDataSrc* pDS);

public:

// /*
// 
// 	// predicted state
// 	double GetX0() {return X0;} 
// 	
// 	// predicted covariance
// 	double GetP0() {return P0;} 
// 
// 	// factor of real value to previous real value
// 	double GetF() { return F; } 
// 	
// 	// measurement noise
// 	double GetQ() { return Q; } 
// 
// 	// factor of measured value to real value
// 	double GetH() { return H; } 
// 
// 	// environment noise
// 	double GetR() { return R; } 
// 
// 	
// 	double GetS() { return S; }
// 
// 	double GetC() { return C; }
// 
// 	void SetState(double s, double c);	*/

	virtual double Correct(double val) const;	

private:

	mutable double X0;
	mutable double P0;

	double Q;
	double R;
	double F;
	double H;

	mutable double S;
	mutable double C;
};

class AverageFilter : public BaseFilter
{	

public: 	

	AverageFilter();
	virtual ~AverageFilter();

	//�������� ������� ������ �� XML ����
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

	//�������� ������� ������ �� XML ����
	static AverageFilter* CreateFromXML(IXMLDOMNode* pNode, UINT& error);

	///
	virtual bool InitFilter(const AnalogInputDataSrc* pDS);

public:

	virtual double Correct(double val) const;	

private:

	mutable std::deque <double> databuffer;
	int dbcapacity;
};

#endif //_DATA_FILTERS_H_INCLUDED_