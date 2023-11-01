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

ob_lex::ob_lex(ob_istreamcommon* i,ob_obasic* p):currentlexasstring(ob_maxlengthstring){
in.set_stream_with_startposition(i);
currentlex=ENDPROGRAMM;
setbasicptr(p);
}

ob_varbase* ob_lex::extractvarwithoutcreate(){

ob_type_dim ptrdimcurrent;
ob_varbase* varptr;

ob_vararray*       nestedcurrentarray;

switch(currentlex){

case ob_lex::OLDARRAY:
nestedcurrentarray=currentarray;
getsizearray(&ptrdimcurrent);
nestedcurrentarray->setsize(&ptrdimcurrent);
varptr=nestedcurrentarray;
break;

case ob_lex::OLDVARIABLE:
varptr=currentvar;
break;

default:
varptr=OB_NULL;
break;
}//switch
return varptr;
}

ob_varbase* ob_lex::extractvar(){

ob_varbase* varptr;
ob_varone* varoneptr;

if(currentlex==ob_lex::NEWNAME){
varoneptr=new ob_varone(currentlexasnamestatic);
varptr=varoneptr;
ptrbasic->tablevar.put(varoneptr,currentlexasnamestatic.gethash());
}//if
else{
varptr=extractvarwithoutcreate();
}//else

return varptr;
}

void ob_lex::workvariable(ob_savewrapper* left){

ob_savewrapper right;

get_lex();
if(currentlex!=ob_lex::ASSIGN) OB_ERR(ob_err8)
get_lex();

expr(&right);

left->asg(&right);
}

void ob_lex::partostream(ob_ostrstream* name,ob_type_streampos one){

ob_type_streampos tp=in.getposition();
ob_type_streampos tp1=tp-(ob_type_streampos)sizeof(ob_type_char);

ob_type_char ch;
setposition(one);

for(;;){
if(in.getposition()>=(tp1)) break;
in.get(ch);
*name<<ch;
}//for

in.setposition(tp);
*name<<ends;
}

#ifdef OB_WCHAR
}//end namespace ob_wcharspace
#else
}//end namespace ob_charspace
#endif

