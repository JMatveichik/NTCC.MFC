
//Copyright (C) 2004 MKTMK Moscow, Russia
//e-mail: mktmk<at>narod.ru
//v.1.00 12.02.2001(last modification 21.02.2002)
//Vector

#ifndef MVECT_THIS
#define MVECT_THIS

#ifndef MVECT_NULL
#define MVECT_NULL (0)
#endif

#ifndef MVECT_INLINE
#define MVECT_INLINE inline
#endif

#include "mstore.h"

template <class typebase>
class mvect_common:public typebase{
public:

typedef typename typebase::tstore typestore;
typedef typename typebase::tcount typecount;

mvect_common(){}
mvect_common(typecount s):typebase(s){}
mvect_common(typestore* data1,typecount s):typebase(data1,s){}

mvect_common(typestore* data1,typestore end,typecount max):
typebase(data1,end,max){}

typestore& operator[](typecount i){return typebase::data[i];}

//operator typestore*() {return c_str();}
typestore* c_str(){return typebase::data;}

template <class F>
bool find(
typecount startcount,
typecount endcount,
typecount* current,
F& f);

template <class typebase1>
mvect_common& operator =(const mvect_common<typebase1>& d){
*((typebase*)this)=*((typebase1*)&d);
}

mvect_common& operator =(const mvect_common& d){
*((typebase*)this)=*((typebase*)&d);
return *this;
}

mvect_common(const mvect_common<typebase>& d):typebase(*((typebase*)&d)){
}

template <class typebase1>
mvect_common(const mvect_common<typebase1>& d):typebase(*((typebase1*)&d)){
}

template <class typebase1>
bool operator ==(mvect_common<typebase1>& d);

template <class typebase1>
bool operator <(mvect_common<typebase1>& d);

template <class typebase1>
bool operator >(mvect_common<typebase1>& d);

template <class typebase1>
bool operator <=(mvect_common<typebase1>& d){return !(operator >(d));}

template <class typebase1>
bool operator >=(mvect_common<typebase1>& d){return !(operator <(d));}

template <class typebase1>
bool operator !=(mvect_common<typebase1>& d){return !(operator ==(d));}

bool
compareless(typestore* data,typestore* data1,typecount count,typecount count1);

bool
comparegreat(typestore* data,typestore* data1,typecount count,typecount count1);

template <class typebase1>
bool compareless(mvect_common<typebase1>& d);

template <class typebase1>
bool comparegreat(mvect_common<typebase1>& d);
};

template <class typebase>
template <class typebase1>
MVECT_INLINE
bool mvect_common<typebase>::operator ==(mvect_common<typebase1>& d){

typecount l=typebase::getcount();
if(l==d.getcount()){

typecount i;

  for(i=typecount();i!=l;++i){
    if(typebase::data[i]!=d.getdata()[i]){
    return false;
    }//if
  }//for
return true;
}//if

else{
return false;
}//else
}

template <class typebase>
bool
mvect_common<typebase>::
compareless(typestore* data,typestore* data1,typecount count,typecount count1){

typecount l;
if(count<count1) l=count; else l=count1;

typecount i;

  for(i=typecount();i!=l;++i){
    if(data[i]>=data1[i]){
    return false;
    }//if
  }//for

if(count>count1) return false;
if(count==typecount()) return false;

return true;
}

template <class typebase>
bool
mvect_common<typebase>::
comparegreat(typestore* data,typestore* data1,typecount count,typecount count1){

typecount l;
if(count<count1) l=count; else l=count1;

typecount i;

  for(i=typecount();i!=l;++i){
    if(data[i]<=data1[i]){
    return false;
    }//if
  }//for

if(count<count1) return false;
if(count1==typecount()) return false;

return true;
}

//compare for string
template <class typebase>
template <class typebase1>
MVECT_INLINE
bool mvect_common<typebase>::operator <(mvect_common<typebase1>& d){
return 
compareless(
typebase::getdata(),d.getdata(),typebase::getcount()-1,d.getcount()-1);
}

template <class typebase>
template <class typebase1>
MVECT_INLINE
bool mvect_common<typebase>::operator >(mvect_common<typebase1>& d){
return 
comparegreat(
typebase::getdata(),d.getdata(),typebase::getcount()-1,d.getcount()-1);
}

//compare for full vector
template <class typebase>
template <class typebase1>
MVECT_INLINE
bool mvect_common<typebase>::compareless(mvect_common<typebase1>& d){
return compareless(getdata(),d.getdata(),getcount(),d.getcount());
}

template <class typebase>
template <class typebase1>
MVECT_INLINE
bool mvect_common<typebase>::comparegreat(mvect_common<typebase1>& d){
return comparegreat(getdata(),d.getdata(),getcount(),d.getcount());
}

template <class typebase>
template <class F>
MVECT_INLINE
bool mvect_common<typebase>::
find(
typecount startcount,
typecount endcount,
typecount* current,
F& f){

typecount i;

typecount start=startcount;
typecount end=endcount;

if(start>end) {i=start;start=end;end=i;}
if(end>=count) {end=count;--end;}

++end;

for(i=start;i!=end;++i){
  if(f(data[i])==true){
  current=&(data[i]);
  return true;
  }//if
}//for
return false;
}

#endif

