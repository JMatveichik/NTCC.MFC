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

//here is ob_obasic::function and read-only static member

const ob_type_small_char*        ob_obasic::aboutmsg    =
OB_STRINGDEFINE("(C)1998-2010 MKTMK Open Basic v1.90 www.mktmk.narod.ru mktmk@narod.ru");

const ob_type_serialnum    ob_obasic::sernum=101;
const ob_type_version      ob_obasic::ver   =190;

ob_obasic::typeend ob_obasic::run1(){

ob_operator::flbreak b;
ob_operator::flbreak a;

ob_api* funcptr;
ob_savewrapper left;
ob_par par;

if(baselex.iszero()==true){
te=NOLOADEND;
}//if
else{

    baselex.get_lex();

    for(;;){

    if(endoperatordetect==true){
    baselex.putbacklex();
    te=ENDOPERATORDETECT;
    goto m1;
    }//if

    switch(baselex.gettype()){

    case ob_lex::FUNC://call function without save return value
    funcptr=baselex.getcurrentfunction();
    par.work(&baselex);
    left.clear();
    funcptr->run(this,&par,&left);
    left.clear();
    baselex.get_lex();
    break;

    case ob_lex::OPERATOR:

    tmpoperator=baselex.getcurrentoperator();

    tmpoperator->set_get_break(&b,&a);

    if(b==ob_operator::BREAK){
      if(te!=BREAKBEFOREOPERATOR){
      baselex.putbacklex();
      te=BREAKBEFOREOPERATOR;
      goto m1;
      }//if
    }//if

    //after BREAKBEFOREOPERATOR point of control return to this point
    //because after putbacklex+get_lex baselex.gettype()==ob_lex::OPERATOR

    te=DUMMYEND;

    retvalueptr=baselex.getposition();
    tmpoperator->runbefore(this);

    tmpoperator->run(&baselex);

    retvalueptr=baselex.getposition();
    tmpoperator->runafter(this);

    if(a==ob_operator::BREAK){
    baselex.putbacklex();
    te=BREAKAFTEROPERATOR;
    goto m1;
    }//if
    else{
    baselex.putbacklex();//for balance
    baselex.get_lex();
    }//else

    break;

    case ob_lex::OLDARRAY:
    case ob_lex::OLDVARIABLE:
    case ob_lex::NEWNAME:
    baselex.workvariable(baselex.extractvar()->get());
    break;

    case ob_lex::EOL:
    if(stepflag==ob_obasic::STEP){
    te=EOLEND;
    goto m1;
    }//if

    case ob_lex::LABEL://in this level this is label
    case ob_lex::DECINTNUMBER://in this level this is label
    baselex.get_lex();
    break;

    case ob_lex::BREAKPOINT:
    te=BREAKPOINTEND;
    goto m1;

    case ob_lex::ENDPROGRAMM:
    te=ENDFILEEND;
    goto m1;

    default:
    OB_ERR(ob_err3)
    break;
    }//switch
    }//for(;;)
}//else

m1://exit for breakpoint token or step flag

retvalueptr=baselex.getposition();

return te;
}

ob_operator* ob_obasic::findoperator(ob_type_namestatic* namestatic){

ob_type_statictablesize j;
for(j=0;j!=ob_maxnumoperator;++j){

  ob_operator* current=&(tableoperator[j]);

  if(*((ob_type_namestatic*)(current))==ob_DUMMYkwrd) return OB_NULL;

  if(*((ob_type_namestatic*)(current))==(*namestatic)){
  return current;
  }//if

}//for
return OB_NULL;
}

#ifndef OB_WCHAR
#define OB_ADD(name1,name2)  ob_operator((ob_type_char*)name1,&ob_lex::name2);
#define OB_SETFUN(name1,name2) setfun((ob_type_char*)name1,name2);
#define OB_SETFUN1(name1,name2,name3) setfun((ob_type_char*)name1,name2,name3);
#else
#define OB_ADD(name1,name2)  ob_operator((ob_type_char*)L##name1,&ob_lex::name2);
#define OB_SETFUN(name1,name2) setfun((ob_type_char*)L##name1,name2);
#define OB_SETFUN1(name1,name2,name3) setfun((ob_type_char*)L##name1,name2,name3);
#endif

ob_obasic::ob_obasic(ob_ostreamcommon* o,ob_istreamcommon* rd):
tablevar(ob_hashlength),   tablearray(ob_hashlength),
tablefun(ob_hashlength),   tablelabel(ob_hashlength),
tableforfor(ob_hashlength),tablefile(ob_hashlength),

