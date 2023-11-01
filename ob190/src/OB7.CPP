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

//function, build in ob_obasic in the beginning (with ob_api interface)

//random
ob_type_flo ob_fun_rnd(
ob_obasic* ptr,const ob_type_char* parstring,
const ob_type_parnum *descrf, const ob_type_parnum *descri, const ob_type_parnum *descrc,
const ob_type_flo*    parf,   const ob_type_int*    pari,   const ob_type_char*   parc
){
ob_type_flo data=((ob_type_flo)rand())/((ob_type_flo)RAND_MAX);
if(ptr&&parstring&&descrf&&descri&&descrc&&parf&&pari&&parc) return data;
return data;
}

#define OB_TYPEFUN(name,fun,typeret)                                                      \
typeret name(                                                                             \
ob_obasic* ptr,const ob_type_char* parstring,                                             \
const ob_type_parnum *descrf, const ob_type_parnum *descri, const ob_type_parnum *descrc, \
const ob_type_flo*    parf,   const ob_type_int*    pari,   const ob_type_char*   parc    \
){                                                                                        \
ob_type_flo data=0;                                                                       \
                                                                                          \
if( ((descrf[0]+descri[0]+descrc[0])!=1) || ((descrf[0]+descri[0])!=1)) OB_ERR(ob_err23); \
                                                                                          \
if(descri[0]!=0) data=(ob_type_flo)pari[0];                                                            \
if(descrf[0]!=0) data=parf[0];                                                            \
if(ptr&&parstring&&descrf&&descri&&descrc&&parf&&pari&&parc)                              \
return (typeret)fun(data);                                                                \
return (typeret)fun(data);                                                                \
}

OB_TYPEFUN(ob_fun_abs,fabs,   ob_type_flo)
OB_TYPEFUN(ob_fun_sin,sin,    ob_type_flo)
OB_TYPEFUN(ob_fun_cos,cos,    ob_type_flo)
OB_TYPEFUN(ob_fun_atn,atan,   ob_type_flo)
OB_TYPEFUN(ob_fun_sqr,sqrt,   ob_type_flo)
OB_TYPEFUN(ob_fun_exp,exp,    ob_type_flo)
OB_TYPEFUN(ob_fun_log,log,    ob_type_flo)
OB_TYPEFUN(ob_fun_log10,log10,ob_type_flo)
OB_TYPEFUN(ob_fun_int,ceil,   ob_type_int)

//signum
ob_type_int ob_fun_sgn(
ob_obasic* ptr,const ob_type_char* parstring,
const ob_type_parnum *descrf, const ob_type_parnum *descri, const ob_type_parnum *descrc,
const ob_type_flo*    parf,   const ob_type_int*    pari,   const ob_type_char*   parc
){

ob_type_flo data=0;
int ret;

if( ((descrf[0]+descri[0]+descrc[0])!=1) || ((descrf[0]+descri[0])!=1)) OB_ERR(ob_err23);

if(descri[0]!=0) data=(ob_type_flo)pari[0];
if(descrf[0]!=0) data=parf[0];

if(data>0)  ret=1;
else
if(data==0) ret=0;
else
            ret=-1;

if(ptr&&parstring&&descrf&&descri&&descrc&&parf&&pari&&parc)//for warning off
return ret;
return ret;
}

//len
ob_type_int ob_fun_len(
ob_obasic* ptr,const ob_type_char* parstring,
const ob_type_parnum *descrf, const ob_type_parnum *descri, const ob_type_parnum *descrc,
const ob_type_flo*    parf,   const ob_type_int*    pari,   const ob_type_char*   parc
){
if(descrc[0]!=0) return (ob_type_int)(ob_strlen((ob_type_char*)parc)-1);
if(ptr&&parstring&&descrf&&descri&&descrc&&parf&&pari&&parc)
OB_ERR(ob_err23);
OB_ERR(ob_err23);
return 0;
}

