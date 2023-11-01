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

	//Преобразование
	virtual double Correct(double val) const = 0;

	//Описание
	std::string Description() const;

	//идентификатор
	std::string Name() const;
public:

	//СТАТИЧЕСКИЕ ФУНКЦИИ
	//Добавить фильтр
	static bool AddFilter( BaseFilter* filter );

	//Получить фильтр по идентификатору
	static const BaseFilter* GetFilter(std::string id);

	//удалить все фильтры
	static void ClearAll();


protected:

	static std::map<std::string, const BaseFilter*> mapFilters; 

	std::string m_strDesc;
	std::string m_strID;
};


/*
F — переменная описывающая динамику системы, в случае с топливом — это может быть 
коэффициент определяющий расход топлива на холостых оборотах за время дискретизации 
(время между шагами алгоритма). Однако помимо расхода топлива, существуют ещё и заправки… 
поэтому для простоты примем эту переменную равную 1 (то есть мы указываем, что предсказываемое 
значение будет равно предыдущему состоянию).

B — переменная определяющая применение управляющего воздействия. Если бы у нас были 
дополнительная информация об оборотах двигателя или степени нажатия на педаль акселератора, 
то этот параметр бы определял как изменится расход топлива за время дискретизации. 
Так как управляющих воздействий в нашей модели нет (нет информации о них), то принимаем B = 0.

H — матрица определяющая отношение между измерениями и состоянием системы,
пока без объяснений примем эту переменную также равную 1.

Определение сглаживающих свойств

R — ошибка измерения может быть определена испытанием измерительных приборов и 
определением погрешности их измерения.

Q — определение шума процесса является более сложной задачей, так как требуется 
определить дисперсию процесса, что не всегда возможно. В любом случае, можно подобрать 
этот параметр для обеспечения требуемого уровня фильтрации.
*/

class KalmanFilter : public BaseFilter
{	

public: 	

	KalmanFilter();

	KalmanFilter(double q, double r, double f = 1.0, double h = 1.0);
	virtual ~KalmanFilter();

	//создание фильтра данных из XML узла
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

	//создание фильтра данных из XML узла
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

	//создание фильтра данных из XML узла
	virtual bool CreateFromXMLNode(IXMLDOMNode* pNode);

	//создание фильтра данных из XML узла
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