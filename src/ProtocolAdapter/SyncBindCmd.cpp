/*
 * SyncBindCmd.cpp
 *
 *  Created on: Dec 11, 2012
 *      Author: Scorpion
 */

#include "SyncBindCmd.h"
#include "../Notice/NoticeCenter.h"
#include "../EventMgr/EventCfg.h"
#include "../EventMgr/EventMgr.h"
#include "../common.h"
#include "../ProtocolAdapter/NetCfg.h"
namespace centny {
namespace fs = boost::filesystem;

SyncBindCmd::SyncBindCmd(EventCfg* ecfg, NetCfg* ncfg, NetAdapterBase* neta,
		EventMgr::EventMgrId mid, io_service& ios) :
		iosev(ios), log(C_LOG("SyncBindCmd")) {
			this->ncfg=ncfg;
			this->ecfg=ecfg;
			this->netstate=0;
			this->neta=neta;
			this->mid=mid;
			assert(this->reinit());
		}

SyncBindCmd::~SyncBindCmd() {
}
void SyncBindCmd::startRead() {
	boost::asio::async_read_until(*this->socket, buf, DEFAULT_EOC,
			boost::bind(&SyncBindCmd::readHandle, this, _1, _2));
}
bool SyncBindCmd::binding() {
	if (this->netstate != 200) {
		return false;
	}
#if DEV_IN_TEST
	string session = "centny";
#else
	string session=this->neta->name;
#endif
	//
	char buf[1024];
	size_t blen = 0;
	assert(session.size());
	blen = sprintf(buf, "BINDING %s" DEFAULT_EOC, session.c_str());
	this->socket->write_some(buffer(buf, blen), ec);
	if (ec.value()) {
		this->netstate = 500;
		log.error("bind error by session:%s,msg:%s", session.c_str(),
				ec.message().c_str());
		return false;
	}
	blen = this->socket->read_some(buffer(buf, 1024), ec);
	if (ec.value()) {
		this->netstate = 500;
		log.error("bind error by session:%s,msg:%s", session.c_str(),
				ec.message().c_str());
		return false;
	}
	stringstream data(buf);
	int code;
	data >> code;
	if (code != 200) {
		log.error("bind error by session:%s,msg:%s", session.c_str(), buf);
		return false;
	} else {
		log.debug("bind success");
		return true;
	}
}
bool SyncBindCmd::reinit() {
	this->socket = boost::shared_ptr<tcp::socket>(new tcp::socket(iosev));
	ip::tcp::endpoint ep(ip::address_v4::from_string(this->ncfg->bhost()),
			atoi(this->ncfg->bport().c_str()));
	this->socket->connect(ep, ec);
	if (ec) {
		log.error("initial bind client error:%s", ec.message().c_str());
		this->netstate = 500;
		return 0;
	} else {
		log.debug("connect binding server success");
		char buf[1024];
		size_t blen = 0;
		blen = sprintf(buf, "LOGIN %s %s %s" DEFAULT_EOC,
				this->ncfg->busername().c_str(),
				this->ncfg->bpassword().c_str(), this->ncfg->bcname().c_str());
		this->socket->write_some(buffer(buf, blen), ec);
		if (ec.value()) {
			this->netstate = 500;
			log.error("login faild:%s", ec.message().c_str());
			return false;
		}
		blen = this->socket->read_some(buffer(buf, 1024), ec);
		if (ec.value()) {
			this->netstate = 500;
			log.error("login faild,msg:%s", ec.message().c_str());
			return false;
		}
		buf[blen] = 0;
		stringstream data(buf);
		int code;
		data >> code;
		if (code != 200) {
			log.error("login faild,msg:%s", buf);
			this->netstate = 500;
			return false;
		} else {
			log.info("initial bind client success");
			this->netstate = 200;
			return true;
		}
		return 1;
	}
}
void SyncBindCmd::shutdown() {
	if (this->socket.get() == 0) {
		return;
	}
	this->socket->cancel();
	boost::system::error_code ignored_ec;
	this->socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both,
			ignored_ec);
}
void SyncBindCmd::readHandle(const boost::system::error_code& ec,
		std::size_t bytes_transferred) {
	if (ec.value()) {
		this->log.debug("bind connection error");
		this->shutdown();
		return;
	}
	if (bytes_transferred > 2048) {
		this->writeErrMsg(500, "\nread bind command error:%s" + ec.message());
		return;
	}
	std::istream ibuf(&buf);
	char cbuf[2048];
	ibuf.read(cbuf, bytes_transferred);
	cbuf[bytes_transferred] = 0;
	string cdata(cbuf);
	cdata += DEFAULT_EOC;
	if (!this->initCmd(cdata)) {
		this->writeErrMsg(500, "\nread invalid command:" + cdata);
		this->startRead();
		return;
	}
//	cout << "Cmd:";
//	for (vector<string>::iterator it = cmds.begin(); it != cmds.end(); it++) {
//		cout << (*it) << " ";
//	}
//	cout << endl;
	vector<string> tcmds;
	tcmds.assign(++cmds.begin(), cmds.end());
	/*
	 * N_EVENT <event name> <args>
	 * N_SYNC <notice name> <args>
	 * LOG <event name> <begin point> <end point>
	 */
	string cmd = cmds[0];
	boost::to_lower(cmd);
	this->log.debug("receive command:%s", cmd.c_str());
	if ("n_event" == cmd) {
		this->noticeEvent(tcmds);
	} else if ("n_sync" == cmd) {
		this->noticeSync(tcmds);
	} else if ("t_log" == cmd) {
		this->transfLog(tcmds);
	} else if ("list" == cmd) {
		this->list(tcmds);
	} else {
		this->writeErrMsg(500, "\nread invalid command:" + cdata);
	}
	this->startRead();
}
void SyncBindCmd::noticeEvent(vector<string>& cmds) {
	if (cmds.size() < 1) {
		this->writeErrMsg(500, "\r\n "
				"invalid command"
				"\r\nUsage:N_EVENT <Event Name> "
				" [Event Period(FEP_PRE|FEP_CMD|FEP_POST) default:FEP_CMD ]"
				" [environment value(eg:key=value)]");
		return;
	}
	string name = cmds[0];
	string nlnames = this->ecfg->nlistenerNames();
	if (nlnames.find(name) == string::npos) {
		this->writeErrMsg(500, "\r\n "
				"event name support notice not found\r\n"
				"using 'LIST N_EVENT' to show support event names");
		return;
	}
	FEventPeriod ep = FEP_CMD;
	map<string, string> kvs;
	if (cmds.size() > 1) {
		if (cmds[1] == "FEP_PRE") {
			ep = FEP_PRE;
		} else if (cmds[1] == "FEP_POST") {
			ep = FEP_POST;
		}
	}
	if (cmds.size() > 2) {
		for (unsigned int i = 2; i < cmds.size(); i++) {
			vector<string> kv;
			boost::split(kv, cmds[i], boost::is_any_of("="));
			if (kv.size() > 1) {
				kvs[kv[0]] = kv[1];
			}
		}
	}
	EventMgr* em = EventMgr::demo(this->mid);
	if (em) {
		em->postEvent(name, ep, kvs);
		this->writeBackMsg("success");
	} else {
		this->log.error("invalid event manager id:%d", this->mid);
		this->writeErrMsg(500, "\r\n "
				"invalid event manager id\r\n");
	}
}
void SyncBindCmd::noticeSync(vector<string>& cmds) {
	EventNoticeArg *arg = new EventNoticeArg();
	arg->name = this->ncfg->syncChkNotice();
	if (cmds.size() > 0) {
		arg->args.assign(cmds.begin(), cmds.end());
	}
	NoticeCenter::defaultCenter().postObj(arg->name, arg);
	this->writeBackMsg("success");
}
void SyncBindCmd::transfLog(vector<string>& cmds) {
	if (cmds.empty()) {
		this->writeErrMsg(500, "invalid command"
				"\r\n  Usage:T_LOG <Event Name>");
		return;
	}
	string name = cmds[0];
	ListenerCfg *lc = this->ecfg->listener(name);
	if (!lc) {
		this->writeErrMsg(500, "can't find the event listener by name:" + name);
		return;
	}
	if (lc->logFile.size() < 1) {
		this->writeErrMsg(500, "not log file in event listener:" + name);
		return;
	}
	fs::path fp(lc->logFile);
	if (!fs::exists(fp)) {
		this->writeErrMsg(404, "log file not exist, listener:" + name);
		return;
	}
	size_t fsize = fs::file_size(fp, ec);
	if (ec.value()) {
		this->writeErrMsg(500, "get file size error, listener:" + name);
		return;
	}
	if (fsize < 1) {
		this->writeErrMsg(202, "log file size is zero, listener:" + name);
		return;
	}
	size_t beg = 0, end = 0;
	if (cmds.size() > 1) {
		beg = atol(cmds[1].c_str());
	}
	if (cmds.size() > 2) {
		end = atol(cmds[2].c_str());
	}
	if (beg < 0 || beg > fsize) {
		beg = 0;
	}
	if (end == 0 || end > fsize) {
		end = fsize;
	}
	if (beg > end) {
		this->writeErrMsg(202, "invalid data seek, listener:" + name);
		return;
	}
	fstream fs(lc->logFile.c_str(), ios::in|ios::binary);
	if (!fs.is_open()) {
		this->writeErrMsg(500, "open log file error, listener:" + name);
		return;
	}
	char buf[R_BUF_SIZE], buf2[1024];
	size_t blen = 0, blen2 = 0;
	fs.seekg(beg);
	size_t remain = end - beg;
	blen2 = sprintf(buf2, "200\nLOG %ld\n", remain);
	buf2[blen2]=0;
	blen = sprintf(buf, "T_LOG_BACK %ld"DEFAULT_EOC, remain + blen2);
	blen+=sprintf(buf+blen, "%s", buf2);
	buf[blen]=0;
	this->socket->write_some(buffer(buf, blen), ec);
	while (!fs.eof()) {
		fs.read(buf, R_BUF_SIZE);
		size_t rlen = fs.gcount();
		blen = rlen;
		if (remain < rlen) {
			blen = remain;
		}
		this->socket->write_some(buffer(buf, blen), ec);
		if (ec.value()) {
			log.error("send data error:%s", ec.message().c_str());
			break;
		}
		remain -= rlen;
		if (remain < 1) {
			break;
		}
	}
	fs.close();
}
void SyncBindCmd::list(vector<string>& cmds) {
	if (cmds.empty()) {
		this->writeErrMsg(500, "\r\n invalid command"
				"\r\n Usage:LIST <Command Name>");
		return;
	}
	string cmd = cmds[0];
	boost::to_lower(cmd);
	if (cmd == "n_event") {
		this->writeBackMsg(this->ecfg->nlistenerNames());
	} else if (cmd == "t_log") {
		this->writeBackMsg(this->ecfg->listenerNames());
	} else {
		this->writeErrMsg(500,
				"invalid command name:" + cmds[0]
						+ "only support <T_LOG|N_EVETNT>");
		return;
	}
}
void SyncBindCmd::writeErrMsg(int code, string msg) {
	stringstream sdata;
	sdata << code << endl << msg << endl;
	size_t blen = 0;
	blen = sprintf(cbuf, "ERROR %ld"DEFAULT_EOC"%s", sdata.str().size(),
			sdata.str().c_str());
	this->socket->write_some(buffer(cbuf, blen), ec);
	if (ec.value()) {
		log.error("send message error:%s", ec.message().c_str());
	}
}
void SyncBindCmd::writeBackMsg(string msg) {
	size_t blen = 0;
	blen = sprintf(cbuf, "B_MSG %ld"DEFAULT_EOC"%s", msg.size(), msg.c_str());
	this->socket->write_some(buffer(cbuf, blen), ec);
	if (ec.value()) {
		log.error("send message error:%s", ec.message().c_str());
	}
}
} /* namespace centny */
