#if !defined(_DATA_CONVERTERS_H_INCLUDED_)
#define _DATA_CONVERTERS_H_INCLUDED_

#include <muParser.h>

class BaseConverter
{
public:
	
	BaseConverter();
	virtual ~BaseConverter();

	bool Create(std::string id, std::string description, double minV = DBL_MIN, double maxV = DBL_MAX);

	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

public:

	//��������������
	virtual double Convert(double inpValue) const = 0;

	//�������� ��������������
	virtual double RevConvert(double y) const = 0;
	

	//��������� ������������� 
	virtual std::string ToString(const char x='x', const char y='y') const = 0;
	
	//��������
	std::string Description() const;

	//�������������� ��������
	virtual void GetMinMax(double& minV, double& maxV ) const;

public:

	//����������� �������
	static bool AddConverter( BaseConverter* conv );

	static const BaseConverter* GetConverter(std::string id);

	static void ClearAll();


protected:
	
	static std::map<std::string, const BaseConverter*> mapConv; 
	
	std::string m_strDesc;
	std::string m_strID;

	double m_dMin;
	double m_dMax;	
};
  

/********************************/
/*   class LinearConverter		*/
/********************************/
class LinearConverter : public BaseConverter
{
public:
	
	LinearConverter();	
	virtual ~LinearConverter();

	static LinearConverter* CreateFromXML(IXMLDOMNode* pNode, UINT& nError);

	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

public:

	//������������� ������� ��������
	virtual double Convert (double x) const;

	//�������� ��������������
	virtual double RevConvert(double y) const;

	//��������� ������������� 
	virtual std::string ToString(const char x='x', const char y='y') const;

			
	// ���������� �������� ������������� k � b ������
	// ��������� ����� ��� ����� 
	bool Create(std::string id, std::string description, double x1, double y1, double x2, double y2 );

	// y = k*x + b
	// �������� �������� ������������� k � b ������ 
	void GetLinear(double& dK, double &dB) const { dK = m_dK; dB = m_dB; };
	
protected:

	// y = k*x + b
	// ����������� ������� ������
	double m_dK;	
	
	//	y = k*x + b
	// �������� ������ ������������ ������� �����
	double m_dB;

};

/********************************/
/*   class PolynomialConverter	*/
/********************************/
class PolynomialConverter : public BaseConverter 
{

public:

	PolynomialConverter();
	virtual ~PolynomialConverter();

	static PolynomialConverter* CreateFromXML(IXMLDOMNode* pNode, UINT& nError);
	
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

public:

	virtual double Convert (double inpValue) const;

	virtual double RevConvert(double y) const;

	//������� �������� ��� ������ ������� �������������� �������������
	bool Create(std::string id, std::string description, const double* pdPoly, int nOrder);

	//������� �������� ��� ������ ������� �������������� �������������
	bool Create(std::string id, std::string description, std::vector<double> vdPoly);


	//������� �������� ��� ������ ������ � �������������
	//������ ������ ��������� ������������ �������� �� ������� ������� 
	//� ������� ������� ����������� ����� �� ��������� ������������  ���������, ������,
	//���  ;:,!|
	bool Create(std::string id, std::string description, std::string strPolynom);


	//������� ��������� ��������� ��������	
	virtual std::string ToString(const char x='x', const char y='y') const;

protected:

	//������� �������� ��� ������ ������� �������������� �������������
	bool SetPolynom(const double* pdPoly, int nOrder); 

	//������� �������� ��� ������ ������� �������������� �������������
	bool SetPolynom(std::vector<double> vdPoly);

	//������� �������� ��� ������ ������ � �������������
	//������ ������ ��������� ������������ �������� �� ������� ������� 
	//� ������� ������� ����������� ����� �� ��������� ������������  ���������, ������,
	//���  ;:,!|
	bool SetPolynom(std::string strPolynom);
	

protected:

	std::vector <double> m_vPolynom;
	std::map<double, double> rtable;
};

class PolynomialNegativeConverter : public PolynomialConverter 
{

public:

	PolynomialNegativeConverter();
	virtual ~PolynomialNegativeConverter();

	static PolynomialNegativeConverter* CreateFromXML(IXMLDOMNode* pNode, UINT& nError);
	
	
public:

	virtual double Convert (double inpValue) const;

	//������� ��������� ��������� ��������	
	virtual std::string ToString(const char x='x', const char y='y') const;

protected:


};

class MultiRangeConverter : public BaseConverter 
{

public:

	MultiRangeConverter();
	virtual ~MultiRangeConverter();

	static MultiRangeConverter* CreateFromXML(IXMLDOMNode* pNode, UINT& nError);
	
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

public:

	virtual double Convert (double inpValue) const;

	virtual double RevConvert(double y) const;

	//������� ��������� ��������� ��������	
	virtual std::string ToString(const char x='x', const char y='y') const;

protected:
	
	const BaseConverter* Find(double x) const;
	
	std::vector <const BaseConverter*> m_vSubConv;
	std::map<double, double> rtable;
};


class double_equal
{
public:
	double_equal(double val, double e = 1.0e-5) : v(val), eps(e)
	{
	}
	bool operator() (std::pair<const double, double> elem);
	

private :
	double v;
	double eps;
};

class TableConverter : public BaseConverter
{
public:
	
	
	TableConverter();
	virtual ~TableConverter();

	static TableConverter* CreateFromXML(IXMLDOMNode* pNode, UINT& nError);

	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

	//��������������
	virtual double Convert(double inpValue) const;

	//�������� ��������������
	virtual double RevConvert(double inpValue) const;

	//��������� ������������� 
	virtual std::string ToString(const char x='x', const char y='y') const;
	
	//�������������� ��������
	virtual void GetMinMax(double& minV, double& maxV ) const;

private:

	bool LoadData(const char* pszFileName);
	std::map<double, double> tbl;
	std::map<double, double> tblr;
	double defEps;
};



class FormulaConverter : public BaseConverter
{
public :

	FormulaConverter();
	virtual ~FormulaConverter();

	static FormulaConverter* CreateFromXML(IXMLDOMNode* pNode, UINT& nError);

	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

	//��������������
	virtual double Convert(double inpValue) const;

	//�������� ��������������
	virtual double RevConvert(double y) const;

	//��������� ������������� 
	virtual std::string ToString(const char x='x', const char y='y') const;

	//�������������� ��������
	virtual void GetMinMax(double& minV, double& maxV ) const;

private :

	mutable double x;
	mu::Parser parser;
};

#endif //_DATA_CONVERTERS_H_INCLUDED_