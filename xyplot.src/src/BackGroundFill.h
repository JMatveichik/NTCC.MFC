#pragma once

#include "savableitem.h"

class BackGround : public SavableItem
{
public:

	BackGround();
	virtual ~BackGround() {};

	/*��������/��������� ��������� */
	void Enable(BOOL enable = TRUE) { enabled = enable; };


	/*��������  ����� ����������*/
	// clrStart  - ��������� ����   ���������
	// clrFinish - ��������  ����   ���������
	// ���� clrStart = clrFinish ����������� ������� 
	void GetFillColors(COLORREF& clrStart, COLORREF& clrFinish) const;

	/*���������� ����� ����������*/
	// clrStart  - ��������� ����   ���������
	// clrFinish - ��������  ����   ���������
	// ���� clrStart = clrFinish ����������� ������� 
	void SetFillColors(COLORREF clrStart, COLORREF clrFinish);
	
	/*�������� ������������ ����������*/
	// opacityStart  - ��������� �������������� ��������� %
	// opacityFinish - ��������  �������������� ��������� %
	void GetOpacity(double& alphaStart, double& alphaFinish) const;

	/*���������� ������������ ����������*/
	// opacityStart  - ��������� �������������� ���������, %
	// opacityFinish - ��������  �������������� ���������, %
	void SetOpacity(double alphaStart, double alphaFinish);

	/*���������� ����������� �������*/
	//vertical = TRUE  - ������� � ����� ����  
	//vertical = FALSE - ������� � ����� �� ����
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

	/*���������� ������������� ��� ������� */
	void SetRect(const RECT& rc);

	/*��������� ���������*/
	void OnDraw( HDC hdc ) const;
	
	/*�������� ��������� � ����*/
	virtual BOOL Write(HANDLE hFile) const;

	/*��������� ��������� �� �����*/
	virtual BOOL Read(HANDLE hFile);

	/*��������� ������������� ����������*/
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