//clk
ob_type_char* ob_fun_clk(
ob_obasic* ptr,const ob_type_char* parstring,
const ob_type_parnum *descrf, const ob_type_parnum *descri, const ob_type_parnum *descrc,
const ob_type_flo*    parf,   const ob_type_int*    pari,   const ob_type_char*   parc
){

const time_t tt=time(OB_NULL);

//struct tm t=*localtime(&tt);
struct tm t;
localtime_s(&t, &tt);

ob_ostrstream os;

os<<t.tm_hour<<":"<<t.tm_min<<":"<<t.tm_sec;

ob_type_char* p=new ob_type_char[os.str().length()+1];

ob_strcpy(p,os.str().c_str());

if(ptr&&parstring&&descrf&&descri&&descrc&&parf&&pari&&parc) return p;
return p;
}

//data
ob_type_char* ob_fun_dat(
ob_obasic* ptr,const ob_type_char* parstring,
const ob_type_parnum *descrf, const ob_type_parnum *descri, const ob_type_parnum *descrc,
const ob_type_flo*    parf,   const ob_type_int*    pari,   const ob_type_char*   parc
){

const time_t tt=time(OB_NULL);

//struct tm d=*localtime(&tt);
struct tm d;
localtime_s(&d, &tt);

ob_ostrstream os;

os<<(d.tm_mon+1)<<"-"<<d.tm_mday<<"-"<<(d.tm_year+1900);

ob_type_char* p=new ob_type_char[os.str().length()+1];

ob_strcpy(p,os.str().c_str());

if(ptr&&parstring&&descrf&&descri&&descrc&&parf&&pari&&parc) return p;
return p;
};

ob_type_char* ob_fun_digit2str(
ob_obasic* ptr,const ob_type_char* parstring,
const ob_type_parnum *descrf, const ob_type_parnum *descri, const ob_type_parnum *descrc,
const ob_type_flo*    parf,   const ob_type_int*    pari,   const ob_type_char*   parc
){

ob_ostrstream os;

if(descrf[0]!=0){
os<<parf[0];
}//if
else{
  if(descri[0]!=0){
  os<<pari[0];
  }//if
}//else

ob_type_char* p=new ob_type_char[os.str().length()+1];

ob_strcpy(p,os.str().c_str());

if(ptr&&parstring&&descrf&&descri&&descrc&&parf&&pari&&parc) return p;
return p;
}

#define OB_TYPEFUN1(name,typeret)                                                         \
typeret name(                                                                             \
ob_obasic* ptr,const ob_type_char* parstring,                                             \
const ob_type_parnum *descrf, const ob_type_parnum *descri, const ob_type_parnum *descrc, \
const ob_type_flo*    parf,   const ob_type_int*    pari,   const ob_type_char*   parc    \
){                                                                                        \
                                                                                          \
typeret ret=typeret();                                                                    \
                                                                                          \
if(descrc[0]!=0){                                                                         \
ob_istrstream is(parc);                                                                   \
is>>ret;                                                                                  \
}                                                                                         \
                                                                                          \
if(ptr&&parstring&&descrf&&descri&&descrc&&parf&&pari&&parc) return ret;                  \
return ret;                                                                               \
}                                                                                         \

OB_TYPEFUN1(str2flo,ob_type_flo)
OB_TYPEFUN1(str2int,ob_type_int)

ob_type_char* ob_fun_digit2hexstr(
ob_obasic* ptr,const ob_type_char* parstring,
const ob_type_parnum *descrf, const ob_type_parnum *descri, const ob_type_parnum *descrc,
const ob_type_flo*    parf,   const ob_type_int*    pari,   const ob_type_char*   parc

){

ob_ostrstream os;

if(descri[0]!=0){
os<<hex<<"0x"<<pari[0];
}//if

ob_type_char* p=new ob_type_char[os.str().length()+1];

ob_strcpy(p,os.str().c_str());

if(ptr&&parstring&&descrf&&descri&&descrc&&parf&&pari&&parc) return p;
return p;
}

#ifdef OB_WCHAR
}//end namespace ob_wcharspace
#else
}//end namespace ob_charspace
#endif

