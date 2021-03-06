
#ifndef AUTUMN_MULTIINHERITEBEAN_H
#define AUTUMN_MULTIINHERITEBEAN_H

#include "IMultiBase.h"

class Base1:public IBase1{
	int i, j, k;
public:
	int f1(){ return 1;}	
};

class Base2:public IBase2{
	char a, b, c;
public:
	int f2(){ return 2;}	
};

class Multi_Base1: public IBase3, public IBase2, public IBase1{
public:
	int f1(){ return 1;}	
	int f2(){ return 2;}	
	int f3(){ return 3;}	
};

class Multi_Bean: public IMulti_Bean{
	IBase1* b1;
	IBase2* b2;
public:
	Multi_Bean(IBase2* b){ this->b2 = b; }
	void setB1(IBase1* b){ this->b1 = b; }
	int getB1(){ return b1->f1(); }
	int getB2(){ return b2->f2(); }
};

/** 
 * @base IBase1
 * @base IBase2
 */
class Multi_Base2: public IBase3, public Base2, public Base1{
public:
	int f3(){ return 3;}	
};

#endif

