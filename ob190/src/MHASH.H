 
//Copyright (C) 2004 MKTMK Moscow, Russia
//e-mail: mktmk<at>narod.ru
//v.1.00 12.02.2001(last modification 21.02.2002)
//Hash table

#ifndef MHASH_THIS
#define MHASH_THIS

#include "mstore.h"
#include "mvect.h"
#include "mlist.h"

#ifndef MHASH_INLINE
#define MHASH_INLINE inline
#endif

#ifndef MHASH_NULL
#define MHASH_NULL (0)
#endif

template <class typestorehash,class typecountvect,class typecountlist>
class mhash:
public
mvect_common<
mstore_dynamicalloc<mlist<typestorehash,typecountlist>,typecountvect> >
{
public:

typedef
mvect_common<
mstore_dynamicalloc<mlist<typestorehash,typecountlist>,typecountvect> >
thisvector;

mhash(typecountvect s):
mvect_common<
mstore_dynamicalloc<mlist<typestorehash,typecountlist>,typecountvect> >(s){}

void put(const typestorehash& d,typecountvect hash){

mlist<typestorehash,typecountlist>* ptr=
&(
(*((thisvector*)this))[
(typecountvect)(hash% ( ((thisvector*)this)->getcount() ))]
);

ptr->seek(typecountlist());
ptr->putb(d);
}


template <class F>
typestorehash* findobj(F& f){

mlist<typestorehash,typecountlist>* ptr=
&((*((thisvector*)this))[
(typecountvect)(f.gethash()%(((thisvector*)this)->getcount()))]);

typestorehash* ret1;
if(ptr->findobj(typecountlist(),ptr->getcount(),f)==true){
ret1=(typestorehash*)ptr->copy();
}//if
else{
ret1=MHASH_NULL;
}//else
return ret1;
}

template <class F>
bool get(F& f,typestorehash* dd){

mlist<typestorehash,typecountlist>* ptr=
&((*((thisvector*)this))[
(typecountvect)(f.gethash()%(((thisvector*)this)->getcount()))]);

bool ret=ptr->findobj(typecountlist(),ptr->getcount(),f);
if(ret==true){
ptr->get(dd);
}//if
return ret;
}

template <class F>
bool replaceobj(F& f,typestorehash& d,typestorehash* dd){

mlist<typestorehash,typecountlist>* ptr=
&((*((thisvector*)this))[
(typecountvect)(f.gethash()%(((thisvector*)this)->getcount()))]);

bool ret=ptr->findobj(typecountlist(),ptr->getcount(),f);

if(ret==true){
ptr->get(dd);
}//if

ptr->seek(typecountlist());
ptr->putb(d);

return ret;
}

void clear();

template <class countcommon> countcommon getcounttable();
};

template <class typestorehash,class typecountvect,class typecountlist>
template <class countcommon>
MHASH_INLINE
countcommon mhash<typestorehash,typecountvect,typecountlist>::getcounttable(){
typecountvect i;

typecountvect lenvect=this->getcount();

countcommon sum=countcommon();

for(i=typecountvect();i!=lenvect;++i){
sum+=(*this)[i].getcount();
}//for

return sum;
}

template <class typestorehash,class typecountvect,class typecountlist>
MHASH_INLINE
void mhash<typestorehash,typecountvect,typecountlist>::clear(){

typecountvect i;
typecountvect lenvect=this->getcount();

for(i=typecountvect();i!=lenvect;++i){
(*this)[i].clear();
}//for

}

#endif

