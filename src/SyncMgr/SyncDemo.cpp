/*
 * SyncDemo.cpp
 *
 *  Created on: Nov 18, 2012
 *      Author: Scorpion
 */

#include "SyncDemo.h"
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
namespace centny {

SyncDemo::SyncDemo(string ncf, string ecf) :
		log(C_LOG("SyncDemo")) {
			this->db=0;
			this->net=0;
			this->loc=0;
			this->thr=0;
			this->mgr=0;
			this->bcmd=0;
			this->ncf=ncf;
			this->ecf=ecf;
			this->ncfg = new NetCfg(ncf);
			if (!this->ncfg->valid) {
				log.error("create NetCfg error:%s", this->ncfg->msg.c_str());
				assert("create NetCfg error"==0);
			}
			this->ecfg = new EventCfg(ecf);
			if (!this->ecfg->valid) {
				log.error("create EventCfg error:%s", this->ecfg->msg.c_str());
				assert("create EventCfg error"==0);
			}
			assert(SQLITE_OK==sqlite3_open(this->ncfg->dbPath().c_str(),&this->db));
			this->checkDbTable();
			string ntype = this->ncfg->type();
			if ("FTP" == ntype) {
				this->net = new FtpAdapter(this->db, this->ncfg);
			}
			if ("SYNC" == ntype) {
				SyncAdapter *sa = new SyncAdapter(this->db, this->ncfg);
				this->net = sa;
				assert(sa->isLogined());
			}
			assert(this->net);
			this->loc = new LocAdapter(this->db, this->ncfg->locSyncDir());
			this->mid = EventMgr::createDemo(this->ecfg);
			this->mgr = new SyncMgr(this->loc, this->net, this->ncfg, this->mid);
			this->thr=new boost::thread(boost::bind(&SyncDemo::run,this));
			}

SyncDemo::~SyncDemo() {
	if (this->mid) {
		log.debug("free demos");
		EventMgr::freDemo(this->mid);
		this->mid = 0;
	}
	if (this->mgr) {
		log.debug("free SyncManager");
		delete this->mgr;
		this->mgr = 0;
	}
	if (this->thr) {
		log.debug("waiting SyncMgr thread stopped");
		this->thr->join();
		log.info("sync thread already stopped");
		delete this->thr;
		this->thr = 0;
	}
	if (this->net) {
		log.debug("free NetAdapter");
		delete this->net;
		this->net = 0;
	}
	if (this->loc) {
		log.debug("free LocAdapter");
		delete this->loc;
		this->loc = 0;
	}
	if (this->ncfg) {
		delete this->ncfg;
		this->ncfg = 0;
	}
	if (this->ecfg) {
		delete this->ecfg;
		this->ecfg = 0;
	}
	if (this->db) {
		sqlite3_close(this->db);
		this->db = NULL;
	}
}
void SyncDemo::checkDbTable() {
	assert(sqlite3_exec(this->db, CREATE_TABLE, 0, 0, 0)==SQLITE_OK);
}
void SyncDemo::run() {
	this->mgr->sync();
}
void SyncDemo::initBCmd(io_service& ios) {
	this->bcmd = new SyncBindCmd(this->ecfg, this->ncfg, this->net, this->mid,
			ios);
	assert(this->bcmd->binding());
	this->bcmd->startRead();
}
/*
 * static field.
 */
static vector<SyncDemo*> _sync_demoes;
static boost::shared_mutex _sd_mutex;
//
SyncDemo* SyncDemo::createDemo(string ncf, string ecf) {
	SyncDemo *sd = new SyncDemo(ncf, ecf);
	_sync_demoes.push_back(sd);
	return sd;
}
int SyncDemo::demoes() {
	int size = 0;
	_sd_mutex.lock_shared();
	size = _sync_demoes.size();
	_sd_mutex.unlock_shared();
	return size;
}
void SyncDemo::fre(SyncDemo* sd) {
	_sd_mutex.lock();
	vector<SyncDemo*>::iterator it, end;
	it = _sync_demoes.begin();
	end = _sync_demoes.end();
	if (sd) {
		for (; it != end; it++) {
			SyncDemo *sd2 = *it;
			if (sd2 == sd) {
				break;
			}
		}
		if (it != end) {
			_sync_demoes.erase(it);
			delete sd;
		}
	} else {
		for (; it != end; it++) {
			SyncDemo *sd2 = *it;
			delete sd2;
		}
		_sync_demoes.clear();
	}
	_sd_mutex.unlock();
}
} /* namespace centny */
