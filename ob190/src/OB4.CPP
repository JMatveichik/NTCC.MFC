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

//function for ob_var:: and children

#define OB_BASEFUN(ob_class1,ident)                               \
ob_var* ob_class1::addv(ob_var* a){return a->addv(this);}         \
ob_var* ob_class1::subv(ob_var* a){return a->subv(this);}         \
ob_var* ob_class1::mulv(ob_var* a){return a->mulv(this);}         \
ob_var* ob_class1::divv(ob_var* a){return a->divv(this);}         \
ob_var* ob_class1::pwwv(ob_var* a){return a->pwwv(this);}         \
ob_var* ob_class1::asgv(ob_var* a){return a->asgv(this);}         \
bool     ob_class1::compare(ob_var* a,                            \
ob_var::ob_type_if currentlexif){                                 \
return a->compare(this,currentlexif);}                            \
ob_var* ob_class1::duplicate()                                    \
{ob_var* a=new ob_class1(var);return a;}                          \
ob_type_ident ob_class1::isA() const {return ident;}              \

#define OB_BASEFUNDIFFER(ob_class1)                                      \
ob_var* ob_class1::uniminus() {var=-var;return this;}                    \
bool ob_class1::eq() const  {if(var==0) return true; else return false;} \
void ob_class1::tostream(ob_ostream* out){                               \
if(out==OB_NULL) OB_ERR(ob_err81)                                        \
if((!out->good())) OB_ERR(ob_err81)                                      \
out->putdigit(var);                                                      \
}

OB_BASEFUN(ob_float,OB_IDENTFLO)
OB_BASEFUNDIFFER(ob_float)
OB_BASEFUN(ob_int,OB_IDENTINT)
OB_BASEFUNDIFFER(ob_int)
//OB_BASEFUN(ob_str,OB_IDENTSTR)

#define OB_BASEFUN1(ob_class1,ident)                              \
ob_var* ob_class1::addv(ob_var* a){return a->addv(this);}         \
ob_var* ob_class1::subv(ob_var* a){return a->subv(this);}         \
ob_var* ob_class1::mulv(ob_var* a){return a->mulv(this);}         \
ob_var* ob_class1::divv(ob_var* a){return a->divv(this);}         \
ob_var* ob_class1::pwwv(ob_var* a){return a->pwwv(this);}         \
bool     ob_class1::compare(ob_var* a,                            \
ob_var::ob_type_if currentlexif){                                 \
return a->compare(this,currentlexif);}                            \
ob_type_ident ob_class1::isA() const {return ident;}              \

OB_BASEFUN1(ob_str,OB_IDENTSTR)

ob_var* ob_str::duplicate()
{
ob_var* a=new ob_str(var.c_str());
return a;
}

ob_var* ob_str::asgv(ob_var* a){
return a->asgv(this);
}         

ob_var* ob_str::uniminus() {OB_ERR(ob_err50);return OB_NULL;}
bool ob_str::eq() const {OB_ERR(ob_err51);return 0;}

void ob_str::tostream(ob_ostream* out){
if(out==OB_NULL) OB_ERR(ob_err81)
if((!out->good())) OB_ERR(ob_err81)

ob_type_stringsize l=size();

ob_type_stringsize i;
for(i=0;i!=l;++i){
out->put(var.c_str()[i]);
}//for
}

#define OB_DOPFUNCOMPARE(ob_class1,ob_class2)                     \
bool ob_class1::compare(ob_class2* a,                             \
ob_var::ob_type_if currentlexif){                                 \
bool rez=false;                                                   \
switch(currentlexif){                                             \
case LESSIF:if(a->var<var) rez=true;break;                        \
case GREATIF:if(a->var>var) rez=true;break;                       \
case EQVIF:if(a->var==var) rez=true;break;                        \
case LESSEQVIF:if(a->var<=var) rez=true;break;                    \
case GREATEQVIF:if(a->var>=var) rez=true;break;                   \
case NOTEQVIF:if(a->var!=var) rez=true;break;                     \
default:OB_ERR(ob_err2);                                          \
}                                                                 \
return rez;                                                       \
}

#define OB_DOPFUN(ob_class1,ob_class2)                                                             \
ob_var* ob_class1::addv(ob_class2* a){a->var=(ob_class2::typedatavar)(a->var+var);return a;}       \
ob_var* ob_class1::subv(ob_class2* a){a->var=(ob_class2::typedatavar)(a->var-var);return a;}       \
ob_var* ob_class1::mulv(ob_class2* a){a->var=(ob_class2::typedatavar)(a->var*var);return a;}       \
ob_var* ob_class1::divv(ob_class2* a){a->var=(ob_class2::typedatavar)(a->var/var);return a;}       \
ob_var* ob_class1::asgv(ob_class2* a){a->var=(ob_class2::typedatavar)(var);return a;}              \
OB_DOPFUNCOMPARE(ob_class1,ob_class2)

OB_DOPFUN(ob_float,ob_float)
ob_var* ob_float::pwwv(ob_float* a){a->var=(ob_float::typedatavar)(pow((ob_float::typedatavar)a->var,(ob_float::typedatavar)var));return a;}

//OB_DOPFUN(ob_float,ob_int)     // to be change

