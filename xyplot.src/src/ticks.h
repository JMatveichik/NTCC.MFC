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

/*����� ������� ����� ���*/
class Ticks : public SavableItem
{
	friend class Axis;
public:	

	Ticks(Axis* parentAxis);
	virtual ~Ticks();

	/**************/
	/* ������     */
	/**************/

	/*���������� ������ ������������� ������� ��� � ��������*/
	void SetMinorLength(int length) { minorLength = length; }
	
	/*�������� ������ ������������� ������� ��� � ��������*/
	int  GetMinorLength() const		{ return minorLength;	  }

	/*���������� ������ �������� ������� ��� � ��������*/
	void SetMajorLength(int length) { majorLength = length; }

	/*���������� ������ �������� ������� ��� � ��������*/
	int  GetMajorLength() const		{ return majorLength; }

	/*�������� ������������ ������ ������� ��� � �������� 
	(������������� ��� ��������)*/
	int	 GetMaxLength () const;

	/*���������� ���������� ������������� ������� ���*/
	void SetMinorCount(int count) { minorCount = count; }
	
	/*���������� ���������� �������� ������� ���*/
	void SetMajorCount(int count) { majorCount = count; }

	/*�������� ���������� ������������� ������� ���*/
	int  GetMinorCount()	const { return minorCount; }

	/*�������� ���������� ������������� ������� ���*/
	int  GetMajorCount()	const { return majorCount; }

	/*��������/��������� ����������� �������� ������� ���*/
	void EnableMajor(BOOL enable = TRUE) { enabledMajor = enable; }

	/*��������� �������� �� ����������� �������� ������� ���*/
	BOOL IsMajorEnabled() const   { return enabledMajor; }
	
	/*��������/��������� ����������� �������������� ������� ���*/
	void EnableMinor(BOOL enable) { enabledMinor = enable; }

	/*��������� �������� �� ����������� �������������� ������� ���*/
	BOOL IsMinorEnabled() const { return enabledMinor; }

	/*�������� ������������� � ������� ����������� ��� ������� ���*/
	//void GetRect(RECT &self) const; 

	/*����������� ����� ������� ������� (������ ���)*/
	void SetOrigin(POINT origin) { this->origin = origin;};

	/*���������� ����� ����������� ������� ���*/
	void SetStyle(unsigned long lStyle = TICKS_BOTH) { style = lStyle; } ;

	/*�������� ����� ����������� ������� ���*/
	unsigned long GetStyle() { return style; } ;

	/*�������� ��������� ����� ��� ����������� ������� ��� (��� �������� ��� ��������������)*/
	LineData& GetLine(BOOL bMajor = TRUE);	
	
	/*���������� ��������� ����� ��� ����������� ������� ��� (��� �������� ��� ��������������)*/
	void SetLine(const LineData& line, BOOL bMajor/* = TRUE*/); 

	void PreDraw();	

	void CalcTicksPos();

	/*��������� �������� �������*/
	void OnDraw(HDC hdc);

	/*������ ���������� ������� � ����*/
	virtual BOOL Write(HANDLE hFile) const;

	/*������ ��������� ������� �� �����*/
	virtual BOOL Read(HANDLE hFile);


protected:
	
	/*��������� ������ �������*/
	void DrawTick(HDC hdc, int x, int y, int len);

	/*������� �������� ��� �������� ���������� ��������� � ��������� ����������*/
	void Clear();

	/**************/
	/* ��������   */
	/**************/
	
	Axis* parentAxis;	//��� ������� ����������� �������

	int minorLength;	// ����� �������������� �������   
	int majorLength;	// ����� �������� �������   
    
	int majorCount;	// ���������� �������� �������   
	int minorCount;   // ���������� ��������������� �������   
	
	LineData* lineMajor;	//����� ��� ����������� �������� �������
	LineData* lineMinor;  //����� ��� ����������� ������������� �������

	POINT		  origin;		// ����� ������� �������
	unsigned long style;		// ����� ����������� �������

	BOOL enabledMajor;		//���� ����������� �������� �������
	BOOL enabledMinor;		//���� ����������� �������������� �������
	

	std::vector <POINT> majorPoints;	// ��������� ��������� �������� �������
	std::vector <POINT> minorPoints;	// ��������� ��������� �������������� �������
	
	std::vector <double> majorValues;	// ��������� ���������� ��������� �������� �������
	std::vector <double> minorValues;	// ��������� ���������� ��������� �������������� �������
	
	/*��������� ���������� ��������� �������� �������*/
	const std::vector<POINT>& GetMajorTicksPos () const { return majorPoints; };	

	/*��������� ���������� �������������� �������*/
	const std::vector<POINT>& GetMinorTicksPos() const { return minorPoints; };	

	/*��������� ���������� ��������� �������� �������*/
	const std::vector<double>& GetMajorTicksValues () const { return majorValues; };	

	/*��������� ���������� �������������� �������*/
	const std::vector<double>& GetMinorTicksValues() const { return minorValues; };	

	BOOL GetNearestTick(double& val, POINT& pt);

};

#endif