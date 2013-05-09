/*
 * EventMgr.cpp
 *
 *  Created on: Nov 5, 2012
 *      Author: Scorpion
 */

#include "EventMgr.h"
#include "EventRunner.h"
#include <boost/regex.hpp>
#include <ExtCaller.h>
namespace centny {
//
string toFEventType(FEventType et) {
	switch (et) {
	case FET_NEW:
		return "FET_NEW";
	case FET_UPDATE:
		return "FET_UPDATE";
	case FET_DELETE:
		return "FET_DELETE";
	default:
		return "FET_NONE";
	}
}
string toFEventPeriod(FEventPeriod ep) {
	switch (ep) {
	case FEP_PRE:
		return "FEP_PRE";
	case FEP_POST:
		return "FEP_POST";
	case FEP_CMD:
		return "FEP_CMD";
	default:
		return "FEP_NONE";
	}
}
string toFEventLocation(FEventLocation el) {
	switch (el) {
	case FEL_LOC:
		return "FEL_LOC";
	case FEL_NET:
		return "FEL_NET";
	default:
		return "FEL_NONE";
	}
}
//
EventNode::EventNode() {
	this->period = FEP_CMD;
}
EventNode::~EventNode() {

}
string EventNode::toString() {
	return "";
}
void EventNode::add2Caller(ExtCaller* caller) {
	map<string, string>& envs = this->kvs;
	if (envs.size()) {
		map<string, string>::iterator it;
		for (it = envs.begin(); it != envs.end(); it++) {
			caller->addEnv(it->first, it->second);
		}
	}
}
NEventNode::NEventNode(string name) {
	this->name = name;
}
string NEventNode::toString() {
	stringstream ss;
	ss << "(name:" << name << ",period:" << toFEventPeriod(this->period) << ")";
	return ss.str();
}
bool NEventNode::match(ListenerCfg* lc) {
	if (this->name == lc->name) {
		return lc->isNotice;
	} else {
		return false;
	}
}
string FEventNode::toString() {
	stringstream ss;
	ss << "(locf:" << locf << ",netf:" << netf;
	ss << ",type:" << toFEventType(type) << ",period:"
			<< toFEventPeriod(this->period);
	ss << ",uloc:" << toFEventLocation(uloc) << ")";
	return ss.str();
}
void FEventNode::add2Caller(ExtCaller* caller) {
	EventNode::add2Caller(caller);
	if (this->locf.size()) {
		caller->addEnv("LOCF", this->locf);
	}
	if (this->locfn.size()) {
		caller->addEnv("LOCFN", this->locfn);
	}
	if (this->netf.size()) {
		caller->addEnv("NETF", this->locf);
	}
	if (this->netfn.size()) {
		caller->addEnv("NETFN", this->locf);
	}
	caller->addEnv("FEventType", toFEventType(this->type));
	caller->addEnv("FEventPeriod", toFEventPeriod(this->period));
	caller->addEnv("FEventLocation", toFEventLocation(this->uloc));
}
bool FEventNode::match(ListenerCfg* lc) {
	if (!lc->isNormal || lc->isSynced) {
		return false;
	}
	string tf = "", tn = "";
	switch (this->uloc) {
	case FEL_LOC: {
		tf = this->locf;
		tn = this->locfn;
		break;
	}
	case FEL_NET: {
		tf = this->netf;
		tn = this->netfn;
	}
		break;
	default:
		return false;
	}
	string ttype;
	switch (this->type) {
	case FET_DELETE:
		ttype = "DELETE";
		break;
	case FET_NEW:
		ttype = "NEW";
		break;
	case FET_UPDATE:
		ttype = "UPDATE";
		break;
	default:
		return false;
	}
	bool tmatch = false;
	for (size_t i = 0; i < lc->types.size(); i++) {
		if (lc->types[i] == ttype) {
			tmatch = true;
			break;
		}
	}
	if (!tmatch) {
		return false;
	}
	try {
		vector<string>::iterator fit, fend;
		fit = lc->files.begin(), fend = lc->files.end();
		for (; fit != fend; fit++) {
			regex expr(*fit);
			if (boost::regex_match(tn, expr)) {
				return true;
			}
			if (boost::regex_match(tf, expr)) {
				return true;
			}
		}
	} catch (...) {
		printf("match file error in listener:%s\n", lc->name.c_str());
	}
	return false;
}
string SEventNode::toString() {
	return "SEventNode";
}
void SEventNode::add2Caller(ExtCaller* caller) {
	string val;
	val = this->toString(this->locu);
	if (val.size()) {
		caller->addEnv("LOCU", val);
	}
	val = this->toString(this->locd);
	if (val.size()) {
		caller->addEnv("LOCD", val);
	}
	val = this->toString(this->netu);
	if (val.size()) {
		caller->addEnv("NETU", val);
	}
	val = this->toString(this->netd);
	if (val.size()) {
		caller->addEnv("NETD", val);
	}
}
bool SEventNode::match(ListenerCfg* lc) {
	if (!lc->isSynced) {
		return false;
	}
	try {
		vector<string>::iterator fit, fend, fit2, fend2;
		fit = lc->files.begin(), fend = lc->files.end();
		string tn;
		for (; fit != fend; fit++) {
			regex expr(*fit);
			if (lc->isNew || lc->isUpdate) {
				fit2 = this->locu.begin();
				fend2 = this->locu.end();
				for (; fit2 != fend2; fit2++) {
					tn = *fit2;
					if (boost::regex_match(tn, expr)) {
						return true;
					}
				}
				fit2 = this->netu.begin();
				fend2 = this->netu.end();
				for (; fit2 != fend2; fit2++) {
					tn = *fit2;
					if (boost::regex_match(tn, expr)) {
						return true;
					}
				}
			}
			if (lc->isDelete) {
				fit2 = this->locd.begin();
				fend2 = this->locd.end();
				for (; fit2 != fend2; fit2++) {
					tn = *fit2;
					if (boost::regex_match(tn, expr)) {
						return true;
					}
				}
				fit2 = this->netd.begin();
				fend2 = this->netd.end();
				for (; fit2 != fend2; fit2++) {
					tn = *fit2;
					if (boost::regex_match(tn, expr)) {
						return true;
					}
				}
			}
		}
	} catch (...) {
		printf("match file error in listener:%s\n", lc->name.c_str());
	}
	return false;
}
string SEventNode::toString(vector<string> vals) {
	if (vals.empty()) {
		return "";
	}
	stringstream sdata;
	sdata << vals[0];
	for (size_t i = 1; i < vals.size(); i++) {
		sdata << "," << vals[1];
	}
	return sdata.str();
}
/*
 * static fields.
 */
static vector<EventMgr*> _em_demons;
EventMgr::EventMgrId EventMgr::createDemo(EventCfg *cfg) {
	EventMgr *em = new EventMgr(cfg);
	_em_demons.push_back(em);
	return _em_demons.size();
}
void EventMgr::fre() {
	while (_em_demons.size()) {
		vector<EventMgr*>::iterator it;
		it = _em_demons.begin();
		EventMgr* em = *it;
		_em_demons.erase(it);
		delete em;
	}
}
void EventMgr::freDemo(EventMgrId id) {
	if (id > 0 && id <= _em_demons.size()) {
		EventMgr* em = _em_demons[id - 1];
		_em_demons.erase(_em_demons.begin() + (id - 1));
		delete em;
	}
}
EventMgr* EventMgr::demo(EventMgr::EventMgrId id) {
	if (id > 0 && id <= _em_demons.size()) {
		return _em_demons[id - 1];
	} else {
		return 0;
	}
}
EventMgr::EventMgr(EventCfg *cfg) :
		log(C_LOG("EventMgr")) {
			assert(cfg&&cfg->valid);
			this->cfg = cfg;
			this->stopped=1;
			this->thr =0;
			this->running=0;
			this->cfg =cfg;
			this->valid=this->cfg->valid;
			this->msg=this->cfg->msg;
			if(!this->valid) {
				return;
			}
			this->thr = new boost::thread(boost::bind(&EventMgr::run, this));
		}
void EventMgr::run() {
	{
		boost::mutex::scoped_lock _lock(this->stopped_mutex);
		this->stopped = 0;
	}
	this->log.info("start running the EventMgr,configure path:%s",
			this->cfg->cfgPath.c_str());
	long stime = this->cfg->eventChkTime();
	while (1) {
		this->stopped_mutex.lock();
		if (this->stopped) {
			this->stopped_mutex.unlock();
			break;
		}
		this->stopped_mutex.unlock();
		this->events_mutex.lock();
//		this->log.debug("current event size:%d", this->events.size());
		if (this->events.size() < 1) {
			this->events_mutex.unlock();
//			this->log.debug("not received event");
			bsleep(stime);
			continue;
		}
		EventNode *e = this->events.front();
		this->events.erase(this->events.begin());
		this->events_mutex.unlock();
		ListenerCfg *lcfg = this->listener(e);
		if (lcfg) {
			this->log.info("start run listener:%s", lcfg->name.c_str());
			{
				boost::mutex::scoped_lock _lock(this->running_mutex);
				this->running = new EventRunner(lcfg, e);
				this->running->en = e;
			}
			this->running->run();
			{
				boost::mutex::scoped_lock _lock(this->running_mutex);
				delete this->running;
				this->running = 0;
			}
			this->log.info("completed listener:%s", lcfg->name.c_str());
		} else {
//			this->log.debug("can't find listener:%s", e->toString().c_str());
		}
		delete e;
	}
	this->log.info("the EventMgr will stop,configure path:%s",
			this->cfg->cfgPath.c_str());
	{
		boost::mutex::scoped_lock _lock(this->stopped_mutex);
		this->stopped = 1;
	}
}
ListenerCfg* EventMgr::listener(EventNode* en) {
	map<string, ListenerCfg*>::iterator it, end;
	end = this->cfg->listeners.end();
	for (it = this->cfg->listeners.begin(); it != end; it++) {
		ListenerCfg* lc = it->second;
		if (this->match(en, lc)) {
			return lc;
		}
	}
	return 0;
}
bool EventMgr::match(EventNode* en, ListenerCfg* lc) {
	return en->match(lc);
}
//void EventMgr::postEvent(string name, string type, DataPool::DId did) {
//	EVENTS_LOCK;
//	this->events.push_back(pair<pair<string,string>,DataPool::DId>(pair<string,string>(name,type),did));
//}
void EventMgr::postEvent(string locf, string locfn, string netf, string netfn,
		FEventLocation uloc, FEventType type, FEventPeriod period,
		map<string, string>& kvs) {
	FEventNode *en = new FEventNode();
	en->locf = locf;
	en->locfn = locfn;
	en->netf = netf;
	en->netfn = netfn;
	en->uloc = uloc;
	en->type = type;
	en->period = period;
	en->kvs = kvs;
//	this->log.debug("receive one event:%s", en->toString().c_str());
	EVENTS_LOCK;
	this->events.push_back(en);
}
void EventMgr::postEvent(string name, FEventPeriod period,
		map<string, string>& kvs) {
	NEventNode *en = new NEventNode(name);
	en->period = period;
	en->kvs = kvs;
//	this->log.debug("receive one event:%s", en->toString().c_str());
	EVENTS_LOCK;
	this->events.push_back(en);
}
void EventMgr::postEvent(EventNode* en) {
	EVENTS_LOCK;
	this->events.push_back(en);
}
int EventMgr::popEvent(string locf, string netf, FEventLocation uloc,
		FEventType type, FEventPeriod period) {
	vector<EventNode*>::iterator it;
	int erased = 0;
	EVENTS_LOCK;
	for (it = this->events.begin(); it != this->events.end(); it++) {
		FEventNode* en = dynamic_cast<FEventNode*>(*it);
		if (en == NULL) {
			continue;
		}
		if (locf.size() && locf != en->locf) {
			continue;
		}
		if (netf.size() && netf != en->netf) {
			continue;
		}
		if (type != FET_NONE && type != en->type) {
			continue;
		}
		if (period != FEP_NONE && period != en->period) {
			continue;
		}
		if (uloc != FEL_NONE && uloc != en->uloc) {
			continue;
		}
		this->events.erase(it);
		delete en;
		erased++;
	}
	return erased;
}
int EventMgr::popEvent(string name, FEventPeriod period = FEP_NONE) {
	vector<EventNode*>::iterator it;
	int erased = 0;
	EVENTS_LOCK;
	for (it = this->events.begin(); it != this->events.end(); it++) {
		NEventNode* en = dynamic_cast<NEventNode*>(*it);
		if (en == NULL) {
			continue;
		}
		if (period != FEP_NONE && period != en->period) {
			continue;
		}
		if (name == en->name) {
			continue;
		}
		this->events.erase(it);
		delete en;
		erased++;
	}
	return erased;
}
EventMgr::~EventMgr() {
	log.debug("free EventMgr");
	this->running_mutex.lock();
	if (this->running) {
		this->running_mutex.unlock();
		this->running->stop();
	} else {
		this->running_mutex.unlock();
	}
	this->stopped_mutex.lock();
	if (this->stopped) {
		this->stopped_mutex.unlock();
	} else {
		this->stopped = 1;
		this->stopped_mutex.unlock();
		log.debug("waiting EventMgr thread stop");
		this->thr->join();
	}
	if (this->thr) {
		delete this->thr;
	}
}

} /* namespace centny */
