#ifndef __GRIDLINES_H_INCLUDED__
#define __GRIDLINES_H_INCLUDED__

#include "SavableItem.h"
#include <vector>

class LineData;
class Axis;

/*Класс деления шкалы оси*/
class GridLines : public SavableItem
{
public:	

	GridLines(Axis* parentAxis);
	virtual ~GridLines();

	/**************/
	/* МЕТОДЫ     */
	/**************/
	/*Включить/выключить отображение основных делений оси*/
	void EnableMajor(BOOL enable = TRUE) { enabledMajor = enable; }

	/*Проверить включено ли отображение основных делений оси*/
	BOOL IsMajorEnabled() const   { return enabledMajor; }

	/*Включить/выключить отображение дополнительных делений оси*/
	void EnableMinor(BOOL enable) { enabledMinor = enable; }

	/*Проверить включено ли отображение дополнительных делений оси*/
	BOOL IsMinorEnabled() const { return enabledMinor; }

	/*Получить параметры линии для отображения сетки оси (для основных или дополнительных)*/
	LineData& GetLine(BOOL bMajor = TRUE);	

	/*Установить параметры линии для отображения сетки оси (для основных или дополнительных)*/
	void SetLine(const LineData& line, BOOL bMajor = TRUE); 

	/*Процедура отисовки сетки*/
	void OnDraw(HDC hdc);

	/*Запись прарметров сетки в файл*/
	virtual BOOL Write(HANDLE hFile) const;

	/*Чтение праметров сетки из файла*/
	virtual BOOL Read(HANDLE hFile);	

protected:	

	/**************/
	/* АТРИБУТЫ   */
	/**************/

	Axis* parentAxis;	//Ось которой принадлежат деления		
	
	LineData* lineMajor;   //линия для отображения основных линий сетки
	LineData* lineMinor;   //линия для отображения дополнителных линий сетки

	BOOL enabledMajor;   //Флаг отображения основных линий сетки
	BOOL enabledMinor;   //Флаг отображения дополнительных линий сетки

};

#endif