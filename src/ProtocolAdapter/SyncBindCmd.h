/*
 * SyncBindCmd.h
 *
 *  Created on: Dec 11, 2012
 *      Author: Scorpion
 */

#ifndef SYNCBINDCMD_H_
#define SYNCBINDCMD_H_
#include <iostream>
#include <string>
#include <sstream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include "../log/LogFactory.h"
#include "AdapterBase.h"
#include "../Common/Cmd.h"
#include "../EventMgr/EventMgr.h"
#include "../Notice/NoticeTimer.h"
namespace centny {
using namespace boost;
using namespace boost::asio;
using namespace std;
using namespace boost::system;
using namespace file;
using ip::tcp;
class EventCfg;
class NetCfg;

class SyncBindCmd: public Cmd,public TimeNoticeable {
private:
	NetAdapterBase* neta;
	EventMgr::EventMgrId mid;
	EventCfg* ecfg;
	NetCfg* ncfg;
	io_service& iosev;
	Log log;
	boost::shared_ptr<tcp::socket> socket;
	boost::asio::streambuf buf;
	boost::system::error_code ec;
	stringstream data;
	char cbuf[R_BUF_SIZE];
public:
	int netstate;
public:
	SyncBindCmd(EventCfg* ecfg, NetCfg* ncfg, NetAdapterBase* neta,
			EventMgr::EventMgrId mid, io_service& ios);
	virtual ~SyncBindCmd();
	void startRead();
	//
	bool binding();
	//
	bool reinit();
	void shutdown();
	virtual bool timeout();
private:
	void readHandle(const boost::system::error_code& ec,
			std::size_t bytes_transferred);
	void noticeEvent(vector<string>& cmds);
	void noticeSync(vector<string>& cmds);
	void transfLog(vector<string>& cmds);
	void list(vector<string>& cmds);
	void writeErrMsg(int code, string msg);
	void writeBackMsg(string msg);
};

} /* namespace centny */
#endif /* SYNCBINDCMD_H_ */
