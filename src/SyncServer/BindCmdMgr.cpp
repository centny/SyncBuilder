///*
// * BindCmdMgr.cpp
// *
// *  Created on: Dec 5, 2012
// *      Author: Scorpion
// */
//
//#include "BindCmdMgr.h"
//#include "HttpCmdMgr.h"
//namespace centny {
//
//BindCmdMgr::BindCmdMgr(FileCmdMgr *fcm) {
//	this->fcm = fcm;
//	this->server = 0;
//	this->hcm = 0;
//}
//
//BindCmdMgr::~BindCmdMgr() {
//}
//void BindCmdMgr::setHttpCmdMgr(HttpCmdMgr* hcm) {
//	this->hcm = hcm;
//}
//void BindCmdMgr::sendCmd(vector<string> cmds, ConClient* http) {
//	size_t blen = 0;
//	char cbuf[2048];
//	ConClient *fc = this->fcm->nameClient(cmds[0]);
//	if (!fc) {
//		this->hcm->response(http, "EventServer", 500,
//				"can't find client by name:" + cmds[0]);
//		return;
//	}
//	ConClient *c = this->s2bc[fc->session()];
//	string cmd;
//	for (size_t i = 1; i < cmds.size(); i++) {
//		cmd.append(cmds[i] + " ");
//	}
//	blen = sprintf(cbuf, "%d\nlog %s"DEFAULT_EOC, 100, cmd.c_str());
//	boost::system::error_code ec;
//	c->syncWrite(cbuf, blen, ec);
//	if (ec) {
//		this->hcm->response(http, "EventServer", 500, ec.message());
//		return;
//	}
//	boost::array<char, 2048> buf;
//	blen = c->syncRead(buf, ec);
//	stringstream sbuf;
//	sbuf.write(buf.data(), blen);
//	string tmp;
//	sbuf >> tmp;
//	if ("200" != tmp) {
//		this->hcm->response(http, "EventServer", 500, sbuf.str());
//		return;
//	}
//	sbuf >> tmp >> tmp;
//	size_t flen = atol(tmp.c_str());
//	this->hcm->sendHeader(http, 200, flen);
//	size_t rlen = 0;
//	while (rlen < flen) {
//		blen = c->syncRead(buf, ec);
//		http->syncWrite(buf.data(), blen, ec);
//		rlen += blen;
//	}
//	http->shutdown();
//}
//ConClient* BindCmdMgr::client(string s) {
//	map<string, ConClient*>::iterator it;
//	it = this->s2bc.find(s);
//	if (it != this->s2bc.end()) {
//		return it->second;
//	} else {
//		return 0;
//	}
//}
//void BindCmdMgr::setServer(SyncServer* s) {
//	this->server = s;
//}
//void BindCmdMgr::shutdown(ConClient* c) {
//	this->s2bc.erase(c->session());
//}
//void BindCmdMgr::execCmd(ConClient* c,std::istream* isbuf) {
//	assert(this->server&&this->fcm&&this->hcm);
//	char sbuf[1024];
//	size_t blen = 0;
//	if (cmds.size() < 1) {
//		blen = sprintf(sbuf, "%d\n%s"DEFAULT_EOC, 500, "invalid command");
//		c->asyncWrite(sbuf, blen);
//		c->shutdown();
//		return;
//	}
//	string cmd = cmds[0];
//	boost::to_lower(cmd);
//	if (cmd == "bind") {
//		if (cmds.size() != 2) {
//			blen = sprintf(sbuf, "%d\n%s"DEFAULT_EOC, 500,
//					"invalid command.\nUsage:BIND <Session ID>");
//			c->asyncWrite(sbuf, blen);
//			c->startRead();
//			return;
//		}
//		ConClient *fc = this->fcm->sessionClient(cmds[1]);
//		if (!fc) {
//			blen = sprintf(sbuf, "%d\n%s"DEFAULT_EOC, 500,
//					"invalid session.\nUsage:BIND <Session ID>");
//			c->asyncWrite(sbuf, blen);
//			c->startRead();
//			return;
//		}
//		if (this->s2bc.find(cmds[1]) != this->s2bc.end()) {
//			blen = sprintf(sbuf, "%d\n%s"DEFAULT_EOC, 500, "already binded.\n");
//			c->asyncWrite(sbuf, blen);
//			c->startRead();
//			return;
//		}
//		this->s2bc[cmds[1]] = c;
//		blen = sprintf(sbuf, "%d\n%s"DEFAULT_EOC, 200, "Bind success.\n");
//		c->asyncWrite(sbuf, blen);
//		//c->startRead();
//	}else{
//		c->startRead();
//	}
////	if (cmd == "log") {
////		if (cmds.size() != 4) {
////			blen =
////					sprintf(sbuf, "%d\n%s"DEFAULT_EOC, 500,
////							"invalid command.\nUsage:LOG <Client Name> <Event Name> <Log Length>");
////			c->asyncWrite(sbuf, blen);
////			c->startRead();
////			return;
////		}
////		size_t flen = atol(cmds[3].c_str());
////		this->server->thrGrps().create_thread(
////				boost::bind(&BindCmdMgr::log, this, c, cmds[1], cmds[2], flen));
////	}
//}
////size_t BindCmdMgr::log(ConClient* c, string name, string event, size_t len) {
////	string res = this->hcm->writeHeader(name, event, len);
////	char sbuf[1024];
////	size_t blen = 0;
////	if (res.size()) {
////		blen = sprintf(sbuf, "500\nABOR\r\n");
////		c->asyncWrite(sbuf, blen);
////		c->startRead();
////		return 0;
////	} else {
////		blen = sprintf(sbuf, "125\nSTART\r\n");
////		c->asyncWrite(sbuf, blen);
////	}
////	boost::array<char, 2048> buf;
////	boost::system::error_code ec;
////	size_t rlen = 0;
////	while (rlen < len) {
////		blen = c->syncRead(buf, ec);
////		this->hcm->writeData(name, event, buf.data(), blen);
////		rlen += blen;
////	}
////	c->startRead();
////	return rlen;
////}
//} /* namespace centny */
