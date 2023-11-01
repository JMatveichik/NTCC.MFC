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

//here is ob_lex::function (operators)

void ob_lex::operprint(){

ob_ostream* o=&(ptrbasic->out);

get_lex();
if(currentlex==ob_lex::SHARP){

get_lex();

ob_type_filenum number=gettypevalue<ob_int>();

ob_file_base* tmpfile;
if((tmpfile=ptrbasic->findfile(number))==OB_NULL) OB_ERR(ob_err38);
tmpfile->get_stream(&o);
}//if print to file

typelex prevlex;

for(;;){

prevlex=currentlex;

m2:

switch(currentlex){

case ob_lex::COMMA:
o->put(ob_const_tab);

case ob_lex::SEMICOLON:
get_lex();
goto m2;

case ob_lex::OPERATOR:
case ob_lex::EOL:
case ob_lex::ENDPROGRAMM:
goto m1;
default:break;
}//switch

ob_savewrapper tmp;
expr(&tmp);
tmp.getptr()->tostream(o);

}//for
m1:

putbacklex();

if((prevlex!=ob_lex::SEMICOLON)&&(prevlex!=ob_lex::COMMA)) o->putendl();
}

void ob_lex::dopinput(ob_lex* lex){

ob_savewrapper* tmp;
ob_savewrapper  tmp1;

for(;;){

switch(currentlex){
case ob_lex::OLDARRAY:
case ob_lex::OLDVARIABLE:
case ob_lex::NEWNAME:

tmp=extractvar()->get();

  switch(lex->currentlex){
  case ob_lex::ENDPROGRAMM:
  goto m1;

  case ob_lex::EOL:
  case ob_lex::COMMA:
  lex->get_lex();
  break;

  default:break;
  }//switch lex

tmp1.clear();
lex->expr(&tmp1);

tmp->asg(&tmp1);

get_lex();
break;

case ob_lex::COMMA:
get_lex();
break;

default:
goto m1;
}//switch baselex

}//for
m1:;
}

void ob_lex::operinput(){

ob_lex* r=&ptrbasic->readlex;

get_lex();

if(currentlex==ob_lex::SHARP){

get_lex();

ob_type_filenum number=gettypevalue<ob_int>();

ob_file_base* tmpfile;
if((tmpfile=ptrbasic->findfile(number))==OB_NULL) OB_ERR(ob_err38);

ptrbasic->read_from_file_lex.setbasicptr(ptrbasic);

ob_istream* ptr;
tmpfile->get_stream(&ptr);

ptrbasic->read_from_file_lex.get_input_stream()->assign(ptr);
r=&ptrbasic->read_from_file_lex;
}//if read from file

r->get_lex();
dopinput(r);
}

void
ob_lex::findkeywordforif(ob_type_namestatic* kwrd1,ob_type_namestatic* kwrd2){

ob_type_streampos one(getposition());

ob_type_countnestedoperator nestedcount=ob_type_countnestedoperator();

for(;;){
get_lex();

if((currentlex==ob_lex::OPERATOR)&&(getstring()==ob_REMkwrd)){
getcurrentoperator()->run(this);
continue;
}//if

if(currentlex==ob_lex::ENDPROGRAMM){
setposition(one);
OB_ERR(ob_err70)
}//if

if(kwrd1!=OB_NULL){
  if((getstring()==(*kwrd1))&&(nestedcount==0)){
  get_lex();
  break;
  }//if
}//if

if(kwrd2!=OB_NULL){
  if((getstring()==(*kwrd2))&&(nestedcount==0)){
  get_lex();
  break;
  }//if
}//if

if(getstring()==ob_THENkwrd){
get_lex();
if(currentlex==ob_lex::EOL) ++nestedcount;
}//if

if(getstring()==ob_ENDIFkwrd){
  if(nestedcount!=ob_type_countnestedoperator()){
  --nestedcount;
  }//if
  else{
  setposition(one);
  OB_ERR(ob_err70)
  }//else
}//if

}//for

}

