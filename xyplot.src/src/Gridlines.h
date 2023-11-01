#ifndef __GRIDLINES_H_INCLUDED__
#define __GRIDLINES_H_INCLUDED__

#include "SavableItem.h"
#include <vector>

class LineData;
class Axis;

/*����� ������� ����� ���*/
class GridLines : public SavableItem
{
public:	

	GridLines(Axis* parentAxis);
	virtual ~GridLines();

	/**************/
	/* ������     */
	/**************/
	/*��������/��������� ����������� �������� ������� ���*/
	void EnableMajor(BOOL enable = TRUE) { enabledMajor = enable; }

	/*��������� �������� �� ����������� �������� ������� ���*/
	BOOL IsMajorEnabled() const   { return enabledMajor; }

	/*��������/��������� ����������� �������������� ������� ���*/
	void EnableMinor(BOOL enable) { enabledMinor = enable; }

	/*��������� �������� �� ����������� �������������� ������� ���*/
	BOOL IsMinorEnabled() const { return enabledMinor; }

	/*�������� ��������� ����� ��� ����������� ����� ��� (��� �������� ��� ��������������)*/
	LineData& GetLine(BOOL bMajor = TRUE);	

	/*���������� ��������� ����� ��� ����������� ����� ��� (��� �������� ��� ��������������)*/
	void SetLine(const LineData& line, BOOL bMajor = TRUE); 

	/*��������� �������� �����*/
	void OnDraw(HDC hdc);

	/*������ ���������� ����� � ����*/
	virtual BOOL Write(HANDLE hFile) const;

	/*������ ��������� ����� �� �����*/
	virtual BOOL Read(HANDLE hFile);	

protected:	

	/**************/
	/* ��������   */
	/**************/

	Axis* parentAxis;	//��� ������� ����������� �������		
	
	LineData* lineMajor;   //����� ��� ����������� �������� ����� �����
	LineData* lineMinor;   //����� ��� ����������� ������������� ����� �����

	BOOL enabledMajor;   //���� ����������� �������� ����� �����
	BOOL enabledMinor;   //���� ����������� �������������� ����� �����

};

#endif