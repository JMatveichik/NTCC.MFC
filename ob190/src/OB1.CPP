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

#ifdef OB_WCHAR
#define OB_KWRDDEF(xxx)                       \
ob_type_char* ob_support_string##xxx=L"xxx";  \
ob_type_namestatic ob_##xxx##kwrd(ob_support_string##xxx,sizeof(ob_support_string##xxx));
#else
#define OB_KWRDDEF(xxx)                       \
ob_type_namestatic ob_##xxx##kwrd(#xxx,sizeof(#xxx));
#endif

OB_KWRDDEF(THEN)
OB_KWRDDEF(ENDIF)
OB_KWRDDEF(TO)
OB_KWRDDEF(STEP)
OB_KWRDDEF(NEXT)
OB_KWRDDEF(FOR)
OB_KWRDDEF(INPUT)
OB_KWRDDEF(OUTPUT)
OB_KWRDDEF(AS)
OB_KWRDDEF(FILE)
OB_KWRDDEF(ELSE)
OB_KWRDDEF(DATA)
ob_type_namestatic ob_DUMMYkwrd;

#ifdef OB_WCHAR
ob_type_namestatic ob_SHARPkwrd((ob_type_char*)L"#",sizeof(L"#"));
#else
ob_type_namestatic ob_SHARPkwrd((ob_type_char*)"#",sizeof("#"));
#endif

OB_KWRDDEF(REM)

//for version 1.90
OB_KWRDDEF(UNICODE)
OB_KWRDDEF(UNICODE_REVERSE)
OB_KWRDDEF(UTF8)
OB_KWRDDEF(CHAR)

inline void ob_lex::findother(ob_type_char other){

ob_type_char ch;
in.get(ch);

if(in.good()==false){
OB_ERR(ob_err9);
}//if
else{
if((in.eof()==false)&&(ch!=other)) in.putback();
}//else
}

