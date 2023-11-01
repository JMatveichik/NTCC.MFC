#include "stdafx.h"
#include <map>
#include <string>
#include <vector>

using namespace std;

typedef struct tagOBAERRORMSG
{
	string en;
	string ru;
} OBAERRORMSG, *LPOBAERRORMSG;

static map<int, OBAERRORMSG> strOBAErrorMessage;


/*
 #define DECLARE_OBA_ERROR(a, b, c) OBAERRORMSG Tem_ = { b, c }; strOBAErrorMessage[a] = Tem_;

class oba_error_initializer
{
public:
	oba_error_initializer()
	{
		DECLARE_OBA_ERROR(0, "No errors", "��������� ��������� ��� ������");
	}	
} oba_error_initializer_instance;
*/

std::vector<std::string>  szOBAErrorMessage_en = {
	{"No errors"}, //0
	{"Divide by zero"}, //1
	{"Incompatible operand for this operation"}, //2
	{"Syntax error"}, //3
	{"Missing ')'"}, //4
	{"Missing primary"}, //5
	{"Too long name or string"}, //6
	{"Use of a variable before definition"}, //7
	{"Missing '='"}, //8
	{"Error reading input stream"}, //9
	{"Duplicate label detected"}, //10
	{"��������� ������. ��������� � ����������� ���������"}, //11
	{"Label not found in GOTO or GOSUB operator"}, //12
	{"Missing compare operator in operator IF"}, //13
	{"Missing keyword THEN in operator IF"}, //14
	{"��������� ������. ��������� � ����������� ���������"}, //15
	{"��������� ������. ��������� � ����������� ���������"}, //16
	{"No GOSUB operator for this RETURN operator"}, //17
	{"Counter parameter of FOR operator must be variable or array element"}, //18
	{"Missing keyword TO in operator FOR"}, //19
	{"��������� ������. ��������� � ����������� ���������"}, //20
	{"��������� ������. ��������� � ����������� ���������"}, //21
	{"��������� ������. ��������� � ����������� ���������"}, //22
	{"Error in parameters of built-in function"}, //23
	{"Missing name array in DIM operator"}, //24
	{"��������� ������. ��������� � ����������� ���������"}, //25
	{"Too many array dimensions"}, //26
	{"Missing ',' in DIM operator to separate values"}, //27
	{"��������� ������. ��������� � ����������� ���������"}, //28
	{"Size of array is negative or zero"}, //29
	{"��������� ������. ��������� � ����������� ���������"}, //30
	{"Size of array is zero"}, //31
	{"Current index of array too big"}, //32
	{"Missing keyword FOR in operator OPEN"}, //33
	{"Missing file open mode in operator OPEN"}, //34
	{"Missing keyword 'AS' or 'FILE' or '#' in operator OPEN"}, //35
	{"��������� ������. ��������� � ����������� ���������"}, //36
	{"��������� ������. ��������� � ����������� ���������"}, //37
	{"File not opened"}, //38
	{"��������� ������. ��������� � ����������� ���������"}, //39
	{"��������� ������. ��������� � ����������� ���������"}, //40
	{"Too many parameters in call function"}, //41
	{"��������� ������. ��������� � ����������� ���������"}, //42
	{"��������� ������. ��������� � ����������� ���������"}, //43
	{"��������� ������. ��������� � ����������� ���������"}, //44
	{"��������� ������. ��������� � ����������� ���������"}, //45
	{"��������� ������. ��������� � ����������� ���������"}, //46
	{"��������� ������. ��������� � ����������� ���������"}, //47
	{"��������� ������. ��������� � ����������� ���������"}, //48
	{"��������� ������. ��������� � ����������� ���������"}, //49
	{"Unary minus can not be used for string type"}, //50
	{"Compare with zero can not be used for string type"}, //51
	{"Arithmetic operator can not be used for string type"}, //52
	{"Can not compare string and numeric type"}, //53
	{"Error format of float number (two points in float number)"}, //54
	{"Error format of hex number"}, //55
	{"Error format of number (point and 'x' letter in hex number)"}, //56
	{"String does not have right quote"}, //57
	{"Current count dimension of array is not equal that DIM operator"}, //58
	{"One or many current dimension of array is too big"}, //59
	{"��������� ������. ��������� � ����������� ���������"}, //60
	{"��������� ������. ��������� � ����������� ���������"}, //61
	{"��������� ������. ��������� � ����������� ���������"}, //62
	{"��������� ������. ��������� � ����������� ���������"}, //63
	{"Do not clear left in primary (debug error)"}, //64
	{"Do not clear left in run user function (debug error)"}, //65
	{"Cannot not find NEXT operator for FOR operator"}, //66
	{"��������� ������. ��������� � ����������� ���������"}, //67
	{"��������� ������. ��������� � ����������� ���������"}, //68
	{"Parameter of NEXT operator must be variable or array element"}, //69
	{"Do not found ELSE or ENDIF operator for IF operator"}, //70
	{"Do not found FOR operator for this NEXT operator"}, //71
	{"Missing '(' in DIM operator or array"}, //72
	{"Can not cast float to string"}, //73
	{"Can not cast int to string"}, //74
	{"Can not cast string to float"}, //75
	{"Can not cast string to int"}, //76
	{"Index of array must not be zero"}, //77
	{"Do not clear ptr in setptr for savewrapper (debug error)"}, //78
	{"Array dimension less than zero"}, //79
	{"Error file name format in OPEN operator"}, //80
	{"Error writing to output stream"}, //81
	{"Error convert Unicode name to ASCII name"}, //82
	{"Error convert ASCII name to Unicode name"}, //83
	{"Field of 'file name' of OPEN operator must be string or string variable"}, //84
	{"Error format for conversion float"}, //85
	{"Error format for conversion int"}, //86
	{"stop message"}, //87
	{"WAIT: Invalid number/type of parameters"}, //88
	{"CHECK: Invalid parameters"},	//89
	{"SWITCH: Invalid parameters"},	//90
	{"SWITCH: \"on/off\" command is missing"}, //91
	{"VALUE: Invalid parameters"},	//92
	{"VALUE: Invalid parameters"},	//93
	{"VALUE: Invalid parameters"},	//94	
	{"VALUE: Invalid parameters"},	//95
	{"VALUE: Invalid parameters"},	//96	
	{"CHECK: Invalid id"}, //97
	{"SWITCH: Invalid id"}, //98
	{"VALUE: Invalid id"}, //99
	{"VALUE: Invalid id"}, //100
	{"TEMPERATURE: Invalid id"}, //101
	{"VALUE: Invalid id"}, //102
	{"VALUE: Invalid id"}, //103
	{"WAIT: Unable to get timer resolution"}, //104
	{"SETTIMER: Invalid parameters"},	//105
	{"CHECKTIMER: Invalid number/type of parameters"},	//106
	{"CHECKTIMER: Timer not found"},	//107
	{"PAUSETIMER: Invalid number/type of parameters"},	//108
	{"PAUSETIMER: Timer not found"},	//109
	{"PAUSETIMER: \"on/off\" command is missing"},	//110
	{"MILLISECONDS: Invalid number/type of parameters"},	//111
	{"JOIN: : Invalid number/type of parameters"},	//112
	{"FORMAT: Invalid number/type of parameters"},	//113
	{"STRING: Invalid number/type of parameters" } //114
};

