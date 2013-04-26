/*
 * SyncMgr.cpp
 *
 *  Created on: Nov 14, 2012
 *      Author: Scorpion
 */

#include "SyncMgr.h"
#include <boost/filesystem.hpp>

namespace centny {
namespace fs = boost::filesystem;
SyncMgr::SyncMgr(AdapterBase *loc, NetAdapterBase *net, NetCfg *cfg,
		EventMgr::EventMgrId emi) :
		log(C_LOG("SyncMgr")) {
			assert(loc&&net&&cfg&&emi);
			this->loc = loc;
			this->net = net;
			this->cfg = cfg;
			this->emi = emi;
			this->emgr=EventMgr::demo(this->emi);
			assert(this->emgr);
			this->stopped = 1;
			this->ctime = this->cfg->syncChkTime();
			this->isDownload = this->cfg->isDownload();
			this->isUpload = this->cfg->isUpload();
			this->noticed = false;
		}
SyncMgr::~SyncMgr() {
	log.info("sending stop sync mutex");
	this->stop_mutex.lock();
	if (!this->stopped) {
		this->stopped = 1;
	}
	this->stop_mutex.unlock();
}
void SyncMgr::sync() {
	this->stopped = 0;
	assert(ctime>=1000);
	if (!this->cfg->isNoticeSync && this->cfg->isTimeSync) {
		this->log.error(
				"the specified NetCfg is not time or notice sync,sync will not start");
		return;
	}
	NoticeCenter::defaultCenter().reg(this->cfg->syncChkNotice(), this);
	FInfo *locf = 0;
	NetFInfo *netf = 0;
	this->isUpload = this->isUpload && this->net->uploadSupported();
	this->isDownload = this->isDownload && this->net->downloadSupported();
	this->log.info("start running sync(up:%d,down:%d)"
			" by sync type (time:%d,notice:%d)", this->isUpload,
			this->isDownload, this->cfg->isTimeSync, this->cfg->isNoticeSync);
	long remain = 0;
	bool run_sync = false;
	while (1) {
		this->stop_mutex.lock_shared();
		if (this->stopped) {
			this->stop_mutex.unlock_shared();
			break;
		}
		this->stop_mutex.unlock_shared();
		run_sync = false;
		if (this->cfg->isNoticeSync) {
			this->notice_mutex.lock_shared();
			run_sync = this->noticed;
			this->noticed = false;
			this->notice_mutex.unlock_shared();
		}
		if (!run_sync) {
			if (!this->cfg->isTimeSync) {
				bsleep(500);
				continue;
			}
			if (remain > 0) {
				bsleep(500);
				remain -= 500;
				continue;
			}
		}
		if (this->net->netstate != 200) {
			try {
				log.error("the NetAdapter state is:%d,it will be reconnect",
						this->net->netstate);
				if (!this->net->reinit()) {
					remain = ctime;
					continue;
				}
			} catch (...) {
				log.error("reconnect faild,it will try again");
			}
		}
		try {
			locf = this->loc->root();
			netf = (NetFInfo*) this->net->root();
			this->sync(locf, netf);
		} catch (const char* e) {
			log.error("sync error:%s", e);
		}
		remain = ctime;
	}
	this->log.info("sync end");
}
//void SyncMgr::remove(FInfo * tf, vector<FInfo*>& fis) {
//	if (fis.size() < 1) {
//		return;
//	}
//	vector<FInfo*>::iterator it, end;
//	end = fis.end();
//	for (it = fis.begin(); it != end; it++) {
//		tf->remove((*it)->name);
//	}
//	log.debug("remove %ld items", fis.size());
//}
void SyncMgr::receive(string name, DataPool::DId did) {
	this->notice_mutex.lock();
	this->noticed = true;
	this->notice_mutex.unlock();
}
void SyncMgr::sync(FInfo* locf, NetFInfo* netf) {
	this->syncRemoved(locf, netf);
	if (this->isUpload) {
		log.debug("sync upload (%s,%s)", locf->absUrl().c_str(),
				netf->absUrl().c_str());
		this->syncUp(locf, netf);
	}
	if (this->isDownload) {
		log.debug("sync download (%s,%s)", netf->absUrl().c_str(),
				locf->absUrl().c_str());
		this->syncDown(netf, locf);
	}
}
void SyncMgr::syncRemoved(FInfo* locf, NetFInfo* netf) {
	locf->subs();
	netf->subs();
	set<pair<string, string> > rsubs;
	set<pair<string, string> >::iterator rit, rend;
	//remove.
	rsubs = netf->removedSubs();
	if (!rsubs.empty()) {
		rit = rsubs.begin(), rend = rsubs.end();
		for (; rit != rend; rit++) {
			if (this->isDownload) {
				locf->remove(rit->first);
			}
			loc->delDbParent(rit->second);
			net->delDbParent(rit->second);
		}
		locf->refreshSubs();
	}
	//remove.
	rsubs = locf->removedSubs();
	if (!rsubs.empty()) {
		set<pair<string, string> >::iterator rit, rend;
		rit = rsubs.begin(), rend = rsubs.end();
		for (; rit != rend; rit++) {
			if (this->isUpload) {
				netf->remove(rit->first);
			}
			net->delDbParent(rit->second);
			loc->delDbParent(rit->second);
		}
		netf->refreshSubs();
	}
}
void SyncMgr::syncUp(FInfo* locf, NetFInfo* netf) {
	vector<FInfo*>& lsubs = locf->subs();
	vector<FInfo*>::iterator it;
	vector<FInfo*>::iterator end;
	//loop for sync.
	NetFInfo *n;
	for (it = lsubs.begin(), end = lsubs.end(); it != end; it++) {
		FInfo* lit = *it;
		if (lit->isDir()) {
			if (lit->isNewOrUpdated()) {
				this->log.debug("create remote directory:%s",
						lit->name.c_str());
				netf->mkdir(lit->name);
				netf->refreshSubs();
				n = (NetFInfo*) netf->contain(lit->name);
				if (n) {
					lit->update2Normal();
					n->update2Normal();
					//update next directory.
					this->sync(lit, n);
				} else {
					log.error("create remote folder %s error in path %s",
							lit->name.c_str(), netf->absUrl().c_str());
				}
			} else {
				n = (NetFInfo*) netf->contain(lit->name);
				if (n) {
					//update next directory.
					this->sync(lit, n);
				} else {
					log.error("find remote folder %s error in path %s",
							lit->name.c_str(), netf->absUrl().c_str());
				}
			}
		} else {
			if (lit->isNew()) {
				this->log.debug("start upload:%s", lit->absUrl().c_str());
				emgr->postEvent(lit->absUrl(), lit->name,
						netf->absUrl() + "/" + lit->name, lit->name, FEL_NET,
						FET_NEW, FEP_PRE);
				netf->upload(lit->absUrl(), lit->name);
				netf->refreshSubs();
				n = (NetFInfo*) netf->contain(lit->name);
				if (n) {
					n->update2Normal();
				} else {
					this->log.error("upload:%s error", lit->absUrl().c_str());
				}
				emgr->postEvent(lit->absUrl(), lit->name,
						netf->absUrl() + "/" + lit->name, lit->name, FEL_NET,
						FET_NEW, FEP_POST);
			} else if (lit->isUpdated()) {
				this->log.debug("start upload:%s", lit->absUrl().c_str());
				emgr->postEvent(lit->absUrl(), lit->name,
						netf->absUrl() + "/" + lit->name, lit->name, FEL_NET,
						FET_UPDATE, FEP_PRE);
				netf->upload(lit->absUrl(), lit->name);
				netf->refreshSubs();
				n = (NetFInfo*) netf->contain(lit->name);
				if (n) {
					n->update2Normal();
				} else {
					this->log.error("upload:%s error", lit->absUrl().c_str());
				}
				emgr->postEvent(lit->absUrl(), lit->name,
						netf->absUrl() + "/" + lit->name, lit->name, FEL_NET,
						FET_UPDATE, FEP_POST);
			}
		}
	}
}
//void SyncMgr::syncUpNetSub(FInfo* locf, NetFInfo* parent) {
//	vector<FInfo*>& nsubs = parent->subs();
//	vector<FInfo*>::iterator it, end = nsubs.end();
//	NetFInfo *n;
//	for (it = nsubs.begin(); it != end; it++) {
//		n = (NetFInfo*) *it;
//		if (locf->name != n->name) {
//			continue;
//		}
//		if (locf->type != n->type) {
//			continue;
//		}
//		if (locf->type == 'd') {
//			//update next directory.
//			this->syncUp(locf, n);
//			return;
//		}
//		if (locf->mtime <= n->mtime) {
//			return;
//		}
//
//		return;
//	}
//	//add new file.
//	if (locf->type == 'd') {
//
//	} else {
//
//	}
//}
void SyncMgr::syncDown(NetFInfo* netf, FInfo* locf) {
	vector<FInfo*>& nsubs = netf->subs();
	vector<FInfo*>::iterator it;
	vector<FInfo*>::iterator end;
	//loop for sync.
	FInfo* n;
	for (it = nsubs.begin(), end = nsubs.end(); it != end; it++) {
		NetFInfo *nit = (NetFInfo *) *it;
		if (nit->isDir()) {
			if (nit->isNewOrUpdated()) {
				this->log.debug("create remote directory:%s",
						nit->name.c_str());
				locf->mkdir(nit->name);
				locf->refreshSubs();
				n = locf->contain(nit->name);
				if (n) {
					nit->update2Normal();
					n->update2Normal();
					//update next directory.
					this->sync(n, nit);
				} else {
					log.error("create local folder %s error in path %s",
							nit->name.c_str(), locf->absUrl().c_str());
				}
			} else {
				n = locf->contain(nit->name);
				if (n && n->isDir()) {
					//update next directory.
					this->sync(n, nit);
				} else {
					log.error("find local folder %s error in path %s",
							nit->name.c_str(), locf->absUrl().c_str());
				}
			}
		} else {
			if (nit->isNew()) {
				string lf = locf->absUrl();
				if (locf->parent) {
					lf += F_SEQ;
				}
				lf += nit->name;
				log.debug("donwload %s to %s", nit->absUrl().c_str(),
						lf.c_str());
				emgr->postEvent(nit->absUrl(), nit->name, lf, nit->name,
						FEL_LOC, FET_NEW, FEP_PRE);
				nit->download(lf);
				locf->refreshSubs();
				nit->update2Normal();
				n = locf->contain(nit->name);
				if (n) {
					n->update2Normal();
				} else {
					log.error("donwload %s to %s error", nit->absUrl().c_str(),
							lf.c_str());
				}
				emgr->postEvent(nit->absUrl(), nit->name, lf, nit->name,
						FEL_LOC, FET_NEW, FEP_POST);
			} else if (nit->isUpdated()) {
				string lf = locf->absUrl();
				if (locf->parent) {
					lf += F_SEQ;
				}
				lf += nit->name;
				log.debug("donwload %s to %s", nit->absUrl().c_str(),
						lf.c_str());
				emgr->postEvent(nit->absUrl(), nit->name, lf, nit->name,
						FEL_LOC, FET_UPDATE, FEP_PRE);
				nit->download(lf);
				locf->refreshSubs();
				nit->update2Normal();
				n = locf->contain(nit->name);
				if (n) {
					n->update2Normal();
				} else {
					log.error("donwload %s to %s error", nit->absUrl().c_str(),
							lf.c_str());
				}
				emgr->postEvent(nit->absUrl(), nit->name, lf, nit->name,
						FEL_LOC, FET_UPDATE, FEP_POST);
			}
		}
	}
}
//void SyncMgr::syncDownLocSub(NetFInfo* netf, FInfo* parent) {
//	vector<FInfo*>& lsubs = parent->subs();
//	vector<FInfo*>::iterator it, end;
//	FInfo *n;
//	for (it = lsubs.begin(), end = lsubs.end(); it != end; it++) {
//		n = *it;
//		if (netf->name != n->name) {
//			continue;
//		}
//		if (netf->type != n->type) {
//			continue;
//		}
//		if (netf->type == 'd') {
//			//update next directory.
//			this->syncDown(netf, n);
//			return;
//		}
//		if (netf->mtime <= n->mtime) {
//			return;
//		}
//
//		return;
//	}
//	//add new file.
//	if (netf->type == 'd') {
//
//	} else {
//
//	}
//}
//void SyncMgr::syncRemove(FInfo* locf, NetFInfo* netf) {
//	netf->subs();
//	vector<FInfo*>& lsubs = locf->subs();
//	if (netf->netstate() != 200) {
//		throw "network error";
//	}
//	vector<FInfo*>::iterator it;
//	vector<FInfo*>::iterator end;
//	//loop for remove.
//	for (it = lsubs.begin(), end = lsubs.end(); it != end; it++) {
//		if ((*it)->isShouldRemoved((*it)->cwd)) {
//			if ((*it)->parent->remove((*it)->name)) {
//				locf->delShouldRemoved((*it)->cwd);
//			}
//			continue;
//		}
//		if (locf->isRemoved((*it)->cwd)) {
//			if (!(*it)->parent->remove((*it)->name)) {
//				locf->addShouldRemoved((*it)->cwd);
//			}
//			continue;
//		}
//	}
//	locf->refreshSubs();
//}
//void SyncMgr::syncRemove(NetFInfo* netf, FInfo* locf) {
//	vector<FInfo*>& nsubs = netf->subs();
//	locf->subs();
//	if (netf->netstate() != 200) {
//		throw "network error";
//	}
//	vector<FInfo*>::iterator it;
//	vector<FInfo*>::iterator end;
//	//loop for remove.
//	for (it = nsubs.begin(), end = nsubs.end(); it != end; it++) {
//		if ((*it)->isShouldRemoved((*it)->cwd)) {
//			if ((*it)->parent->remove((*it)->name)) {
//				netf->delShouldRemoved((*it)->cwd);
//			}
//			continue;
//		}
//		if (locf->isRemoved((*it)->cwd)) {
//			if (!(*it)->parent->remove((*it)->name)) {
//				netf->addShouldRemoved((*it)->cwd);
//			}
//			continue;
//		}
//	}
//	netf->refreshSubs();
//}
} /* namespace centny */
