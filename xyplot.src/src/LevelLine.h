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
	/* ������     */
	/**************/
	
	/*�������� ��������� ����� ��� ����������� ������� ��� (��� �������� ��� ��������������)*/
	LineData& GetLine() { return *line; };	

	/*���������� ��������� ����� ��� ����������� ������� ��� (��� �������� ��� ��������������)*/
	void SetLine(const LineData& line) { *this->line = line; }; 

	/*��������� �������� �����*/
	void OnDraw(HDC hdc);

	/*���������� �������*/
	void SetValue(double value) { this->value = value; };

	/*�������� �������� �������*/
	double GetValue() const { return value; };

	/*�������� ����� ������� ������*/
	const std::string& GetLabel() const;

	/*���������� ����� ������� ������*/
	void SetLabel(std::string);

	/*���������� ��������� ������� ������*/
	void SetLabelPosition(unsigned long pos) { this->pos = pos; };

	/*���������� ��������� ������� ������*/
	long GetLabelPosition() { return pos; };

	/*��������-��������� ������� ����� ������*/
	void EnableLabel(BOOL enable = TRUE) { enabledLabel = enable; };

	/*�������� �� ������� ����� ������*/
	BOOL IsLabelEnabled() { return enabledLabel; };

	/*��������-��������� ����� ������*/
	void Enable(BOOL enable = TRUE) { this->enabled = enable; };

	/*�������� ��������� ��� ������� ����������� ����� ������*/
	long GetAxisToAttach();

protected:	

	/**************/
	/* ��������   */
	/**************/

	Axis* parentAxis;		//��� ������� ����������� ����� ������
	double value;			//������� �������
	LineData* line;			//����� ��� ����������� ������ 
	
	//std::string labelText;  //����� ������� ������
	//EnhancedString* label;
	RTFString* label;

	unsigned long pos;		//��������� ������� ������������ ����� ������
	BOOL enabledLabel;		//���������� ��� ��� ������� ������ 
	BOOL enabled;			//���������/�������� ����� ������
};
#endif