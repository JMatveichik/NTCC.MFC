#include "stdafx.h"


//Copyright (C) 1998-2010 MKTMK Moscow, Russia
//e-mail: mktmk<at>narod.ru
//interpreter Open Basic (OB).

#include "ob.h"

#ifdef OB_WCHAR
namespace ob_wcharspace{
#else
namespace ob_charspace{
#endif

//here is messages

const ob_type_small_char* ob_englmessage[]={
OB_STRINGDEFINE("End program OK"),//0
OB_STRINGDEFINE("Divide by zero"), //1
OB_STRINGDEFINE("Incompatible operand for this operation"),//2
OB_STRINGDEFINE("Sintax error"),//3
OB_STRINGDEFINE("Missing ')'"),//4
OB_STRINGDEFINE("Missing primary"),//5
OB_STRINGDEFINE("Too long name or string"),//6
OB_STRINGDEFINE("Use variable before definition"),//7
OB_STRINGDEFINE("Missing '='"),//8
OB_STRINGDEFINE("Error read input stream"),//9
OB_STRINGDEFINE("Duplicate label in program"),//10

OB_STRINGDEFINE("System error. Please contact your dealer"),//11

OB_STRINGDEFINE("Label not found in GOTO or GOSUB operator"),//12
OB_STRINGDEFINE("Missing compare operator in operator IF"),//13
OB_STRINGDEFINE("Missing keyword THEN in operator IF"),//14

OB_STRINGDEFINE("System error. Please contact your dealer"),//15

OB_STRINGDEFINE("System error. Please contact your dealer"),//16

OB_STRINGDEFINE("Not GOSUB operator for this RETURN operator"),//17
OB_STRINGDEFINE("Counter parameter of FOR operator must be variable or array element"),//18
OB_STRINGDEFINE("Missing keyword TO in operator FOR"),//19

OB_STRINGDEFINE("System error. Please contact your dealer"),//20
OB_STRINGDEFINE("System error. Please contact your dealer"),//21
OB_STRINGDEFINE("System error. Please contact your dealer"),//22

OB_STRINGDEFINE("Error in parameters built-in function"),//23
OB_STRINGDEFINE("Missing name array in DIM operator"),//24

OB_STRINGDEFINE("System error. Please contact your dealer"),//25

OB_STRINGDEFINE("Too many array dimension"),//26
OB_STRINGDEFINE("Missing ',' in DIM operator to separate value"),//27

OB_STRINGDEFINE("System error. Please contact your dealer"),//28

OB_STRINGDEFINE("Size of array is negative or zero"),//29

OB_STRINGDEFINE("System error. Please contact your dealer"),//30

OB_STRINGDEFINE("Size of array is zero"),//31
OB_STRINGDEFINE("Current index of array too big"),//32
OB_STRINGDEFINE("Missing keyword FOR in operator OPEN"),//33
OB_STRINGDEFINE("Missing mode opened file in operator OPEN"),//34
OB_STRINGDEFINE("Missing keyword 'AS' or 'FILE' or '#' in operator OPEN"),//35

OB_STRINGDEFINE("System error. Please contact your dealer"),//36
OB_STRINGDEFINE("System error. Please contact your dealer"),//37

OB_STRINGDEFINE("File not open"),//38

OB_STRINGDEFINE("System error. Please contact your dealer"),//39
OB_STRINGDEFINE("System error. Please contact your dealer"),//40

OB_STRINGDEFINE("Too many parameters in function call"),//41

OB_STRINGDEFINE("System error. Please contact your dealer"),//42
OB_STRINGDEFINE("System error. Please contact your dealer"),//43
OB_STRINGDEFINE("System error. Please contact your dealer"),//44
OB_STRINGDEFINE("System error. Please contact your dealer"),//45
OB_STRINGDEFINE("System error. Please contact your dealer"),//46
OB_STRINGDEFINE("System error. Please contact your dealer"),//47
OB_STRINGDEFINE("System error. Please contact your dealer"),//48
OB_STRINGDEFINE("System error. Please contact your dealer"),//49

OB_STRINGDEFINE("Unary minus can not use for string type"),//50
OB_STRINGDEFINE("Compare with zero can not use for string type"),//51
OB_STRINGDEFINE("Arithmetic operator can not use for string type"),//52
OB_STRINGDEFINE("Can not compare string and digital type"),//53
OB_STRINGDEFINE("Error format of float number (is two point in float number)"),//54
OB_STRINGDEFINE("Error format of hex number"),//55
OB_STRINGDEFINE("Error format of number (is point and 'x' letter in hex number)"),//56
OB_STRINGDEFINE("String do not have right quote"),//57
OB_STRINGDEFINE("Current count dimension of array is not equal that DIM operator"),//58
OB_STRINGDEFINE("One or many current dimension of array is too big"),//59

OB_STRINGDEFINE("System error. Please contact your dealer"),//60
OB_STRINGDEFINE("System error. Please contact your dealer"),//61
OB_STRINGDEFINE("System error. Please contact your dealer"),//62
OB_STRINGDEFINE("System error. Please contact your dealer"),//63

OB_STRINGDEFINE("Do not clear left in primary (debug error)"),//64
OB_STRINGDEFINE("Do not clear left in run user function (debug error)"),//65
OB_STRINGDEFINE("Do not found NEXT operator for FOR operator"),//66

OB_STRINGDEFINE("System error. Please contact your dealer"),//67
OB_STRINGDEFINE("System error. Please contact your dealer"),//68

OB_STRINGDEFINE("Parameter of NEXT operator must be variable or array element"),//69
OB_STRINGDEFINE("Do not found ELSE or ENDIF operator for IF operator"),//70
OB_STRINGDEFINE("Do not found FOR operator for this NEXT operator"),//71
OB_STRINGDEFINE("Missing '(' in DIM operator or array"),//72

OB_STRINGDEFINE("Can not cast float to string"),//73
OB_STRINGDEFINE("Can not cast int to string"),//74
OB_STRINGDEFINE("Can not cast string to float"),//75
OB_STRINGDEFINE("Can not cast string to int"),//76
OB_STRINGDEFINE("Index of array must not be zero"),//77
OB_STRINGDEFINE("Do not clear ptr in setptr for savewrapper (debug error)"),//78
OB_STRINGDEFINE("Array dimension less than zero"),//79
OB_STRINGDEFINE("Error file name format in OPEN operator"),//80
OB_STRINGDEFINE("Error write to output stream"),//81

//for version 1.60
OB_STRINGDEFINE("Error convert Unicode name to ASCII name"),//82
OB_STRINGDEFINE("Error convert ASCII name to Unicode name"),//83

//for version 1.80
OB_STRINGDEFINE("Field of 'file name' of OPEN operator must be string or string variable"),//84
OB_STRINGDEFINE("Error format for convertion float"),//85
OB_STRINGDEFINE("Error format for convertion int"),//86

//for version 1.90
OB_STRINGDEFINE("Error UTF-8 format (too many characters)"),//87
OB_STRINGDEFINE("System error. Please contact your dealer"),//88
OB_STRINGDEFINE("Error UTF-8 format (error UTF-8 record)"),//89
OB_STRINGDEFINE("Error UTF-8 format (too big simbol)"),//90
OB_STRINGDEFINE("Error execution 'put' for input  stream"),//91
OB_STRINGDEFINE("Error execution 'get' for output stream"),//92
OB_STRINGDEFINE("Error UTF-8 format (expect char, but obtain EOF)"),//93
OB_STRINGDEFINE("Sintax error during load program"),//94

OB_STRINGDEFINE("System error. Please contact your dealer"),//95
OB_STRINGDEFINE("System error. Please contact your dealer"),//96

OB_STRINGDEFINE("Error execution 'get output stream' for input file"),//97
OB_STRINGDEFINE("Error execution 'get input stream' for output file"),//98
OB_STRINGDEFINE("Error execution 'get output stream' for input  stream"),//99
OB_STRINGDEFINE("Error execution 'get input stream' for output stream"),//100
OB_STRINGDEFINE("Error execution 'set output stream' for input  stream"),//101
OB_STRINGDEFINE("Error execution 'set input stream' for output stream"),//102
OB_STRINGDEFINE("Too many nested GOSUB"),//103

OB_STRINGDEFINE("")
OB_STRINGDEFINE("stop message")
};

#ifdef OB_WCHAR
}//end namespace ob_wcharspace
#else
}//end namespace ob_charspace
#endif

