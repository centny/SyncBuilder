/*
 * NoticeTimer.h
 *
 *  Created on: Apr 26, 2013
 *      Author: Scorpion
 */

#ifndef NOTICETIMER_H_
#define NOTICETIMER_H_
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/smart_ptr.hpp>
#include <string>
#include <set>
#include "../log/LogFactory.h"
using namespace std;
namespace centny {
#define NT_TIMEOUT 300
//
class NoticeTimer;
//
class TimeNoticeable {
	friend class NoticeTimer;
private:
	long remain;
protected:
	long rtime;
public:
	TimeNoticeable() {
		this->rtime = 10000;
		this->remain = 10000;
	}
	TimeNoticeable(long time) {
		this->rtime = time;
		this->remain = time;
	}
	//return true to remove listener from NoticeTimer,or no.
	virtual bool timeout()=0;
	virtual ~TimeNoticeable() {
	}
};
class NoticeTimer {
public:
	static NoticeTimer& defaultNoticeTimer();
	static void fre();
private:
	Log log;
	boost::shared_ptr<boost::asio::deadline_timer> dtimer;
	set<TimeNoticeable*> listeners;
	boost::mutex notice_mutex;
#define NOTICE_LOCK boost::mutex::scoped_lock lock(this->notice_mutex)
public:
	NoticeTimer();
	virtual ~NoticeTimer();
	void handle();
	void reg(TimeNoticeable* rec);
	void rmv(TimeNoticeable* rec);
	void initTimer(boost::asio::io_service& ios);
private:
	void timerHandler(const boost::system::error_code& ec);
};

} /* namespace centny */
#endif /* NOTICETIMER_H_ */
