/*
 * SyncAdapter.cpp
 *
 *  Created on: Dec 4, 2012
 *      Author: Scorpion
 */

#include "SyncAdapter.h"

namespace centny {
namespace fs = boost::filesystem;
#define CHK_LOGIN if (this->netstate != 200) {\
		log.error("current net state is:%d", this->netstate);\
		throw "not login";\
}
void SyncAdapter::setFileInfo(FInfo* fi, string fp) {
	system::error_code ec;
	fs::last_write_time(fs::path(fp), fi->mtime, ec);
//	time_t t = fs::last_write_time(fs::path(fp), ec);
//	cout << "mtime:" << fi->mtime << ",t:" << t << endl;
	if (ec.value()) {
		cout << fp << ":" << ec.message() << endl;
	}
}
SyncAdapter::SyncAdapter(sqlite3 *db, NetCfg *ncfg) :
		NetAdapterBase(db), log(C_LOG("SyncAdapter")) {
			this->ncfg=ncfg;
			this->rurl=this->ncfg->path();
			this->usr=this->ncfg->username();
			this->pwd=this->ncfg->password();
			this->name=this->ncfg->cname();
			this->sexc=this->ncfg->sexc();
			this->sinc=this->ncfg->sinc();
			this->logined=false;
			this->blen=0;
			memset(buf,0,R_BUF_SIZE);
			this->reinit();
		}

