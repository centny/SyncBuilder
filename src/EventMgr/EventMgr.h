/*
 * EventMgr.h
 *
 *  Created on: Nov 5, 2012
 *      Author: Scorpion
 */

#ifndef EVENTMGR_H_
#define EVENTMGR_H_
#include "../ProtocolAdapter/AdapterBase.h"
#include "../common.h"
#include "EventCfg.h"
#include "EventRunner.h"
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <map>
namespace centny {
using namespace file;
static map<string, string> EMPTY_KVS;
enum FEventType {
	FET_NEW = 1, FET_UPDATE, FET_DELETE, FET_NONE
};
string toFEventType(FEventType et);
enum FEventPeriod {
	FEP_PRE = 1, FEP_POST, FEP_CMD, FEP_NONE
};
string toFEventPeriod(FEventPeriod ep);
enum FEventLocation {
	FEL_LOC = 1, FEL_NET, FEL_NONE
};
string toFEventLocation(FEventLocation el);
class EventNode {
public:
	FEventPeriod period;
public:
	map<string, string> kvs;
	virtual ~EventNode();
	virtual string toString();
	virtual void add2Caller(ExtCaller* caller);
	virtual bool match(ListenerCfg* lc)=0;
};
class NEventNode: public EventNode {
public:
	string name;
public:
	NEventNode(string name);
	virtual string toString();
	virtual bool match(ListenerCfg* lc);
};
class FEventNode: public EventNode {
public:
	string locf;
	string locfn;
	string netf;
	string netfn;
	FEventType type;
	FEventLocation uloc;
public:
	virtual string toString();
	virtual void add2Caller(ExtCaller* caller);
	virtual bool match(ListenerCfg* lc);
};
//
//
class EventMgr {
public:
	typedef size_t EventMgrId;
private:
	Log log;
	EventCfg *cfg;
	boost::thread *thr;
	EventRunner *running;
	boost::mutex running_mutex;bool stopped;
	boost::mutex stopped_mutex;
	vector<EventNode*> events;
	boost::mutex events_mutex;
public:
	bool valid;
	string msg;
#define EVENTS_LOCK boost::mutex::scoped_lock lock(this->events_mutex)
public:
	static EventMgrId createDemo(EventCfg *cfg);
	static void fre();
	static void freDemo(EventMgrId id);
	static EventMgr* demo(EventMgrId id);
private:
	EventMgr(EventCfg *cfg);
	void run();
	ListenerCfg* listener(EventNode* en);bool match(EventNode* en,
			ListenerCfg* lc);
public:
//	void postEvent(string name, string type, DataPool::DId did);
	void postEvent(string locf, string locfn, string netf, string netfn,
			FEventLocation uloc, FEventType type, FEventPeriod period,
			map<string, string>& kvs = EMPTY_KVS);
	void postEvent(string name, FEventPeriod period, map<string, string>& kvs =
			EMPTY_KVS);
	int popEvent(string locf, string netf, FEventLocation uloc = FEL_NONE,
			FEventType type = FET_NONE, FEventPeriod period = FEP_NONE);
	int popEvent(string name, FEventPeriod period);
	virtual ~EventMgr();
};

} /* namespace centny */
#endif /* EVENTMGR_H_ */
