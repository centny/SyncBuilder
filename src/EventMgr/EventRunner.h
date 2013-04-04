/*
 * EventRunner.h
 *
 *  Created on: Nov 5, 2012
 *      Author: Scorpion
 */

#ifndef EVENTRUNNER_H_
#define EVENTRUNNER_H_
#include "../common.h"
#include "EventCfg.h"
#include "../Common/SyncFstream.h"
class ExtCaller;
namespace centny {
//

#define ET_PREVIOUS "ET_PREVIOUS"
#define ET_POST "ET_POST"
class EventNode;
//
class EventRunner {
private:
	Log log;
	ExtCaller *caller;
	SyncFstream cmdlog;
//	thread *thr;
//	int state;
public:
	ListenerCfg *lcfg;
	EventNode* en;
public:
	EventRunner(ListenerCfg* lcfg, EventNode *en);
	~EventRunner();
	void run();
	void stop();
	void addEn2Caller(EventNode *en);
};

} /* namespace centny */
#endif /* EVENTRUNNER_H_ */