SyncAdapter::~SyncAdapter() {
	this->quit();
}
bool SyncAdapter::isLogined() {
	return this->logined != 0;
}
bool SyncAdapter::reinit() {
	if (this->socket.get()) {
		this->socket->close(ec);
	}
	this->logined = 0;
	this->socket = boost::shared_ptr<ip::tcp::socket>(
			new ip::tcp::socket(this->iosev));
	ip::tcp::endpoint ep(ip::address_v4::from_string(this->ncfg->host()),
			atoi(this->ncfg->port().c_str()));
	this->socket->connect(ep, ec);
	if (ec) {
		this->netstate = 500;
		log.error("initial client adapter error:%s", ec.message().c_str());
		return 0;
	} else {
		this->netstate = 200;
		log.info("initial client adapter success");
		this->login();
		return 1;
	}
}
bool SyncAdapter::uploadSupported() {
	return 1;
}
bool SyncAdapter::downloadSupported() {
	return 1;
}
void SyncAdapter::login() {
	if (this->logined) {
		return;
	}
	log.info("login server...");
	blen = sprintf(buf, "login %s %s %s" DEFAULT_EOC, this->usr.c_str(),
			this->pwd.c_str(), this->name.c_str());
	this->socket->write_some(buffer(buf, blen), ec);
	if (ec) {
		log.error("login error:%s", ec.message().c_str());
		this->netstate = 500;
		throw "login error";
	}

	blen = this->socket->read_some(buffer(buf, R_BUF_SIZE), ec);
	if (ec) {
		log.error("login error:%s", ec.message().c_str());
		this->netstate = 500;
		throw "login error";
	}
	stringstream data;
	data.write(buf, blen);
	string line;
	getline(data, line);
	if ("200" == line) {
		this->logined = true;
		data >> line >> line;
		data >> session;
		log.info("login success,session id:%s", session.c_str());
		this->netstate = 200;
	} else {
		this->logined = false;
		getline(data, line);
		log.info("login error:%s", line.c_str());
		assert(this->logined);
	}
}
bool SyncAdapter::quit() {
	if (!this->socket->is_open()) {
		return false;
	}
	blen = sprintf(buf, "quit" DEFAULT_EOC);
	this->socket->write_some(buffer(buf, blen), ec);
	if (ec) {
		log.error("login error:%s", ec.message().c_str());
		return false;
	}
	this->socket->close();
	return true;
}
FInfo* SyncAdapter::createRootNode() {
	FInfo *fi = new NetFInfo(this, 0);
	fi->cwd = "";
	fi->type = 'd';
	return fi;
}
//the return FInfo* object will be auto free by call fre().
vector<FInfo*> SyncAdapter::listSubs(FInfo* parent) {
	CHK_LOGIN;
	vector<FInfo*> fis;
	memset(buf, 0, R_BUF_SIZE);
	blen = sprintf(buf, "LIST %s" DEFAULT_EOC, parent->cwd.c_str());
	this->socket->write_some(buffer(buf, blen), ec);
	if (blen > 2) {
		buf[blen - 2] = 0;
		this->log.debug("sending command:%s", buf);
	}
	if (ec) {
		log.error("list subs error:%s", ec.message().c_str());
		this->netstate = 500;
		throw "list subs error";
	}
	blen = this->socket->read_some(buffer(buf, R_BUF_SIZE), ec);
	if (ec) {
		log.error("list subs error:%s", ec.message().c_str());
		this->netstate = 500;
		throw "list subs error";
	}
	std::stringstream header(buf);
	string line;
	size_t hlen = 0; //the header.
	getline(header, line);
	hlen += line.size() + 1;
	if (line != "125") {
		getline(header, line);
		log.error("list subs error:%s", line.c_str());
		throw "list subs error";
	}
	getline(header, line);
	hlen += line.size() + 1;
	vector<string> cmds;
	boost::split(cmds, line, is_any_of(" \t"));
	if (cmds.size() != 3) {
		log.error("list subs error:%s,%s", "data error", line.c_str());
		throw "list subs error";
	}
	size_t dlen = atol(cmds[2].c_str());
	if (dlen < 1) {
//		log.error("list subs error:%s,%s", "data error",
//				"list data size is zero");
		return fis;
	}
	stringstream data;
	size_t rlen = blen - hlen;
	if (rlen) {
		data.write(buf + hlen, rlen);
	}
	while (rlen < dlen) {
		blen = this->socket->read_some(buffer(buf, R_BUF_SIZE), ec);
		if (ec) {
			log.error("list subs error:%s", ec.message().c_str());
			this->netstate = 500;
			throw "list subs error";
		}
		data.write(buf, blen);
		rlen += blen;
	}
	return this->convert(parent, data);
}
vector<FInfo*> SyncAdapter::convert(FInfo* parent, stringstream& dbuf) {
	vector<FInfo*> fis;
	while (!dbuf.eof()) {
		string line;
		getline(dbuf, line);
		if (line.size() < 1) {
			continue;
		}
		FInfo* f = this->convertOne(parent, line);
		if (f) {
			fis.push_back(f);
		}
	}
	return fis;
}
FInfo* SyncAdapter::convertOne(FInfo* parent, string line) {
	vector<string> ifs;
	boost::split(ifs, line, is_any_of(" \t"));
	if (ifs.size() != 4) {
		log.error("convert to instance error:%s", line.c_str());
		return 0;
	}
	string cwd;
	if (parent->cwd.empty()) {
		cwd = ifs[1];
	} else {
		cwd = parent->cwd + "/" + ifs[1];
	}
//	replaceAll(cwd, "//", "/");
	vector<string>::iterator it, end;
	bool exced = false, inced = false;
	if (this->sexc.size()) {
		for (it = this->sexc.begin(), end = this->sexc.end(); it != end; it++) {
			if (boost::regex_match(cwd, boost::regex(*it))) {
				exced = true;
				break;
			}
		}
		if (exced) {
			return 0;
		}
	}
	if (this->sinc.size()) {
		for (it = this->sinc.begin(), end = this->sinc.end(); it != end; it++) {
			if (boost::regex_match(cwd, boost::regex(*it))) {
				inced = true;
				break;
			}
		}
		if (!inced) {
			return 0;
		}
	}
	FInfo *fi = new NetFInfo(this, (NetFInfo*) parent);
	fi->type = ifs[0].at(0);
	fi->name = ifs[1];
	fi->size = atol(ifs[2].c_str());
	fi->mtime = atol(ifs[3].c_str());
	fi->parent = parent;
	fi->cwd = cwd;
//	cout << "list:" << fi->name << endl;
	return fi;
}
void SyncAdapter::mkdir(FInfo* fi, string name) {
	string ncwd = (fi->cwd + "/" + name);
	blen = sprintf(buf, "MKD %s" DEFAULT_EOC, ncwd.c_str());
	this->socket->write_some(buffer(buf, blen), ec);
	if (ec) {
		log.error("create directory error:%s", ec.message().c_str());
		throw "create directory error";
	}
	boost::asio::streambuf rbuf;
	boost::asio::read_until(*this->socket, rbuf, DEFAULT_EOC, ec);
	std::istream in(&rbuf);
	string line;
	std::getline(in, line);
	if (line == "200") {
		log.debug("create directory(%s) success", ncwd.c_str());
	} else {
		std::getline(in, line);
		log.error("create directory(%s) error", ncwd.c_str(), line.c_str());
		throw "create directory error";
	}
}
bool SyncAdapter::remove(FInfo* fi, string name) {
	FInfo *child;
	if (name.empty()) {
		child = fi;
	} else {
		child = fi->contain(name);
		if (!child) {
			log.debug("target %s not found in %s", name.c_str(),
					fi->cwd.c_str());
			throw "remove error";
		}
	}
	blen = sprintf(buf, "RMD %s" DEFAULT_EOC, child->cwd.c_str());
	this->socket->write_some(buffer(buf, blen), ec);
	if (ec) {
		log.error("remove error:%s", ec.message().c_str());
		throw "remove error";
	}
	boost::asio::streambuf rbuf;
	boost::asio::read_until(*this->socket, rbuf, DEFAULT_EOC, ec);
	if (ec) {
		log.error("upload error:%s", ec.message().c_str());
		throw "remove error";
	}
	std::istream in(&rbuf);
	string line;
	std::getline(in, line);
	if (line == "200") {
		log.debug("remove %s success", child->cwd.c_str());
		return true;
	} else {
		std::getline(in, line);
		log.error("remove %s error", child->cwd.c_str(), line.c_str());
		return false;
	}
}

