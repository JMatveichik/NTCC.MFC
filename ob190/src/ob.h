
//Copyright (C) 1998-2010 MKTMK Moscow, Russia
//e-mail: mktmk<at>narod.ru
//interpreter Open Basic (OB).

//you may use Open Basic for:
// - script language expandable by user without compilation need
// - configuration file language
// - input language for terminal program (batch file is obtained automatically)

//features:
// - Open Basic (OB) is fully interpreter
// - OB have Basic input language, expandable by user
// - OB is a C++ class with member-function (load, run etc.)
// - OB designed for embed to user application
// - OB distributed as a library or source
// - OB support float, integer and string variable and array of this type
// - OB written entirely in ANSI C++ without any compiler extention
// - OB is OS independent (do not use any OS API)
// - OB do not use any graphic libraries
// - OB do not use any third-party virtual machine
// - OB use standart C++ header file and streams
// - OB use exception for error handling (for eliminate memory leak)
// - OB use template and virtual function
// - OB have multithread-safe code
// - user may connect user function with OB execution system
// - do not need SWIG for connect user function with OB execution system
// - user function may be C/C++, ASM and other language
// - user function may be connect during run-time
// - user function may be call from Basic program as Basic function
// - user function may receive data from Basic program
// - user function may return data to Basic program
// - user function have access to all variable and array of Basic by name
// - user function may read and write variable and array of Basic program
// - source code size of OB less then 150K
// - x86.exe code size of OB less then 100K if other libraries is dynamic link

#ifndef OB_THIS
#define OB_THIS

//for version 1.80
//if you build Unicode version library
//you must define OB_WCHAR for use Unicode input file

//common includes
#ifdef _MSC_VER

//MSVC7

#include <sstream>
#include <iostream>
#include <fstream>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <time.h>

#else

//BCB60

#ifdef _BORLAND_

#include <sstream>
#include <iostream.h>
#include <fstream.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <time.h>

#else

//GCC 3.2.2

#include <sstream>
#include <iostream>
#include <fstream>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits>
#include <time.h>

#endif
#endif

using namespace std;

