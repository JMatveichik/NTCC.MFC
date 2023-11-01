
//Copyright (C) 2004 MKTMK Moscow, Russia
//e-mail: mktmk<at>narod.ru
//v.1.00 07.04.2001(last modification 02.08.2003)
//List.

#ifndef MLIST_THIS
#define MLIST_THIS

#ifndef MLIST_NULL
#define MLIST_NULL (0)
#endif

#ifndef MLIST_INLINE
#define MLIST_INLINE inline
#endif

//--------------
template <class typestore> class mlist_container{
protected:
typestore data;
mlist_container<typestore>* next;
mlist_container<typestore>* prev;
public:
//init pointer for get one member
mlist_container(const typestore& d):data(d){prev=next=this;}
const typestore& getdata() const {return data;}
//puting this after curr
void puta(mlist_container<typestore>* curr){
prev=curr;next=curr->next;prev->next=next->prev=this;
}
//puting this before curr
void putb(mlist_container<typestore>* curr){
next=curr;prev=curr->prev;prev->next=next->prev=this;
}
//deleting this from queue
void get(){prev->next=next;next->prev=prev;}
mlist_container<typestore>* getnext()const {return next;}
mlist_container<typestore>* getprev()const {return prev;}
//virtual ~mlist_container(){}
};//class mlist_container

//--------------

template <class typestore,class typecount> class mlist{
protected:
typecount count;
typecount currentcount;
mlist_container<typestore>* first;
mlist_container<typestore>* current;

public:
mlist():
count(typecount()),
currentcount(typecount()),
first(MLIST_NULL),
current(MLIST_NULL){}
typecount getcount() const {return count;}
typecount tell() const {return currentcount;}
void seek(typecount n);

void next(){
if(count!=typecount()){//this condition for empty list
   if(current->getnext()!=first){
   current=current->getnext();
   ++currentcount;
   }//if
}//if
}

void prev(){
if(current!=first){//condition for empty list is always include
current=current->getprev();
--currentcount;
}//if
}

void puta(const typestore& d){
mlist_container<typestore>* ptr1=new mlist_container<typestore>(d);
if(count==typecount()){
first=current=ptr1;
currentcount=typecount();
}//if
else{
ptr1->puta(current);
}//else
++count;
}

void putb(const typestore& d){
mlist_container<typestore>* ptr1=new mlist_container<typestore>(d);
if(count==typecount()){
first=current=ptr1;
currentcount=typecount();
}//if
else{
ptr1->putb(current);
++currentcount;
if(first==current) first=current->getprev();
}//else
++count;
}

bool get(typestore* d){

if(count==typecount()) return false;

mlist_container<typestore>* current1;

--count;
if(count==typecount()){
first=current1=MLIST_NULL;
currentcount=typecount();
}//if
else{
   if(current->getnext()==first){
   current1=current->getprev();
   --currentcount;
   }//if
   else{
   current1=current->getnext();
   if(first==current) first=current1;
   }//else
}//else

if(d!=MLIST_NULL) *d=current->getdata();
current->get();
delete current;//for data member destructor is called
current=current1;
return true;
}

template <class F>
bool findobj(const typecount startcount,const typecount endcount,F& f);

bool copy(typestore* d) const {
if(count==typecount()) return false;
*d=current->getdata();
return true;
}

const typestore* copy() const {
if(count==typecount())
return MLIST_NULL;
else
return &(current->getdata());
}

mlist<typestore,typecount>& operator =(const mlist<typestore,typecount>& bl);

mlist(const mlist<typestore,typecount>& bl);

bool operator ==(const mlist& bl) const;
bool operator !=(const mlist& bl) const {return !((*this)==bl);}

mlist<typestore,typecount>& operator +=(const mlist<typestore,typecount>& bl);

mlist<typestore,typecount>& operator +(const mlist<typestore,typecount>& bl){
mlist<typestore,typecount> r;
r=*this;
r+=bl;
return r;
}

void clear();
virtual ~mlist(){
clear();
}
};//class mlist

