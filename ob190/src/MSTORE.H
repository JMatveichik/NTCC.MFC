
//Copyright (C) 2004 MKTMK Moscow, Russia
//e-mail: mktmk<at>narod.ru
//v.1.00 15.07.2004(last modification 15.07.2004)
//store classes

#ifndef MSTORE_THIS
#define MSTORE_THIS

#ifndef MSTORE_NULL
#define MSTORE_NULL (0)
#endif

#ifndef MSTORE_INLINE
#define MSTORE_INLINE inline
#endif

template <class typestore,class typecount>
MSTORE_INLINE
typecount
mstore_strlen(
typestore* data,
typestore end,
typecount max){

typecount i=typecount();
  for(;;){
  if(data[i]==end){
  ++i;

  break;
  }//if
  ++i;
    if(i==max){
    break;
    }//if
  }//for
return i;
}

template <class typestore,class typecount>
MSTORE_INLINE
void mstore_copy(
typestore* datadst,
typestore* datasrc,
typecount count){
typecount i;
for(i=0;i!=count;++i) datadst[i]=datasrc[i];
}

template <class typestore,class typecount>
MSTORE_INLINE
void mstore_copy(
typestore* datadst,
typecount countdst,
typestore* datasrc,
typecount countsrc){

  typecount i;
  typecount l;

  if(countsrc<countdst){
  l=countsrc;
  for(i=countsrc;i!=countdst;++i) datadst[i]=typestore();
  }//if
  else{
  l=countdst;
  }//else

  for(i=0;i!=l;++i)  datadst[i]=datasrc[i];
}

template <class typestore,class typecount>
class mstore_basestore{
public:
typedef typestore tstore;
typedef typecount tcount;
};

template <class typestore,class typecount>
class mstore_dynamicallocbase:public mstore_basestore<typestore,typecount>{

protected:

typestore* data;
typecount count;

void create(typecount s){
count=s;
if(count!=typecount()) data= new typestore[count]; else data=MSTORE_NULL;
}

void create(typestore* ptr,typecount c){
create(c);
mstore_copy(data,ptr,c);
}

public:

mstore_dynamicallocbase(){create(typecount());}

mstore_dynamicallocbase(typecount s){create(s);}

mstore_dynamicallocbase(const mstore_dynamicallocbase<typestore,typecount>& d){
create(d.data,d.getcount());
}

mstore_dynamicallocbase(typestore* data1,typecount count1){
create(data1,count1);
}

mstore_dynamicallocbase(typestore* data1,typestore end,typecount max){
typecount count1=mstore_strlen(data1,end,max);
create(data1,count1);
}

void copy(typestore* data1,typecount count1){
delete[] data;
create(data1,count1);
}

void
copy(typestore* data1,typestore end=typestore(),typecount max=typecount()){
delete[] data;
typecount count1=mstore_strlen(data1,end,max);
create(data1,count1);
}

void set(typestore* data1,typecount count1){
data=data1;
count=count1;
}

void transfer(mstore_dynamicallocbase& d){
set(d.data,d.count);
d.set(MSTORE_NULL,typecount());
}

void recount(typecount s){
delete[] data;
create(s);
}

typecount getcount() const {return count;}
typestore* getdata() const {return data;}

~mstore_dynamicallocbase(){delete[] data;}
};

template <class typestore,class typecount,typecount count>
class mstore_staticdata:public mstore_basestore<typestore,typecount>{
protected:
typestore data[count];
public:
typecount getcount() const {return count;}
typestore* getdata() const {return data;}
};

template <class typestore,class typecount>
class
mstore_staticdatawithoutbuf:public mstore_basestore<typestore,typecount>{
protected:
typestore* data;
typecount count;
public:
mstore_staticdatawithoutbuf(typestore* d,typecount s):data(d),count(s){}
typecount getcount() const {return count;}
typestore* getdata() const {return data;}

void set(typestore* data1,typecount count1){
data=data1;
count=count1;
}

};

template <class typestore,class typecount,typecount count1>
class mstore_dynamicdata:public mstore_dynamicallocbase<typestore,typecount>{
protected:
mstore_dynamicdata():mstore_dynamicallocbase<typestore,typecount>(count1){}
public:
typecount getcount(){
return mstore_dynamicallocbase<typestore,typecount>::getcount();
}
};

template <class typestore,class typecount> class mstore_dynamicalloc;

