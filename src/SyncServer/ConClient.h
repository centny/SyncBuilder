/*
 * ConClient.h
 *
 *  Created on: Nov 30, 2012
 *      Author: Scorpion
 */

#ifndef CONCLIENT_H_
#define CONCLIENT_H_
#include <iostream>
#include <string>
#include <sstream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include "SyncServer.h"
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/array.hpp>
#include "../Common/ReplaceAll.h"
#include "../log/LogFactory.h"
#include "../common.h"
#include "../Common/Cmd.h"
//#include "CmdBase.h"
namespace centny {
using namespace std;
using namespace boost;
using namespace boost::asio;
using namespace boost::system;
using ip::tcp;
class SyncServer;
class CmdBase;
//

//
class ConClient {
public:
	friend class SyncServer;
public:
	class ScopedLock {
	private:
		ConClient* client;
	public:
		ScopedLock(ConClient* c) {
			this->client = c;
			this->client->lock();
		}
		~ScopedLock() {
			this->client->unlock();
			this->client = 0;
		}
	};
public:
	boost::shared_ptr<tcp::socket> psocket;
private:
	Log log;
	boost::asio::streambuf buf;
	boost::system::error_code ec;
	char cbuf[BUF_SIZE];
	CmdBase* cmdb;
	boost::shared_ptr<deadline_timer> rtimer;
//	boost::shared_ptr<deadline_timer> wtimer;
	SyncServer& server;
	//
	string cname;
	string csession;
	boost::shared_ptr<ConClient> csp;
	//
	//
	bool inited;
	//if locked.
	bool locked;
	boost::mutex lmutex;
#define LM_LOCK	boost::mutex::scoped_lock lmlock(this->lmutex)
public:
	ConClient(SyncServer& server, CmdBase* cmd);
	virtual ~ConClient();
	//
	void setName(string n);
	string name();
	//
	void setSession(string n);
	string session();
	//
	void setSp(boost::shared_ptr<ConClient>& sp);
	boost::shared_ptr<ConClient>& sp();
	//
	SyncServer& tServer();
	//it will free this object.
//	void fre();
	//
	bool isInited();
	//
	bool isLocked();
	void lock();
	void unlock();
	void shutdown();
	void startRead();
	int asyncWrite(const char* data, size_t len);
	int syncWrite(const char* data, size_t len);
	int syncWrite(const char* data, size_t len, boost::system::error_code ec);
	size_t syncRead(boost::array<char, R_BUF_SIZE>& buf,
			boost::system::error_code& ec);
	string address();
private:
	void writeHandler(const boost::system::error_code& ec, size_t bytes_transferred);
	void readHandle(boost::shared_ptr<ConClient> sp, const boost::system::error_code& ec,
			std::size_t bytes_transferred);
public:
	void timeoutHandler(boost::shared_ptr<ConClient> sp, const boost::system::error_code& ec);
//	int executeCmd();
};
class CmdBase:public Cmd {
public:
	CmdBase();
	virtual ~CmdBase();
	virtual void execCmd(ConClient* c,std::istream* isbuf)=0;
	virtual void shutdown(ConClient* c);
};
} /* namespace centny */
#endif /* CONCLIENT_H_ */
