/*
 * SyncDemo.cpp
 *
 *  Created on: Apr 26, 2013
 *      Author: Scorpion
 */

#include "SServerSyncDemo.h"

namespace centny {

SServerSyncDemo::SServerSyncDemo(io_service& ios, SServerDemo* d) {
	this->fcm = new FileCmdMgr(d->fucfg);
	this->fss = new SyncServer(ios, d->fport, this->fcm, "FCM");
	this->fcm->setServer(fss);
	this->fss->accept();
	this->scm = new ShellCmdMgr(d->sucfg, d->swcfg);
	this->sss = new SyncServer(ios, d->sport, this->scm, "SCM");
	this->sss->accept();
}

SServerSyncDemo::~SServerSyncDemo() {
	delete this->fcm;
	delete this->fss;
	delete this->scm;
	delete this->sss;
}
static vector<SServerSyncDemo*> _sserve_demoes;
static boost::shared_mutex _ss_mutex;
//
SServerSyncDemo* SServerSyncDemo::createDemo(io_service& ios, SServerDemo* d) {
	SServerSyncDemo *sd = new SServerSyncDemo(ios, d);
	_sserve_demoes.push_back(sd);
	return sd;
}
int SServerSyncDemo::demoes() {
	int size = 0;
	_ss_mutex.lock_shared();
	size = _sserve_demoes.size();
	_ss_mutex.unlock_shared();
	return size;
}
void SServerSyncDemo::fre(SServerSyncDemo* sd) {
	_ss_mutex.lock();
	vector<SServerSyncDemo*>::iterator it, end;
	it = _sserve_demoes.begin();
	end = _sserve_demoes.end();
	if (sd) {
		for (; it != end; it++) {
			SServerSyncDemo *sd2 = *it;
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
			SServerSyncDemo *sd2 = *it;
			delete sd2;
		}
		_sserve_demoes.clear();
	}
	_ss_mutex.unlock();
}
} /* namespace centny */