ob_var* ob_float::addv(ob_int* a){var=a->var+var;return this;}
ob_var* ob_float::subv(ob_int* a){var=a->var-var;return this;}
ob_var* ob_float::mulv(ob_int* a){var=a->var*var;return this;}
ob_var* ob_float::divv(ob_int* a){var=a->var/var;return this;}
ob_var* ob_float::pwwv(ob_int* a){
var=(ob_float::typedatavar)pow((ob_float::typedatavar)a->var,(ob_float::typedatavar)var);return this;}
ob_var* ob_float::asgv(ob_int* a){a->var=(ob_int::typedatavar)var;return a;}

OB_DOPFUNCOMPARE(ob_float,ob_int)

OB_DOPFUN(ob_int,ob_float)
ob_var* ob_int::pwwv(ob_float* a){a->var=(ob_float::typedatavar)(pow((ob_float::typedatavar)a->var,(ob_float::typedatavar)var));return a;}

OB_DOPFUN(ob_int,ob_int)
ob_var* ob_int::pwwv(ob_int* a){a->var=(ob_int::typedatavar)(pow((ob_float::typedatavar)a->var,(ob_float::typedatavar)var));return a;}

#define OB_DOPFUNONESTR(name,ob_class1,ob_class2)              \
ob_var* ob_class1::name(ob_class2*){OB_ERR(ob_err52);return OB_NULL;}

#define OB_DOPFUNONESTRCOMPARE(ob_class1,ob_class2)     \
bool ob_class1::compare(ob_class2*,ob_var::ob_type_if){ \
OB_ERR(ob_err53);return 0;}

#define OB_DOPFUNSTR(ob_class1,ob_class2)        \
OB_DOPFUNONESTR(addv,ob_class1,ob_class2)        \
OB_DOPFUNONESTR(subv,ob_class1,ob_class2)        \
OB_DOPFUNONESTR(mulv,ob_class1,ob_class2)        \
OB_DOPFUNONESTR(divv,ob_class1,ob_class2)        \
OB_DOPFUNONESTR(pwwv,ob_class1,ob_class2)        \
OB_DOPFUNONESTR(asgv,ob_class1,ob_class2)        \
OB_DOPFUNONESTRCOMPARE(ob_class1,ob_class2)

OB_DOPFUNSTR(ob_float,ob_str)
OB_DOPFUNSTR(ob_int,ob_str)
OB_DOPFUNSTR(ob_str,ob_float)
OB_DOPFUNSTR(ob_str,ob_int)

OB_DOPFUNCOMPARE(ob_str,ob_str)

ob_var* ob_str::addv(ob_str* a){
ob_type_string tmpstr(a->var.c_str(),a->var.getcount()-1);
tmpstr+=var;
a->var=tmpstr;
return a;
}

ob_var* ob_str::asgv(ob_str* a){a->var=var;return a;}

OB_DOPFUNONESTR(subv,ob_str,ob_str)
OB_DOPFUNONESTR(mulv,ob_str,ob_str)
OB_DOPFUNONESTR(divv,ob_str,ob_str)
OB_DOPFUNONESTR(pwwv,ob_str,ob_str)

//constructors
ob_float::ob_float(ob_type_char* a){
ob_istrstream b(a);
b>>var;
}

ob_int::ob_int(ob_type_char* a){
ob_istrstream b(a);
b>>var;
}

ob_float::ob_float(ob_type_flo a){var=a;}
ob_int::ob_int(ob_type_int a){var=a;}
ob_str::ob_str(ob_type_char* s,ob_type_stringsize l):var(s,l){}
ob_str::ob_str(ob_type_char* s):var(s){}

ob_float::ob_float(){var=0;}
ob_int::ob_int(){var=0;}
ob_str::ob_str():var((ob_type_char*)""){}

//topar
#define OB_TOPAR(ob_class1,offset,showget)                    \
void ob_class1::topar(ob_par* p,const unsigned int i){        \
showget                                                       \
(p->mo)[offset][0]++;                                         \
(p->mo)[offset][(p->mo)[offset][0]]=i;                        \
}

OB_TOPAR(ob_float,OB_TYPEFLO,(p->pf)[(p->mo)[OB_TYPEFLO][0]]=var;)
OB_TOPAR(ob_int,OB_TYPEINT,(p->pi)[(p->mo)[OB_TYPEINT][0]]=var;)
OB_TOPAR(ob_str,OB_TYPESTR,*(p->pc)<<var.c_str()<<ends;)

//casts
void ob_float::cast(ob_type_flo* a){*a=var;}
void ob_float::cast(ob_type_int* a){*a=(ob_type_int)var;}
void ob_float::cast(ob_type_char** a){OB_ERR(ob_err73);*a=OB_NULL;}

void ob_int::cast(ob_type_flo* a){*a=(ob_type_flo)var;}
void ob_int::cast(ob_type_int* a){*a=var;}
void ob_int::cast(ob_type_char** a){OB_ERR(ob_err74);*a=OB_NULL;}

void ob_str::cast(ob_type_flo* a){OB_ERR(ob_err75);*a=ob_type_flo();}
void ob_str::cast(ob_type_int* a){OB_ERR(ob_err76);*a=ob_type_int();}
void ob_str::cast(ob_type_char** a){*a=var.c_str();}

#ifdef OB_WCHAR
}//end namespace ob_wcharspace
#else
}//end namespace ob_charspace
#endif

