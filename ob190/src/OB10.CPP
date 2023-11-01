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

//here is not-member function, data, ob_varone::, ob_vararray::, ob_par:: ob_operator::
ob_type_int ob_hexconvert(ob_type_char* a){

ob_type_stringsize l=ob_strlen(a);

if((l<4)||(l>=ob_maxlengthstring)) OB_ERR(ob_err55) //check 0x or 0X in prim

ob_type_int ret=ob_type_int();
ob_type_int r=0;
ob_type_stringsize i;

for(i=0;i!=l-3;++i){//attention - start index is 2

switch(a[l-2-i]){
case ob_const_0:r=0;break;
case ob_const_1:r=1;break;
case ob_const_2:r=2;break;
case ob_const_3:r=3;break;
case ob_const_4:r=4;break;
case ob_const_5:r=5;break;
case ob_const_6:r=6;break;
case ob_const_7:r=7;break;
case ob_const_8:r=8;break;
case ob_const_9:r=9;break;
case ob_const_a:case ob_const_A:r=10;break;
case ob_const_b:case ob_const_B:r=11;break;
case ob_const_c:case ob_const_C:r=12;break;
case ob_const_d:case ob_const_D:r=13;break;
case ob_const_e:case ob_const_E:r=14;break;
case ob_const_f:case ob_const_F:r=15;break;
default:OB_ERR(ob_err55)break;
}//switch
r<<=i*4;
ret+=r;
}//for

return ret;
}

void ob_errorgeneration(ob_type_codeerror e){throw ob_err(e);}

const ob_type_char*
ob_getstringparam(
const ob_type_parnum *descrc,
const ob_type_char* parc,
const ob_type_parnum i
){
ob_type_parnum k;

ob_type_char* ptr;

ptr=(ob_type_char*)parc;

for(k=0;k!=descrc[0];++k){

if(i==k) break;

  ob_type_stringsize j;
  for(j=0;;++j){
  if(ptr[j]==ob_const_endstring) break;
  }//for

  ptr+=j+1;

}//for

return ptr;
}

ob_type_stringsize ob_strlen(ob_type_char* str){
return mstore_strlen(str,ob_const_endstring,ob_type_stringsize());
}

ob_type_stringsize ob_strlen_small(ob_type_small_char* str){
return 
mstore_strlen<ob_type_small_char,ob_type_stringsize>(str,ob_const_endstring,ob_type_stringsize());
}

void ob_strcpy(ob_type_char* datadst,const ob_type_char* datasrc){
ob_type_stringsize i;
for(i=0;;++i){
datadst[i]=datasrc[i];
if(datasrc[i]==ob_const_endstring) break;
}//for
}

ob_type_hashtablelength ob_hash(ob_type_char* data,ob_type_stringsize l){
ob_type_hashtablelength hashvalue=ob_starthashvalue;
  if(data!=OB_NULL){
  hashvalue+=(ob_type_hashtablelength)l;
  hashvalue+=(ob_type_hashtablelength)(data[0]);
  if(l>=2) hashvalue+=(ob_type_hashtablelength)(data[l-2]);
  if(l>=3) hashvalue+=(ob_type_hashtablelength)(data[l-3]);
  }//if
return hashvalue;
}

void ob_allocspaceforvar(
ob_type_namestatic* str,ob_savewrapper* value,ob_type_arraysize size){
ob_type_arraysize i;
ob_type_char ch;
ob_type_stringsize l=str->getcount();

if(l>=2){

ch=str->getdata()[l-2];

if(ch==ob_const_intlabel)
for(i=0;i!=size;++i) value[i].setptr(new ob_int);
else
if(ch==ob_const_strlabel)
for(i=0;i!=size;++i) value[i].setptr(new ob_str);
else
for(i=0;i!=size;++i) value[i].setptr(new ob_float);
}//if
}

ob_varone::ob_varone(ob_type_namestatic& name):ob_varbase(name){
ob_allocspaceforvar(&name,&value,1);
}

ob_type_name ob_varone::bigname(){return *this;}

ob_vararray::
ob_vararray(ob_type_namestatic& name,ob_type_dim* ptrdim):ob_varbase(name){

dimension.transfer(*ptrdim);

ob_type_arraydimension kr=dimension.getcount();
dimension1.recount(kr);

ob_type_arraydimension i;
sizecreat=1;
for(i=0;i!=kr;++i){
if(dimension[i]<=0) OB_ERR(ob_err79)
sizecreat*=dimension[i];
}//for

dimension1[0]=1;
for(i=1;i!=kr;++i)
dimension1[i]=dimension1[i-1]*dimension[i-1];

value.recount(sizecreat);

ob_allocspaceforvar(&name,value.c_str(),sizecreat);
sizecurrent=0;
}

