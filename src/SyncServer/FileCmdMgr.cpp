/*
 * FileCmdMgr.cpp
 *
 *  Created on: Dec 1, 2012
 *      Author: Scorpion
 */

#include "FileCmdMgr.h"
#include "../Common/ComDef.h"
#include "../Common/ReplaceAll.h"
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <stdlib.h>
#include <cstdlib>
using namespace std;
namespace fs = boost::filesystem;
namespace centny {
#define H_LEN 5
#define STOR_USG "STOR <path> <size> <modify time> \t upload file"
#define DOWN_USG "DOWN <file path> \t\t download file"
#define LIST_USG "LIST <directory path> \t\t list directory info"
#define MKD_USG "MKD <directory path> \t\t make a directory"
#define RMD_USG "RMD <target path> \t\t remove a directory or file"
#define QUIT_USG "QUIT \t\t\t\t close connection"
#define HELP_USG "HELP \t\t\t\t show this message"
#define CONTAIN_USG "CONTAIN <directory> <name> \t check contain target"
#define HELP_MSG "Usage:\n"HELP_USG"\n"QUIT_USG"\n"LOGIN_USG"\n"STOR_USG"\n"DOWN_USG"\n"LIST_USG"\n"MKD_USG"\n"RMD_USG"\n"CONTAIN_USG

FileCmdMgr::FileCmdMgr(string ucfg) :
		LoginCmdBase(ucfg),log(C_LOG("FileCmdMgr")) {
	server = 0;
}

FileCmdMgr::~FileCmdMgr() {
}

void FileCmdMgr::execCmd(ConClient* c, std::istream* isbuf) {
	assert(this->server);
//	c->startRead();
	if (c->isLocked()) {
		return;
	}
	boost::system::error_code ec;
	if (cmds.size() < 1) {
		blen = sprintf(cbuf, "%d\n%s"DEFAULT_EOC, 500, "invalid command");
		c->asyncWrite(cbuf, blen);
		return;
	}
	string cmd = cmds[0];
	boost::to_lower(cmd);
	//not login command
	if (cmd == "login") {
		this->login(c, cmds);
		return;
	}
	if (cmd == "help") {
		this->help(c);
		c->startRead();
		return;
	}
	if (cmd == "quit") {
		this->quit(c);
		return;
	}
	if (!this->isLogin(c)) {
		blen = sprintf(cbuf, "%d\n%s"DEFAULT_EOC, 500, "please login first");
		c->asyncWrite(cbuf, blen);
		c->startRead();
		return;
	}
	//already login command.
	if (cmd == "logout") {
		this->logout(c);
		c->startRead();
		return;
	}
	if (cmd == "stor") {
		//
#define P_STOR_ERR blen=sprintf(cbuf, "%d\n%s\n%s"DEFAULT_EOC, 500, "invalid command","Usage:"STOR_USG)\
	,c->asyncWrite(cbuf, blen)
		///
		if (cmds.size() != 4) {
			P_STOR_ERR;
			return;
		}
		long fz = atol(cmds[2].c_str());
		time_t mtime = atol(cmds[3].c_str());
		if (fz < 1 || mtime < 1 || cmds[1].size() < 1) {
			P_STOR_ERR;
			return;
		}
		this->server->thrGrps().create_thread(
				boost::bind(&FileCmdMgr::stor, this, c, cmds[1], fz, mtime));
		return;
	}
	if (cmd == "down") {
		//
#define P_DOWN_ERR blen=sprintf(cbuf, "%d\n%s\n%s"DEFAULT_EOC, 500, "invalid command","Usage:"DOWN_USG)\
	,c->asyncWrite(cbuf, blen)
		//
		if (cmds.size() != 2 || cmds[1].size() < 1) {
			P_DOWN_ERR;
			return;
		}
		this->server->thrGrps().create_thread(
				boost::bind(&FileCmdMgr::down, this, c, cmds[1]));
		return;
	}
	if (cmd == "list") {
		//
#define P_LIST_ERR blen=sprintf(cbuf, "%d\n%s\n%s"DEFAULT_EOC, 500, "invalid command","Usage:"LIST_USG)\
	,c->asyncWrite(cbuf, blen)
		//
		if (cmds.size() < 1 || cmds.size() > 2) {
			P_LIST_ERR;
			c->startRead();
			return;
		}
		string tdir = "";
		if (cmds.size() > 1) {
			tdir = cmds[1];
		}
		this->list(c, tdir);
		c->startRead();
		return;
	}
	if (cmd == "mkd") {
		//
#define P_MKD_ERR blen=sprintf(cbuf, "%d\n%s\n%s"DEFAULT_EOC, 500, "invalid command","Usage:"MKD_USG)\
	,c->asyncWrite(cbuf, blen)
		//
		if (cmds.size() != 2) {
			P_MKD_ERR;
			c->startRead();
			return;
		}
		this->mkd(c, cmds[1]);
		c->startRead();
		return;
	}
	if (cmd == "rmd") {
		//
#define P_RMD_ERR blen=sprintf(cbuf, "%d\n%s\n%s"DEFAULT_EOC, 500, "invalid command","Usage:"RMD_USG)\
	,c->asyncWrite(cbuf, blen)
		//
		if (cmds.size() != 2) {
			P_RMD_ERR;
			c->startRead();
			return;
		}
		this->rmd(c, cmds[1]);
		c->startRead();
		return;
	}
	if (cmd == "contain") {
		//
#define P_CONTAIN_ERR blen=sprintf(cbuf, "%d\n%s\n%s"DEFAULT_EOC, 500, "invalid command","Usage:"CONTAIN_USG)\
	,c->asyncWrite(cbuf, blen)
		//
		if (cmds.size() != 3) {
			P_CONTAIN_ERR;
			c->startRead();
			return;
		}
		this->contain(c, cmds[1], cmds[2]);
		c->startRead();
	}
	//
	blen = sprintf(cbuf, "%d\n%s\n%s"DEFAULT_EOC, 500, "invalid command",
			"Usage:"HELP_MSG);
	c->asyncWrite(cbuf, blen);
	c->startRead();
	//
}
void FileCmdMgr::setServer(SyncServer* s) {
	this->server = s;
}
size_t FileCmdMgr::quit(ConClient* c) {
	if (isLogin(c)) {
		logout(c);
	}
	c->shutdown();
	return 0;
}
size_t FileCmdMgr::help(ConClient* c) {
	blen = sprintf(cbuf, "%d\n%s"DEFAULT_EOC, 200, HELP_MSG);
	c->asyncWrite(cbuf, blen);
	return blen;
}
size_t FileCmdMgr::list(ConClient* c, string path) {
	log.debug("create new directory:%s", path.c_str());
	string rpath = this->rdir(c) + F_SEQ + path;
	replaceAll(rpath, F_SEQ F_SEQ, F_SEQ, 0);
	fs::path ppath(rpath);
	if (!fs::exists(ppath)) {
		blen = sprintf(cbuf, "%d\n%s"DEFAULT_EOC, 500, "invalid directory");
		c->asyncWrite(cbuf, blen);
		cbuf[blen] = 0;
		log.debug("execute list command error:%s,send:%ld", cbuf, blen);
		return blen;
	}
	fs::directory_iterator it(ppath);
	fs::directory_iterator end;
	stringstream sbuf;
	boost::system::error_code ec;
	for (; it != end; it++) {
		fs::path fn = it->path().filename();
		string ftype = "-";
		size_t fsize = 0;
		if (fs::is_directory(it->path(), ec)) {
			ftype = "d";
		} else if (fs::is_regular_file(it->path(), ec)) {
			fsize = fs::file_size(it->path(), ec);
		} else {
			continue;
		}
		if (ec.value()) {
			break;
		}
		time_t fmt = 0;
		if (ftype == "-") {
			fmt = fs::last_write_time(it->path(), ec);
			if (ec.value()) {
				break;
			}
		}
		blen = sprintf(cbuf, "%s %s %ld %ld\n", ftype.c_str(),
				fn.string().c_str(), fsize, fmt);
		sbuf.write(cbuf, blen);
	}
	if (ec.value()) {
		blen = sprintf(cbuf, "%d\n%s"DEFAULT_EOC, 500, ec.message().c_str());
		c->asyncWrite(cbuf, blen);
	} else {
		blen = sprintf(cbuf, "%d\nLIST %s %ld" DEFAULT_EOC, 125, path.c_str(),
				sbuf.str().size());
		c->syncWrite(cbuf, blen);
		c->asyncWrite(sbuf.str().c_str(), sbuf.str().size());
		blen = sbuf.str().size() + 4;
	}
	log.debug("execute list command end,send:%ld", blen);
	return blen;
}
size_t FileCmdMgr::mkd(ConClient* c, string path) {
	log.debug("create new directory:%s", path.c_str());
	fs::path npath(this->rdir(c) + F_SEQ + path);
	boost::system::error_code ec;
	if (fs::exists(npath, ec)) {
		blen = sprintf(cbuf, "%d\n%s"DEFAULT_EOC, 202,
				"directory already exist");
		c->asyncWrite(cbuf, blen);
		cbuf[blen] = 0;
		log.debug("not execute mkd command:directory already exist,send:%ld",
				cbuf, blen);
		return blen;
	}
	fs::create_directories(npath, ec);
	if (ec.value()) {
		blen = sprintf(cbuf, "%d\n%s"DEFAULT_EOC, 202, ec.message().c_str());
	} else {
		blen = sprintf(cbuf, "%d\nOK"DEFAULT_EOC, 200);
	}
	c->asyncWrite(cbuf, blen);
	log.debug("execute mkd command end,send:%ld", blen);
	return blen;
}
size_t FileCmdMgr::rmd(ConClient* c, string path) {
	log.debug("remove  %s", path.c_str());
	fs::path tpath(this->rdir(c) + F_SEQ + path);
	boost::system::error_code ec;
	if (!fs::exists(tpath, ec)) {
		blen = sprintf(cbuf, "%d\n%s"DEFAULT_EOC, 202,
				"No such file or directory");
		c->asyncWrite(cbuf, blen);
		return blen;
	}
	if (ec.value()) {
		blen = sprintf(cbuf, "%d\n%s"DEFAULT_EOC, 500, ec.message().c_str());
		c->asyncWrite(cbuf, blen);
		return blen;
	}
	fs::remove_all(tpath, ec);
	if (ec.value()) {
		blen = sprintf(cbuf, "%d\n%s"DEFAULT_EOC, 202, ec.message().c_str());
	} else {
		blen = sprintf(cbuf, "%d\nOK"DEFAULT_EOC, 200);
	}
	c->asyncWrite(cbuf, blen);
	log.debug("execute rmd command end,send:%ld", blen);
	return blen;
}
size_t FileCmdMgr::stor(ConClient* c, string path, long flen, time_t mtime) {
	log.debug("storing %s, length:%ld", path.c_str(), flen);
	boost::shared_ptr<ConClient> csp = c->sp();
	ConClient::ScopedLock lock(c);
	fs::path tpath(this->rdir(c) + F_SEQ + path);
	boost::system::error_code ec;
	if (fs::is_directory(tpath, ec)) {
		blen = sprintf(cbuf, "%d\n%s"DEFAULT_EOC, 202,
				"can't upload directory");
		c->syncWrite(cbuf, blen);
		c->startRead();
		log.debug("execute stor command:can't upload directory,send:%ld", blen);
		return blen;
	}
	fs::path ppath = tpath.parent_path();
	if (!fs::is_directory(ppath)) {
		blen = sprintf(cbuf, "%d\n%s %s"DEFAULT_EOC, 202, ppath.c_str(),
				"directory not found");
		c->syncWrite(cbuf, blen);
		c->startRead();
		log.debug("execute stor command:parent directory not found,send:%ld",
				blen);
		return blen;
	}
	fstream lfs;
	lfs.open(tpath.c_str(), ios::out | ios::binary);
	if (!lfs.is_open()) {
		blen = sprintf(cbuf, "%d\n%s:%s"DEFAULT_EOC, 500, "open file error",
				tpath.c_str());
		c->asyncWrite(cbuf, blen);
		c->startRead();
		log.debug("execute stor command:open file error,send:%ld", blen);
		return blen;
	}
	blen = sprintf(cbuf, "%d\n%s" DEFAULT_EOC, 125, "start receive data");
	c->syncWrite(cbuf, blen);
	boost::shared_ptr<deadline_timer> timer = this->server->deadlineTimer();
	boost::array<char, R_BUF_SIZE> rbuf;
	long rlen = 0;
	while (rlen < flen) {
		timer->expires_from_now(boost::posix_time::milliseconds(5000));
		timer->async_wait(
				boost::bind(&ConClient::timeoutHandler, c, c->sp(), _1));
		blen = c->syncRead(rbuf, ec);
		timer->cancel();
		if (ec.value() == ECANCELED) {
			return rlen;
		} else if (ec.value()) {
			log.error("read data error for client:%s", c->address().c_str());
			lfs.close();
			fs::remove(tpath, ec);
			c->shutdown();
			return rlen;
		}
		lfs.write(rbuf.data(), blen);
		rlen += blen;
	}
	lfs.close();
	fs::last_write_time(tpath, mtime, ec);
	if (rlen == flen) {
		blen = sprintf(cbuf, "%d\n%s"DEFAULT_EOC, 226, "receive file success");
	} else {
		blen = sprintf(cbuf, "%d\n%s"DEFAULT_EOC, 500, "data error");
		log.error("read data error for client:%s", c->address().c_str());
		fs::remove(tpath, ec);
	}
	c->asyncWrite(cbuf, blen);
	c->startRead();
	log.debug("execute stor command end,send:%ld", blen);
	return rlen;
}
size_t FileCmdMgr::down(ConClient* c, string path) {
	log.debug("start down %s", path.c_str());
	boost::shared_ptr<ConClient> csp = c->sp();
	ConClient::ScopedLock lock(c);
	fs::path tpath(this->rdir(c) + F_SEQ + path);
	boost::system::error_code ec;
	if (fs::is_directory(tpath, ec)) {
		blen = sprintf(cbuf, "%d\n%s"DEFAULT_EOC, 202, "can't down folder");
		c->syncWrite(cbuf, blen);
		c->startRead();
		return blen;
	}
	if (!fs::is_regular_file(tpath, ec)) {
		blen = sprintf(cbuf, "%d\n%s"DEFAULT_EOC, 202, "not a regular file");
		c->syncWrite(cbuf, blen);
		c->startRead();
		return blen;
	}
	size_t fsize = fs::file_size(tpath, ec);
	if (ec.value()) {
		blen = sprintf(cbuf, "%d\n%s"DEFAULT_EOC, 500, ec.message().c_str());
		c->syncWrite(cbuf, blen);
		c->startRead();
		return blen;
	}
	if (fsize < 1) {
		blen = sprintf(cbuf, "%d\n%s"DEFAULT_EOC, 202,
				"target file size is zero");
		c->syncWrite(cbuf, blen);
		c->startRead();
		return blen;
	}
	time_t fmt = fs::last_write_time(tpath, ec);
	if (ec.value()) {
		blen = sprintf(cbuf, "%d\n%s"DEFAULT_EOC, 500, ec.message().c_str());
		c->syncWrite(cbuf, blen);
		c->startRead();
		return blen;
	}
	fstream lfs;
	lfs.open(tpath.c_str(), ios::in | ios::binary);
	if (!lfs.is_open()) {
		blen = sprintf(cbuf, "%d\n%s:%s"DEFAULT_EOC, 500, "open file error",
				tpath.c_str());
		c->asyncWrite(cbuf, blen);
		c->startRead();
		return blen;
	}
	log.debug("start sending data");
	blen = sprintf(cbuf, "%d\nDOWN %s %ld %ld" DEFAULT_EOC, 125,
			tpath.filename().c_str(), fsize, fmt);
	c->syncWrite(cbuf, blen);
	size_t rlen = blen;
	while (!lfs.eof()) {
		lfs.read(cbuf, 1024);
		blen = lfs.gcount();
		if (c->syncWrite(cbuf, blen)) {
			log.error("write data error for client:%s", c->address().c_str());
			lfs.close();
			c->shutdown();
			return rlen;
		}
		rlen += blen;
	}
	lfs.close();
	c->startRead();
	log.debug("down %s end,send size:%ld", path.c_str(), rlen);
	return rlen;
}
size_t FileCmdMgr::contain(ConClient* c, string path, string name) {
	string rpath = this->rdir(c) + F_SEQ + path;
	replaceAll(rpath, F_SEQ F_SEQ, F_SEQ, 0);
	fs::path ppath(rpath);
	if (!fs::exists(ppath)) {
		blen = sprintf(cbuf, "%d\n%s"DEFAULT_EOC, 500, "invalid directory");
		c->asyncWrite(cbuf, blen);
		return blen;
	}
	fs::directory_iterator it(ppath);
	fs::directory_iterator end;
	stringstream sbuf;
	boost::system::error_code ec;
	bool contained = false;
	for (; it != end; it++) {
		fs::path fn = it->path().filename();
		if (fn.string() == name) {
			contained = true;
		}
	}
	if (contained) {
		blen = sprintf(cbuf, "%d\n%s"DEFAULT_EOC, 200, "YES");
	} else {
		blen = sprintf(cbuf, "%d\n%s"DEFAULT_EOC, 450, "NO");
	}
	c->asyncWrite(cbuf, blen);
	return blen;
}

} /* namespace centny */
