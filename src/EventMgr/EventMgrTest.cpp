/*
 * testEventMgr.cpp
 *
 *  Created on: Nov 5, 2012
 *      Author: Scorpion
 */
#include "../common.h"
#include "EventMgr.h"
using namespace centny;
void testEventMgr(){
#ifdef WIN32
	string str="EventMgr/e.cfg";
#else
//	string str="src/EventMgr/e.cfg";
//	EventMgr::EventMgrId mid=EventMgr::createDemon(str);
//	EventMgr *mgr=EventMgr::demon(1);
#endif
//	EventMgr::EventMgrId mid=EventMgr::createDemon(str);
//	if(!mid){
//		return;
//	}
//	EventMgr *mgr=EventMgr::demon(mid);
//	mgr->postEvent("A",ET_PREVIOUS,"");
#ifdef WIN32
//	mgr->postEvent("C",ET_PREVIOUS,"");
//	mgr->postEvent("C",ET_POST,"");
#else
//	mgr->postEvent("B",ET_PREVIOUS,"");
#endif
	bsleep(5000);
	EventMgr::fre();
	printf("--------------------------------testEventMgr--------------------------------\n");
}



