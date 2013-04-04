/*
 * EventRunner.cpp
 *
 *  Created on: Nov 5, 2012
 *      Author: Scorpion
 */

#include "EventRunner.h"
#include <ExtCaller.h>
#include "EventMgr.h"
namespace centny {
EventRunner::EventRunner(ListenerCfg* lcfg, EventNode *en) :
		log(C_LOG("EventRunner")) {
			this->lcfg=lcfg;
			this->en=en;
			this->caller=new ExtCaller();

		}
void EventRunner::run() {
	if (!lcfg->valid) {
		log.warn("the listener configure is invalid,name:%s",
				lcfg->name.c_str());
		return;
	}
	if (FEP_PRE == this->en->period) {
		if (this->lcfg->preCmd.length() < 1) {
			return;
		}
		this->caller->cmd = this->lcfg->preCmd;
		if (this->lcfg->preArg.length()) {
			this->caller->addArgs(this->lcfg->preArg);
		}
	}
	if (FEP_POST == this->en->period) {
		if (this->lcfg->postCmd.length() < 1) {
			return;
		}
		this->caller->cmd = this->lcfg->postCmd;
		if (this->lcfg->postArg.length()) {
			this->caller->addArgs(this->lcfg->postArg);
		}
	}
	if (FEP_CMD == this->en->period) {
		if (this->lcfg->cmd.length() < 1) {
			return;
		}
		this->caller->cmd = this->lcfg->cmd;
		if (this->lcfg->arg.length()) {
			this->caller->addArgs(this->lcfg->arg);
		}
	}
#ifdef WIN32
	if (this->lcfg->logFile.length()) {
		ios_base::openmode mode = ios::out;
		if (!this->lcfg->isClearLog) {
			mode = mode | ios::app;
		}
		this->cmdlog.open(this->lcfg->logFile.c_str(), mode);
		if (this->cmdlog.is_open()) {
			this->caller->out = &this->cmdlog;
			log.info("using command log file:%s", this->lcfg->logFile.c_str());
		} else {
			log.error("open command log file error:%s",
					this->lcfg->logFile.c_str());
		}
	}
#else
	string clog;
	if (this->lcfg->isClearLog) {
		clog = ">";
	} else {
		clog = ">>";
	}
	this->caller->addArgs(clog + this->lcfg->logFile);
#endif
	this->addEn2Caller(this->en);
	log.info("start execute listener:%s,type:%s", this->lcfg->name.c_str(),
			this->en->toString().c_str());
	this->caller->execute();
#ifdef WIN32
	this->cmdlog.close();
#endif
	log.info("execute listener end:%s,EventNode:%s", this->lcfg->name.c_str(),
			this->en->toString().c_str());
}
void EventRunner::stop() {
	if (this->caller) {
		log.info("terminating the caller,listener(name:%s,EventNode:%s)",
				this->lcfg->name.c_str(), this->en->toString().c_str());
		this->caller->stop();
	}
}
void EventRunner::addEn2Caller(EventNode *en) {
}
EventRunner::~EventRunner() {
	if (this->cmdlog.is_open()) {
		this->cmdlog.close();
	}
	if (this->caller) {
		delete this->caller;
		this->caller = 0;
	}
#if SHOW_FRE_MSG
	printf("free EventRunner\n");
#endif
}

} /* namespace centny */