baselex(OB_NULL,OB_NULL),
readlex(rd,OB_NULL),
read_from_file_lex(OB_NULL,OB_NULL),
datalex(OB_NULL,OB_NULL),
datastream(OB_NULL),
maxnestedgosub(OB_MAX_NESTED_GOSUB_DEFAULT)
{

baselex.setbasicptr(this);
readlex.setbasicptr(this);
read_from_file_lex.setbasicptr(this);
datalex.setbasicptr(this);

//it is problem with initialization order if tableoperator still global

tableoperator[0]=OB_ADD("PRINT",operprint)         //1
tableoperator[1]=OB_ADD("INPUT",operinput)         //2
tableoperator[2]=OB_ADD("IF",operif)               //3
tableoperator[3]=OB_ADD("FOR",operfor)             //4
tableoperator[4]=OB_ADD("NEXT",opernext)           //5
tableoperator[5]=OB_ADD("GOTO",opergoto)           //6
tableoperator[6]=OB_ADD("GOSUB",opergosub)         //7
tableoperator[7]=OB_ADD("RETURN",operreturn)       //8
tableoperator[8]=OB_ADD("LET",operlet)             //9
tableoperator[9]=OB_ADD("DIM",operdim)             //10
tableoperator[10]=OB_ADD("STOP",operstop)           //11
tableoperator[11]=OB_ADD("END",operend)             //12
tableoperator[12]=OB_ADD("REM",operrem)             //13
tableoperator[13]=OB_ADD("OPEN",operopen)           //14
tableoperator[14]=OB_ADD("CLOSE",operclose)         //15
tableoperator[15]=OB_ADD("KILL",operkill)           //16
tableoperator[16]=OB_ADD("READ",operread)           //17
tableoperator[17]=OB_ADD("DATA",operdata)           //18
tableoperator[18]=OB_ADD("RESTORE",operrestore)     //19
tableoperator[19]=OB_ADD("RANDOMIZE",operrandomize) //20
tableoperator[20]=OB_ADD("ELSE",operelse)           //21
tableoperator[21]=OB_ADD("ENDIF",operendif)         //22
tableoperator[22]=ob_operator();                    //23

OB_SETFUN("RND",ob_fun_rnd)                           //1
OB_SETFUN("ABS",ob_fun_abs)                           //2
OB_SETFUN("SIN",ob_fun_sin)                           //3
OB_SETFUN("COS",ob_fun_cos)                           //4
OB_SETFUN("ATN",ob_fun_atn)                           //5
OB_SETFUN("SQR",ob_fun_sqr)                           //6
OB_SETFUN("EXP",ob_fun_exp)                           //7
OB_SETFUN("LOG",ob_fun_log)                           //8
OB_SETFUN("LOG10",ob_fun_log10)                       //9
OB_SETFUN("STR2FLOAT",str2flo)                        //10
OB_SETFUN("STR2INT%",str2int)                         //11
OB_SETFUN("INT%",ob_fun_int)                          //12
OB_SETFUN("SGN%",ob_fun_sgn)                          //13
OB_SETFUN("LEN%",ob_fun_len)                          //14
OB_SETFUN1("DAT$",ob_fun_dat,OB_DELETE)               //15
OB_SETFUN1("CLK$",ob_fun_clk,OB_DELETE)               //16
OB_SETFUN1("D2STR$",ob_fun_digit2str,OB_DELETE)       //17
OB_SETFUN1("D2HEXSTR$",ob_fun_digit2hexstr,OB_DELETE) //18

out.set_stream_with_startposition(o);
setstep(NOSTEP);
}

void ob_obasic::label_insert(ob_type_streampos mbl){

ob_label* current=ob_findcommon<ob_label>(&(baselex.getstring()),&tablelabel);

if(current==OB_NULL){
ob_istream* i;
i=baselex.get_input_stream();
void* ptrvoid=new ob_label(baselex.getstring(),mbl,i);
tablelabel.put(ptrvoid,baselex.getstring().gethash());
}//if
else{
OB_ERR(ob_err10);
}//alse
}

void ob_obasic::load1(ob_istreamcommon* i){

baselex.get_input_stream()->set_stream_with_startposition(i);

if(i==OB_NULL) return;

//find label and store it in tablelabel
ob_type_streampos one(baselex.getposition());

for(;;){

ob_type_streampos mbl;

for(;;){
mbl=baselex.getposition();
baselex.get_lex();

if(baselex.gettype()==ob_lex::BAD) OB_ERR(ob_err94);

if((baselex.gettype()==ob_lex::OPERATOR)&&(baselex.getstring()==ob_REMkwrd)){
baselex.getcurrentoperator()->run(&baselex);
//REM operator must leave currentlex ENDPROGRAMM or EOL
}//if

if(baselex.gettype()==ob_lex::ENDPROGRAMM) goto m1;
if(baselex.gettype()==ob_lex::EOL) break;
}//for

baselex.get_lex();

  if((baselex.gettype()==ob_lex::DECINTNUMBER)||(baselex.gettype()==ob_lex::LABEL)){
  label_insert(mbl);
  }//if
  else{
  baselex.putbacklex();
  }//else

}//for

m1:

baselex.setposition(one);

ob_ostrstream datastream1;

for(;;){
baselex.get_lex();

if((baselex.gettype()==ob_lex::OPERATOR)&&(baselex.getstring()==ob_REMkwrd)){
baselex.getcurrentoperator()->run(&baselex);
continue;
}//if

if(baselex.gettype()==ob_lex::ENDPROGRAMM) break;

if((baselex.getstring()==ob_DATAkwrd)&&(baselex.gettype()==ob_lex::OPERATOR))
for(;;){
baselex.get_lex();
if(baselex.gettype()==ob_lex::ENDPROGRAMM) goto m2;
if(baselex.gettype()==ob_lex::EOL) break;
datastream1<<baselex.getstring().c_str();
}//for

}//for

m2:

datastream1<<ends;

if(datastream==OB_NULL) datastream= new ob_shortstrstream;

ob_standartstring st=datastream1.str();

ob_wstream_to_stream(datastream,&st);

datalex.get_input_stream()->set_stream_with_startposition((ob_istreamcommon*)datastream);
datalex.restore();

baselex.setposition(one);

retvalueptr=baselex.getposition();
te=DUMMYEND;
endoperatordetect=false;

}//load

ob_file_base* ob_obasic::findfile(ob_type_filenum number){

ob_filefind filefind(number);

void** voidptr=tablefile.findobj(filefind);
ob_file_base* currentptr;
if(voidptr!=OB_NULL) currentptr=(ob_file_base*)(*voidptr); else currentptr=OB_NULL;
return currentptr;
}

#ifdef OB_WCHAR
}//end namespace ob_wcharspace
#else
}//end namespace ob_charspace
#endif

