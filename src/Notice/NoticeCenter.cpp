/*
 * NoticeCenter.cpp
 *
 *  Created on: Nov 27, 2012
 *      Author: Scorpion
 */

#include "NoticeCenter.h"

namespace centny {
Noticeable::~Noticeable() {

}
//
static boost::mutex _defaultCenterMutex;
static NoticeCenter* _defaultCenter = 0;
NoticeCenter& NoticeCenter::defaultCenter() {
	boost::mutex::scoped_lock lock(_defaultCenterMutex);
	if (_defaultCenter == 0) {
		_defaultCenter = new NoticeCenter();
	}
	return *_defaultCenter;
}
void NoticeCenter::fre() {
	if (_defaultCenter) {
		delete _defaultCenter;
		_defaultCenter = 0;
	}
}
NoticeCenter::NoticeCenter() :
		log(C_LOG("NoticeCenter")) {
			this->log.debug("initial one NoticeCenter");
		}

NoticeCenter::~NoticeCenter() {
}
int NoticeCenter::handle() {
	this->notice_mutex.lock();
	if (this->notices.size() < 1) {
		this->notice_mutex.unlock();
		return 0;
	}
	this->log.debug("handle %d notice", this->notices.size());
	pair<string, DataPool::DId> n = this->notices.front();
	this->notices.erase(this->notices.begin());
	size_t rsize = this->notices.size();
	this->notice_mutex.unlock();
	vector<Noticeable*>& ns = this->listeners[n.first];
	this->log.debug("found %d listener for '%s' notice", ns.size(),
			n.first.c_str());
	if (ns.size() < 1) {
		return rsize;
	}
	vector<Noticeable*>::iterator it, end;
	end = ns.end();
	for (it = ns.begin(); it != end; it++) {
		(*it)->receive(n.first, n.second);
	}
	DataPool::sharedPool().fre(n.first);
	return rsize;
}
//
void NoticeCenter::reg(string name, Noticeable* rec) {
	NOTICE_LOCK;
	vector<Noticeable*> ns = this->listeners[name];
	ns.push_back(rec);
	this->listeners[name]=ns;
	this->log.debug("register notice listener:%s",name.c_str());
}
void NoticeCenter::rmv(string name) {
	NOTICE_LOCK;
	this->listeners.erase(name);
	this->log.debug("remove notice listener:%s",name.c_str());
}
void NoticeCenter::postDid(string name, DataPool::DId did) {
	NOTICE_LOCK;
	this->notices.push_back(pair<string,DataPool::DId>(name,did));
	this->log.debug("receive post notice:%s",name.c_str());
}
void NoticeCenter::postObj(string name, DataPool::DObj* dobj) {
	NOTICE_LOCK;
	DataPool::DId did="";
	if(dobj) {
		did=DataPool::sharedPool().add(dobj);
	}
	this->notices.push_back(pair<string,DataPool::DId>(name,did));
	this->log.debug("receive post notice:%s",name.c_str());
}
void NoticeCenter::initTimer(boost::asio::io_service& ios) {
	this->dtimer = boost::shared_ptr<boost::asio::deadline_timer>(
			new boost::asio::deadline_timer(ios));
	this->dtimer->expires_from_now(
			boost::posix_time::milliseconds(HEVENT_TIMEOUT));
	this->dtimer->async_wait(
			boost::bind(&NoticeCenter::timerHandler, this, _1));

}
void NoticeCenter::timerHandler(const boost::system::error_code& ec) {
	if (ec.value() == ECANCELED) {
		return;
	}
//	this->log.debug("handling event");
	this->handle();
	this->dtimer->expires_from_now(
			boost::posix_time::milliseconds(HEVENT_TIMEOUT));
	this->dtimer->async_wait(
			boost::bind(&NoticeCenter::timerHandler, this, _1));
}
//
} /* namespace centny */