template <class typestore,class typecount>
template <class F>
MLIST_INLINE
bool mlist<typestore,typecount>::findobj(const typecount startcount,
const typecount endcount,F& f){
typecount i;

typecount start=startcount;
typecount end=endcount;

if(start>end) {i=start;start=end;end=i;}
if(end>=count) {end=count;--end;}

++end;

typecount oldptr=tell();
seek(start);

mlist_container<typestore>* ptr=current;
for(i=currentcount;i!=end;++i){
  if(f((typestore&)ptr->getdata())==true){
  current=ptr;
  currentcount=i;
  return true;
  }//if
ptr=ptr->getnext();
}//for
seek(oldptr);
return false;
}

template <class typestore,class typecount>
MLIST_INLINE
void mlist<typestore,typecount>::seek(typecount n){

if(count==typecount()) return;

if(n==typecount()){
currentcount=typecount();
current=first;
return;
}//if

if(n>=count) {n=count;--n;}

typecount delta;
mlist_container<typestore>* ptr;
mlist_container<typestore>* (mlist_container<typestore>::*ptr2)() const;

ptr=current;

if(n>currentcount){
delta=n-currentcount;
ptr2=&mlist_container<typestore>::getnext;
}else{
delta=currentcount-n;
ptr2=&mlist_container<typestore>::getprev;
}

if(delta>n){
delta=n;
ptr2=&mlist_container<typestore>::getnext;
ptr=first;
}

typecount delta1=count-n;

if(delta>delta1){
delta=delta1;
ptr2=&mlist_container<typestore>::getprev;
ptr=first;
}

current=ptr;
typecount i;
for(i=typecount();i!=delta;++i) current=(current->*ptr2)();

currentcount=n;
}

template <class typestore,class typecount>
MLIST_INLINE
void mlist<typestore,typecount>::clear(){
while(count!=typecount()) get(MLIST_NULL);
}

template <class typestore,class typecount>
MLIST_INLINE
mlist<typestore,typecount>& mlist<typestore,typecount>::operator =
(const mlist<typestore,typecount>& bl){
if(this!=&bl){//may be assign the same exemplar
clear();
typecount i;
mlist_container<typestore>* ptr=bl.first;
   for(i=typecount();i!=bl.count;++i){
   puta(ptr->getdata());
   next();
   ptr=ptr->getnext();
   }//for
seek(bl.tell());
}//if !=this
return *this;
}

template <class typestore,class typecount>
MLIST_INLINE
mlist<typestore,typecount>::mlist(const mlist<typestore,typecount>& bl):
count(typecount()),
currentcount(typecount()),
first(MLIST_NULL),
current(MLIST_NULL){
typecount i;
mlist_container<typestore>* ptr=bl.first;
   for(i=typecount();i!=bl.count;++i){
   puta(ptr->getdata());
   next();
   ptr=ptr->getnext();
   }//for
seek(bl.tell());
}

template <class typestore,class typecount>
MLIST_INLINE
bool mlist<typestore,typecount>::operator ==(const mlist& bl) const {
if(count!=bl.count) return false;
typecount i;
mlist_container<typestore>* ptr =   first;
mlist_container<typestore>* ptr1=bl.first;
for(i=typecount();i!=bl.count;++i){
if(ptr->getdata()!=ptr1->getdata()) return false;
ptr =ptr ->getnext();
ptr1=ptr1->getnext();
}//for
return true;
}

template <class typestore,class typecount>
MLIST_INLINE
mlist<typestore,typecount>& mlist<typestore,typecount>::operator +=
(const mlist<typestore,typecount>& bl){
typecount i;
mlist_container<typestore>* ptr=bl.first;

typecount tmp=tell();

seek(count);

typecount blcount=bl.count;

for(i=typecount();i!=blcount;++i){
puta(ptr->getdata());
next();
ptr=ptr->getnext();
}//for

seek(tmp);

return *this;
}

#endif

