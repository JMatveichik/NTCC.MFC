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
		DECLARE_OBA_ERROR(0, "No errors", "Программа завершена без ошибок");
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
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //11
	{"Label not found in GOTO or GOSUB operator"}, //12
	{"Missing compare operator in operator IF"}, //13
	{"Missing keyword THEN in operator IF"}, //14
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //15
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //16
	{"No GOSUB operator for this RETURN operator"}, //17
	{"Counter parameter of FOR operator must be variable or array element"}, //18
	{"Missing keyword TO in operator FOR"}, //19
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //20
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //21
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //22
	{"Error in parameters of built-in function"}, //23
	{"Missing name array in DIM operator"}, //24
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //25
	{"Too many array dimensions"}, //26
	{"Missing ',' in DIM operator to separate values"}, //27
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //28
	{"Size of array is negative or zero"}, //29
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //30
	{"Size of array is zero"}, //31
	{"Current index of array too big"}, //32
	{"Missing keyword FOR in operator OPEN"}, //33
	{"Missing file open mode in operator OPEN"}, //34
	{"Missing keyword 'AS' or 'FILE' or '#' in operator OPEN"}, //35
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //36
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //37
	{"File not opened"}, //38
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //39
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //40
	{"Too many parameters in call function"}, //41
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //42
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //43
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //44
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //45
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //46
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //47
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //48
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //49
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
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //60
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //61
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //62
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //63
	{"Do not clear left in primary (debug error)"}, //64
	{"Do not clear left in run user function (debug error)"}, //65
	{"Cannot not find NEXT operator for FOR operator"}, //66
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //67
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //68
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
	{"Программа завершена без ошибок"}, //0
	{"Деление на ноль"}, //1
	{"Несовместимые операнды (например строковые и числовые в арифметической операции)"}, //2
	{"Ошибка синтаксиса"}, //3
	{"Пропущена правая скобка (возможно слишком много аргументов в функции или размерностей в массиве)"}, //4
	{"В этом месте программы ожидается примитив"}, //5
	{"Слишком длинная строка или имя переменной"}, //6
	{"Использование переменной в правой части оператора присвоения до использования ее в левой части"}, //7
	{"В этом месте программы ожидается знак присваивания"}, //8
	{"Ошибка чтения из входного потока"}, //9
	{"Метка описана в программе дважды"}, //10
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //11
	{"В таблице меток не найдена метка, на которую ссылается оператор GOTO"}, //12
	{"В операторе IF ожидается символ выражения отношения (<,>,<=, и.т.д.)"}, //13
	{"В операторе IF ожидается ключевое слово THEN"}, //14
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //15
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //16
	{"Не найден GOSUB оператор для оператора RETURN"}, //17
	{"В операторе FOR ожидается имя переменной или элемента массива в качестве счетчика цикла"}, //18
	{"В операторе FOR ожидается ключевое слово TO"}, //19
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //20
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //21
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //22
	{"Ошибка при разборе параметров функции"}, //23
	{"В операторе DIM ожидается имя массива с размерностями"}, //24
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //25
	{"У массива задано слишком много измерений"}, //26
	{"В операторе DIM ожидается запятая при разделении имен массивов"}, //27
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //28
	{"Нулевой или отрицательный размер массива"}, //29
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //30
	{"Нулевой размер массива"}, //31
	{"Текущий индекс массива слишком большой"}, //32
	{"В операторе OPEN ожидается ключевое слово FOR"}, //33
	{"В операторе OPEN ожидаются ключевые слова режима открытия файла"}, //34
	{"В операторе OPEN ожидаются ключевые слова AS FILE"}, //35
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //36
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //37
	{"Файл не открыт"}, //38
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //39
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //40
	{"Слишком много параметров в вызове функции"}, //41
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //42
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //43
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //44
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //45
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //46
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //47
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //48
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //49
	{"Попытка применить операцию унарного минуса к строковой переменной"}, //50
	{"Попытка применить операцию сравнения с нулем к строковой переменной"}, //51
	{"Попытка применить арифметическую операцию к строковой переменной"}, //52
	{"Попытка применить операцию сравнения с числом к строковой переменной"}, //53
	{"Ошибка формата константы с плавающей точкой"}, //54
	{"Ошибка формата шестнадцатеричной константы"}, //55
	{"Ошибка формата шестнадцатеричной константы"}, //56
	{"Не найдена правая кавычка в строковой константе"}, //57
	{"Количество размерностей в текущем обращении к массиву не равно количеству размерностей в операторе DIM"}, //58
	{"Попытка обратится к элементу массива с номером большим, чем он описан в операторе DIM"}, //59
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //60
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //61
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //62
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //63
	{"Отладочная информация. Не должна появляться при нормальной работе"}, //64
	{"Отладочная информация. Не должна появляться при нормальной работе"}, //65
	{"Не найден оператор NEXT для текущего оператора FOR"}, //66
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //67
	{"Системная ошибка. Свяжитесь с поставщиком программы"}, //68
	{"В операторе NEXT ожидается имя переменной или элемента массива в качестве счетчика цикла"}, //69
	{"Не найдены ключевые слова ELSE или ENDIF для текущего оператора IF"}, //70
	{"Не найден оператор FOR для текущего оператора NEXT"}, //71
	{"Не найдена левая скобка после имени массива в операторе DIM"}, //72
	{"Попытка приведения числа к строке"}, //73
	{"Попытка приведения числа к строке"}, //74
	{"Попытка приведения строки к числу"}, //75
	{"Попытка приведения строки к числу"}, //76
	{"Текущий индекс массива равен нулю"}, //77
	{"Отладочная информация. Не должна появляться при нормальной работе"}, //78
	{"Текущий индекс массива меньше нуля"}, //79
	{"Ошибка имени файла (возможно пустая строка)"}, //80
	{"Ошибка записи в выходной файл в операторе PRINT"}, //81
	{"Ошибка преобразования Unicode в ASCII"}, //82
	{"Ошибка преобразования ASCII в Unicode"}, //83
	{"Field of 'file name' of OPEN operator must be string or string variable"}, //84
	{"Ошибка формата при преобразовании во float"}, //85
	{"Ошибка формата при преобразовании в int"}, //86
	{"Последнее сообщение"}, //87
	{"WAIT: Неверное число/тип параметров"}, //88 
	{"CHECK: Неверные параметры"}, //89
	{"SWITCH: Неверные параметры"}, //90
	{"SWITCH: \"on/off\" параметр отсутствует"}, //91
	{"VALUE: Неверные параметры"}, //92
	{"VALUE: Неверные параметры"}, //93
	{"VALUE: Неверные параметры"}, //94
	{"VALUE: Неверные параметры"}, //95
	{"VALUE: Неверные параметры"}, //96
	{"CHECK: Неверный идентификатор"}, //97
	{"SWITCH: Неверный идентификатор"}, //98
	{"VALUE: Неверный идентификатор"}, //99
	{"VALUE: Неверный идентификатор"}, //100
	{"VALUE: Неверный идентификатор"}, //101
	{"VALUE: Неверный идентификатор"}, //102
	{"VALUE: Неверный идентификатор"}, //103
	{"WAIT: Не удалось установить разрешение системного таймера"}, //104
	{"SETTIMER: Неверный идентификатор"},	//105
	{"CHECKTIMER: Неверное число/тип параметров"},	//106
	{"CHECKTIMER: Таймер не найден"},		//107
	{"PAUSETIMER: Неверное число/тип параметров"},		//108	
	{"PAUSETIMER: Таймер не найден"},		//109
	{"PAUSETIMER: \"on/off\" параметр отсутствует"},	//110
	{"MILLISECONDS: Неверное число/тип параметров"},	//111
	{"JOIN: Неверное число/тип параметров"},	//112
	{"FORMAT: Неверное число/тип параметров"},	//113
	{"STRING: Неверное число/тип параметров" }, //114

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