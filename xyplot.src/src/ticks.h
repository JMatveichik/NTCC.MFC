#ifndef __TICKS_H_INCLUDED__
#define __TICKS_H_INCLUDED__

#include "SavableItem.h"
#include <vector>

const unsigned long  TICKS_NONE     = 0x0000;
const unsigned long  TICKS_OUTSIDE  = 0x0001;
const unsigned long  TICKS_INSIDE   = 0x0002;
const unsigned long  TICKS_BOTH  = TICKS_OUTSIDE | TICKS_INSIDE;

class LineData;
class Axis;

/*Класс деления шкалы оси*/
class Ticks : public SavableItem
{
	friend class Axis;
public:	

	Ticks(Axis* parentAxis);
	virtual ~Ticks();

	/**************/
	/* МЕТОДЫ     */
	/**************/

	/*Установить размер промежуточных делений оси в пикселях*/
	void SetMinorLength(int length) { minorLength = length; }
	
	/*Получить размер промежуточных делений оси в пикселях*/
	int  GetMinorLength() const		{ return minorLength;	  }

	/*Установить размер основных делений оси в пикселях*/
	void SetMajorLength(int length) { majorLength = length; }

	/*Установить размер основных делений оси в пикселях*/
	int  GetMajorLength() const		{ return majorLength; }

	/*Получить максимальный размер делений оси в пикселях 
	(промежуточных или основных)*/
	int	 GetMaxLength () const;

	/*Установить количество промежуточных делений оси*/
	void SetMinorCount(int count) { minorCount = count; }
	
	/*Установить количество основных делений оси*/
	void SetMajorCount(int count) { majorCount = count; }

	/*Получить количество промежуточных делений оси*/
	int  GetMinorCount()	const { return minorCount; }

	/*Получить количество промежуточных делений оси*/
	int  GetMajorCount()	const { return majorCount; }

	/*Включить/выключить отображение основных делений оси*/
	void EnableMajor(BOOL enable = TRUE) { enabledMajor = enable; }

	/*Проверить включено ли отображение основных делений оси*/
	BOOL IsMajorEnabled() const   { return enabledMajor; }
	
	/*Включить/выключить отображение дополнительных делений оси*/
	void EnableMinor(BOOL enable) { enabledMinor = enable; }

	/*Проверить включено ли отображение дополнительных делений оси*/
	BOOL IsMinorEnabled() const { return enabledMinor; }

	/*Получить прямоугольник в который вписываются все деления оси*/
	//void GetRect(RECT &self) const; 

	/*Устанорвить точку отсчета делений (начало оси)*/
	void SetOrigin(POINT origin) { this->origin = origin;};

	/*Установить стиль отображения делений оси*/
	void SetStyle(unsigned long lStyle = TICKS_BOTH) { style = lStyle; } ;

	/*Получить стиль отображения делений оси*/
	unsigned long GetStyle() { return style; } ;

	/*Получить параметры линии для отображения делений оси (для основных или дополнительных)*/
	LineData& GetLine(BOOL bMajor = TRUE);	
	
	/*Установить параметры линии для отображения делений оси (для основных или дополнительных)*/
	void SetLine(const LineData& line, BOOL bMajor/* = TRUE*/); 

	void PreDraw();	

	void CalcTicksPos();

	/*Процедура отисовки делений*/
	void OnDraw(HDC hdc);

	/*Запись прарметров делений в файл*/
	virtual BOOL Write(HANDLE hFile) const;

	/*Чтение праметров делений из файла*/
	virtual BOOL Read(HANDLE hFile);


protected:
	
	/*Отрисовка одного деления*/
	void DrawTick(HDC hdc, int x, int y, int len);

	/*Очистка массивов для хранения физических координат и координат устройства*/
	void Clear();

	/**************/
	/* АТРИБУТЫ   */
	/**************/
	
	Axis* parentAxis;	//Ось которой принадлежат деления

	int minorLength;	// длина дополнительных делений   
	int majorLength;	// длина основных делений   
    
	int majorCount;	// количество основных делений   
	int minorCount;   // количество дополнитьельных делений   
	
	LineData* lineMajor;	//линия для отображения основных делений
	LineData* lineMinor;  //линия для отображения дополнителных делений

	POINT		  origin;		// точка отсчета делений
	unsigned long style;		// стиль отображения делений

	BOOL enabledMajor;		//Флаг отображения основных делений
	BOOL enabledMinor;		//Флаг отображения дополнительных делений
	

	std::vector <POINT> majorPoints;	// хранилище координат основных делений
	std::vector <POINT> minorPoints;	// хранилище координат дополнительных делений
	
	std::vector <double> majorValues;	// хранилище физических координат основных делений
	std::vector <double> minorValues;	// хранилище физических координат дополнительных делений
	
	/*Хранилище физических координат основных делений*/
	const std::vector<POINT>& GetMajorTicksPos () const { return majorPoints; };	

	/*Хранилище физических дополнительных делений*/
	const std::vector<POINT>& GetMinorTicksPos() const { return minorPoints; };	

	/*Хранилище физических координат основных делений*/
	const std::vector<double>& GetMajorTicksValues () const { return majorValues; };	

	/*Хранилище физических дополнительных делений*/
	const std::vector<double>& GetMinorTicksValues() const { return minorValues; };	

	BOOL GetNearestTick(double& val, POINT& pt);

};

#endif