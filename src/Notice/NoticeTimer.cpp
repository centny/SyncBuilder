/*
 * NoticeTimer.cpp
 *
 *  Created on: Apr 26, 2013
 *      Author: Scorpion
 */

#include "NoticeTimer.h"

namespace centny {
static boost::mutex _defaultNoticeTimerMutex;
static NoticeTimer* _defaultNoticeTimer = 0;
NoticeTimer& NoticeTimer::defaultNoticeTimer() {
	boost::mutex::scoped_lock lock(_defaultNoticeTimerMutex);
	if (_defaultNoticeTimer == 0) {
		_defaultNoticeTimer = new NoticeTimer();
	}
	return *_defaultNoticeTimer;
}
void NoticeTimer::fre() {
	boost::mutex::scoped_lock lock(_defaultNoticeTimerMutex);
	if (_defaultNoticeTimer) {
		delete _defaultNoticeTimer;
		_defaultNoticeTimer = 0;
	}
}
NoticeTimer::NoticeTimer() :
		log(C_LOG("NoticeTimer")) {
		}

NoticeTimer::~NoticeTimer() {
	this->dtimer->cancel();
}
void NoticeTimer::handle() {
	this->notice_mutex.lock();
	if (this->listeners.empty()) {
		this->notice_mutex.unlock();
		return;
	}
	set<TimeNoticeable*>::iterator it, end;
	it = this->listeners.begin();
	end = this->listeners.end();
	TimeNoticeable* nt;
	for (; it != end; it++) {
		nt = *it;
		nt->remain -= NT_TIMEOUT;
		if (nt->remain > 0) {
			continue;
		}
		bool res = nt->timeout();
		if (res) {
			this->listeners.erase(it);
		} else {
			nt->remain = nt->rtime;
		}
	}
	this->notice_mutex.unlock();
}
void NoticeTimer::reg(TimeNoticeable* rec) {
	NOTICE_LOCK;
	this->listeners.insert(rec);
}
void NoticeTimer::rmv(TimeNoticeable* rec) {
	NOTICE_LOCK;
	this->listeners.erase(rec);
}
void NoticeTimer::initTimer(boost::asio::io_service& ios) {
	this->dtimer = boost::shared_ptr<boost::asio::deadline_timer>(
			new boost::asio::deadline_timer(ios));
	this->dtimer->expires_from_now(boost::posix_time::milliseconds(NT_TIMEOUT));
	this->dtimer->async_wait(boost::bind(&NoticeTimer::timerHandler, this, _1));

}
void NoticeTimer::timerHandler(const boost::system::error_code& ec) {
	if (ec.value() == ECANCELED) {
		return;
	}
//	this->log.debug("handling event");
	this->handle();
	this->dtimer->expires_from_now(boost::posix_time::milliseconds(NT_TIMEOUT));
	this->dtimer->async_wait(boost::bind(&NoticeTimer::timerHandler, this, _1));
}
} /* namespace centny */
