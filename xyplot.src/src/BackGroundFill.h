#pragma once

#include "savableitem.h"

class BackGround : public SavableItem
{
public:

	BackGround();
	virtual ~BackGround() {};

	/*Включить/выключить отрисовку */
	void Enable(BOOL enable = TRUE) { enabled = enable; };


	/*Получить  цвета заполнения*/
	// clrStart  - начальный цвет   градиента
	// clrFinish - конечный  цвет   градиента
	// если clrStart = clrFinish одноцветная заливка 
	void GetFillColors(COLORREF& clrStart, COLORREF& clrFinish) const;

	/*Установить цвета заполнения*/
	// clrStart  - начальный цвет   градиента
	// clrFinish - конечный  цвет   градиента
	// если clrStart = clrFinish одноцветная заливка 
	void SetFillColors(COLORREF clrStart, COLORREF clrFinish);
	
	/*Получить прозрачность заполнения*/
	// opacityStart  - начальная непрозрачность градиента %
	// opacityFinish - конечный  непрозрачность градиента %
	void GetOpacity(double& alphaStart, double& alphaFinish) const;

	/*Установить прозрачность заполнения*/
	// opacityStart  - начальная непрозрачность градиента, %
	// opacityFinish - конечный  непрозрачность градиента, %
	void SetOpacity(double alphaStart, double alphaFinish);

	/*Установить направление заливки*/
	//vertical = TRUE  - заливка с верху вниз  
	//vertical = FALSE - заливка с права на лево
	void SetGradientDirection(BOOL vertical) { this->vertical = vertical; }
	BOOL GetGradientDirection() const { return this->vertical; }


	//HS_HORIZONTAL		  0 	  /* ----- */
	//HS_VERTICAL         1       /* ||||| */
	//HS_FDIAGONAL        2       /* \\\\\ */
	//HS_BDIAGONAL        3       /* ///// */
	//HS_CROSS            4       /* +++++ */
	//HS_DIAGCROSS        5       /* xxxxx */
	/* Return Hatch Styles */	
	long GetHatchStyle() const;

	/* Set Hatch Styles */	
	void SetHatchStyle(long hstyle);

	COLORREF GetHatchColor() const;

	void SetHatchColor(COLORREF clr);

	/*Установить прямоугольник для заливки */
	void SetRect(const RECT& rc);

	/*Процедура отрисовки*/
	void OnDraw( HDC hdc ) const;
	
	/*Записать параметры в файл*/
	virtual BOOL Write(HANDLE hFile) const;

	/*Прочитать параметры из файла*/
	virtual BOOL Read(HANDLE hFile);

	/*Заполнить прямоугольник градиентом*/
	void static FillRectWithGradient(HDC hdc, const RECT& rc, COLORREF clr1, COLORREF clr2, BYTE alpha1, BYTE alpha2, BOOL bVertical);

protected:

	BOOL enabled;

	COLORREF clrStart;
	COLORREF clrFinish;

	BYTE alphaStart;
	BYTE alphaFinish;

	double opacityStart;
	double opacityFinish;

	COLORREF hatchColor;
	long hatchStyle;
	
	BOOL vertical;

	RECT rcFill;
};