void ob_lex::operelse(){
findkeywordforif(&ob_ENDIFkwrd,OB_NULL);
}

void ob_lex::operendif(){
get_lex();
}

void ob_lex::operif(){

ob_var::ob_type_if currentlexif;

ob_savewrapper left;

get_lex();
expr(&left);

{//start blocks
switch(currentlex){

case ob_lex::LESS:
currentlexif=ob_var::LESSIF;
get_lex();

if(currentlex==ob_lex::ASSIGN){
currentlexif=ob_var::LESSEQVIF;
get_lex();
}//if
else{
  if(currentlex==ob_lex::GREAT){
  currentlexif=ob_var::NOTEQVIF;
  get_lex();
  }//if
}//else
break;

case ob_lex::GREAT:
currentlexif=ob_var::GREATIF;
get_lex();

if(currentlex==ob_lex::ASSIGN){
currentlexif=ob_var::GREATEQVIF;
get_lex();
}//if
break;

case ob_lex::ASSIGN:
currentlexif=ob_var::EQVIF;
get_lex();
break;

default:OB_ERR(ob_err13);break;
}//switch
}//end blocks

ob_savewrapper right;
expr(&right);

if(getstring()!=ob_THENkwrd) OB_ERR(ob_err14);

get_lex();
bool flform1;
if(currentlex!=ob_lex::EOL) flform1=true; else flform1=false;

bool flcondition=left.getptr()->compare(right.getptr(),currentlexif);

if(flform1==true){
  if(flcondition==false){
  findeol();
  }//if
  else{}//else
}//if
else{
  if(flcondition==false){

  findkeywordforif(&ob_ELSEkwrd,&ob_ENDIFkwrd);

  }//if

  else{}//else
}//else

}

void ob_lex::operfor(){

ob_varbase* tmp;

ob_savewrapper zero(new ob_int(ob_type_int()));

get_lex();

switch(currentlex){
case ob_lex::OLDARRAY:
case ob_lex::OLDVARIABLE:
case ob_lex::NEWNAME:
tmp=extractvar();
workvariable(tmp->get());
break;
default:OB_ERR(ob_err18)
}//switch

if(getstring()!=ob_TOkwrd) OB_ERR(ob_err19);

ob_type_name nameforfor=tmp->bigname();

get_lex();

ob_savewrapper last,step;

expr(&last);

//without STEP keyword STEP=1
if(getstring()!=ob_STEPkwrd){
step.setptr(new ob_int(1));
}//if
else{
get_lex();
expr(&step);
}//else

//check if ob_forfor object already exist in tableforfor

ob_forfor* tmpptr1=
new ob_forfor(nameforfor,previousptr,&last,&step);//previousptr for support step mode debugger

replaceobjcommon2<ob_type_name,ob_forfor>
(&(ptrbasic->tableforfor),
tmpptr1,
&nameforfor
);

//check first pass, STEP is present in check
if(tmpptr1->check(tmp->get())==false){

ob_type_streampos one(getposition());

//delete comment if NEXT may be located before FOR but performance decrease
//restore();

  for(;;){

  get_lex();//find NEXT keyword for this FOR

  if(currentlex==ob_lex::ENDPROGRAMM){
  setposition(one);
  OB_ERR(ob_err66)
  }

  if(getstring()==ob_NEXTkwrd){

  ob_varbase* tmp1;
  get_lex();

  switch(currentlex){
  case ob_lex::OLDARRAY:
  case ob_lex::OLDVARIABLE:
  tmp1=extractvar();
  break;
  case ob_lex::NEWNAME:continue;
  default:OB_ERR(ob_err69)
  }//switch

  ob_type_name nameforfor1=tmp1->bigname();
  
    if(nameforfor==nameforfor1){
    get_lex();
    goto m1;
    }//if
    
  }//if

  }//for
}//if
m1:;
}