void ob_vararray::setsize(ob_type_dim* ptrdimcurrent){

ob_type_arraydimension kr=dimension.getcount();

if(ptrdimcurrent->getcount()!=kr) OB_ERR(ob_err58);

ob_type_arraydimension i;
sizecurrent=ob_type_arraysize();

for(i=0;i!=kr;++i){
ob_type_arraysize currentindex=ptrdimcurrent->c_str()[i];
if(currentindex==0) OB_ERR(ob_err77)
--currentindex;
if(currentindex>=dimension[i]) OB_ERR(ob_err59);
sizecurrent+=(currentindex)*dimension1[i];
}//for

if(sizecurrent>=sizecreat)
OB_ERR(ob_err32);//spare check
}

ob_type_name ob_vararray::bigname(){

ob_ostrstream os;

os<<this->c_str()<<sizecurrent;

ob_type_name ret((ob_type_char*)os.str().c_str());

return ret;
}

void ob_par::initpar(){
ob_type_parnum i;
for(i=0;i!=OB_NUMTYPES;++i) mo[i]=OB_NULL;
name=pc=OB_NULL;
pi=OB_NULL;
pf=OB_NULL;
}

void ob_par::deletepar(){
ob_type_parnum i;
for(i=0;i!=OB_NUMTYPES;++i) if(mo[i]!=OB_NULL) delete[] (mo[i]);
if(pf!=OB_NULL) delete[] pf;
if(pi!=OB_NULL) delete[] pi;
if(name!=OB_NULL) {delete name;}
if(pc!=OB_NULL)   {delete pc;}
initpar();
}

void ob_par::work(ob_lex* curr){
ob_type_parnum i;

name=new ob_ostrstream;
pc=new ob_ostrstream;
pf=new ob_type_flo[ob_maxnumpar];
pi=new ob_type_int[ob_maxnumpar];

for(i=0;i!=OB_NUMTYPES;++i){
mo[i]=new ob_type_parnum[ob_maxnumpar+1];
mo[i][0]=0;
}

curr->get_lex();//LP read

ob_type_streampos one(curr->getposition());

if(curr->gettype()==ob_lex::LP) curr->get_lex();

for(i=0;i!=ob_maxnumpar;++i){

  switch(curr->gettype()){

  case ob_lex::COMMA:
  one=curr->getposition();
  curr->get_lex();
  break;

  case ob_lex::EOL:
  curr->putbacklex();//for debugger
  case ob_lex::RP:
  goto m1;

  default:break;
  }//switch

ob_savewrapper a;
curr->expr(&a);
a.getptr()->topar(this,i);

curr->partostream(name,one);
}//for

OB_ERR(ob_err41);

m1:

*name<<ends;//if is not parameters
*pc<<ends;  //if is not string parameters
}

void ob_operator::run(ob_lex* lex){(lex->*ptroper)();}//indeed

ob_savewrapper*
ob_functoraccessarray::operator()(ob_obasic* ptr,ob_type_char* name){

ob_vararray* current=ob_findcommon<ob_vararray>(name,&(ptr->tablearray));
ob_type_arraydimension kr;

  if(current!=OB_NULL){
  current->getsize(&kr);
  ob_type_dim ptrdimcurrent(as,kr);
  current->setsize(&ptrdimcurrent);
  ob_savewrapper* sw=current->get();
  return sw;
  }//if
  else{
  return OB_NULL;
  }//else
}

ob_operator::ob_operator():
ptroper(OB_NULL),
before(NOBREAK),
after(NOBREAK),
ptrbreak(OB_NULL)
{}//for trailer of operator table

ob_operator::ob_operator(ob_type_char* name,ob_oper p,flbreak b,flbreak a)
:ob_type_namestatic(name),
ptroper(p),
before(b),
after(a),
ptrbreak(OB_NULL)
{}

enum ob_enum_for_loadbreakstr{
char_detected,
bad_detected,
eof_detected,
endl_detected,
ptr0_detected
};

ob_enum_for_loadbreakstr ob_check_endl_delim(ob_type_char ch){

ob_enum_for_loadbreakstr ret;

switch(ch){

#ifdef OB_WCHAR
case ob_const_nel:
case ob_const_ls:
case ob_const_ps:
#endif

case ob_const_cr: 
case ob_const_lf: ret=endl_detected;break;

default:          ret=char_detected;break;
}//switch

return ret;
}

ob_enum_for_loadbreakstr ob_get_backward(ob_istream* in,ob_type_char* ch){

ob_enum_for_loadbreakstr ret=char_detected;

ob_type_streampos prev=in->getposition();

if(prev==ob_type_streampos()){
in->get(*ch);
in->setposition(prev);
ret=ptr0_detected;
}//if
else{
ob_type_streampos prev1=prev;
prev1-=sizeof(ob_type_char);
in->setposition(prev1);
in->get(*ch);
in->setposition(prev1);
}//else

if(in->good()==false) ret=bad_detected;

return ret;
}

