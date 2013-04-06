/*
 * ShellCmdMgr.cpp
 *
 *  Created on: Mar 27, 2013
 *      Author: Scorpion
 */

#include "ShellCmdMgr.h"
#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
using namespace std;
namespace centny {
namespace fs = boost::filesystem;
WebCfg::WebCfg(string& cfgPath) :
		CfgParser(cfgPath) {

}
string WebCfg::rootPath() {
	CFG_SLOCK;
	return this->kvs["ROOT_PATH"];
}
string WebCfg::contentType(string fpath) {
	CFG_SLOCK;
	string::size_type idx=fpath.find_last_of('.');
	if(idx==string::npos) {
		return "text/plain";
	} else {
		string sext=fpath.substr(idx);
		if(this->kvs.find(sext)==this->kvs.end()) {
			return "text/plain";
		} else {
			return this->kvs[sext];
		}
	}
}
//
//ShellCmdMgr::ShellCmdMgr(WebCfg* wcfg) {
//	this->blen = 0;
//	this->wcfg=wcfg;
//}
ShellCmdMgr::ShellCmdMgr(string ucfg, string cfgPath) :
		LoginCmdBase(ucfg), log(C_LOG("ShellCmdMgr")) {
			this->blen = 0;
			if (cfgPath.empty()) {
				this->wcfg = 0;
			} else {
				this->wcfg = new WebCfg(cfgPath);
			}
		}
ShellCmdMgr::~ShellCmdMgr() {
	if (this->wcfg) {
		delete this->wcfg;
	}
}
void ShellCmdMgr::sendCmd(vector<string>& cmds, ConClient* tar) {
	this->blen = 0;
	int clen = 0;
	char *cbuf = buf;
	for (unsigned int i = 0; i < cmds.size(); i++) {
		clen = sprintf(cbuf, "%s ", cmds[i].c_str());
		blen += clen;
		cbuf += clen;
	}
	clen = sprintf(cbuf, "%s", DEFAULT_EOC);
	blen += clen;
	tar->asyncWrite(buf, blen);
}
void ShellCmdMgr::httpBing(ConClient* c, ConClient* tar, vector<string> cmds) {
	c->tServer().thrGrps().create_thread(
			boost::bind(&ShellCmdMgr::thrHttpBing, this, c, tar, cmds));
}
void ShellCmdMgr::thrHttpBing(ConClient* c, ConClient* tar,
		vector<string> cmds) {
	boost::timed_mutex* tmutex = this->conMutex(tar);
	bool locked = tmutex->timed_lock(
			boost::get_system_time() + boost::posix_time::seconds(10));
	if (locked) {
		this->binded[c] = tar;
		this->http.insert(c);
		this->sendCmd(cmds, tar);
	} else {
		this->response(c, "EServer Error", 423,
				"try lock target client timeout.");
		c->shutdown();
	}
}
void ShellCmdMgr::cmdBing(ConClient* c, ConClient* tar, vector<string> cmds) {
	c->tServer().thrGrps().create_thread(
			boost::bind(&ShellCmdMgr::thrCmdBing, this, c, tar, cmds));
}
void ShellCmdMgr::thrCmdBing(ConClient* c, ConClient* tar,
		vector<string> cmds) {
	boost::timed_mutex* tmutex = this->conMutex(tar);
	bool locked = tmutex->timed_lock(
			boost::get_system_time() + boost::posix_time::seconds(10));
	if (locked) {
		this->binded[c] = tar;
		if (cmds.size()) {
			this->sendCmd(cmds, c);
		}
		this->writeMsg(c, 200, "bind success");
	} else {
		this->writeMsg(c, 423, "bind timeout");
	}
}
boost::timed_mutex* ShellCmdMgr::conMutex(ConClient* c) {
	boost::timed_mutex* tmutex = 0;
	this->cmutex_lock.lock();
	tmutex = this->cmutex[c];
	this->cmutex_lock.unlock();
	assert(tmutex);
	return tmutex;
}
void ShellCmdMgr::bind(ConClient* c, std::istream* isbuf) {
	if (cmds.size() < 2) {
		this->writeMsg(c, 500, "invalid command");
		c->startRead();
		return;
	}
	ConClient *tar = 0;
	string key = cmds[1];
	map<string, ConClient*>::iterator it;
	it = this->binding.find(key);
	if (it == this->binding.end()) {
		tar = 0;
	} else {
		tar = it->second;
	}
	if (tar == 0) {
		this->writeMsg(c, 500, "can't find binded connect by name:" + key);
	} else {
		if (tar == c) {
			this->writeMsg(c, 500,
					"aleady binding by name " + key
							+ "waiting another connect bind");
		} else {
			this->cmdBing(c, tar, vector<string>());
		}
	}
	c->startRead();
}
void ShellCmdMgr::bing(ConClient* c, std::istream* isbuf) {
	if (cmds.size() < 2) {
		this->writeMsg(c, 500, "invalid command");
		c->startRead();
		return;
	}
	string key = cmds[1];
	map<string, ConClient*>::iterator it = this->binding.find(key);
	if (it == this->binding.end()) {
		this->cmutex_lock.lock();
		this->binding[key] = c;
		this->cmutex[c] = new boost::timed_mutex();
		this->cmutex_lock.unlock();
		this->writeMsg(c, 200,
				"binding by name " + key + ",waiting another connect bind");
	} else {
		if (it->second == c) {
			this->writeMsg(c, 500,
					"aleady binding by name " + key
							+ "waiting another connect bind");
		} else {
			this->writeMsg(c, 500, "binding name aleady be used");
		}
	}
	c->startRead();
}
void ShellCmdMgr::get(ConClient* c, std::istream* isbuf) {
	if (cmds.size() < 2) {
		c->shutdown();
		return;
	}
//	for (unsigned int i = 0; i < cmds.size(); i++) {
//		cout << cmds[i] << endl;
//	}
	stringstream sdata;
	while (!isbuf->eof()) {
		isbuf->read(cbuf, R_BUF_SIZE);
		blen = isbuf->gcount();
		sdata.write(cbuf, blen);
	}
	if (sdata.str().size() > 4) {
		while ((sdata.str().substr(sdata.str().size() - 4))
				!= DEFAULT_EOC""DEFAULT_EOC) {
			blen = c->syncRead(abuf, ec);
			sdata.write(abuf.c_array(), blen);
		}
	}
	string cookie = "";
	string line;
	vector<string> vtmp;
	while (!sdata.eof()) {
		vtmp.clear();
		getline(sdata, line);
		boost::split(vtmp, line, boost::is_any_of(":"));
		if (vtmp.size() < 2) {
			continue;
		}
		string key = vtmp[0];
		boost::trim(key);
		if (key == "Cookie") {
			cookie = vtmp[1];
			boost::trim(cookie);
		}
	}
	this->log.debug("reques by cookie:%s", cookie.c_str());
	string session = "";
	vtmp.clear();
	boost::split(vtmp, cookie, boost::is_any_of(";"));
	for (size_t i = 0; i < vtmp.size(); i++) {
		vector<string> vtmp2;
		boost::split(vtmp2, vtmp[i], boost::is_any_of("="));
		if (vtmp2.size() < 2) {
			continue;
		}
		string key = vtmp2[0];
		boost::trim(key);
		if ("session" == key) {
			session = vtmp2[1];
			boost::trim(session);
		}
	}
	this->log.debug("reques by session:%s", session.c_str());
	map<string, string> param;
	vtmp.clear();
	boost::split(vtmp, cmds[1], is_any_of("?"));
	//
	ConClient *tar = 0;
	string acmd;
	vector<string> args;
	boost::split(args, vtmp[0], is_any_of("/"));
	for (vector<string>::iterator it = args.begin(); it != args.end(); it++) {
		if ((*it).empty()) {
			it = args.erase(it);
			if (it == args.end()) {
				break;
			}
		}
	}
	if (vtmp.size() > 1) {
		vector<string> pars;
		boost::split(pars, vtmp[1], is_any_of("&"));
		for (size_t i = 0; i < pars.size(); i++) {
			vector<string> pars2;
			boost::split(pars2, pars[i], is_any_of("="));
			if (pars2.size() > 1) {
				param[pars2[0]] = pars2[1];
			}
		}
	}
	if (args.size() >0 &&args[0]=="Logout"){
		if(session.empty()){
			this->response(c, "EServer", 500, "not login");
			c->shutdown();
			return;
		}
		set<string>::iterator it=this->sessions.find(session);
		if(it==this->sessions.end()){
			this->response(c, "EServer", 500, "not login");
		}else{
			this->sessions.erase(it);
			this->response(c, "EServer", 200, "logout success");
		}
		c->shutdown();
		return;
	}
	if (args.size() >0 &&args[0]=="CheckLogin"){
		if(session.empty()){
			this->response(c, "EServer", 500, "not login");
			c->shutdown();
			return;
		}
		if(this->sessions.find(session)==this->sessions.end()){
			this->response(c, "EServer", 500, "not login");
		}else{
			this->sendHeader(c,200,session.size());
			c->syncWrite(session.c_str(), session.size());
		}
		c->shutdown();
		return;
	}
	if (args.size() > 0 && args[0] == "Login") {
		string user = param["usr"];
		string pass = param["pwd"];
		string name = param["name"];
		if (user.empty() || pass.empty()) {
			this->response(c, "EServer", 500, "invalid username or password");
			c->shutdown();
			return;
		}
		log.debug("start execute login command by name:%s", name.c_str());
		string tpass = "";
		{
			SESSION_LOCK;
			tpass= this->users->pass(user);
		}
		if (pass != tpass) {
			this->response(c, "CmdServer", 500,
					"invalid username or password.");
			c->shutdown();
			return;
		}
		string session = LoginCmdBase::newSession();
		this->sendCookie(c, 200, session.size(), "session=" + session);
		c->syncWrite(session.c_str(), session.size());
		this->sessions.insert(session);
		c->shutdown();
		return;
	}
	if (this->sessions.find(session) == this->sessions.end()) {
		this->webfile(c);
		return;
	}
	if (args.size() > 0 && args[0] == "Cmd") {
		if (args.size() < 2) {
			this->response(c, "CmdServer", 500,
					"invalid command.</br>"
							"Usage:/Cmd/&lt;Command Name&gt;/&lt;Other Parameter&gt;...");
			c->shutdown();
			return;
		}
		acmd = args[1];
		boost::to_lower(acmd);
		if (acmd == "list") {
			vector<string> nargs;
			nargs.assign((args.begin() + 2), args.end());
			this->list(c, nargs, isbuf, true);
			return;
		} else if (acmd == "help") {
			this->response(c, "CmdServer", 200, "Usage:</br>"
					"/Cmd/list[/&lt;Client Name&gt;/&lt;Command Name&gt;]"
					"  list the connected client or support event.</br>"
					"/Cmd/help  show this.");
			c->shutdown();
		} else {
			this->response(c, "CmdServer", 500,
					"invalid command.</br>"
							"Usage:/Cmd/&lt;Command Name&gt;/&lt;Other Parameter&gt;...");
			c->shutdown();
		}
	}
	if (args.size() > 0 && args[0] == "EServer") {
		if (args.size() == 2) {
			acmd = args[1];
			boost::to_lower(acmd);
			if (acmd == "help") {
				this->response(c, "EServer", 200,
						"Usage:</br>"
								"/EServer/&lt;Client Name&gt;/&lt;T_LOG|N_SYNC&gt;/&lt;Other Parameter&gt;.</br>"
								"&nbsp;&nbsp;&nbsp;&nbsp;T_LOG[/&lt;Begin Postion&gt;/&lt;End Position&gt;] show event log.</br>"
								"&nbsp;&nbsp;&nbsp;&nbsp;N_SYNC notice client sync.</br>"
								"/EServer/help   show this.");
				c->shutdown();
				return;
			}
		}
		if (args.size() < 3) {
			this->response(c, "EServer", 500,
					"invalid command.</br>"
							"Usage:/EServer/&lt;Client Name&gt;/&lt;Event Name&gt;/&lt;Other Parameter&gt;...");
			c->shutdown();
			return;
		}
		stringstream sdata;
		size_t rlen = 0;
		while (!isbuf->eof()) {
			isbuf->read(buf, BUF_SIZE);
			rlen = isbuf->gcount();
			sdata.write(buf, rlen);
		}
		string star = sdata.str();
		if (star.size() < 3 || star.substr(star.size() - 3) != "\n\r\n") {
			boost::system::error_code ec;
			while (true) {
				rlen = c->syncRead(abuf, ec);
				if (ec) {
					this->writeMsg(c, 500, "server error");
					c->shutdown();
					return;
				}
				star = sdata.str();
				if (star.size() > 2
						&& star.substr(star.size() - 3) == "\n\r\n") {
					break;
				}
			}
		}
		if (this->binding.find(args[1]) == this->binding.end()) {
			this->response(c, "EventServer", 500,
					"can't find the client by name:" + args[1]);
			c->shutdown();
			return;
		}
		tar = this->binding[args[1]];
		vector<string> rcmds;
		rcmds.assign((args.begin() + 2), args.end());
		this->httpBing(c, tar, rcmds);
	} else {
		this->webfile(c);
	}
}
void ShellCmdMgr::webfile(ConClient* c) {
	if (this->wcfg == 0) {
		this->response(c, "WCfg not found", 500,
				"the web configure is not found");
		c->shutdown();
		return;
	}
	string rpath = this->wcfg->rootPath();
	if (rpath.empty()) {
		this->response(c, "WCfg error", 500, "the root path not configured");
		c->shutdown();
		return;
	}
	if (!fs::exists(rpath)) {
		this->response(c, "WCfg error", 500,
				"the root path not found:" + rpath);
		c->shutdown();
		return;
	}
	string tpath = rpath + cmds[1];
	if (!fs::exists(tpath)) {
		this->response(c, "EServer Error", 500, "the path not found:" + tpath);
		c->shutdown();
		return;
	}
	if (fs::is_directory(tpath)) {
		this->response(c, "EServer Error", 500,
				"the path is not a normal file:" + tpath);
		c->shutdown();
		return;
	}
//	c->tServer().thrGrps().create_thread(
//			boost::bind(&ShellCmdMgr::sendFile, this, c, tpath));
	this->sendFile(c, tpath);
}
void ShellCmdMgr::sendFile(ConClient* c, string tpath) {
	size_t fsize = fs::file_size(tpath);
	if (fsize) {
		fstream fs;
		fs.open(tpath.c_str(), ios::in | ios::binary);
		if (!fs.is_open()) {
			this->response(c, "EServer Error", 500, "open file error:" + tpath);
			c->shutdown();
			return;
		}
		string ctype = this->wcfg->contentType(tpath);
		//
		this->sendHeader(c, 200, fsize, ctype);
		while (!fs.eof()) {
			fs.read(buf, R_BUF_SIZE);
			size_t rlen = fs.gcount();
			c->syncWrite(buf, rlen);
		}
		fs.close();
	} else {
		this->sendHeader(c, 200, 0, "text/plain");
	}
	c->shutdown();
}
void ShellCmdMgr::help(ConClient* c) {
	this->writeMsg(c, 200, "");
}
bool ShellCmdMgr::list(ConClient* c, vector<string>& args, std::istream* isbuf,
		bool byHttp) {
	stringstream sdata;
	if (args.empty()) {
		map<string, ConClient*>::iterator it;
		for (it = this->binding.begin(); it != this->binding.end(); it++) {
			sdata << (it->first) << "\r\n";
		}
		if (sdata.str().empty()) {
			this->response(c,"EServer",204,"");
		} else {
			c->syncWrite(sdata.str().c_str(), sdata.str().size());
		}
		if (byHttp) {
			c->shutdown();
		}
		return false;
	}
	if (args.size() == 1) {
		this->writeMsg(c, 500,
				"invalid command\r\nUsage:\r\n"
						"LIST  list the binding clients.\r\n"
						"LIST <Client Name> <T_LOG|N_EVENT> list the support event name.\r\n");
		if (byHttp) {
			c->shutdown();
		}
		return false;
	}
	string cname = args[0];
	if (this->binding.find(cname) == this->binding.end()) {
		this->writeMsg(c, 500,
				"can't find the binding client by name " + cname);
		if (byHttp) {
			c->shutdown();
		}
		return false;
	}
	ConClient* tar = this->binding[cname];
	vector<string> ncmds;
	ncmds.push_back("LIST");
	ncmds.push_back(args[1]);
	if (byHttp) {
		this->httpBing(c, tar, ncmds);
	} else {
		this->cmdBing(c, tar, ncmds);
	}
	return true;
}
void ShellCmdMgr::execCmd(ConClient* c, std::istream* isbuf) {
	string cmd = cmds[0];
	boost::to_lower(cmd);
	ConClient *tar = 0;
	map<ConClient*, ConClient*>::iterator it;
	if (cmd == "quit" || cmd == "exit") {
		c->shutdown();
		return;
	}
	if (cmd == "help") {
		this->help(c);
		c->startRead();
		return;
	}
	if (cmd == "get") {
		this->get(c, isbuf);
		return;
	}
	if (cmd == "login") {
		this->login(c, cmds);
		return;
	}
	if (!this->isLogin(c)) {
		this->writeMsg(c, 500, "please login first");
		c->startRead();
		return;
	}
	if (cmd == "bind") {
		this->bind(c, isbuf);
		return;
	}
	if (cmd == "binding") {
		this->bing(c, isbuf);
		return;
	}
	if (cmd == "list") {
		vector<string> nargs;
		nargs.assign((cmds.begin() + 1), cmds.end());
		this->list(c, nargs, isbuf, false);
		c->startRead();
		return;
	}
	if (this->binded.find(c) != this->binded.end()) {
		tar = this->binded[c];
	} else {
		for (it = this->binded.begin(); it != this->binded.end(); it++) {
			if (it->second == c) {
				tar = it->first;
				break;
			}
		}
	}
	if (tar) {
		this->execBindedCmd(c, tar, isbuf);
	} else {
		this->writeMsg(c, 500, "Usage:bind name\n\t"
				" to bind another connect or wait be binded");
		c->startRead();
	}
}
void ShellCmdMgr::execBindedCmd(ConClient* c, ConClient* tar,
		std::istream* isbuf) {
	string cmd = cmds[0];
	boost::to_lower(cmd);
	if (cmd == "t_log_back" || cmd == "error" || cmd == "b_msg") {
		size_t bsize = atol(cmds[1].c_str());
		size_t rlen = 0;
		while (bsize > 0 && !isbuf->eof()) {
			isbuf->read(buf, BUF_SIZE);
			rlen = isbuf->gcount();
			tar->syncWrite(buf, rlen);
			bsize -= rlen;
		}
		boost::system::error_code ec;
		while (bsize > 0) {
			rlen = c->syncRead(abuf, ec);
			if (ec) {
				this->writeMsg(tar, 500, "server error");
				break;
			}
			tar->syncWrite(buf, rlen);
			bsize -= rlen;
		}
		c->startRead();
	} else {
		this->sendCmd(cmds, tar);
		c->startRead();
	}
	set<ConClient*>::iterator it = this->http.find(tar);
	if (it != this->http.end()) {
		this->http.erase(it);
		tar->shutdown();
	}

}
void ShellCmdMgr::shutdown(ConClient* c) {
	map<string, ConClient*>::iterator scit;
	for (scit = this->binding.begin(); scit != this->binding.end(); scit++) {
		if (scit->second == c) {
#ifdef WIN32
			scit = this->binding.erase(scit);
#else
			this->binding.erase(scit);
#endif
			if (scit == this->binding.end()) {
				break;
			}
		}
	}
	this->cmutex_lock.lock();
	map<ConClient*, ConClient*>::iterator it;
	map<ConClient*, boost::timed_mutex*>::iterator ctit;
	for (it = this->binded.begin(); it != this->binded.end(); it++) {
		if (it->second == c) {
			ctit = this->cmutex.find(it->first);
			if (ctit != this->cmutex.end()) {
				ctit->second->try_lock();
				ctit->second->unlock();
			}
#ifdef WIN32
			it=this->binded.erase(it);
#else
			this->binded.erase(it);
#endif
			if (it == this->binded.end()) {
				break;
			}
			continue;
		}
		if (it->first == c) {
			ctit = this->cmutex.find(it->second);
			if (ctit != this->cmutex.end()) {
				ctit->second->try_lock();
				ctit->second->unlock();
			}
#ifdef WIN32
			it=this->binded.erase(it);
#else
			this->binded.erase(it);
#endif
			if (it == this->binded.end()) {
				break;
			}
			continue;
		}
	}
	ctit = this->cmutex.find(c);
	if (ctit != this->cmutex.end()) {
		ctit->second->try_lock();
		ctit->second->unlock();
		delete ctit->second;
		this->cmutex.erase(ctit);
	}
	this->cmutex_lock.unlock();
}
void ShellCmdMgr::writeMsg(ConClient* c, int code, string msg) {
	blen = sprintf(buf, "%d\r\n%s"DEFAULT_EOC, code, msg.c_str());
	c->asyncWrite(buf, blen);
}
void ShellCmdMgr::response(ConClient* c, string name, int code, string msg) {
	stringstream body;
	body << "<html>" << endl;
	body << "<header>" << endl;
	body
			<< "\
			<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\
			<meta charset=\"utf-8\">\
			<meta name=\"viewport\" content=\"width=device-width,initial-scale=1.0,maximum-scale=1.0,user-scalable=0\">"
			<< endl;
	body << "<title>" << name << "</title>" << endl;
	body
			<< "\
			<meta name=\"keywords\" content=\"\">\
			<meta name=\"description\" content=\"\">\
			<meta name=\"author\" content=\"zhuanghuanbin_gz\">\
			<meta name=\"apple-mobile-web-app-capable\" content=\"yes\">\
			<meta name=\"apple-mobile-web-app-status-bar-style\" content=\"black\">\
			<meta name=\"format-detection\" content=\"telephone=no\">\
			<meta http-equiv=\"Content-Type\" content=\"text/html;charset=UTF-8\" />"
			<< endl;
	body << "<html><body><div>" << msg << "</div></body></html>";
	this->sendHeader(c,code,body.str().size(),this->wcfg->contentType(".html"));
	c->syncWrite(body.str().c_str(), body.str().size());
}
void ShellCmdMgr::sendHeader(ConClient* c, int code, size_t len, string ctype) {
	blen = 0;
	char* tbuf = 0;
	tbuf = cbuf + blen;
	blen += sprintf(tbuf, "HTTP/1.0 %d OK\r\n",code);
	tbuf = cbuf + blen;
	blen += sprintf(tbuf, "Cache-Control: no-cache\r\n");
	tbuf = cbuf + blen;
	blen += sprintf(tbuf, "Expires:0 \r\n");
	tbuf = cbuf + blen;
	blen += sprintf(tbuf, "Pragma: no-cache\r\n");
	tbuf = cbuf + blen;
	blen += sprintf(tbuf, "Connection: close\r\n");
	tbuf = cbuf + blen;
	blen += sprintf(tbuf, "Server: EServer\r\n");
	tbuf = cbuf + blen;
	blen += sprintf(tbuf, "Content-Length: %ld\r\n", len);
	tbuf = cbuf + blen;
	blen += sprintf(tbuf, "Content-Type: %s\r\n", ctype.c_str());
	tbuf = cbuf + blen;
	blen += sprintf(tbuf, "\r\n");
	c->syncWrite(cbuf, blen);
//	stringstream buf;
//	buf << "HTTP/1.1 " << code << " OK\r\n";
//	buf << "Cache-Control:no-cache\r\n";
//	buf << "Expires:0\r\n";
//	buf << "Pragma:no-cache\r\n";
//	buf << "Connection:close\r\n";
//	buf << "Content-Type:" << ctype << ";charset=UTF-8\r\n";
//	buf << "Server:EServer\r\n";
//	buf << "Content-Length:" << len << "\r\n\n";
//	c->syncWrite(buf.str().c_str(), buf.str().size());
}
void ShellCmdMgr::sendCookie(ConClient* c, int code, size_t len, string cookie,
		string ctype) {
	blen = 0;
	char* tbuf = 0;
	tbuf = cbuf + blen;
	blen += sprintf(tbuf, "HTTP/1.0 200 OK\r\n");
	tbuf = cbuf + blen;
	blen += sprintf(tbuf, "Cache-Control: no-cache\r\n");
	tbuf = cbuf + blen;
	blen += sprintf(tbuf, "Expires:0 \r\n");
	tbuf = cbuf + blen;
	blen += sprintf(tbuf, "Pragma: no-cache\r\n");
	tbuf = cbuf + blen;
	blen += sprintf(tbuf, "Connection: close\r\n");
	tbuf = cbuf + blen;
	blen += sprintf(tbuf, "Server: EServer\r\n");
	tbuf = cbuf + blen;
	blen += sprintf(tbuf, "Set-Cookie: %s\r\n", cookie.c_str());
	tbuf = cbuf + blen;
	blen += sprintf(tbuf, "Content-Length: %ld\r\n", len);
	tbuf = cbuf + blen;
	blen += sprintf(tbuf, "Content-Type: %s\r\n", ctype.c_str());
	tbuf = cbuf + blen;
	blen += sprintf(tbuf, "\r\n");
	c->syncWrite(cbuf, blen);
//	stringstream buf;
//	buf << "HTTP/1.0 " << code << " OK\r\n";
//	buf << "Cache-Control:no-cache\r\n";
//	buf << "Expires:0\r\n";
//	buf << "Pragma:no-cache\r\n";
//	buf << "Connection:close\r\n";
//	buf << "Content-Type:" << ctype << ";charset=UTF-8\r\n";
//	buf << "Server:EServer\r\n";
//	buf << "Set-Cookie:" << cookie << "\r\n";
//	buf << "Content-Length:" << len << "\r\n\n";
//	c->syncWrite(buf.str().c_str(), buf.str().size());
}
//void ShellCmdMgr::startTransfter(ConClient* c,ConClient* tar){
//	c->psocket->async_read_some(buffer(abuf),boost::bind(&ShellCmdMgr::readHandle,this,c,tar,_1,_2));
//}
//void ShellCmdMgr::readHandle(ConClient* c,ConClient* tar,const boost::system::error_code& ec,std::size_t bytes_transferred){
//	if(ec){
//		this->writeMsg(tar,500,"server error");
//		return;
//	}
//	tar->asyncWrite(abuf.c_array(),bytes_transferred);
//	this->tfsize[c]-=bytes_transferred;
//	if(this->tfsize[c]<1){
//		this->tfsize.erase(this->tfsize.find(c));
//		c->startRead();
//	}else{
//		this->startTransfter(c,tar);
//	}
//}
} /* namespace centny */