void ob_lex::opernext(){

ob_varbase* tmp;
get_lex();

switch(currentlex){
case ob_lex::OLDARRAY:
case ob_lex::OLDVARIABLE:
tmp=extractvar();
break;
default:OB_ERR(ob_err69)
}//switch

ob_type_name nameforfor=tmp->bigname();

ob_hashfind<ob_type_name,ob_forfor> findfor(nameforfor);

void** voidptr;

voidptr=ptrbasic->tableforfor.findobj(findfor);
if(voidptr==OB_NULL) OB_ERR(ob_err71)

ob_forfor* forptr=(ob_forfor*)(*voidptr);

forptr->add(tmp->get());
if(forptr->check(tmp->get())==true){
setposition(forptr);
}//if

get_lex();
}

void ob_lex::opergo(typego typego){

get_lex();

ob_label* current=
ob_findcommon<ob_label>(&getstring(),&ptrbasic->tablelabel);
if(current==OB_NULL) OB_ERR(ob_err12);

if(typego==GOSUB){

ob_type_countlist mng=ptrbasic->get_max_nested_gosub();
if((mng!=0)&&(mng==ptrbasic->stackgosub.getcount())) OB_ERR(ob_err103);

ob_data_for_jump data_for_return_operator;

data_for_return_operator.set(&in);//set stream

ptrbasic->stackgosub.put(data_for_return_operator);
}//if

setposition(current);

get_lex();
}

void ob_lex::opergoto(){opergo(GOTO);}
void ob_lex::opergosub(){opergo(GOSUB);}

void ob_lex::operreturn(){

if(ptrbasic->stackgosub.getcount()==ob_type_countlist()) OB_ERR(ob_err17);

ob_data_for_jump data_for_return_operator;

ptrbasic->stackgosub.get(&data_for_return_operator);

setposition(&data_for_return_operator);

get_lex();
}

void ob_lex::operlet(){get_lex();}

void ob_lex::operdim(){

for(;;){

get_lex();

if(!((currentlex==ob_lex::OLDARRAY)||(currentlex==ob_lex::NEWNAME)))
OB_ERR(ob_err24);

ob_type_name namearray;

namearray=getstring();

ob_type_namestatic namearraystatic(namearray.c_str(),namearray.getcount());

ob_type_dim ptrdimcurrent;
getsizearray(&ptrdimcurrent);

ptrbasic->createarray(&namearraystatic,&ptrdimcurrent);

get_lex();
if(currentlex!=ob_lex::COMMA) break;
}//for
}

void ob_lex::getsizearray(ob_type_dim* ptrdimcurrent){

ob_type_arraysize tmparraydimention[ob_maxarraydimention];

ob_type_arraydimension i;
ob_type_arraydimension kr;
get_lex();

if(currentlex!=ob_lex::LP) OB_ERR(ob_err72);

get_lex();

if(currentlex==ob_lex::RP) OB_ERR(ob_err31);

for(kr=0;;++kr){

if(kr==ob_maxarraydimention) OB_ERR(ob_err26);

tmparraydimention[kr]=gettypevalue<ob_int>();

if(tmparraydimention[kr]<=0) OB_ERR(ob_err29);

if(currentlex==ob_lex::RP){
++kr;
break;
}//if

if(currentlex!=ob_lex::COMMA) OB_ERR(ob_err27);
get_lex();
}//for

ptrdimcurrent->recount(kr);

for(i=0;i!=kr;++i){
(*ptrdimcurrent)[i]=tmparraydimention[i];
}//for

}

void ob_lex::operstop(){
ptrbasic->clrtabl(ob_obasic::NOOVERLAY);
get_lex();
}

void ob_lex::operend(){
ptrbasic->setendoperatordetect(true);
operstop();
}

void ob_lex::findeol(){
for(;;){
get_lex();
if((currentlex==ob_lex::ENDPROGRAMM)||(currentlex==ob_lex::EOL)) break;
}//for
}

void ob_lex::operrem(){
findeol();
}