ob_enum_for_loadbreakstr ob_get_forward(ob_istream* in,ob_type_char* ch){

ob_enum_for_loadbreakstr ret=char_detected;

in->get(*ch);

if(in->good()==false){
ret=bad_detected;
}//if
else{
  if(in->eof()==true){
  in->setposition(0,ios::end);
  ret=eof_detected;
  }//if
}//else

return ret;
}

ob_enum_for_loadbreakstr ob_find_backward(ob_istream* in){
ob_enum_for_loadbreakstr ret;
ob_type_char ch;

for(;;){
ret=ob_get_backward(in,&ch);

if(ret==bad_detected)  break;
if(ret==ptr0_detected) break;

ret=ob_check_endl_delim(ch);

if(ret==endl_detected){
ob_get_forward(in,&ch);
break;
}//if

}//for

return ret;
}

ob_enum_for_loadbreakstr ob_load_string_forward(
ob_istream* in,
ob_type_stringsize lengthbuffers,
ob_type_char* ptrstring
){

ob_enum_for_loadbreakstr ret=char_detected;

if((lengthbuffers!=0)&&(ptrstring!=OB_NULL)){

ptrstring[0]=OB_NULL;

ob_type_char ch;

ob_type_stringsize i;

for(i=0;i!=lengthbuffers-1;){

ret=ob_get_forward(in,&ch);
if(ret==bad_detected) break;
if(ret==eof_detected) break;

#ifdef OB_WCHAR
if((ch==ob_const_dummychar1)||(ch==ob_const_dummychar2)) continue;
#endif

ptrstring[i]=ch;

ret=ob_check_endl_delim(ch);
if(ret==endl_detected) break;
++i;
}//for

ptrstring[i]=OB_NULL;

}//if
return ret;
}

bool ob_loadbreakstr(
ob_istreamcommon* in1,
ob_type_stringsize lengthbuffers,
ob_type_char* ptrbefore,
ob_type_char* ptrcurrent,
ob_type_char* ptrafter,
ob_modestream ms
){

bool fl=false;

if((in1!=OB_NULL)&&(lengthbuffers!=ob_type_stringsize())){

if(ptrbefore !=OB_NULL) ptrbefore[0] =ob_const_endstring;
if(ptrcurrent!=OB_NULL) ptrcurrent[0]=ob_const_endstring;
if(ptrafter  !=OB_NULL) ptrafter[0]  =ob_const_endstring;

ob_istream in;
*((ob_modestream*)(&in))=ms;
in.set_stream_with_startposition(in1);

ob_enum_for_loadbreakstr ret;
ob_type_char ch;

ob_type_streampos p0=in.getposition();
ob_type_streampos p2;
ob_type_streampos p3;
ob_type_streampos p4;
ob_type_streampos pp=-1;

if(p0==pp) in.setposition(0,ios::end);
if(p0!=pp){

//start check CR+LF middle

ret=ob_get_forward(&in,&ch);
if(ret==bad_detected) return fl;

if(ch==ob_const_lf){

ret=ob_get_backward(&in,&ch);
if(ret==bad_detected) return fl;

ret=ob_get_backward(&in,&ch);
if(ret==bad_detected) return fl;
if(ch!=ob_const_cr) in.setposition(p0);
}//if
else{
in.setposition(p0);
}//else

//stop check CR+LF middle

ret=ob_find_backward(&in);
if(ret==bad_detected)  return fl;

if(ret==ptr0_detected){
ret=ob_load_string_forward(&in,lengthbuffers,ptrcurrent);
if(ret==eof_detected) goto m1;
}//if
else{
p2=in.getposition();
ret=ob_load_string_forward(&in,lengthbuffers,ptrcurrent);
if(ret==eof_detected) goto m1;

p3=in.getposition();
in.setposition(p2);

//start skip endl backward

ret=ob_get_backward(&in,&ch);
if(ret==bad_detected) return fl;

if(ch==ob_const_lf){
p4=in.getposition();
ret=ob_get_backward(&in,&ch);
if(ret==bad_detected) return fl;
if(ch!=ob_const_cr) in.setposition(p4);
}//if

//stop  skip endl backward

ret=ob_find_backward(&in);
if(ret==bad_detected)  return fl;
ob_load_string_forward(&in,lengthbuffers,ptrbefore);

in.setposition(p3);
}//else

ret=ob_get_backward(&in,&ch);
if(ret==bad_detected) return fl;

//start skip endl forward

ret=ob_get_forward(&in,&ch);
if(ret==bad_detected) return fl;

if(ch==ob_const_cr){
p4=in.getposition();
ret=ob_get_forward(&in,&ch);
if(ret==bad_detected) return fl;
if(ch!=ob_const_lf) in.setposition(p4);
}//if

//stop  skip endl forward

ob_load_string_forward(&in,lengthbuffers,ptrafter);

m1:
in.setposition(p0);
}//if
}//if

return fl;
}

#ifdef OB_WCHAR
}//end namespace ob_wcharspace
#else
}//end namespace ob_charspace
#endif

