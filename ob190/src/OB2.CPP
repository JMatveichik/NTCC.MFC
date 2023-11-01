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

//here is ob_lex::function

void ob_lex::expr(ob_savewrapper* left){

term(left);

for(;;){

ob_savewrapper right;

switch(currentlex){
  case PLUS:

  get_lex();
  term(&right);

  left->add(&right);

  break;
  case MINUS:

  get_lex();
  term(&right);

  left->sub(&right);

  break;
  default:
  return;
}//switch
}//for

}

void ob_lex::term(ob_savewrapper* left){

power(left);

for(;;){

ob_savewrapper right;

switch(currentlex){
  case MUL:

  get_lex();
  power(&right);

  left->mul(&right);

  break;
  case DIV:

  get_lex();
  power(&right);

  if(right.getptr()->eq()==true){OB_ERR(ob_err1);}

  left->div(&right);

  break;
  default:
  return;
}//switch
}//for
}

void ob_lex::power(ob_savewrapper* left){

prim(left);

for(;;){

ob_savewrapper right;

switch(currentlex){
  case POWER:

  get_lex();
  prim(&right);

  left->pww(&right);

  break;
  default:
  return;
}//switch
}//for

}

void ob_lex::prim(ob_savewrapper* left){

ob_par par;
ob_type_char quotes;

ob_type_stringsize len=currentlexasnamestatic.getcount()-1;
ob_type_char*       cl=currentlexasnamestatic.c_str();

if(left->getptr()!=OB_NULL) OB_ERR(ob_err64);//for debug

ob_api*            nestedcurrentfunction;
switch(currentlex){

case FUNC:
nestedcurrentfunction=currentfunction;
par.work(this);
nestedcurrentfunction->run(ptrbasic,&par,left);
get_lex();
break;

case FLOATNUMBER:
left->setptr(new ob_float(cl));
get_lex();
break;

case DECINTNUMBER:
left->setptr(new ob_int(cl));
get_lex();
break;

case HEXINTNUMBER:
left->setptr(new ob_int(ob_hexconvert(cl)));
get_lex();
break;

case STRING:
quotes=cl[ob_type_stringsize()];
cl[len-1]=ob_const_endstring;//eliminate quotes
left->setptr(new ob_str(cl+1,len-1));
cl[len-1]=quotes;
get_lex();
break;

case OLDARRAY:
case OLDVARIABLE:
left->setptr(extractvar()->get()->getptr()->duplicate());
get_lex();
break;

case NEWNAME:
OB_ERR(ob_err7);
break;

case MINUS:
get_lex();
power(left);
left->getptr()->uniminus();
break;

case PLUS:
get_lex();
expr(left);
break;//uniplus

case LP:
get_lex();
expr(left);
if(currentlex!=RP) OB_ERR(ob_err4);
get_lex();
break;

case DUMMYCHAR1:
case DUMMYCHAR2:
case LABEL:
get_lex();
break;

default:
OB_ERR(ob_err5);
break;
}//switch
}

#ifdef OB_WCHAR
}//end namespace ob_wcharspace
#else
}//end namespace ob_charspace
#endif