#ifdef OB_WCHAR
namespace ob_wcharspace{
#else
namespace ob_charspace{
#endif

#ifndef OB_WCHAR

typedef stringstream   ob_strstream;//common ptr for conversion wchar->char stream
typedef ostringstream  ob_ostrstream;//for conversion digit->string
typedef istringstream  ob_istrstream;//for conversion string->digit
typedef string         ob_standartstring;//for create temporary string object

typedef stringstream   ob_shortstrstream;//for datastream

typedef istream        ob_istreamcommon;//for common pointer
typedef ostream        ob_ostreamcommon;//for common pointer
typedef fstream        ob_fstream;//for file structure
typedef ifstream       ob_ifstream;//for DLL
typedef ofstream       ob_ofstream;//for DLL

#else

typedef wstringstream  ob_strstream;
typedef wostringstream ob_ostrstream;
typedef wistringstream ob_istrstream;
typedef wstring        ob_standartstring;

typedef stringstream   ob_shortstrstream;

typedef istream        ob_istreamcommon;
typedef ostream        ob_ostreamcommon;
typedef fstream        ob_fstream;
typedef ifstream       ob_ifstream;
typedef ofstream       ob_ofstream;

#endif

//compilers condition
#ifdef __BORLANDC__
#define MSTORE_INLINE
#define MVECT_INLINE
#define MLIST_INLINE
#define MSTACK_INLINE
#define MHASH_INLINE
#define OB_INLINE
#endif

//containers for ob
#include "mstore.h"
#include "mvect.h"
#include "mlist.h"
#include "mstack.h"
#include "mhash.h"

//define
#ifndef OB_INLINE
#define OB_INLINE inline
#endif

#ifndef OB_NULL
#define OB_NULL (0)
#endif

//forward declarations
class ob_obasic;
class ob_par;
class ob_var;
class ob_int;
class ob_float;

class ob_str;

//types of Open Basic
//service types
typedef unsigned int      ob_type_arraydimension;       //1
typedef unsigned int      ob_type_arraysize;            //2
typedef streampos         ob_type_streampos;            //3
typedef unsigned int      ob_type_parnum;               //4
typedef unsigned int      ob_type_filenum;              //5
typedef unsigned char     ob_type_hashtablelength;      //6
typedef size_t            ob_type_stringsize;           //7
typedef unsigned int      ob_type_codeerror;            //8
typedef unsigned long int ob_type_serialnum;            //9
typedef unsigned int      ob_type_version;              //10
typedef unsigned int      ob_type_countlist;            //11

//variable types
#ifndef OB_WCHAR
typedef          char     ob_type_char;                 //12
#else
typedef          wchar_t  ob_type_char;                 //12
#endif

typedef float             ob_type_flo;                  //13
typedef int               ob_type_int;                  //14 

//service types
typedef unsigned int      ob_type_statictablesize;      //15
typedef unsigned int      ob_type_countnestedoperator;  //16
typedef unsigned int      ob_type_srandargument;        //17

typedef          char     ob_type_small_char;           //18
typedef          wchar_t  ob_type_wide_char;            //19

typedef size_t            ob_type_data_operator_length; //20
typedef unsigned int      ob_type_num_previous_eol_ptr; //21

typedef mstore_dynamicalloc<ob_type_char,ob_type_stringsize> ob_type_d;
typedef mvect_common<ob_type_d>                              ob_type_s;

typedef mstore_staticdatawithoutbuf<ob_type_char,ob_type_stringsize> ob_type_d1;
typedef mvect_common<ob_type_d1>                                     ob_type_s1;

typedef mstore_dynamicalloc<ob_type_arraysize,ob_type_arraydimension> ob_type_a;
typedef mvect_common<ob_type_a>                                     ob_type_dim;

typedef mstore_dynamicalloc<ob_type_small_char,ob_type_stringsize> ob_type_f;
typedef mvect_common<ob_type_f>                         ob_type_name_char_small;

typedef ob_type_name_char_small ob_type_string_char_small;

//const of Open Basic
//max length string in input operator
const ob_type_stringsize      ob_maxlengthstring=4096;
const ob_type_stringsize      ob_maxlengthstring_for_convert=200;
const ob_type_parnum          ob_maxnumpar=64;
const ob_type_stringsize      ob_timedatasize=100;
const ob_type_arraydimension  ob_maxarraydimention=20;
const ob_type_statictablesize ob_maxnumoperator=25;

#define OB_CHARDEFINE(xxx,MMM)                        \
const ob_type_char ob_const_at         xxx MMM##'@';  \
const ob_type_char ob_const_equ        xxx MMM##'=';  \
const ob_type_char ob_const_cr         xxx MMM##'\r'; \
const ob_type_char ob_const_lf         xxx MMM##'\n'; \
const ob_type_char ob_const_asterisk   xxx MMM##'*';  \
const ob_type_char ob_const_slash      xxx MMM##'/';  \
const ob_type_char ob_const_bslash     xxx MMM##'\\'; \
const ob_type_char ob_const_plus       xxx MMM##'+';  \
const ob_type_char ob_const_minus      xxx MMM##'-';  \
const ob_type_char ob_const_pow        xxx MMM##'^';  \
const ob_type_char ob_const_lp         xxx MMM##'(';  \
const ob_type_char ob_const_rp         xxx MMM##')';  \
const ob_type_char ob_const_sc         xxx MMM##';';  \
const ob_type_char ob_const_colon      xxx MMM##':';  \
const ob_type_char ob_const_comma      xxx MMM##',';  \
const ob_type_char ob_const_lp1        xxx MMM##'<';  \
const ob_type_char ob_const_rp1        xxx MMM##'>';  \
const ob_type_char ob_const_quotes     xxx MMM##'\"'; \
const ob_type_char ob_const_quotes1    xxx MMM##'\''; \
const ob_type_char ob_const_point      xxx MMM##'.';  \
                                                      \
const ob_type_char ob_const_0          xxx MMM##'0';  \
const ob_type_char ob_const_1          xxx MMM##'1';  \
const ob_type_char ob_const_2          xxx MMM##'2';  \
const ob_type_char ob_const_3          xxx MMM##'3';  \
const ob_type_char ob_const_4          xxx MMM##'4';  \
const ob_type_char ob_const_5          xxx MMM##'5';  \
const ob_type_char ob_const_6          xxx MMM##'6';  \
const ob_type_char ob_const_7          xxx MMM##'7';  \
const ob_type_char ob_const_8          xxx MMM##'8';  \
const ob_type_char ob_const_9          xxx MMM##'9';  \
                                                      \
const ob_type_char ob_const_a          xxx MMM##'a';  \
const ob_type_char ob_const_b          xxx MMM##'b';  \
const ob_type_char ob_const_c          xxx MMM##'c';  \
const ob_type_char ob_const_d          xxx MMM##'d';  \
const ob_type_char ob_const_e          xxx MMM##'e';  \
const ob_type_char ob_const_f          xxx MMM##'f';  \
const ob_type_char ob_const_A          xxx MMM##'A';  \
const ob_type_char ob_const_B          xxx MMM##'B';  \
const ob_type_char ob_const_C          xxx MMM##'C';  \
const ob_type_char ob_const_D          xxx MMM##'D';  \
const ob_type_char ob_const_E          xxx MMM##'E';  \
const ob_type_char ob_const_F          xxx MMM##'F';  \
                                                      \
const ob_type_char ob_const_space      xxx MMM##' ';  \
const ob_type_char ob_const_tab        xxx MMM##'\t'; \
const ob_type_char ob_const_ff         xxx MMM##'\f'; \
                                                      \
const ob_type_char ob_const_hex        xxx MMM##'x';  \
const ob_type_char ob_const_HEX        xxx MMM##'X';  \
                                                      \
const ob_type_char ob_const_underscore xxx MMM##'_';  \
const ob_type_char ob_const_strlabel   xxx MMM##'$';  \
const ob_type_char ob_const_intlabel   xxx MMM##'%';

const ob_type_char ob_const_endstring=0;

#ifndef OB_WCHAR

const ob_type_char ob_const_sharp    ='#';

OB_CHARDEFINE(=,)

#else

const ob_type_char ob_const_sharp    =L'#';

const ob_type_char ob_const_nel      =0x0085;//UNICODE new line
const ob_type_char ob_const_ls       =0x2028;//UNICODE line selector
const ob_type_char ob_const_ps       =0x2029;//UNICODE paragraph selector

OB_CHARDEFINE(=,L)

#endif

#define OB_STRINGDEFINE(str) str

const ob_type_wide_char ob_const_dummychar1  =0xFEFF;
const ob_type_wide_char ob_const_dummychar2  =0xFFFE;

//length of hash table
const ob_type_hashtablelength ob_hashlength=111;
const ob_type_hashtablelength ob_starthashvalue=1;

class ob_type_string:public ob_type_s{
public:
ob_type_string(){}//for ob_str default constructor
ob_type_string(ob_type_stringsize c):ob_type_s(c){}//for curr. string in ob_lex
ob_type_string(ob_type_char* data1,ob_type_stringsize c):
ob_type_s(data1,c){}//for create string without new call strlen in prim()
ob_type_string(ob_type_char* data1)://for create string after return char* from user func.
ob_type_s(data1,ob_type_char(),ob_maxlengthstring){}
};

//prototypes
ob_type_hashtablelength ob_hash(ob_type_char* data,ob_type_stringsize l);

const ob_type_char*
ob_getstringparam(
const ob_type_parnum *descrc,
const ob_type_char* parc,
const ob_type_parnum i);

ob_type_stringsize ob_strlen(ob_type_char* str);
ob_type_stringsize ob_strlen_small(ob_type_small_char* str);
void ob_strcpy(ob_type_char* datadst,const ob_type_char* datasrc);

//prototypes
//for version 1.80
#ifdef OB_WCHAR
void wchar2char(ob_type_string_char_small* sfn,ob_type_char* name);
void char2wchar(ob_type_string* name,ob_type_small_char* sfn);
#endif

//prototypes
//for version 1.90
void ob_wstream_to_stream(ob_shortstrstream* datastream,ob_standartstring* st);

class ob_type_namestatic:public ob_type_s1{
public:
ob_type_namestatic():ob_type_s1(OB_NULL,ob_type_stringsize()){}//for member ob_lex
ob_type_namestatic(ob_type_char* data1):ob_type_s1(data1,ob_strlen(data1)){}//for findcommon
ob_type_namestatic(ob_type_char* data1,ob_type_stringsize c):ob_type_s1(data1,c){}//for get_lex

ob_type_hashtablelength gethash(){return ob_hash(data,count);}
};

class ob_type_name:public ob_type_s{
public:
ob_type_name(){}//for local ob_label
ob_type_name(const ob_type_name& name):ob_type_s(name.getdata(),name.getcount()){}//for ob_forfor
ob_type_name(const ob_type_namestatic& name):ob_type_s(name.getdata(),name.getcount()){}//for ob_label and ob_varbase
ob_type_name(ob_type_char* data1):ob_type_s(data1,ob_type_char(),ob_maxlengthstring){}
ob_type_hashtablelength gethash(){return ob_hash(data,count);}//for insert new var in tables
};

const ob_type_codeerror ob_basecodeerror=0;

#define OB_CODEERROR(ecode)              \
const ob_type_codeerror ob_err##ecode=(ob_basecodeerror+(ecode));

#define OB_CODEERRORGROUP(e0,e1,e2,e3,e4,e5,e6,e7,e8,e9) \
OB_CODEERROR(e0)                                         \
OB_CODEERROR(e1)                                         \
OB_CODEERROR(e2)                                         \
OB_CODEERROR(e3)                                         \
OB_CODEERROR(e4)                                         \
OB_CODEERROR(e5)                                         \
OB_CODEERROR(e6)                                         \
OB_CODEERROR(e7)                                         \
OB_CODEERROR(e8)                                         \
OB_CODEERROR(e9)

OB_CODEERRORGROUP(  0,  1,  2,  3,  4,  5,  6,  7,  8,  9)
OB_CODEERRORGROUP( 10, 11, 12, 13, 14, 15, 16, 17, 18, 19)
OB_CODEERRORGROUP( 20, 21, 22, 23, 24, 25, 26, 27, 28, 29)
OB_CODEERRORGROUP( 30, 31, 32, 33, 34, 35, 36, 37, 38, 39)
OB_CODEERRORGROUP( 40, 41, 42, 43, 44, 45, 46, 47, 48, 49)
OB_CODEERRORGROUP( 50, 51, 52, 53, 54, 55, 56, 57, 58, 59)
OB_CODEERRORGROUP( 60, 61, 62, 63, 64, 65, 66, 67, 68, 69)
OB_CODEERRORGROUP( 70, 71, 72, 73, 74, 75, 76, 77, 78, 79)
OB_CODEERRORGROUP( 80, 81, 82, 83, 84, 85, 86, 87, 88, 89)
OB_CODEERRORGROUP( 90, 91, 92, 93, 94, 95, 96, 97, 98, 99)
OB_CODEERRORGROUP(100,101,102,103,104,105,106,107,108,109)
OB_CODEERRORGROUP(110,111,112,113,114,115,116,117,118,119)
OB_CODEERRORGROUP(120,121,122,123,124,125,126,127,128,129)

void ob_errorgeneration(ob_type_codeerror e);//do not inline for debug

#define OB_ERR(eee) ob_errorgeneration(eee);

//-----------------------------------------------------------
//for version 1.90
//ob_iostream class hierarchy
//for support UTF-8 and UNICODE input-output stream

//UTF-8 Table
//00000000 0000007F 0xxxxxxx
//00000080 000007FF 110xxxxx 10xxxxxx
//00000800 0000FFFF 1110xxxx 10xxxxxx 10xxxxxx
//00010000 001FFFFF 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
//00200000 03FFFFFF 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
//04000000 7FFFFFFF 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx

const unsigned int ob_utf8_maxlength=6;

class ob_modestream{
public:
enum modestream_enum{FLAG_NORMAL=0x00,FLAG_REVERSE=0x01,FLAG_UTF8=0x02,FLAG_SHORT=0x04};

private:
modestream_enum ms;

public:
ob_modestream():ms(FLAG_NORMAL){}//use for ob_stream_base

void set_modestream(modestream_enum ms1){ms=(modestream_enum)(ms|ms1);}
modestream_enum get_modestream(){return ms;}
bool check_modestream(modestream_enum ms1){if((ms&ms1)==0) return false; else return true;}

};

template <size_t size>
struct ob_wide_char_mask{
static const ob_type_wide_char array[];
};

template <>
const ob_type_wide_char ob_wide_char_mask<2>::array[]={
(const ob_type_wide_char)0x007fL,
(const ob_type_wide_char)0x07FFL,
(const ob_type_wide_char)0xFFFFL,
(const ob_type_wide_char)0xFFFFL,
(const ob_type_wide_char)0xFFFFL,
(const ob_type_wide_char)0xFFFFL
};

template <>
const ob_type_wide_char ob_wide_char_mask<4>::array[]={
(const ob_type_wide_char)0x0000007fL,
(const ob_type_wide_char)0x000007FFL,
(const ob_type_wide_char)0x0000FFFFL,
(const ob_type_wide_char)0x001FFFFFL,
(const ob_type_wide_char)0x03FFFFFFL,
(const ob_type_wide_char)0x7FFFFFFFL
};

class ob_stream_base:public ob_modestream{

public:

protected:

ob_type_streampos startposition;

bool flagreverse(){if((get_modestream()&FLAG_REVERSE)!=0) return true; else return false;}
bool flagutf8(){if((get_modestream()&FLAG_UTF8)!=0) return true; else return false;}

static const ob_type_small_char utf8_codeprefix[];
static const ob_type_small_char utf8_codemask[];
static const ob_type_small_char utf8_codemask_inverse[];

static const ob_type_small_char add_code_prefix;
static const ob_type_small_char add_code_mask;
static const ob_type_small_char add_code_mask_inverse;

static const ob_type_small_char utf8_shiftvalue;

static const ob_type_wide_char* code_bound;

ob_type_small_char reverse(ob_type_small_char ch){return ch;}

ob_type_wide_char reverse(ob_type_wide_char ch){

ob_type_wide_char ch1=ch;

if(flagreverse()==true){

const size_t l=sizeof(ob_type_char);
size_t i;
ob_type_small_char* pch=(ob_type_small_char*)&ch;
ob_type_small_char* pch1=(ob_type_small_char*)&ch1;

for(i=0;i!=l;++i) pch1[l-i-1]=pch[i];

}//if
return ch1;
}

unsigned int utf8_length(ob_type_small_char ch){return 1;}

unsigned int utf8_length(ob_type_wide_char ch){
unsigned int l;
unsigned int i;

for(i=0;i!=ob_utf8_maxlength;++i){
if(ch<=code_bound[i]){
l=i+1;
break;
}//if
}//for

if(i==ob_utf8_maxlength) OB_ERR(ob_err90)

return l;
}

public:

ob_stream_base():startposition(ob_type_streampos()){}
virtual ~ob_stream_base(){}

virtual bool good()=0;
virtual bool eof()=0;

virtual void get(ob_type_small_char& ch)=0;
virtual void get(ob_type_wide_char& ch)=0;

virtual void put(const ob_type_small_char& ch)=0;
virtual void put(const ob_type_wide_char& ch)=0;

virtual void set_stream_with_startposition(ob_istreamcommon* streamptr1)=0;
virtual void set_stream_with_startposition(ob_ostreamcommon* streamptr1)=0;

virtual void get_stream(ob_istreamcommon** streamptr1) const =0;//for multifile GOTO, GOSUB and RETURN
virtual void get_stream(ob_ostreamcommon** streamptr1) const =0;
};

class ob_istream: public ob_stream_base{

ob_istreamcommon* streamptr;

void clear(){streamptr->clear();}

public:

ob_istream():streamptr(OB_NULL){}

void get(ob_type_small_char& ch){streamptr->get(ch);}

void assign(ob_istream* is){//for operinput from file
*this=*is;//no any fields in memory
}

bool good(){return streamptr->good();}
bool eof(){return streamptr->eof();}

void put(const ob_type_small_char& ch){OB_ERR(ob_err91);}
void put(const ob_type_wide_char& ch){OB_ERR(ob_err91);}

void get(ob_type_wide_char& ch){

ob_type_wide_char ch1=0;

size_t ls=sizeof(ob_type_wide_char);
size_t i;
ob_type_small_char ch2;

if(flagutf8()==false){

if(get_modestream()==FLAG_SHORT) ls=sizeof(ob_type_small_char);

for(i=0;i!=ls;++i){
get(ch2);

if(eof()) break;
if(!good()) OB_ERR(ob_err9);

((ob_type_small_char*)&ch1)[i]=ch2;
}//for

}//if
else{

ob_type_small_char tmparray[ob_utf8_maxlength];

get(ch2);

if(eof()==false){

if(!good()) OB_ERR(ob_err9);

for(i=0;i!=ob_utf8_maxlength;++i){
if((ch2&utf8_codemask[i])==utf8_codeprefix[i]) break;
}//for

if(i==ob_utf8_maxlength) OB_ERR(ob_err87)

size_t ll=i;

tmparray[0]=ch2&(utf8_codemask_inverse[ll]);

for(i=0;i!=ll;++i){

get(ch2);

if(eof()==true) OB_ERR(ob_err93);
if(!good()) OB_ERR(ob_err9);

if((ch2&add_code_mask)!=add_code_prefix) OB_ERR(ob_err89)
tmparray[1+i]=ch2&(add_code_mask_inverse);
}//for

ch1=tmparray[0];

for(i=0;i!=ll;++i){
ch1<<=utf8_shiftvalue;
ch1|=tmparray[i+1];
}//for
}//if

}//else

ch=reverse(ch1);
}

void putback(){

if(eof()==true){
setposition(0,ios::end);
}//if
else{
ob_type_streampos position=streamptr->tellg();

if(position!=ob_type_streampos()){
position-=sizeof(ob_type_char);
setposition(position,ios::beg);
}//if
}//else

}

void restore(){setposition(startposition);}

void set_stream_with_startposition(ob_istreamcommon* streamptr1){
streamptr=streamptr1;

if(streamptr1!=OB_NULL)
startposition=streamptr->tellg();
else
startposition=ob_type_streampos();
}

void set_stream_with_startposition(ob_ostreamcommon* streamptr1){OB_ERR(ob_err101);}

void get_stream(ob_ostreamcommon** streamptr1) const {OB_ERR(ob_err99);}
void get_stream(ob_istreamcommon** streamptr1) const {*streamptr1=streamptr;}

bool iszero(){if(streamptr==OB_NULL) return true; else return false;}

ob_type_streampos getposition() const {
return streamptr->tellg();
}

void setposition(ob_type_streampos pos,ios::seek_dir d=ios::beg){
clear();
streamptr->seekg(pos,d);
}

};

class ob_ostream: public ob_stream_base{

ob_ostreamcommon* streamptr;

protected:

void put1(const ob_type_wide_char& ch){

ob_type_wide_char ch1;

ch1=reverse(ch);

size_t ls=sizeof(ob_type_wide_char);
size_t i;
ob_type_small_char* pch1=(ob_type_small_char*)&ch1;

if(flagutf8()==false){

if(get_modestream()==FLAG_SHORT) ls=sizeof(ob_type_small_char);

for(i=0;i!=ls;++i) put(pch1[i]);

}//if
else{

ob_type_small_char tmparray[ob_utf8_maxlength];

unsigned int ll=utf8_length(ch);

ob_type_small_char ch2;

for(i=0;i!=ll-1;++i){
ch2=(ob_type_small_char)ch1;
ch2&=add_code_mask_inverse;
ch2|=add_code_prefix;
tmparray[i]=ch2;
ch1>>=utf8_shiftvalue;
}//for

ch2=(ob_type_small_char)ch1;
ch2&=utf8_codemask_inverse[i];
ch2|=utf8_codeprefix[i];

tmparray[i]=ch2;

for(i=0;i!=ll;++i){
ch2=tmparray[ll-i-1];
put(ch2);
}//for

}//else

}

public:

ob_ostream():streamptr(OB_NULL){}

void set_stream_with_startposition(ob_istreamcommon* streamptr1){OB_ERR(ob_err102);}

void set_stream_with_startposition(ob_ostreamcommon* streamptr1){
streamptr=streamptr1;

if(streamptr1!=OB_NULL)
startposition=streamptr->tellp();
else
startposition=ob_type_streampos();
}

void get_stream(ob_istreamcommon** streamptr1) const {OB_ERR(ob_err100);}
void get_stream(ob_ostreamcommon** streamptr1) const {*streamptr1=streamptr;}

virtual void put(const ob_type_small_char& ch){streamptr->put(ch);}
virtual void put(const ob_type_wide_char& ch){put1(ch);}

bool good(){return streamptr->good();}
bool eof(){return streamptr->eof();}

void get(ob_type_small_char& ch){OB_ERR(ob_err92);}
void get(ob_type_wide_char& ch){OB_ERR(ob_err92);}

void putendl(){

ob_ostrstream immos;
immos<<endl;

ob_standartstring st1=immos.str();
const ob_type_char* p=st1.c_str();

ob_type_stringsize l=st1.length();
ob_type_stringsize i;

for(i=0;i!=l;++i){
put(p[i]);
}//for

}

template<class T>
void putdigit(T var){

ob_ostrstream immos;
immos<<var;

ob_standartstring st1=immos.str();
const ob_type_char* p=st1.c_str();

ob_type_stringsize l=st1.length();
ob_type_stringsize i;

for(i=0;i!=l;++i){
put(p[i]);
}//for

}

};

class ob_ostream_for_file: public ob_ostream{

bool flfirstchar;

public:

ob_ostream_for_file():flfirstchar(false){}

bool getflfirstchar(){return flfirstchar;}
void setflfirstchar(bool fl){flfirstchar=fl;}

void put(const ob_type_small_char& ch){ob_ostream::put(ch);}

void put(const ob_type_wide_char& ch){

if((flfirstchar==false)&&((get_modestream()!=FLAG_SHORT))){
flfirstchar=true;
ob_type_wide_char ch1=ob_const_dummychar1;
put1(ch1);
}//if

put1(ch);
}

};

//-----------------------------------------------------------

//for version 1.80
//convertors

//isspace
template <class Tchar>
bool ob_isspace(Tchar ch){
bool ret;
switch(ch){
case ob_const_space:case ob_const_tab:case ob_const_ff:ret=true;break;
default:ret=false;break;
}//switch
return ret;
}

//isdelim
template <class Tchar>
bool ob_isdelim(Tchar ch){
bool ret;
switch(ch){
case ob_const_at         ://1
case ob_const_equ        ://2
case ob_const_cr         ://3
case ob_const_lf         ://4
case ob_const_asterisk   ://5
case ob_const_slash      ://6
case ob_const_bslash     ://7
case ob_const_plus       ://8
case ob_const_minus      ://9
case ob_const_pow        ://10
case ob_const_lp         ://11
case ob_const_rp         ://12
case ob_const_sc         ://13
case ob_const_colon      ://14
case ob_const_comma      ://15
case ob_const_lp1        ://16
case ob_const_rp1        ://17
case ob_const_quotes     ://18
case ob_const_quotes1    ://19
case ob_const_point      ://20
case ob_const_space      ://21
case ob_const_tab        ://22
case ob_const_ff         ://23
case ob_const_sharp      ://24

//for version 1.90
#ifdef OB_WCHAR
case ob_const_nel        ://25
case ob_const_ls         ://26
case ob_const_ps         ://27
#endif

ret=true;break;
default:ret=false;break;
}//switch
return ret;
}

//-----------------------------------------------------------

//some local classes
//variable
enum ob_type_ident {OB_IDENTFLO,OB_IDENTINT,OB_IDENTSTR,OB_NOIDENT};

#define OB_DECLF(name,typearg) ob_var* name(typearg* a)

#define OB_DECLSETF(start,typearg,stop)                               \
start OB_DECLF(addv,typearg) stop;start OB_DECLF(subv,typearg) stop;  \
start OB_DECLF(mulv,typearg) stop;start OB_DECLF(divv,typearg) stop;  \
start OB_DECLF(pwwv,typearg) stop;start OB_DECLF(asgv,typearg) stop;  \
start bool                                                            \
compare(typearg* a,ob_var::ob_type_if currentlexif) stop;

#define OB_DECLSETALLF(nameclass,start,stop)                    \
OB_DECLSETF(start,ob_var,stop)                                  \
OB_DECLSETF(start,ob_float,stop)                                \
OB_DECLSETF(start,ob_int,stop)                                  \
OB_DECLSETF(start,ob_str,stop)                                  \
start ob_var* uniminus() stop;                                  \
start bool eq() const stop;                                     \
start ob_var* duplicate() stop;                                 \
start ob_type_ident isA() const stop;                           \
start void tostream(ob_ostream* out) stop;                      \
start void topar(ob_par* p,const ob_type_parnum i) stop;        \
start ~nameclass(){}

//base class for types variable obasic
class ob_var{
public:
enum ob_type_if{LESSIF,GREATIF,EQVIF,LESSEQVIF,GREATEQVIF,NOTEQVIF};
OB_DECLSETALLF(ob_var,virtual,=0)
virtual void cast(ob_type_flo* a)=0;
virtual void cast(ob_type_int* a)=0;
virtual void cast(ob_type_char** a)=0;
virtual ob_type_stringsize size()=0;//for user information
};

//derived class for types variable obasic
class ob_float:public ob_var{
friend class ob_int;
friend class ob_str;

ob_type_flo var;
public:
typedef ob_type_flo typedatavar;

ob_float();
ob_float(ob_type_char*);
ob_float(ob_type_flo);

OB_DECLSETALLF(ob_float, , )

void cast(ob_type_flo* a);
void cast(ob_type_int* a);
void cast(ob_type_char** a);
ob_type_stringsize size(){return sizeof(var);}
};

class ob_int:public ob_var{
friend class ob_float;
friend class ob_str;

ob_type_int var;
public:
typedef ob_type_int typedatavar;

ob_int();
ob_int(ob_type_char*);
ob_int(ob_type_int);

OB_DECLSETALLF(ob_int, , )

void cast(ob_type_flo* a);
void cast(ob_type_int* a);
void cast(ob_type_char** a);
ob_type_stringsize size(){return sizeof(var);}
};

class ob_str:public ob_var{
friend class ob_float;
friend class ob_int;

ob_type_string var;
public:
typedef ob_type_char* typedatavar;

ob_str();//for array of string
ob_str(ob_type_char* s);//for return ob_type_char* from user function
ob_str(ob_type_char* s,ob_type_stringsize l);

OB_DECLSETALLF(ob_str, , )

void cast(ob_type_flo* a);
void cast(ob_type_int* a);
void cast(ob_type_char** a);
ob_type_stringsize size(){return var.getcount()-1;}
};

#define OB_OPERATIONWRAPPER(name)                         \
void name(ob_savewrappercommon* a){                       \
T* tmpptr=ptr->name##v(a->ptr);                           \
if(tmpptr==ptr){ a->clear();} else {clear();transfer(a);} \
}

//some realization
template <class T>
class ob_savewrappercommon{
T* ptr;
ob_savewrappercommon(const ob_savewrappercommon& s):ptr(s.ptr){}
ob_savewrappercommon& operator =(const ob_savewrappercommon& s){}
public:
ob_savewrappercommon():ptr(OB_NULL){}
ob_savewrappercommon(T* p):ptr(p){}
void setptr(T* s){
if((ptr!=OB_NULL)&&(s!=OB_NULL)){
clear();//for eliminate double delete
OB_ERR(ob_err78)
}
ptr=s;
}
T* getptr() const {return ptr;}
void clear(){if(ptr!=OB_NULL) delete ptr;ptr=OB_NULL;}
~ob_savewrappercommon(){if(ptr!=OB_NULL) delete ptr;}

void transfer(ob_savewrappercommon* d){//for ob_forfor and vararray
setptr(d->ptr);
d->setptr(OB_NULL);
}

OB_OPERATIONWRAPPER(add)
OB_OPERATIONWRAPPER(sub)
OB_OPERATIONWRAPPER(mul)
OB_OPERATIONWRAPPER(div)
OB_OPERATIONWRAPPER(pww)

void asg(ob_savewrappercommon* a){
ptr->asgv(a->ptr);//do not check pointer because asgv always return own pointer
a->clear();
}

};

typedef ob_savewrappercommon<ob_var>       ob_savewrapper;

class ob_data_for_jump{
ob_istream i;
ob_type_streampos label;
public:

ob_data_for_jump():label(ob_type_streampos()){}

virtual ~ob_data_for_jump(){}

void set(ob_istream* i1){//for opergo 
i.assign(i1);
label=i1->getposition();
}

void set(ob_type_streampos p){label=p;}//for forfor

void set(ob_type_streampos p,ob_istream* i1){//label for table
i.assign(i1);
label=p;
}

void setposition(ob_istream* in1){
ob_istreamcommon* c1;
i.get_stream(&c1);
if(c1!=OB_NULL) in1->assign(&i);
in1->setposition(label);
}

};

class ob_label:public ob_type_name, public ob_data_for_jump{
public:
ob_label(
ob_type_name& name,
ob_type_streampos p)://label for forfor
ob_type_name(name){ob_data_for_jump::set(p);}

ob_label(
ob_type_namestatic& name,
ob_type_streampos p,
ob_istream* i1)://label for table
ob_type_name(name){ob_data_for_jump::set(p,i1);}

virtual ~ob_label(){}
};

class ob_forfor:public ob_label{
ob_savewrapper lastvalue,step;
ob_var::ob_type_if typeifforcheckinfor;
public:

ob_forfor(ob_type_name& name,ob_type_streampos p,
ob_savewrapper* l,ob_savewrapper* s):ob_label(name,p){
lastvalue.transfer(l);
step.transfer(s);
ob_savewrapper zero(new ob_int(ob_type_int()));
if(step.getptr()->compare(zero.getptr(),ob_var::LESSIF)==false)
typeifforcheckinfor=ob_var::LESSEQVIF;
else
typeifforcheckinfor=ob_var::GREATEQVIF;
}

~ob_forfor(){}

void add(ob_savewrapper* var){
ob_savewrapper step1(step.getptr()->duplicate());
ob_savewrapper var1(var->getptr()->duplicate());
var1.add(&step1);
var->asg(&var1);
}

bool check(ob_savewrapper* var) const{
///return lastvalue.getptr()->compare(var->getptr(),typeifforcheckinfor);
return var->getptr()->compare(lastvalue.getptr(),typeifforcheckinfor);
}
};

//variable for store
class ob_varbase:public ob_type_name{
public:
ob_varbase(ob_type_namestatic& name):ob_type_name(name){}
virtual ob_type_name bigname()=0;//for FOR operator
virtual ob_savewrapper* get()=0;
virtual ~ob_varbase(){}//do not need virtual destructor because not delete varone and vararray as varbase
};

class ob_varone:public ob_varbase{
ob_savewrapper value;
public:

//ob_varone(ob_type_char* name);
ob_varone(ob_type_namestatic& name);

ob_type_ident isA(){return value.getptr()->isA();}

ob_savewrapper* get(){return (&value);}

ob_type_name bigname();//for FOR operator
~ob_varone(){}
};

typedef mstore_dynamicalloc<ob_savewrapper,ob_type_arraysize> ob_type_v;

//array for store
class ob_vararray:public ob_varbase{
mvect_common<ob_type_v> value;
ob_type_dim dimension;
ob_type_dim dimension1;
ob_type_arraysize sizecurrent;
ob_type_arraysize sizecreat;
public:

ob_vararray(ob_type_namestatic& name,ob_type_dim* ptrdim);

ob_type_ident isA(){
return value[0].getptr()->isA();
}

ob_savewrapper* get(){return &(value[sizecurrent]);}
void setsize(ob_type_dim* ptrdimcurrent);

const ob_type_arraysize* getsize(ob_type_arraydimension* kr){
*kr=dimension.getcount();
return dimension.c_str();
}

//return value because need modify name for FOR oper.
ob_type_name bigname();

virtual ~ob_vararray(){}
};

class ob_file_base{

ob_type_filenum number;

public:
ob_file_base(ob_type_filenum num):number(num){}
virtual ~ob_file_base(){}
ob_type_filenum getnum(){return number;}

virtual void get_stream(ob_istream** ptr)=0;
virtual void get_stream(ob_ostream** ptr)=0;
};

class ob_input_file:public ob_file_base{

ob_ifstream file;

ob_istream iss;

public:

ob_input_file(
ob_type_char* name,
ios::openmode mode,
ob_type_filenum num,
ob_modestream ms);

~ob_input_file(){
file.close();
}

void get_stream(ob_istream** ptr){*ptr=&iss;}
void get_stream(ob_ostream** ptr){OB_ERR(ob_err97);}
};

class ob_output_file:public ob_file_base{

ob_ofstream file;

ob_ostream_for_file oss;

public:

ob_output_file(
ob_type_char* name,
ios::openmode mode,
ob_type_filenum num,
ob_modestream ms);

~ob_output_file(){
file.close();
}

void get_stream(ob_istream** ptr){OB_ERR(ob_err98);}
void get_stream(ob_ostream** ptr){*ptr=&oss;}
};

class ob_lex;
typedef void (ob_lex::*ob_oper)();

class ob_functor_break_base{
public:
ob_functor_break_base(){}
virtual ~ob_functor_break_base(){}
virtual void run_before(ob_obasic* basicptr)=0;
virtual void run_after(ob_obasic* basicptr)=0;
};

class ob_operator:public ob_type_namestatic{
public:
enum flbreak{NOBREAK,BREAK};
private:
ob_oper ptroper;
flbreak before,after;

ob_functor_break_base* ptrbreak;

public:

ob_operator();
ob_operator(ob_type_char* name,ob_oper p,flbreak b=NOBREAK,flbreak a=NOBREAK);

void run(ob_lex* lex);

void set_get_break(flbreak b,flbreak a){before=b;after=a;}
void set_get_break(flbreak* b,flbreak* a) const {*b=before;*a=after;}

void set_get_break(ob_functor_break_base* ptrbreak1){ptrbreak=ptrbreak1;}
void set_get_break(ob_functor_break_base** ptrbreak1) const {*ptrbreak1=ptrbreak;}

void runbefore(ob_obasic* basicptr){if(ptrbreak!=OB_NULL) ptrbreak->run_before(basicptr);}
void runafter(ob_obasic* basicptr){if(ptrbreak!=OB_NULL) ptrbreak->run_after(basicptr);}
};

class ob_api:public ob_type_name{
public:
ob_api(ob_type_char* a):ob_type_name(a){}
virtual void run(ob_obasic* ptr,ob_par* p,ob_savewrapper* v)=0;
virtual ~ob_api(){}
};

template <class typecompare,class typestore> class ob_hashfind{
typecompare* d;
public:
ob_hashfind(typecompare& ddd):d(&ddd){}
ob_type_hashtablelength gethash(){return d->gethash();}
bool operator()(void* dd){
if(*((typestore*)dd)==(*d)) return true; else return false;
}
};

class ob_filefind{
const ob_type_filenum d;
public:

ob_filefind(const ob_type_filenum ddd):d(ddd){}
bool operator()(void* dd){
if(((ob_file_base*)dd)->getnum()==d) return true; else return false;
}
ob_type_hashtablelength gethash(){return (ob_type_hashtablelength)d;}
};

extern ob_type_namestatic ob_THENkwrd;
extern ob_type_namestatic ob_ENDIFkwrd;
extern ob_type_namestatic ob_TOkwrd;
extern ob_type_namestatic ob_STEPkwrd;
extern ob_type_namestatic ob_NEXTkwrd;
extern ob_type_namestatic ob_FORkwrd;
extern ob_type_namestatic ob_INPUTkwrd;
extern ob_type_namestatic ob_OUTPUTkwrd;
extern ob_type_namestatic ob_ASkwrd;
extern ob_type_namestatic ob_FILEkwrd;
extern ob_type_namestatic ob_ELSEkwrd;
extern ob_type_namestatic ob_DATAkwrd;
extern ob_type_namestatic ob_DUMMYkwrd;
extern ob_type_namestatic ob_SHARPkwrd;
extern ob_type_namestatic ob_REMkwrd;

//for version 1.90
extern ob_type_namestatic ob_UNICODEkwrd;
extern ob_type_namestatic ob_UNICODE_REVERSEkwrd;
extern ob_type_namestatic ob_UTF8kwrd;
extern ob_type_namestatic ob_CHARkwrd;

#define OB_NUM_PREVIOUS_EOL_PTR 3

class ob_lex{
public:

enum typelex{
PLUS,MINUS,MUL,DIV,POWER,ASSIGN,LP,RP,STRING,SEMICOLON,COMMA,ENDPROGRAMM,EOL,
LESS,GREAT,SHARP,BREAKPOINT,
DECINTNUMBER,HEXINTNUMBER,FLOATNUMBER,
FUNC,OPERATOR,
OLDVARIABLE,OLDARRAY,NEWNAME,
BAD,LABEL,DUMMYCHAR1,DUMMYCHAR2
};

private:

ob_obasic* ptrbasic;

typelex            currentlex;
ob_type_string     currentlexasstring;
ob_type_namestatic currentlexasnamestatic;

ob_operator*       currentoperator;//for find operator

ob_api*            currentfunction;
ob_varone*         currentvar;
ob_vararray*       currentarray;

ob_type_streampos previousptr;
///ob_type_streampos  firstptr;

//istream for program need open in bynary mode
ob_istream in;

enum typego{GOTO,GOSUB};
void opergo(typego tg);
void dopinput(ob_lex* lex);
//for version 1.50
void findkeywordforif(ob_type_namestatic* kwrd1,ob_type_namestatic* kwrd2);

//expression calculation
void power(ob_savewrapper* left);
void term(ob_savewrapper* left);
void prim(ob_savewrapper* left);

//for version 1.50
void findother(ob_type_char other);//for ob_lex find CR-LF

public:

void findeol();

ob_api*            getcurrentfunction(){return currentfunction;}  //for ob_obasic::run
ob_operator*       getcurrentoperator(){return currentoperator;}  //for ob_obasic::run

ob_lex(ob_istreamcommon* i,ob_obasic* p);

void setbasicptr(ob_obasic* p){ptrbasic=p;}//do not valid this in init list ob_obasic

~ob_lex(){}

void expr(ob_savewrapper* left);

typelex get_lex();//token analizer

ob_istream* get_input_stream(){return &in;}//for set UNICODE flags and input stream

bool iszero(){return in.iszero();}

typelex gettype() const {return currentlex;}

ob_type_namestatic& getstring(){return currentlexasnamestatic;}

ob_type_streampos getposition() const {return in.getposition();}//for ob_par::work ob_obasic::load operforfor

void putbacklex(){in.setposition(previousptr);}

void restore(){//for DATA operator
///ob_seekg(in,firstptr);
in.restore();
get_lex();
}

void getsizearray(ob_type_dim* ptrdimcurrent);//for ob_lex::extractvar and ob_lex::operdim

ob_varbase* extractvar();//for ob_lex::operXXXX
ob_varbase* extractvarwithoutcreate();//for ob_obasic::readvar

void workvariable(ob_savewrapper* left);//for ob_obasic::run and ob_lex::operfor

void setposition(ob_label* lab){lab->setposition(&in);}
void setposition(ob_data_for_jump* d){d->setposition(&in);}//for operreturn
void setposition(ob_type_streampos pos){in.setposition(pos);}//for restoreinputpointer and get_lex

void partostream(ob_ostrstream* name,ob_type_streampos one);//for ob_api creat string of all parameters

template <class ob_type>
typename ob_type::typedatavar gettypevalue(){//do not use for ob_str

ob_savewrapper tmp;
expr(&tmp);

typename ob_type::typedatavar ret;

tmp.getptr()->cast(&ret);

return ret;
}

//operators
void operprint();     //1
void operinput();     //2
void operif();        //3
void operfor();       //4
void opernext();      //5
void opergoto();      //6
void opergosub();     //7
void operreturn();    //8
void operlet();       //9
void operdim();       //10
void operstop();      //11
void operend();       //12
void operrem();       //13
void operopen();      //14
void operclose();     //15
void operkill();      //16
void operread();      //17
void operdata();      //18
void operrestore();   //19
void operrandomize(); //20
void operelse();      //21
void operendif();     //22

};//stop class ob_lex

#define OB_NUMTYPES 3

#define OB_TYPEFLO 0
#define OB_TYPEINT 1
#define OB_TYPESTR 2

class ob_par{
public:
ob_type_parnum* mo[OB_NUMTYPES];
ob_ostrstream *name,*pc;
ob_type_flo *pf;
ob_type_int *pi;
void initpar();
ob_par(){initpar();}
void work(ob_lex* p);
void deletepar();
~ob_par(){deletepar();}
};

//for API defines
#define OB_DECLFUNAPI(name,typeret)                      \
typeret name(                                            \
ob_obasic* ptr,                                          \
const ob_type_char* parstring,                           \
const ob_type_parnum *descrf,                            \
const ob_type_parnum *descri,                            \
const ob_type_parnum *descrc,                            \
const ob_type_flo* parf,                                 \
const ob_type_int* pari,                                 \
const ob_type_char* parc)

#define OB_TYPEFUNAPI(name,typeret)                      \
typedef typeret (*name)(                                 \
ob_obasic* ptr,                                          \
const ob_type_char* parstring,                           \
const ob_type_parnum *descrf,                            \
const ob_type_parnum *descri,                            \
const ob_type_parnum *descrc,                            \
const ob_type_flo* parf,                                 \
const ob_type_int* pari,                                 \
const ob_type_char* parc);

#define OB_DECLARE_USER_FUNCTION(name,typeret)           \
typeret name(                                            \
ob_obasic* basic_ptr,                                    \
const ob_type_char* parstring,                           \
const ob_type_parnum *descrf,                            \
const ob_type_parnum *descri,                            \
const ob_type_parnum *descrc,                            \
const ob_type_flo* parf,                                 \
const ob_type_int* pari,                                 \
const ob_type_char* parc)

OB_TYPEFUNAPI(ob_type_flofun,ob_type_flo)
OB_TYPEFUNAPI(ob_type_intfun,ob_type_int)
OB_TYPEFUNAPI(ob_type_charfun,ob_type_char*)

enum ob_type_del{OB_DELETE,OB_NODELETE};

#define OB_CLASSAPI(name,typeptr,dopdata,construct)             \
class name:public ob_api{                                       \
typeptr ptrfun;                                                 \
dopdata                                                         \
public:                                                         \
typedef typeptr typefun;                                        \
construct;                                                      \
void run(ob_obasic* ptr,ob_par* p,ob_savewrapper* v);           \
~name(){}                                                       \
};

OB_CLASSAPI(ob_apif,ob_type_flofun,,ob_apif(ob_type_char* a,ob_type_flofun))
OB_CLASSAPI(ob_apii,ob_type_intfun,,ob_apii(ob_type_char* a,ob_type_intfun))
OB_CLASSAPI(ob_apic,ob_type_charfun,ob_type_del dt;,
ob_apic(ob_type_char* a,ob_type_charfun,ob_type_del d))

extern const ob_type_small_char* ob_englmessage[];

typedef mlist<void*,ob_type_countlist>                         ob_typelist;
typedef mhash<void*,ob_type_hashtablelength,ob_type_countlist> ob_typetable;
typedef mstack<ob_data_for_jump,ob_type_countlist>  ob_typestack;

template<class typestore,class find>
bool
replaceobjcommon1(ob_typetable* table,typestore* tmpptr1,find& f){

void* tmpptr=OB_NULL;
void* tmpptr2=tmpptr1;
bool fl=table->replaceobj(f,tmpptr2,&(tmpptr));
if(fl==true) delete ((typestore*)tmpptr);
return fl;
}

template<class typecompare,class typestore>
bool
replaceobjcommon2(ob_typetable* table,typestore* tmpptr1,typecompare* name){

ob_hashfind<typecompare,typestore> find(*name);

return
replaceobjcommon1<typestore,ob_hashfind<typecompare,typestore> >(
table,tmpptr1,find);
}

template<class T>
void ob_clearlist(ob_typelist* list){
ob_type_countlist       j;

ob_type_countlist       lenlist=list->getcount();

void* delptr;
for(j=0;j!=lenlist;++j){
list->get(&delptr);
T* delptr1=(T*)delptr;
delete delptr1;
}//for

}

template<class T>
void ob_cleartable(ob_typetable* table){

ob_type_hashtablelength i;

ob_type_hashtablelength lenvect=table->getcount();

for(i=0;i!=lenvect;++i){
ob_clearlist<T>(&((*table)[i]));
}//for

}

template <class T>
T* ob_findcommon(ob_type_namestatic* a,ob_typetable* table1){

ob_hashfind<ob_type_namestatic,T> findcomm(*a);
void** voidptr=table1->findobj(findcomm);
T* currentptr;
if(voidptr!=OB_NULL) currentptr=(T*)(*voidptr); else currentptr=OB_NULL;
return currentptr;
}

template <class T> T* ob_findcommon(ob_type_char* name,ob_typetable* table1){
ob_type_namestatic a(name);
return ob_findcommon<T>(&a,table1);
}

class ob_functoraccessarray{
ob_type_arraysize* as;
public:
ob_functoraccessarray(ob_type_arraysize* as1):as(as1){}
ob_savewrapper* operator()(ob_obasic* ptr,ob_type_char* name);
};

template <bool flcreate>
class ob_functoraccessvar{
public:
ob_savewrapper* operator()(ob_obasic* ptr,ob_type_char* name){

ob_istrstream tmp1(name);
ob_shortstrstream tmp;

ob_standartstring st=tmp1.str();

ob_wstream_to_stream(&tmp,&st);

ob_lex local_var_access_lex(&tmp,ptr);//local copy need because recursion call is possible

ob_modestream ms(ptr->modestream_for_var_access_lex);//use default init constructor
ob_modestream* msptr=local_var_access_lex.get_input_stream();
*msptr=ms;//use default assign operator

local_var_access_lex.get_lex();

ob_varbase* vb;

if(flcreate==false)
vb=local_var_access_lex.extractvarwithoutcreate();//may be return NULL pointer
else
vb=local_var_access_lex.extractvar();//may be return NULL pointer

if(vb==OB_NULL)
return OB_NULL;
else
return vb->get();

}

};

#define OB_MAX_NESTED_GOSUB_DEFAULT (4096)

class ob_obasic{

private:

void label_insert(ob_type_streampos mbl);
ob_obasic& operator=(ob_obasic& ob){return *this;}

ob_obasic(ob_obasic& ob):
tablevar(ob_hashlength),   tablearray(ob_hashlength),
tablefun(ob_hashlength),   tablelabel(ob_hashlength),
tableforfor(ob_hashlength),tablefile(ob_hashlength),
baselex(OB_NULL,OB_NULL),
readlex(OB_NULL,OB_NULL),
read_from_file_lex(OB_NULL,OB_NULL),
datalex(OB_NULL,OB_NULL),
datastream(OB_NULL)
{}

friend class ob_lex;
friend class ob_functoraccessarray;
friend class ob_functoraccessvar<true>;
friend class ob_functoraccessvar<false>;

public:

enum namestream{
STREAM_BASE,
STREAM_READ,STREAM_READ_FROM_FILE,STREAM_DATA,STREAM_VAR_ACCESS,STREAM_OUT
};

enum typeload{OVERLAY,NOOVERLAY};
enum step{NOSTEP,STEP};
enum typeend{
DUMMYEND,ENDFILEEND,ENDOPERATORDETECT,EOLEND,BREAKPOINTEND,NOLOADEND,
BREAKBEFOREOPERATOR,BREAKAFTEROPERATOR
};

step stepflag;

private:
//static variable
static const ob_type_small_char* aboutmsg;
//for version 1.50
static const ob_type_serialnum sernum;
static const ob_type_version ver;

//non-static variable
typeend te;//for return value. need member for reenter after bef/aft oper break
ob_type_streampos retvalueptr;
bool endoperatordetect;//for exit if END operator detect
void setendoperatordetect(bool fl){endoperatordetect=fl;}

//containers
ob_operator  tableoperator[ob_maxnumoperator];

ob_typetable tablevar;   //ob_varone
ob_typetable tablearray; //ob_vararray
ob_typetable tablefun;   //ob_savewrapperapi
ob_typetable tablelabel; //ob_label
ob_typetable tableforfor;//ob_forfor
ob_typetable tablefile;  //ob_file

//ob_typelist  listfile;
ob_typestack stackgosub;

//variables
ob_lex baselex;
ob_lex readlex;
ob_lex read_from_file_lex;
ob_lex datalex;
ob_modestream modestream_for_var_access_lex;

ob_ostream out;//out for PRINT operator (need open in text mode)
ob_shortstrstream* datastream;//for DATA operator, create in load

ob_operator* tmpoperator;//for run operator because baselex::currentoperator can overwrite

//for version 1.50
ob_file_base* findfile(ob_type_filenum number);//for ob_lex::operXXXX

//for version 1.90
ob_type_countlist maxnestedgosub;

template <class ob_type,class ob_functoraccess>
bool readvarcommon(
ob_type_char *name,ob_savewrapper* aa,ob_functoraccess& functoraccess){
bool ret;
ob_savewrapper* aaa;
if((aaa=functoraccess(this,name))!=OB_NULL){
ob_savewrapper aaaa(aaa->getptr()->duplicate());
aa->setptr(new ob_type());
aa->asg(&aaaa);
ret=true;
}//if
else{
ret=false;
}//else
return ret;
}

template <class ob_type>
bool readvar1(
ob_type_char* name,
typename ob_type::typedatavar* val,
ob_type_arraysize* as=OB_NULL){

ob_functoraccessvar<false> functoraccess1;
ob_functoraccessarray functoraccess2(as);
ob_savewrapper aa;

bool fl;
if(as==OB_NULL)
fl=readvarcommon<ob_type>(name,&aa,functoraccess1);
else

fl=readvarcommon<ob_type>(name,&aa,functoraccess2);

if(fl==true) aa.getptr()->cast(val);
return fl;
}

template <class ob_type,class ob_functoraccess>
bool writevarcommon(
ob_type_char *name,
typename ob_type::typedatavar val,
ob_functoraccess& functoraccess){
bool ret;
ob_type* a=new ob_type(val);
ob_savewrapper aa(a);
ob_savewrapper* aaa;
if((aaa=functoraccess(this,name))!=OB_NULL){
aaa->asg(&aa);
ret=true;
}//if
else{
ret=false;
}//else
return ret;
}

template <class ob_type>
bool writevar1(
ob_type_char* name,
typename ob_type::typedatavar val,
ob_type_arraysize* as){

ob_functoraccessvar<true> functoraccess1;
ob_functoraccessarray functoraccess2(as);
if(as==OB_NULL)
return writevarcommon<ob_type>(name,val,functoraccess1);
else
return writevarcommon<ob_type>(name,val,functoraccess2);
}

template <class ob_functoraccess>
ob_type_ident strlenvarcommon(
ob_type_char* name,
ob_type_stringsize* len,
ob_functoraccess& functoraccess){

ob_type_ident ret=OB_NOIDENT;


ob_savewrapper* aaa;

if((aaa=functoraccess(this,name))!=OB_NULL){
ret=aaa->getptr()->isA();
*len=aaa->getptr()->size();
}//if

return ret;
}

template <class Tobapi>
bool setfun1(ob_type_char* name,typename Tobapi::typefun f){
ob_type_namestatic a(name);

return
replaceobjcommon2<ob_type_namestatic,ob_api>(&tablefun,new Tobapi(name,f),&a);
}

ob_operator* findoperator(ob_type_namestatic* namestatic);//setbreak and get_lex

void
createarray(ob_type_namestatic* namearraystatic,ob_type_dim* ptrdimcurrent){

replaceobjcommon2<ob_type_namestatic,ob_vararray>(
&tablearray,
new ob_vararray(*namearraystatic,ptrdimcurrent),
namearraystatic
);
}

void load1(ob_istreamcommon* i);
typeend run1();

template<class T>
bool set_get_operatorbreak(ob_type_char* name,T b,T a){
bool ret=true;

ob_type_namestatic namestatic(name);
ob_operator* current=findoperator(&namestatic);

if(current==OB_NULL) ret=false; else current->set_get_break(b,a);

return ret;
}

template<class T>
bool set_get_operatorbreak(ob_type_char* name,T b){
bool ret=true;

ob_type_namestatic namestatic(name);
ob_operator* current=findoperator(&namestatic);

if(current==OB_NULL) ret=false; else current->set_get_break(b);

return ret;
}

public:

ob_obasic(ob_ostreamcommon* o=&cout,ob_istreamcommon* rd=&cin);

virtual ~ob_obasic(){
clrtablfun();
clrtabl(NOOVERLAY);
}

static const ob_type_small_char* about(){return aboutmsg;}

void load(ob_istreamcommon* i,typeload a=ob_obasic::NOOVERLAY){
//clear old ptr and clear old tablelabel and tablevar
clrtabl(a);
load1(i);
}

void clear_project(){clrtabl(NOOVERLAY);}
void load_project(ob_istreamcommon* i){load1(i);}

ob_istreamcommon* get_current_input_stream(){//for find current stream
ob_istreamcommon* cf;
baselex.get_input_stream()->get_stream(&cf);
return cf;
}

void set_current_input_stream(ob_istreamcommon* i){
baselex.get_input_stream()->set_stream_with_startposition(i);
}

typeend run(){return run1();}

void setstep(step ts){stepflag=ts;}
step getstep() const {return stepflag;}

//for API variable and array
ob_type_ident typevar(ob_type_char* name){
ob_varone* current=ob_findcommon<ob_varone>(name,&tablevar);
if(current!=OB_NULL) return current->isA(); else return OB_NOIDENT;
}

ob_type_ident typearray(ob_type_char* name){
ob_vararray* current=ob_findcommon<ob_vararray>(name,&tablearray);
if(current!=OB_NULL) return current->isA(); else return OB_NOIDENT;

}

void createvar(ob_type_char* name){
ob_functoraccessvar<true> functoraccess;
functoraccess(this,name);
}

void createarray(
ob_type_char* name,ob_type_arraydimension kr,ob_type_arraysize* as){

ob_type_namestatic namearraystatic(name);
ob_type_dim ptrdimcurrent(as,kr);
createarray(&namearraystatic,&ptrdimcurrent);
}

ob_type_ident
strlenvar(
ob_type_char* name,
ob_type_stringsize* len,
ob_type_arraysize* as=OB_NULL){

ob_functoraccessvar<false> functoraccess1;
ob_functoraccessarray functoraccess2(as);

if(as==OB_NULL)
return strlenvarcommon(name,len,functoraccess1);
else
return strlenvarcommon(name,len,functoraccess2);
}

bool
writevar(ob_type_char* name,ob_type_flo val,ob_type_arraysize* as=OB_NULL){
return writevar1<ob_float>(name,val,as);
}

bool
writevar(ob_type_char* name,ob_type_int val,ob_type_arraysize* as=OB_NULL){
return writevar1<ob_int>(name,val,as);
}

bool
writevar(ob_type_char* name,ob_type_char* val,ob_type_arraysize* as=OB_NULL){
return writevar1<ob_str>(name,val,as);
}

bool
readvar(ob_type_char* name,ob_type_flo* val,ob_type_arraysize* as=OB_NULL){
return readvar1<ob_float>(name,val,as);
}

bool
readvar(ob_type_char* name,ob_type_int* val,ob_type_arraysize* as=OB_NULL){
return readvar1<ob_int>(name,val,as);
}

bool
readvar(ob_type_char* name,ob_type_char* val,ob_type_arraysize* as=OB_NULL){

ob_functoraccessvar<false> functoraccess1;
ob_functoraccessarray functoraccess2(as);

ob_savewrapper aa;
ob_type_char* val1;

bool fl;
if(as==OB_NULL)
fl=readvarcommon<ob_str>(name,&aa,functoraccess1);
else
fl=readvarcommon<ob_str>(name,&aa,functoraccess2);

if(fl==true){
aa.getptr()->cast(&val1);
ob_strcpy(val,val1);
}//if
return fl;
}

const ob_type_arraysize* getarraysize(
ob_type_char* name,ob_type_arraydimension* kr){

ob_vararray* current=ob_findcommon<ob_vararray>(name,&tablearray);

if(current!=OB_NULL)
return current->getsize(kr);
else
return OB_NULL;
}

//for API user function
bool setfun(ob_type_char* name,ob_type_flofun f){
return setfun1<ob_apif>(name,f);
}

bool setfun(ob_type_char* name,ob_type_intfun f){
return setfun1<ob_apii>(name,f);
}

bool setfun(ob_type_char* name,ob_type_charfun f,ob_type_del dt){
ob_type_namestatic a(name);

return
replaceobjcommon2<ob_type_namestatic,ob_api>
(&tablefun,new ob_apic(name,f,dt),&a);
}

//for version 1.50
bool delfun(ob_type_char* name){

ob_type_namestatic a(name);

ob_hashfind<ob_type_namestatic,ob_api> find(a);
void* delptr=OB_NULL;
bool ret=tablefun.get(find,&(delptr));
if(ret==true) delete ((ob_api*)delptr);
return ret;
}

static ob_type_serialnum serial_number(){return sernum;}
static ob_type_int version(){return ver;}

void clrtabl(typeload a){

ob_cleartable<ob_label>(&tablelabel);
ob_cleartable<ob_forfor>(&tableforfor);
stackgosub.clear();

if(a==NOOVERLAY){
   if(datastream!=NULL) delete datastream;
   datastream=NULL;
   ob_cleartable<ob_varone>(&tablevar);
   ob_cleartable<ob_vararray>(&tablearray);
   ob_cleartable<ob_file_base>(&tablefile);
}//if

}

void clrtablfun(){ob_cleartable<ob_api>(&tablefun);}

bool setoperatorbreak(ob_type_char* name,ob_operator::flbreak b,ob_operator::flbreak a){
return set_get_operatorbreak(name,b,a);
}

bool getoperatorbreak(ob_type_char* name,ob_operator::flbreak* b,ob_operator::flbreak* a){
return set_get_operatorbreak(name,b,a);
}

bool setoperatorbreak(ob_type_char* name,ob_functor_break_base* b){
return set_get_operatorbreak(name,b);
}

bool getoperatorbreak(ob_type_char* name,ob_functor_break_base** b){
return set_get_operatorbreak(name,b);
}

const ob_typetable* gettablevar()   {return &tablevar;}
const ob_typetable* gettablearray() {return &tablearray;}
const ob_typetable* gettablefun()   {return &tablefun;}
const ob_typetable* gettablelabel() {return &tablelabel;}
const ob_typetable* gettableforfor(){return &tableforfor;}
const ob_typetable* gettablefile()  {return &tablefile;}
const ob_typestack* getstackgosub() {return &stackgosub;}

ob_type_char* getcurrentoperatorname(){return tmpoperator->getdata();}

void setin(ob_istreamcommon* i){readlex.get_input_stream()->set_stream_with_startposition(i);}
void setout(ob_ostreamcommon* o){out.set_stream_with_startposition(o);}

ob_lex::typelex gettypenextlex(){
baselex.get_lex();
return baselex.gettype();
}

void restoreinputpointer(){baselex.setposition(retvalueptr);}

//for version 1.90
ob_modestream get_modestream(namestream ns);
void set_modestream(namestream ns,ob_modestream ms);

void set_max_nested_gosub(ob_type_countlist m){maxnestedgosub=m;}
ob_type_countlist get_max_nested_gosub(){return maxnestedgosub;}

};//end class ob_obasic

class ob_err{
ob_type_codeerror code;                        
public:
ob_err(ob_type_codeerror c){code=c;}

virtual void release(ob_ostreamcommon* os,const ob_type_small_char* currentmessage[]){
(*os)<<endl<<(currentmessage)[code-ob_basecodeerror];
(*os)<<endl<<"code error= "<<(code-ob_basecodeerror);
(*os)<<endl;
}

virtual ob_type_codeerror getcode(){return code;}
virtual ~ob_err(){}
};

//prototypes
void ob_allocspaceforvar(
ob_type_namestatic* str,ob_savewrapper* value,ob_type_arraysize size);//for varone and vararray

bool ob_loadbreakstr(
ob_istreamcommon* in,
ob_type_stringsize lengthbuffers,
ob_type_char* ptrbefore,
ob_type_char* ptrcurrent,
ob_type_char* ptrafter,
ob_modestream ms=ob_modestream()
);

ob_type_int ob_hexconvert(ob_type_char* a);

OB_DECLARE_USER_FUNCTION(ob_fun_rnd,ob_type_flo);
OB_DECLARE_USER_FUNCTION(ob_fun_abs,ob_type_flo);
OB_DECLARE_USER_FUNCTION(ob_fun_sin,ob_type_flo);
OB_DECLARE_USER_FUNCTION(ob_fun_cos,ob_type_flo);
OB_DECLARE_USER_FUNCTION(ob_fun_atn,ob_type_flo);
OB_DECLARE_USER_FUNCTION(ob_fun_sqr,ob_type_flo);
OB_DECLARE_USER_FUNCTION(ob_fun_exp,ob_type_flo);
OB_DECLARE_USER_FUNCTION(ob_fun_log,ob_type_flo);
OB_DECLARE_USER_FUNCTION(ob_fun_log10,ob_type_flo);
OB_DECLARE_USER_FUNCTION(ob_fun_int,ob_type_int);
OB_DECLARE_USER_FUNCTION(ob_fun_sgn,ob_type_int);
OB_DECLARE_USER_FUNCTION(ob_fun_len,ob_type_int);
OB_DECLARE_USER_FUNCTION(ob_fun_dat,ob_type_char*);
OB_DECLARE_USER_FUNCTION(ob_fun_clk,ob_type_char*);
OB_DECLARE_USER_FUNCTION(ob_fun_digit2str,ob_type_char*);
OB_DECLARE_USER_FUNCTION(str2flo,ob_type_flo);
OB_DECLARE_USER_FUNCTION(str2int,ob_type_int);
OB_DECLARE_USER_FUNCTION(ob_fun_digit2hexstr,ob_type_char*);

#ifdef OB_WCHAR
}//end namespace ob_wcharspace
#else
}//end namespace ob_charspace
#endif

#endif

