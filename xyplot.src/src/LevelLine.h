#ifndef __LEVELLINES_H_INCLUDED__
#define __LEVELLINES_H_INCLUDED__

#include <map>
#include "linedata.h"

class RTFString;
class LevelLine;
class LineData;
class Axis;
class XYPlot;

typedef std::map<long, LevelLine*>  LevelsMap;
typedef LevelsMap::iterator LevelsMapIterator;

class LevelLine
{
public:	

	LevelLine(XYPlot* parent, Axis* parentAxis, std::string labelText, COLORREF color = 0L, int Width = 2, int LineType = xyplot::PLS_SOLID, double value = 0.0);
	
	virtual ~LevelLine();

	/**************/
	/* МЕТОДЫ     */
	/**************/
	
	/*Получить параметры линии для отображения делений оси (для основных или дополнительных)*/
	LineData& GetLine() { return *line; };	

	/*Установить параметры линии для отображения делений оси (для основных или дополнительных)*/
	void SetLine(const LineData& line) { *this->line = line; }; 

	/*Процедура отисовки линии*/
	void OnDraw(HDC hdc);

	/*Установить уровень*/
	void SetValue(double value) { this->value = value; };

	/*Получить значение уровеня*/
	double GetValue() const { return value; };

	/*Получить текст надписи уровня*/
	const std::string& GetLabel() const;

	/*Установить текст надписи уровня*/
	void SetLabel(std::string);

	/*Установить положение надписи уровня*/
	void SetLabelPosition(unsigned long pos) { this->pos = pos; };

	/*Установить положение надписи уровня*/
	long GetLabelPosition() { return pos; };

	/*Включить-выключить надпись линии уровня*/
	void EnableLabel(BOOL enable = TRUE) { enabledLabel = enable; };

	/*Включена ли надпись линии уровня*/
	BOOL IsLabelEnabled() { return enabledLabel; };

	/*Включить-выключить линию уровня*/
	void Enable(BOOL enable = TRUE) { this->enabled = enable; };

	/*Получить положение оси которой принадлежит линия уровня*/
	long GetAxisToAttach();

protected:	

	/**************/
	/* АТРИБУТЫ   */
	/**************/

	Axis* parentAxis;		//Ось которой принадлежит линия уровня
	double value;			//Текущий уровень
	LineData* line;			//линия для отображения уровня 
	
	//std::string labelText;  //Текст надписи уровня
	//EnhancedString* label;
	RTFString* label;

	unsigned long pos;		//Положение надписи относительно линии уровня
	BOOL enabledLabel;		//Отображать или нет надпись уровня 
	BOOL enabled;			//Отключить/включить линию уровня
};
#endif