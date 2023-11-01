
//Copyright (C) 2004 MKTMK Moscow, Russia
//e-mail: mktmk<at>narod.ru
//v.1.00 08.09.2002(last modification 08.09.2002)
//Stack.

#ifndef MSTACK_THIS
#define MSTACK_THIS

#include "mlist.h"

template <class typestore,class typecount> class mstack:
public mlist<typestore,typecount> {
static int countstack;
public:
void put(const typestore& d){
mlist<typestore,typecount>::putb(d);
mlist<typestore,typecount>::prev();
}
bool get(typestore* d){return mlist<typestore,typecount>::get(d);}
virtual ~mstack(){}
};

#endif

