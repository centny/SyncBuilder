/*
 * SyncDemo.cpp
 *
 *  Created on: Apr 26, 2013
 *      Author: Scorpion
 */

#include "SyncDemo.h"

namespace centny {
namespace SServer {

SyncDemo::SyncDemo(io_service& ios, Demo* d) {
	this->fcm = new FileCmdMgr(d->fucfg);
	this->fss = new SyncServer(ios, d->fport, this->fcm, "FCM");
	this->fcm->setServer(fss);
	this->fss->accept();
	this->scm = new ShellCmdMgr(d->sucfg, d->swcfg);
	this->sss = new SyncServer(ios, d->sport, this->scm, "SCM");
	this->sss->accept();
}

SyncDemo::~SyncDemo() {
	delete this->fcm;
	delete this->fss;
	delete this->scm;
	delete this->sss;
}
static vector<SyncDemo*> _sserve_demoes;
static boost::shared_mutex _ss_mutex;
//
SyncDemo* SyncDemo::createDemo(io_service& ios, Demo* d) {
	SyncDemo *sd = new SyncDemo(ios, d);
	_sserve_demoes.push_back(sd);
	return sd;
}
int SyncDemo::demoes() {
	int size = 0;
	_ss_mutex.lock_shared();
	size = _sserve_demoes.size();
	_ss_mutex.unlock_shared();
	return size;
}
void SyncDemo::fre(SyncDemo* sd) {
	_ss_mutex.lock();
	vector<SyncDemo*>::iterator it, end;
	it = _sserve_demoes.begin();
	end = _sserve_demoes.end();
	if (sd) {
		for (; it != end; it++) {
			SyncDemo *sd2 = *it;
			if (sd2 == sd) {
				break;
			}
		}
		if (it != end) {
			_sserve_demoes.erase(it);
			delete sd;
		}
	} else {
		for (; it != end; it++) {
			SyncDemo *sd2 = *it;
			delete sd2;
		}
		_sserve_demoes.clear();
	}
	_ss_mutex.unlock();
}
} /* namespace SServer */
} /* namespace centny */