std::vector<std::string> szOBAErrorMessage_ru = {
	{"��������� ��������� ��� ������"}, //0
	{"������� �� ����"}, //1
	{"������������� �������� (�������� ��������� � �������� � �������������� ��������)"}, //2
	{"������ ����������"}, //3
	{"��������� ������ ������ (�������� ������� ����� ���������� � ������� ��� ������������ � �������)"}, //4
	{"� ���� ����� ��������� ��������� ��������"}, //5
	{"������� ������� ������ ��� ��� ����������"}, //6
	{"������������� ���������� � ������ ����� ��������� ���������� �� ������������� �� � ����� �����"}, //7
	{"� ���� ����� ��������� ��������� ���� ������������"}, //8
	{"������ ������ �� �������� ������"}, //9
	{"����� ������� � ��������� ������"}, //10
	{"��������� ������. ��������� � ����������� ���������"}, //11
	{"� ������� ����� �� ������� �����, �� ������� ��������� �������� GOTO"}, //12
	{"� ��������� IF ��������� ������ ��������� ��������� (<,>,<=, �.�.�.)"}, //13
	{"� ��������� IF ��������� �������� ����� THEN"}, //14
	{"��������� ������. ��������� � ����������� ���������"}, //15
	{"��������� ������. ��������� � ����������� ���������"}, //16
	{"�� ������ GOSUB �������� ��� ��������� RETURN"}, //17
	{"� ��������� FOR ��������� ��� ���������� ��� �������� ������� � �������� �������� �����"}, //18
	{"� ��������� FOR ��������� �������� ����� TO"}, //19
	{"��������� ������. ��������� � ����������� ���������"}, //20
	{"��������� ������. ��������� � ����������� ���������"}, //21
	{"��������� ������. ��������� � ����������� ���������"}, //22
	{"������ ��� ������� ���������� �������"}, //23
	{"� ��������� DIM ��������� ��� ������� � �������������"}, //24
	{"��������� ������. ��������� � ����������� ���������"}, //25
	{"� ������� ������ ������� ����� ���������"}, //26
	{"� ��������� DIM ��������� ������� ��� ���������� ���� ��������"}, //27
	{"��������� ������. ��������� � ����������� ���������"}, //28
	{"������� ��� ������������� ������ �������"}, //29
	{"��������� ������. ��������� � ����������� ���������"}, //30
	{"������� ������ �������"}, //31
	{"������� ������ ������� ������� �������"}, //32
	{"� ��������� OPEN ��������� �������� ����� FOR"}, //33
	{"� ��������� OPEN ��������� �������� ����� ������ �������� �����"}, //34
	{"� ��������� OPEN ��������� �������� ����� AS FILE"}, //35
	{"��������� ������. ��������� � ����������� ���������"}, //36
	{"��������� ������. ��������� � ����������� ���������"}, //37
	{"���� �� ������"}, //38
	{"��������� ������. ��������� � ����������� ���������"}, //39
	{"��������� ������. ��������� � ����������� ���������"}, //40
	{"������� ����� ���������� � ������ �������"}, //41
	{"��������� ������. ��������� � ����������� ���������"}, //42
	{"��������� ������. ��������� � ����������� ���������"}, //43
	{"��������� ������. ��������� � ����������� ���������"}, //44
	{"��������� ������. ��������� � ����������� ���������"}, //45
	{"��������� ������. ��������� � ����������� ���������"}, //46
	{"��������� ������. ��������� � ����������� ���������"}, //47
	{"��������� ������. ��������� � ����������� ���������"}, //48
	{"��������� ������. ��������� � ����������� ���������"}, //49
	{"������� ��������� �������� �������� ������ � ��������� ����������"}, //50
	{"������� ��������� �������� ��������� � ����� � ��������� ����������"}, //51
	{"������� ��������� �������������� �������� � ��������� ����������"}, //52
	{"������� ��������� �������� ��������� � ������ � ��������� ����������"}, //53
	{"������ ������� ��������� � ��������� ������"}, //54
	{"������ ������� ����������������� ���������"}, //55
	{"������ ������� ����������������� ���������"}, //56
	{"�� ������� ������ ������� � ��������� ���������"}, //57
	{"���������� ������������ � ������� ��������� � ������� �� ����� ���������� ������������ � ��������� DIM"}, //58
	{"������� ��������� � �������� ������� � ������� �������, ��� �� ������ � ��������� DIM"}, //59
	{"��������� ������. ��������� � ����������� ���������"}, //60
	{"��������� ������. ��������� � ����������� ���������"}, //61
	{"��������� ������. ��������� � ����������� ���������"}, //62
	{"��������� ������. ��������� � ����������� ���������"}, //63
	{"���������� ����������. �� ������ ���������� ��� ���������� ������"}, //64
	{"���������� ����������. �� ������ ���������� ��� ���������� ������"}, //65
	{"�� ������ �������� NEXT ��� �������� ��������� FOR"}, //66
	{"��������� ������. ��������� � ����������� ���������"}, //67
	{"��������� ������. ��������� � ����������� ���������"}, //68
	{"� ��������� NEXT ��������� ��� ���������� ��� �������� ������� � �������� �������� �����"}, //69
	{"�� ������� �������� ����� ELSE ��� ENDIF ��� �������� ��������� IF"}, //70
	{"�� ������ �������� FOR ��� �������� ��������� NEXT"}, //71
	{"�� ������� ����� ������ ����� ����� ������� � ��������� DIM"}, //72
	{"������� ���������� ����� � ������"}, //73
	{"������� ���������� ����� � ������"}, //74
	{"������� ���������� ������ � �����"}, //75
	{"������� ���������� ������ � �����"}, //76
	{"������� ������ ������� ����� ����"}, //77
	{"���������� ����������. �� ������ ���������� ��� ���������� ������"}, //78
	{"������� ������ ������� ������ ����"}, //79
	{"������ ����� ����� (�������� ������ ������)"}, //80
	{"������ ������ � �������� ���� � ��������� PRINT"}, //81
	{"������ �������������� Unicode � ASCII"}, //82
	{"������ �������������� ASCII � Unicode"}, //83
	{"Field of 'file name' of OPEN operator must be string or string variable"}, //84
	{"������ ������� ��� �������������� �� float"}, //85
	{"������ ������� ��� �������������� � int"}, //86
	{"��������� ���������"}, //87
	{"WAIT: �������� �����/��� ����������"}, //88 
	{"CHECK: �������� ���������"}, //89
	{"SWITCH: �������� ���������"}, //90
	{"SWITCH: \"on/off\" �������� �����������"}, //91
	{"VALUE: �������� ���������"}, //92
	{"VALUE: �������� ���������"}, //93
	{"VALUE: �������� ���������"}, //94
	{"VALUE: �������� ���������"}, //95
	{"VALUE: �������� ���������"}, //96
	{"CHECK: �������� �������������"}, //97
	{"SWITCH: �������� �������������"}, //98
	{"VALUE: �������� �������������"}, //99
	{"VALUE: �������� �������������"}, //100
	{"VALUE: �������� �������������"}, //101
	{"VALUE: �������� �������������"}, //102
	{"VALUE: �������� �������������"}, //103
	{"WAIT: �� ������� ���������� ���������� ���������� �������"}, //104
	{"SETTIMER: �������� �������������"},	//105
	{"CHECKTIMER: �������� �����/��� ����������"},	//106
	{"CHECKTIMER: ������ �� ������"},		//107
	{"PAUSETIMER: �������� �����/��� ����������"},		//108	
	{"PAUSETIMER: ������ �� ������"},		//109
	{"PAUSETIMER: \"on/off\" �������� �����������"},	//110
	{"MILLISECONDS: �������� �����/��� ����������"},	//111
	{"JOIN: �������� �����/��� ����������"},	//112
	{"FORMAT: �������� �����/��� ����������"},	//113
	{"STRING: �������� �����/��� ����������" }, //114

};

const char* OBAError(unsigned int code, bool bRussian/* = false*/)
{
	if (bRussian)
	{
		if (code < szOBAErrorMessage_ru.size())
			return szOBAErrorMessage_ru.at(code).c_str();

		return "Invalid error code";
	}
	else
	{
		if (code < szOBAErrorMessage_en.size())
			return szOBAErrorMessage_en.at(code).c_str();

		return "Invalid error code";
	}
}