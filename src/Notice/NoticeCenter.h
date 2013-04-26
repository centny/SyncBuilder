/*
 * NoticeCenter.h
 *
 *  Created on: Nov 27, 2012
 *      Author: Scorpion
 */

#ifndef NOTICECENTER_H_
#define NOTICECENTER_H_
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/smart_ptr.hpp>
#include <string>
#include "../DataPool/DataPool.h"
#include "../log/LogFactory.h"
#include "NoticeTimer.h"
namespace centny {
using namespace std;
#define HEVENT_TIMEOUT 1000
class Noticeable {
public:
	virtual void receive(string name, DataPool::DId did)=0;
	virtual ~Noticeable();
};
class NoticeCenter: public TimeNoticeable {
public:
	static NoticeCenter& defaultCenter();
	static void fre();
private:
	Log log;
	boost::shared_ptr<boost::asio::deadline_timer> dtimer;
	map<string, vector<Noticeable*> > listeners;
	vector<pair<string, DataPool::DId> > notices;
	boost::mutex notice_mutex;
#define NOTICE_LOCK boost::mutex::scoped_lock lock(this->notice_mutex)
public:
	NoticeCenter();
	virtual ~NoticeCenter();
	//handle all notice,return remain notice count.
	int handle();
	//
	void reg(string name, Noticeable* rec);
	void rmv(string name);
	void postDid(string name, DataPool::DId did = "");
	void postObj(string name, DataPool::DObj* dobj);
//	void initTimer(boost::asio::io_service& ios);
	virtual bool timeout();
};

} /* namespace centny */
#endif /* NOTICECENTER_H_ */
