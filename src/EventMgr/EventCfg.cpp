/*
 * EventCfg.cpp
 *
 *  Created on: Nov 17, 2012
 *      Author: Scorpion
 */

#include "EventCfg.h"
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
namespace centny {
//ListenerCfg implementation.
ListenerCfg::ListenerCfg(map<string, string>& kvs, string& name) :
		log(C_LOG("ListenerCfg")) {
			log.info("creating listener by name:%s",name.c_str());
			this->isClearLog = false;
			this->logFile = "";
			this->valid = false;
			if (name.size() < 1) {
				this->msg = "the "+name+" event name is empty";
				return;
			}
			this->name=name;
			string val, key;
			/*
			 *types
			 */
			key = "EN_" + name;
			val = kvs[key];
			if (val.size() < 1) {
				this->msg = "the "+name+" event types is empty";
				return;
			}
			boost::split(this->types, val, is_any_of("|"));
			/*
			 * files
			 */
			key = name + "_FILES";
			val = kvs[key];
			if (val.size()) {
//				this->msg = "the "+name+" event files is empty";
				boost::split(this->files, val, is_any_of("&"));
			}
			/*
			 * event command and parameters.
			 */
			this->preCmd = kvs[(name + "_PRE_CMD")];
			this->preArg = kvs[(name + "_PRE_ARG")];
			this->cmd = kvs[(name + "_CMD")];
			this->arg = kvs[(name + "_ARG")];
			this->postCmd = kvs[(name + "_POST_CMD")];
			this->postArg = kvs[(name + "_POST_ARG")];
			/*
			 * log
			 */
			this->logFile = kvs[(name + "_LOG")];
			this->isClearLog = ("YES" == kvs[(name + "_LOG_CLEAR")]);
			this->valid=true;
			this->isDelete=this->isNew=this->isNormal=this->isNotice=this->isSynced=this->isUpdate=0;
			vector<string>::iterator it, end;
			for (it = this->types.begin(), end = this->types.end(); it != end; it++) {
				if ("NOTICE" == *it) {
					this->isNotice=1;
				} else if("SYNCED"==*it) {
					this->isSynced=1;
				} else if("NEW"==*it) {
					this->isNew=1;
					this->isNormal=1;
				} else if("UPDATE"==*it) {
					this->isUpdate=1;
					this->isNormal=1;
				} else if("DELETE"==*it) {
					this->isDelete=1;
					this->isNormal=1;
				}
			}
		}
ListenerCfg::~ListenerCfg() {
}
void EventCfg::initAllEvent() {
	//check the all event configure.
	map<string, string>::iterator it;
	for (it = this->kvs.begin(); it != this->kvs.end(); it++) {
		string en = it->first;
		if (en.size() < 4) {
			continue;
		}
		if ("EN_" == en.substr(0, 3)) {
			string name = en.substr(3);
			ListenerCfg *lc = new ListenerCfg(this->kvs, name);
			if (lc->valid) {
				if (lc->isNormal) {
					this->listeners[name] = lc;
				}
				if (lc->isNotice) {
					this->notices[name] = lc;
				}
			} else {
				log.error(lc->msg.c_str());
				delete lc;
			}
		}
	}
}
//void EventCfg::initNotice(ListenerCfg* lc) {
//	NoticeCenter::defaultCenter().reg("EN_" + lc->name, this);
//}
EventCfg::EventCfg(string& cfgPath) :
		CfgParser(cfgPath) {
	this->initAllEvent();
}
EventCfg::EventCfg(basic_istream<char>& cfgStream) :
		CfgParser(cfgStream) {
	this->initAllEvent();
}
EventCfg::~EventCfg() {
	map<string, ListenerCfg*>::iterator it, end;
	set<ListenerCfg*> ls;
	for (it = this->listeners.begin(), end = this->listeners.end(); it != end;
			it++) {
		ls.insert(it->second);
	}
	this->listeners.clear();
	for (it = this->notices.begin(), end = this->notices.end(); it != end;
			it++) {
		NoticeCenter::defaultCenter().rmv("EN_" + it->first);
		ls.insert(it->second);
	}
	this->notices.clear();
	set<ListenerCfg*>::iterator sit, send;
	sit = ls.begin(), send = ls.end();
	for (; sit != send; sit++) {
		delete (*sit);
	}
}
ListenerCfg* EventCfg::listener(string& name) {
	CFG_SLOCK;
	return this->listeners[name];
}
string EventCfg::nlistenerNames() {
	CFG_SLOCK;
	map<string, ListenerCfg*>::iterator it,end;
	it=this->listeners.begin(),end=this->listeners.end();
	stringstream sdata;
	for(;it!=end;it++) {
		if(it->second->isNotice) {
			sdata<<(it->first)<<"\r\n";
		}
	}
	return sdata.str();
}
string EventCfg::listenerNames() {
	CFG_SLOCK;
	map<string, ListenerCfg*>::iterator it,end;
	it=this->listeners.begin(),end=this->listeners.end();
	stringstream sdata;
	for(;it!=end;it++) {
		sdata<<(it->first)<<"\r\n";
	}
	return sdata.str();
}
long EventCfg::eventChkTime() {
	string st = this->kvs["EVENT_CHK_TIME"];
	long stime;
	if (st.length()) {
		stime = atol(st.c_str());
	} else {
		stime = 500;
	}
	if (stime < 300) {
		stime = 300;
	}
	return stime;
}
void EventNoticeArg::fre() {
	delete this;
}
} /* namespace centny */