const char* SyncAdapter::upload(FInfo* fi, string lf, string name) {
	fs::path lpath(lf);
	if (!fs::exists(lpath)) {
		log.error("file %s to upload not exists", lf.c_str());
		return "not exist";
	}
	size_t fsize = fs::file_size(lpath, ec);
	if (ec) {
		log.error("get file size %s error", lf.c_str());
		throw "get file size error";
	}
	time_t mtime = fs::last_write_time(lpath, ec);
	fstream fs(lf.c_str(), ios::in);
	if (!fs.is_open()) {
		log.error("file %s to upload can't open", lf.c_str());
		throw "open file error";
	}
	const char* msg = this->upload(fi, fs, fsize, mtime, name);
	fs.close();
	return msg;
}
const char* SyncAdapter::upload(FInfo* fi, istream& is, size_t len,
		time_t mtime, string name) {
	blen = sprintf(buf, "STOR %s %ld %ld" DEFAULT_EOC,
			(fi->cwd + "/" + name).c_str(), len, mtime);
	this->socket->write_some(buffer(buf, blen), ec);
	if (ec) {
		log.error("upload error:%s", ec.message().c_str());
		throw "upload error";
	}
	boost::asio::streambuf rbuf;
	boost::asio::read_until(*this->socket, rbuf, DEFAULT_EOC, ec);
	if (ec) {
		log.error("upload error:%s", ec.message().c_str());
		throw "upload error";
	}
	std::istream in(&rbuf);
	string line;
	std::getline(in, line);
	if (line == "125") {
		while (!is.eof()) {
			is.read(buf, R_BUF_SIZE);
			this->socket->write_some(buffer(buf, is.gcount()), ec);
			if (ec) {
				log.error("upload error:%s", ec.message().c_str());
				throw "upload error";
			}
		}
		blen = this->socket->read_some(buffer(buf, R_BUF_SIZE), ec);
		buf[blen] = 0;
		if (ec) {
			log.error("upload error:%s", ec.message().c_str());
			throw "upload error";
		}
//		istream is(&res);
		std::stringstream resin(buf);
//		cout << "res:" << resin.str() << endl;
		std::getline(resin, line);
		if (line == "226") {
			log.debug("upload %s to %s success", name.c_str(), fi->cwd.c_str());
			return 0;
		} else {
			std::getline(in, line);
			log.error("upload %s to %s error:%s", name.c_str(), fi->cwd.c_str(),
					line.c_str());
			throw "upload error";
		}
	} else {
		std::getline(in, line);
		log.error("upload %s to %s error:%s", name.c_str(), fi->cwd.c_str(),
				line.c_str());
		throw "upload error";
	}
}
const char* SyncAdapter::download(FInfo* fi, string lf) {
	if (fi->isDir()) {
		return "is not file";
	}
	if (fi->size < 1) {
		fclose(fopen(lf.c_str(), "wb"));
		log.debug("create one empty file:%s", lf.c_str());
		setFileInfo(fi, lf);
		return 0;
	}
	fstream ofs(lf.c_str(), ios::out);
	if (!ofs.is_open()) {
		log.debug("open %s  error", lf.c_str());
		throw "download error";
	}
	const char* msg = this->download(fi, ofs);
	ofs.close();
	if (msg) {
		log.error("down %s error:%s", fi->cwd.c_str(), msg);
		return msg;
	} else {
		setFileInfo(fi, lf);
		return 0;
	}
}
const char* SyncAdapter::download(FInfo* fi, ostream& os) {
	if (this->netstate != 200) {
		log.error("current net state is:%d", this->netstate);
		throw "not login";
	}
	memset(buf, 0, R_BUF_SIZE);
	blen = sprintf(buf, "DOWN %s" DEFAULT_EOC, fi->cwd.c_str());
	this->socket->write_some(buffer(buf, blen), ec);
	if (ec) {
		log.error("down %s error:%s", fi->cwd.c_str(), ec.message().c_str());
		throw "down error";
	}
	blen = this->socket->read_some(buffer(buf, R_BUF_SIZE), ec);
	if (ec) {
		log.error("down %s error:%s", fi->cwd.c_str(), ec.message().c_str());
		throw "download error";
	}
	std::stringstream header(buf);
	string line;
	size_t hlen = 0; //the header.
	getline(header, line);
	hlen += line.size() + 1;
	if (line != "125") {
		getline(header, line);
		log.error("down %s error:%s", fi->cwd.c_str(), line.c_str());
		throw "download error";
	}
	getline(header, line);
	hlen += line.size() + 1;
	vector<string> cmds;
	boost::split(cmds, line, is_any_of(" \t"));
	if (cmds.size() != 4) {
		log.error("down %s error:%s,%s", fi->cwd.c_str(), "data error",
				line.c_str());
		throw "download error";
	}
	size_t dlen = atol(cmds[2].c_str());
	if (dlen < 1) {
		log.error("down %s error:%s,%s", fi->cwd.c_str(), "data error",
				"file data size is zero");
		throw "download error";
	}
//	stringstream data;
	size_t rlen = blen - hlen;
	if (rlen) {
		os.write(buf + hlen, rlen);
	}
	while (rlen < dlen) {
		blen = this->socket->read_some(buffer(buf, R_BUF_SIZE), ec);
		if (ec) {
			log.error("down %s error:%s", fi->cwd.c_str(),
					ec.message().c_str());
			throw "download error";
		}
		os.write(buf, blen);
		rlen += blen;
	}
	if (rlen != dlen) {
		log.error("down %s error:%s", fi->cwd.c_str(), "data size error");
		throw "download error";
	} else {
		log.debug("down %s success", fi->cwd.c_str());
	}
	return 0;
}
string SyncAdapter::absUrl(FInfo* tg) {
	string aurl;
//	cout << "rurl:" << rurl << endl;
	if (this->rurl.empty()) {
		aurl = tg->cwd;
	} else {
		aurl = this->rurl + "/" + tg->cwd;
	}
	replaceAll(aurl, "//", "/");
	return aurl;
}
} /* namespace centny */
