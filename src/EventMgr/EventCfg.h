/*
 * EventCfg.h
 *
 *  Created on: Nov 17, 2012
 *      Author: Scorpion
 */

#ifndef EVENTCFG_H_
#define EVENTCFG_H_

#include "../CfgParser/CfgParser.h"
#include "../Notice/NoticeCenter.h"
namespace centny {
//the listener configure.
class ListenerCfg {
private:
	Log log;
public:
	string name;
	vector<string> types;
	vector<string> files;
	string preCmd;
	string preArg;
	string cmd;
	string arg;
	string postCmd;
	string postArg;
	//
	bool valid;
	string msg;
	string logFile;
	//
	bool isClearLog;
	//
	bool isNotice, isNormal, isSynced, isNew, isUpdate, isDelete;
public:
//	void clearLog();
//	void info(string& log);
	ListenerCfg(map<string, string>& kvs, string& name);
	virtual ~ListenerCfg();
	//
//	bool isNotice();
//	//
//	bool isNormal();
//	//
//	bool isSynced();
//	//
//	bool isNewOrUpdate();
//	//
//	bool isDelete();
};
class EventCfg: public CfgParser {
public:
	map<string, ListenerCfg*> listeners;    //all listeners.
	map<string, ListenerCfg*> notices;
private:
	void initAllEvent();
//	void initNotice(ListenerCfg* lc);
public:
	EventCfg(string& cfgPath);
	EventCfg(basic_istream<char>& cfgPath);
	virtual ~EventCfg();
	ListenerCfg* listener(string& name);
	//the notice listener names.
	string nlistenerNames();
	//the listener names.
	string listenerNames();
	long eventChkTime();
};
class EventNoticeArg: public DataPool::DObj {
public:
	string name;
	vector<string> args;
	virtual void fre();
};
} /* namespace centny */
#endif /* EVENTCFG_H_ */
