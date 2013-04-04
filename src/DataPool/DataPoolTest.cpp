/*
 * testDataPool.cpp
 *
 *  Created on: Nov 5, 2012
 *      Author: Scorpion
 */
#include "../common.h"

static int TDP_A_IDX=0;
class TDP_A:public DataPool::DObj{
public:
	int idx;
	TDP_A(){
		idx=TDP_A_IDX++;
	}
	virtual ~TDP_A(){
		cout<<"free TDP_A:"<<idx<<endl;
	}
	void log(){
		cout<<"current idx:"<<idx<<endl;
	}
	void fre(){
		delete this;
	}
};
void testDataPool(){
	{
		TDP_A a;
	}
	DataPool::DId did1=DataPool::sharedPool().add(new TDP_A());
	DataPool::DId did2=DataPool::sharedPool().add(new TDP_A());
	DataPool::DId did3=DataPool::sharedPool().add(new TDP_A());
	TDP_A *a1=(TDP_A *)DataPool::sharedPool().data(did1);
	TDP_A *a2=(TDP_A *)DataPool::sharedPool().data(did2);
	TDP_A *a3=(TDP_A *)DataPool::sharedPool().data(did3);
	a1->log();
	a2->log();
	a3->log();
	DataPool::sharedPool().fre(did1);
	DataPool::sharedPool().fre(did2);
	DataPool::sharedPool().fre(did3);
	printf("------------------------------testDataPool------------------------------\n");
//	void* aa=a1;
//	delete aa;
}