void ob_lex::operopen(){

ios::openmode mode;

get_lex();

ob_varbase* tmp1;

ob_savewrapper tmp;

switch(currentlex){
case ob_lex::OLDARRAY:
case ob_lex::OLDVARIABLE:
tmp1=extractvarwithoutcreate();
tmp.setptr(tmp1->get()->getptr()->duplicate());
get_lex();
break;
case ob_lex::STRING:

if(getstring().getcount()<=3) OB_ERR(ob_err80)

expr(&tmp);//expr eliminate quote from string - file name
break;
default:OB_ERR(ob_err84)
}//switch

ob_type_char* ret;

tmp.getptr()->cast(&ret);

ob_type_string filename(ret);

if(getstring()!=ob_FORkwrd) OB_ERR(ob_err33);

get_lex();

if(getstring()==ob_INPUTkwrd) mode=ios::in|ios::binary;
else
if(getstring()==ob_OUTPUTkwrd) mode=ios::out;
else
OB_ERR(ob_err34);

get_lex();
if(getstring()!=ob_ASkwrd) OB_ERR(ob_err35);

get_lex();

ob_modestream mstr;

for(;;){

     if(getstring()==ob_UNICODEkwrd)         mstr.set_modestream(ob_modestream::FLAG_NORMAL);
else if(getstring()==ob_UNICODE_REVERSEkwrd) mstr.set_modestream(ob_modestream::FLAG_REVERSE);
else if(getstring()==ob_UTF8kwrd)            mstr.set_modestream(ob_modestream::FLAG_UTF8);
else if(getstring()==ob_CHARkwrd)            mstr.set_modestream(ob_modestream::FLAG_SHORT);
else break;

get_lex();

}//for

if(getstring()!=ob_FILEkwrd) OB_ERR(ob_err35);

get_lex();
if(getstring()!=ob_SHARPkwrd) OB_ERR(ob_err35);

get_lex();

ob_type_filenum number=gettypevalue<ob_int>();

ob_filefind filefind(number);

if(mode==ios::out){
replaceobjcommon1<ob_file_base,ob_filefind>(
&(ptrbasic->tablefile),(ob_file_base*)new ob_output_file(filename.c_str(),mode,number,mstr),
filefind
);
}//if
else{
replaceobjcommon1<ob_file_base,ob_filefind>(
&(ptrbasic->tablefile),(ob_file_base*)new ob_input_file(filename.c_str(),mode,number,mstr),
filefind
);
}//else

}

void ob_lex::operclose(){

get_lex();

if(currentlex==ob_lex::EOL){
ob_cleartable<ob_file_base>(&(ptrbasic->tablefile));
get_lex();
}//if
else{
  for(;;){
  if(currentlex==ob_lex::SHARP)
  get_lex();

  ob_type_filenum number=gettypevalue<ob_int>();

  ob_filefind filefind(number);

  void* tmpptr=OB_NULL;
  bool fl=ptrbasic->tablefile.get(filefind,&(tmpptr));
  if(fl==true) delete ((ob_file_base*)tmpptr);

  if(currentlex!=ob_lex::COMMA) break;
  get_lex();
  }//for
}//else
}

void ob_lex::operkill(){

for(;;){
get_lex();                                    

ob_savewrapper tmp;
expr(&tmp);//expr eliminate quote from string - file name

ob_type_char* ret;

tmp.getptr()->cast(&ret);

#ifndef OB_WCHAR

remove(ret);

#else

ob_type_string_char_small sfn;
wchar2char(&sfn,ret);
remove(sfn.c_str());

#endif

if(currentlex!=ob_lex::COMMA) break;
}//for
}

void ob_lex::operread(){
get_lex();
dopinput(&(ptrbasic->datalex));
}

void ob_lex::operdata(){
findeol();
}

void ob_lex::operrestore(){
ptrbasic->datalex.restore();
get_lex();
}

void ob_lex::operrandomize(){
srand((ob_type_srandargument)time(OB_NULL));
get_lex();
}

#ifdef OB_WCHAR
}//end namespace ob_wcharspace
#else
}//end namespace ob_charspace
#endif