template <class typebase>
class mstore_staticalloc:public typebase{

public:

typedef typename typebase::tstore typestore;
typedef typename typebase::tcount typecount;

mstore_staticalloc<typebase>&
operator =(const mstore_staticalloc<typebase>& d){
if(&d!=this)
mstore_copy<typestore,typecount>(getdata(),getcount(),d.getdata(),d.getcount());
return *this;
}

template <class typebase1>
mstore_staticalloc<typebase>&
operator =(const mstore_staticalloc<typebase1>& d){
mstore_copy<typestore,typecount>(getdata(),getcount(),d.getdata(),d.getcount());
return *this;
}

mstore_staticalloc<typebase>&
operator =(const mstore_dynamicalloc<typestore,typecount>& d){
mstore_copy<typestore,typecount>(getdata(),getcount(),d.getdata(),d.getcount());
return *this;
}

mstore_staticalloc(const mstore_staticalloc<typebase>& d){
mstore_copy<typestore,typecount>(getdata(),getcount(),d.getdata(),d.getcount());
}

template <class typebase1>
mstore_staticalloc(const mstore_staticalloc<typebase1>& d){
mstore_copy<typestore,typecount>(getdata(),getcount(),d.getdata(),d.getcount());
}

mstore_staticalloc(const mstore_dynamicalloc<typestore,typecount>& d){
mstore_copy<typestore,typecount>(getdata(),getcount(),d.getdata(),d.getcount());
}

mstore_staticalloc(){}

mstore_staticalloc(typestore* data1,typecount count1):typebase(data1,count1){
mstore_copy<typestore,typecount>(getdata(),getcount(),data1,count1);
}

void copy(typestore* data1,typecount count1){
mstore_copy<typestore,typecount>(getdata(),getcount(),data1,count1);
}

void copy(typestore* data1,typestore end=typestore(),typecount max=typecount()){
typecount count1=mstore_strlen(data1,end,max);
mstore_copy<typestore,typecount>(getdata(),getcount(),data1,count1);
}

//~mstore_staticalloc(){}//do not need destructor
};

template <class typestore,class typecount>
class mstore_dynamicalloc:public mstore_dynamicallocbase<typestore,typecount>{

public:

mstore_dynamicalloc(){}
mstore_dynamicalloc(typecount s):
mstore_dynamicallocbase<typestore,typecount>(s){}

mstore_dynamicalloc(typestore* data1,typecount count1):
mstore_dynamicallocbase<typestore,typecount>(data1,count1){}

mstore_dynamicalloc(typestore* data1,typestore end,typecount max):
mstore_dynamicallocbase<typestore,typecount>(data1,end,max){}

mstore_dynamicalloc<typestore,typecount>&
operator =(const mstore_dynamicalloc<typestore,typecount>& d){
if(&d!=this){
delete[] mstore_dynamicallocbase<typestore,typecount>::data;
mstore_dynamicallocbase<typestore,typecount>::
create(
/*(typestore)*/d.getdata(),
(typecount)d.getcount()
);
}//if
return *this;
}

template <class typebase1>
mstore_dynamicalloc<typestore,typecount>&
operator =(const mstore_staticalloc<typebase1>& d){
delete[] data;
create(d.getdata(),d.getcount());
return *this;
}

mstore_dynamicalloc(const mstore_dynamicalloc<typestore,typecount>& d):
mstore_dynamicallocbase<typestore,typecount>(d.getdata(),d.getcount()){}

template <class typebase1>
mstore_dynamicalloc(const mstore_staticalloc<typebase1>& d){
create(d.getdata(),d.getcount());
}

void add(typestore* p,typecount c);

mstore_dynamicalloc<typestore,typecount>&
operator+=(const mstore_dynamicalloc<typestore,typecount>& d){
add(d.getdata(),d.getcount());
return *this;
}

template <class typebase1>
mstore_dynamicalloc<typestore,typecount>&
operator+=(const mstore_staticalloc<typebase1>& d){
add(d.getdata(),d.getcount());
return *this;
}

mstore_dynamicalloc<typestore,typecount>
operator+(const mstore_dynamicalloc<typestore,typecount>& d){
mstore_dynamicalloc<typestore,typecount> d1=*this;
d1+=d;
return d1;
}

template <class typebase1>
mstore_dynamicalloc<typestore,typecount>&
operator+(const mstore_staticalloc<typebase1>& d){
mstore_dynamicalloc<typestore,typecount> d1=*this;
d1+=d;
return d1;
}

//do not need destructor
};

template <class typestore,class typecount>
MSTORE_INLINE
void mstore_dynamicalloc<typestore,typecount>::
add(typestore* p,typecount c){

typestore* olddata=mstore_dynamicallocbase<typestore,typecount>::data;
typecount oldcount=mstore_dynamicallocbase<typestore,typecount>::count;

typestore* p1=p;//for p==this->data

mstore_dynamicallocbase<typestore,typecount>::
create(mstore_dynamicallocbase<typestore,typecount>::count+c);

typecount i;
for(i=typecount();i!=oldcount;++i){
mstore_dynamicallocbase<typestore,typecount>::data[i]=olddata[i];
}//for

for(i=typecount();i!=c;++i){
mstore_dynamicallocbase<typestore,typecount>::data[oldcount+i]=p1[i];
}//for

delete[] olddata;
}

#endif

