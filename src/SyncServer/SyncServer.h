/*
 * SyncServer.h
 *
 *  Created on: Nov 30, 2012
 *      Author: Scorpion
 */

#ifndef SYNCSERVER_H_
#define SYNCSERVER_H_
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include "../log/LogFactory.h"
namespace centny {
using namespace boost;
using namespace boost::asio;
using namespace boost::system;
using ip::tcp;
class ConClient;
class CmdBase;
//
class SyncServer {
	friend class ConClient;
private:
	Log log;
	io_service& iosev;
	ip::tcp::acceptor *acceptor;
	vector<boost::shared_ptr<ConClient> > clients;
//public:
	//socket timeout.
	long stimeout;
	//
	CmdBase* cmdb;
	//
	boost::mutex smutex;
	//
	boost::thread_group tgrps;
	//
	//the clear timer.
	boost::shared_ptr<deadline_timer> ctimer;
public:
	SyncServer(io_service& ios, int port, CmdBase* cmd, string name="");
	virtual ~SyncServer();
	boost::shared_ptr<deadline_timer> deadlineTimer();
	long socketTimeout();
	void setSocketTimeout(long sto);
	boost::thread_group& thrGrps();
	void accept();
//	void run();
//	void stop();
	//it will free the client object.
//	void remove(shared_ptr<ConClient> client);
	//
	void acceptHandler(boost::shared_ptr<ConClient> client, const boost::system::error_code& ec);
	static void cfgSocketTimeouts(tcp::socket& socket, long timeout = 10000);
private:
	void ctimerHandler(const boost::system::error_code& ec);
};

} /* namespace centny */
#endif /* SYNCSERVER_H_ */