ob_lex::typelex ob_lex::get_lex(){
bool flfloat=false;
bool flhex=false;
ob_type_char ch;
ob_type_char ch1;
ob_type_char *p=currentlexasstring.c_str();
ob_type_stringsize currentlexstrlength=0;

if(in.iszero()==true) OB_ERR(ob_err9);

previousptr=in.getposition();

for(;;){

in.get(ch);

if(in.eof()){
return currentlex=ENDPROGRAMM;
}//if

if(!in.good()) OB_ERR(ob_err9);

if(ob_isspace<ob_type_char>(ch)==false) break;
}//for

switch(ch){

#ifdef OB_WCHAR
case ob_const_dummychar1:currentlex=DUMMYCHAR1;goto m1;
case ob_const_dummychar2:currentlex=DUMMYCHAR2;goto m1;
#endif

case ob_const_sharp   :currentlex=SHARP;     goto m1;
case ob_const_at      :currentlex=BREAKPOINT;goto m1;
case ob_const_equ     :currentlex=ASSIGN;    goto m1;

#ifdef OB_WCHAR
case ob_const_nel:
case ob_const_ls :
case ob_const_ps :
currentlex=EOL;
goto m1;
#endif

case ob_const_lf      :
findother(ob_const_cr);
currentlex=EOL;
goto m1;

case ob_const_cr      :
findother(ob_const_lf);
currentlex=EOL;
goto m1;

case ob_const_asterisk:currentlex=MUL;      goto m1;
case ob_const_slash   :currentlex=DIV;      goto m1;
case ob_const_plus    :currentlex=PLUS;     goto m1;
case ob_const_minus   :currentlex=MINUS;    goto m1;
case ob_const_pow     :currentlex=POWER;    goto m1;
case ob_const_lp      :currentlex=LP;       goto m1;
case ob_const_rp      :currentlex=RP;       goto m1;
case ob_const_sc      :currentlex=SEMICOLON;goto m1;
case ob_const_comma   :currentlex=COMMA;    goto m1;
case ob_const_lp1     :currentlex=LESS;     goto m1;
case ob_const_rp1     :currentlex=GREAT;    goto m1;
m1:

p[currentlexstrlength]=ch;
++currentlexstrlength;
p[currentlexstrlength]=ob_const_endstring;
++currentlexstrlength;

currentlexasnamestatic.set(currentlexasstring.c_str(),currentlexstrlength);
return currentlex;//after this do not analyse currentlexasnamestatic, but....

case ob_const_quotes : //code "
case ob_const_quotes1: //code '
ch1=ch;

while(1){
  p[currentlexstrlength]=ch;

  ++currentlexstrlength;
  if(currentlexstrlength==ob_maxlengthstring){
  setposition(previousptr);
  OB_ERR(ob_err6);
  }//if

  in.get(ch);

  if(in.eof()){
  setposition(previousptr);
  OB_ERR(ob_err57);
  }//if

  if(!in.good()) OB_ERR(ob_err9);
  if(ch==ch1) break;
}//while

p[currentlexstrlength]=ch;
++currentlexstrlength;

p[currentlexstrlength]=ob_const_endstring;
++currentlexstrlength;

currentlexasnamestatic.set(currentlexasstring.c_str(),currentlexstrlength);
return currentlex=STRING;

case ob_const_point:
flfloat=true;
case ob_const_0:case ob_const_1:case ob_const_2:case ob_const_3:
case ob_const_4:case ob_const_5:case ob_const_6:case ob_const_7:
case ob_const_8:case ob_const_9:

while(1){
  p[currentlexstrlength]=ch;

  ++currentlexstrlength;
  if(currentlexstrlength==ob_maxlengthstring){
  setposition(previousptr);
  OB_ERR(ob_err6);
  }//if

  in.get(ch);

  if(in.eof())   break;
  if(!in.good()) OB_ERR(ob_err9);

  if((ch==ob_const_point)&&(flfloat==true)) OB_ERR(ob_err54);
  if(ch==ob_const_point) flfloat=true;

  if(((ch==ob_const_hex)||(ch==ob_const_HEX))&&(flhex==true)) OB_ERR(ob_err55);
  if((ch==ob_const_hex)||(ch==ob_const_HEX)){
  if((p[0]!=ob_const_0)||(currentlexstrlength!=1)) OB_ERR(ob_err55);
  flhex=true;
  }//if

  if((flhex==true)&&(flfloat==true)) OB_ERR(ob_err56);

  switch(ch){
  case ob_const_point:
  case ob_const_hex:
  case ob_const_HEX:
  case ob_const_a:
  case ob_const_b:
  case ob_const_c:
  case ob_const_d:
  case ob_const_e:
  case ob_const_f:
  case ob_const_A:
  case ob_const_B:
  case ob_const_C:
  case ob_const_D:
  case ob_const_E:
  case ob_const_F:
  case ob_const_0:
  case ob_const_1:
  case ob_const_2:
  case ob_const_3:
  case ob_const_4:
  case ob_const_5:
  case ob_const_6:
  case ob_const_7:
  case ob_const_8:
  case ob_const_9:
  break;
  default:
  in.putback();
  goto m10;
  }//switch

}//while

m10:

p[currentlexstrlength]=ob_const_endstring;
++currentlexstrlength;

currentlexasnamestatic.set(currentlexasstring.c_str(),currentlexstrlength);
if(flfloat==true) return currentlex=FLOATNUMBER;
else
if(flhex==true)   return currentlex=HEXINTNUMBER;
else
                  return currentlex=DECINTNUMBER;

default:

if(ob_isdelim<ob_type_char>(ch)==true){
return currentlex=BAD;
}//if

while(1){
  p[currentlexstrlength]=ch;

  ++currentlexstrlength;
  if(currentlexstrlength==ob_maxlengthstring){
  setposition(previousptr);
  OB_ERR(ob_err6);
  }//if

  in.get(ch);

  if(in.eof())   break;
  if(!in.good()) OB_ERR(ob_err9);

  if(ob_isdelim<ob_type_char>(ch)==true){
  break;
  }//if

}//while

p[currentlexstrlength]=ob_const_endstring;
++currentlexstrlength;

currentlexasnamestatic.set(currentlexasstring.c_str(),currentlexstrlength);

if(ch==ob_const_colon){
return currentlex=LABEL;
}//if

in.putback();//if do not LABEL detect do not insert delim from input stream

currentoperator=ptrbasic->findoperator(&currentlexasnamestatic);
if(currentoperator!=OB_NULL) return currentlex=OPERATOR;

currentfunction=ob_findcommon<ob_api>(&currentlexasnamestatic,&(ptrbasic->tablefun));
if(currentfunction!=OB_NULL) return currentlex=FUNC;

currentvar=ob_findcommon<ob_varone>(&currentlexasnamestatic,&(ptrbasic->tablevar));
if(currentvar!=OB_NULL) return currentlex=OLDVARIABLE;

currentarray=ob_findcommon<ob_vararray>(&currentlexasnamestatic,&(ptrbasic->tablearray));
if(currentarray!=OB_NULL) return currentlex=OLDARRAY;

return currentlex=NEWNAME;
}//switch
}

#ifdef OB_WCHAR
}//end namespace ob_wcharspace
#else
}//end namespace ob_charspace
#endif

