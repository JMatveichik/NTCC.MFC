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

	/*����������� �� ���������*/
	//Profile();

	/*����������� � �����������*/
	Profile( std::string name, long color, int width,  BOOL showLine, BOOL showMarks, const Axis& axisX, const Axis& axisY );

	~Profile();

	/*��������� ��� ������*/
	void SetName(std::string name);

	/*�������� ��� ������*/
	inline const std::string& GetName() const { return name; }	

	/*������/�������� ������ */
	inline void SetVisible(BOOL visible = TRUE ) { showLine = visible; }

	/*������������ �� ������*/
	inline BOOL IsVisible() const { return showLine; }

	/*��������/��������� ����������� ��������*/
	inline void UseSymblos(BOOL symblols) { showMarks = symblols; }

	/*����������� �� ������� */
	inline BOOL IsUseSymblos() const { return showMarks; }

	void Show (BOOL bShow); 
	//
	long GetUnit(unsigned index, double &x, double &y) const;
	long GetData(unsigned* count, double *pfx, double* pfy) const;
	
	/*���������� ����� ������ ��� ����������� ������*/
	void SetData(const double *pfx, const double* pfy, int count );

	BOOL Empty() const { return dataPhysX.empty(); }

	/*�������� ����� � ������ ��� ����������� ������*/
	void Append(double fx, double fy);

	/*�������� ��������� ����� � ������ ��� ����������� ������*/
	void Append(const double *pfx, const double *pfy, unsigned count);

	/*���������� ����� ��� ����������� ������*/
	unsigned DataSize() const { return unsigned(dataPhysX.size()); }

	/*���������� ��� ����� ��� ����������� ������*/
	void SetLineData(const LineData& srcLine);

	/*�������� ������ ��� �����*/
	inline LineData& GetLineData() { return line; }

	/*�������� ���������� ������� ������ �� ����� ����*/
	void Range(double &xmin, double &xmax, double &ymin, double &ymax) const;

	/*�������� ���������� ������� ������ �� ��� X*/
	void RangeX(double &xmin, double &xmax) const;

	/*�������� ���������� ������� ������ �� ��� Y*/
	void RangeY(double &xmin, double &xmax) const;

	/*���������� ������*/
	void Render(HDC hDC, const RECT rect);
	
	/*�������� ������ ����� � ����������� ����������*/
	const POINT* GetRenderPointArray(unsigned& count) const;

	/**/
	const POINT GetRenderPoint(int index, double& physX, double& physY) const;

	/*�������� ������ �����*/
	ProfileMarks& GetMarks() { return marks; };

	/*�������� ��� X � ������� ����������� ������ */
	Axis& AxisX() { return axisX; }; 

	/*�������� ��� Y � ������� ����������� ������ */
	Axis& AxisY() { return axisY; }; 

	BOOL IsShowInLegend() const { return showInLegend; };

	void ShowInLegend(BOOL show) { showInLegend = show; };

	void GetVisibleRange(unsigned& firstIndex, unsigned& lastIndex) const { firstIndex = firstVisible; lastIndex = lastVisible; }

protected:

	/*������� ������ ��� ����������� ������*/
	void Reset();

	std::string name;	// ��� ������ ��� ����������� � �������
	BOOL showMarks;		// ���������� ��� ��� ����� �� ������
	BOOL showLine;		// ���������� ��� ��� ����� ������	

	LineData line;		// ����� ������
	
	Axis& axisX;		// ��� X � ������� �������� �������
	Axis& axisY;		// ��� Y � ������� �������� �������

	BOOL newData;			// ���� ���������� ������ 
	RECT lastRenderedRect;	// ��������� ������������� ���������

	mutable double minX;	// ����������� �������� ���������� ���������� X
	mutable double maxX;	// ������������ �������� ���������� ���������� X
	mutable double minY;	// ����������� �������� ���������� ���������� Y
	mutable double maxY;	// ������������ �������� ���������� ���������� Y

    ProfileMarks marks;		// ����� ������

	std::vector<POINT>  dataDevice;	//������ ����� � ����������� ����������
	std::vector<double>	dataPhysX;	//������ ����� � ���������� �����������
	std::vector<double>	dataPhysY;	//������ ����� � ���������� �����������

	BOOL showInLegend;
	
	unsigned firstVisible;
	unsigned lastVisible;

	mutable CRITICAL_SECTION cs;
};

#endif