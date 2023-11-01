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

//here is function ob_apiXXXX::

//for API
ob_apif::ob_apif(ob_type_char* a,ob_type_flofun f):ob_api(a){ptrfun=f;}
ob_apii::ob_apii(ob_type_char* a,ob_type_intfun f):ob_api(a){ptrfun=f;}
ob_apic::ob_apic(ob_type_char* a,ob_type_charfun f,ob_type_del d):ob_api(a){
dt=d;ptrfun=f;}


//need ob_standartstring object because it is not destroy during ::run work
#define OB_APIRUN(typeret,typeob,dop,typeapi)                         \
void typeapi::run(ob_obasic* ptr,ob_par* p,ob_savewrapper* v){        \
                                                                      \
ob_type_char *pn,*pc;                                                 \
                                                                      \
ob_standartstring st1=p->name->str();                                 \
ob_standartstring st2=p->pc->str();                                   \
                                                                      \
pn=(ob_type_char*)st1.c_str();                                        \
pc=(ob_type_char*)st2.c_str();                                        \
                                                                      \
typeret dd=ptrfun(ptr,(const ob_type_char*)pn,                        \
p->mo[OB_TYPEFLO],                                                    \
p->mo[OB_TYPEINT],                                                    \
p->mo[OB_TYPESTR],                                                    \
p->pf,                                                                \
p->pi,                                                                \
(const ob_type_char*)pc);                                             \
                                                                      \
if(v->getptr()!=OB_NULL) OB_ERR(ob_err65);                            \
v->setptr(new typeob(dd));                                            \
dop                                                                   \
p->deletepar();                                                       \
}

OB_APIRUN(ob_type_flo,ob_float,,ob_apif)
OB_APIRUN(ob_type_int,ob_int,,ob_apii)
OB_APIRUN(ob_type_char*,ob_str,if(dt==OB_DELETE) delete[] dd;,ob_apic)

#ifdef OB_WCHAR
}//end namespace ob_wcharspace
#else
}//end namespace ob_charspace
#endif

