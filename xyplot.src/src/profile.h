#ifndef __PROFILE_H__
#define __PROFILE_H__

#include <vector>
#include <string>
#include "ProfileMarks.h"
#include "Axis.h"
#include <map>

typedef LONG_PTR PROFILE_KEY;

typedef std::map<PROFILE_KEY, Profile*> ProfileMap;
typedef ProfileMap::iterator ProfileMapIterator;
typedef ProfileMap::const_iterator ProfileMapConstIterator;

class LineData;

class Profile
{
public:

	static const int DEFAULTWIDTH;

	/*Конструктор по умолчанию*/
	//Profile();

	/*Конструктор с параметрами*/
	Profile( std::string name, long color, int width,  BOOL showLine, BOOL showMarks, const Axis& axisX, const Axis& axisY );

	~Profile();

	/*Назначить имя кривой*/
	void SetName(std::string name);

	/*Получить имя кривой*/
	inline const std::string& GetName() const { return name; }	

	/*Скрыть/показать кривую */
	inline void SetVisible(BOOL visible = TRUE ) { showLine = visible; }

	/*Отображается ли кривая*/
	inline BOOL IsVisible() const { return showLine; }

	/*Включить/выключить отображение символов*/
	inline void UseSymblos(BOOL symblols) { showMarks = symblols; }

	/*Отображются ли символы */
	inline BOOL IsUseSymblos() const { return showMarks; }

	void Show (BOOL bShow); 
	//
	long GetUnit(unsigned index, double &x, double &y) const;
	long GetData(unsigned* count, double *pfx, double* pfy) const;
	
	/*Установить новые данные для отображения кривой*/
	void SetData(const double *pfx, const double* pfy, int count );

	BOOL Empty() const { return dataPhysX.empty(); }

	/*Добавить точку к данным для отображения кривой*/
	void Append(double fx, double fy);

	/*Добавить несколько точек к данным для отображения кривой*/
	void Append(const double *pfx, const double *pfy, unsigned count);

	/*Количество точек для отображения кривой*/
	unsigned DataSize() const { return unsigned(dataPhysX.size()); }

	/*Установить тип линии для отображения кривой*/
	void SetLineData(const LineData& srcLine);

	/*Получить текщий тип линии*/
	inline LineData& GetLineData() { return line; }

	/*Получить физические пределы кривой по обеим осям*/
	void Range(double &xmin, double &xmax, double &ymin, double &ymax) const;

	/*Получить физические пределы кривой по оси X*/
	void RangeX(double &xmin, double &xmax) const;

	/*Получить физические пределы кривой по оси Y*/
	void RangeY(double &xmin, double &xmax) const;

	/*Отрисовать кривую*/
	void Render(HDC hDC, const RECT rect);
	
	/*Получить массив точек в координатах устройства*/
	const POINT* GetRenderPointArray(unsigned& count) const;

	/**/
	const POINT GetRenderPoint(int index, double& physX, double& physY) const;

	/*Получить объект меток*/
	ProfileMarks& GetMarks() { return marks; };

	/*Получить ось X к которой прикреплена кривая */
	Axis& AxisX() { return axisX; }; 

	/*Получить ось Y к которой прикреплена кривая */
	Axis& AxisY() { return axisY; }; 

	BOOL IsShowInLegend() const { return showInLegend; };

	void ShowInLegend(BOOL show) { showInLegend = show; };

	void GetVisibleRange(unsigned& firstIndex, unsigned& lastIndex) const { firstIndex = firstVisible; lastIndex = lastVisible; }

protected:

	/*Очистка данных для отображения кривой*/
	void Reset();

	std::string name;	// Имя кривой для отображения в легенде
	BOOL showMarks;		// Отображать или нет метки на кривой
	BOOL showLine;		// Отображать или нет линию кривой	

	LineData line;		// линия кривой
	
	Axis& axisX;		// ось X к которой привязан профиль
	Axis& axisY;		// ось Y к которой привязан профиль

	BOOL newData;			// флаг обновления данных 
	RECT lastRenderedRect;	// последний прямоугольник отрисовки

	mutable double minX;	// минимальное значение физической координаты X
	mutable double maxX;	// максимальное значение физической координаты X
	mutable double minY;	// минимальное значение физической координаты Y
	mutable double maxY;	// максимальное значение физической координаты Y

    ProfileMarks marks;		// метки кривой

	std::vector<POINT>  dataDevice;	//Массив точек в координатах устройства
	std::vector<double>	dataPhysX;	//Массив точек в физических координатах
	std::vector<double>	dataPhysY;	//Массив точек в физических координатах

	BOOL showInLegend;
	
	unsigned firstVisible;
	unsigned lastVisible;

	mutable CRITICAL_SECTION cs;
};

#endif