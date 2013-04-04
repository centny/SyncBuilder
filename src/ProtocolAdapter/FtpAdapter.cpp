/*
 * FtpAdapter.cpp
 *
 *  Created on: Nov 10, 2012
 *      Author: Scorpion
 */

#include "FtpAdapter.h"
#include <assert.h>
#include "../Common/ReplaceAll.h"
#include <boost/date_time.hpp>
namespace centny {
void FtpAdapter::setFileInfo(FInfo* fi, string fp) {
	system::error_code ec;
	fs::last_write_time(fs::path(fp), fi->mtime, ec);
	if (ec.value()) {
		cout << fp << ":" << ec.message() << endl;
	}
//	cout << "fp:" << fp << endl;
//	cout << "setting file info:"
//			<< boost::posix_time::to_simple_string(::from_time_t(fi->mtime))
//			<< endl;
}
FtpAdapter::FtpAdapter(sqlite3 *db, string rurl, string usr, string pwd) :
		NetAdapterBase(db), log(C_LOG("FtpAdapter")) {
			this->init(rurl, usr,pwd);
		}
FtpAdapter::FtpAdapter(sqlite3 *db, NetCfg *ncfg) :
		NetAdapterBase(db), log(C_LOG("FtpAdapter")) {
			assert(ncfg->type()=="FTP");
			string rurl ="ftp://" +ncfg->host() + ":" + ncfg->port() + ncfg->path();
			this->init(rurl, ncfg->username(),ncfg->password());
		}
FtpAdapter::~FtpAdapter() {
}
vector<FInfo*> FtpAdapter::listSubs(FInfo* parent) {
	vector<FInfo*> fis;
	string rcwd;
	if (parent->cwd == "/") {
		rcwd = this->rurl + "/";
	} else {
		rcwd = this->rurl + parent->cwd + "/";
	}
//	log.debug("require subdirs:%s", rcwd.c_str());
	curl.reset();
	this->setUsrPwd();
	curl.setURL(rcwd);
	curl.setCustomRequest("NLST");
	curl.setWriteCallback(defaultStreamWriteCb, &curl.buf);
	const char* prs = curl.perform();
	if (prs) {
		log.error("require subs fail:%s", prs);
		this->netstate = 500;
	} else {
		fis = this->convert(parent, curl.buf);
		this->netstate = 200;
	}
	return fis;
}
void FtpAdapter::init(string rurl, string usr, string pwd) {
	this->rurl = rurl;
	this->usr = usr;
	this->pwd = pwd;
	this->setUsrPwd();
	curl.setWriteCallback(&defaultStreamWriteCb, &curl.buf);
	curl.setOpt(CURLOPT_FTP_RESPONSE_TIMEOUT, 10L);
	curl.setOpt(CURLOPT_CONNECTTIMEOUT, 10L);
	this->log.info("initial FTP adapter by URL:%s", this->rurl.c_str());
	this->noop();
//	curl.showDebug(1L);
}
string FtpAdapter::usrpwd() {
	return this->usr + ":" + this->pwd;
}
FInfo* FtpAdapter::createRootNode() {
	FInfo *fi = new NetFInfo(this, 0);
	fi->cwd = "/";
	fi->type = 'd';
	return fi;
}
vector<FInfo*> FtpAdapter::convert(FInfo* parent, stringstream& buf) {
	vector<FInfo*> fis;
	while (!buf.eof()) {
		string line;
		std::getline(buf, line);
		if (line.length() < 1) {
			continue;
		}
		fis.push_back(this->convertOne(parent, line));
	}
	return fis;
}
FInfo* FtpAdapter::convertOne(FInfo* parent, string line) {
	FInfo *fi = new NetFInfo(this, (NetFInfo*) parent);
	fi->name = line;
	fi->parent = parent;
	string cwd = parent->cwd + "/" + fi->name;
	replaceAll(cwd, "///", "/");
	replaceAll(cwd, "//", "/");
	fi->cwd = cwd;
	curl.reset();
	this->setUsrPwd();
	curl.setURL(this->rurl + "/" + fi->cwd);
	curl.finfo(fi->mtime, fi->size);
	fi->type = fi->mtime < 1 ? 'd' : '-';
//	log.debug("new sub file:%s", fi->cwd.c_str());
	return fi;
}
bool FtpAdapter::uploadSupported() {
	return false;
}
bool FtpAdapter::downloadSupported() {
	return true;
}
//const char* FtpAdapter::upload(FInfo* fi, string lf, string name) {
//	if (!fi->isDir()) {
//		return "is not folder";
//	}
//	CurlBuilder curl;
//	curl.setUsrPwd(this->usrpwd);
//	curl.setUpload(1L);
//	curl.setURL(this->rurl + "/" + fi->cwd + "/" + name);
//	curl.setUFile(lf);
//	curl.setReadCallback();
//	return curl.perform();
//	return 0;
//}
//const char* FtpAdapter::upload(FInfo* fi, istream& is, string name) {
//	if (!fi->isDir()) {
//		return "is not folder";
//	}
//	CurlBuilder curl;
//	curl.setUsrPwd(this->usrpwd);
//	curl.setUpload(1L);
//	curl.setURL(this->rurl + "/" + fi->cwd + "/" + name);
//	curl.setReadCallback(&defaultStreamReadCb, &is);
//	return curl.perform();
//	return 0;
//}
const char* FtpAdapter::download(FInfo* fi, string lf) {
	if (fi->isDir()) {
		return "is not file";
	}
	if (fi->size < 1) {
		fclose(fopen(lf.c_str(), "wb"));
		log.debug("create one empty file:%s", lf.c_str());
		setFileInfo(fi, lf);
		return 0;
	}
	CurlBuilder curl;
	this->setUsrPwd();
	curl.setUpload(0L);
	curl.setURL(this->rurl + "/" + fi->cwd);
	curl.setDFile(lf);
//	curl.setWriteCallback();
	const char* res = curl.perform();
	if (res) {
		this->netstate = 500;
	} else {
		this->netstate = 200;
		setFileInfo(fi, lf);
	}
	return res;
}
const char* FtpAdapter::download(FInfo* fi, ostream& os) {
	if (fi->isDir()) {
		return "is not file";
	}
	CurlBuilder curl;
	this->setUsrPwd();
	curl.setUpload(0L);
	curl.setURL(this->rurl + "/" + fi->cwd);
	curl.setWriteCallback(&defaultStreamWriteCb, &os);
	const char* res = curl.perform();
	if (res) {
		this->netstate = 500;
	} else {
		this->netstate = 200;
	}
	return res;
}
void FtpAdapter::mkdir(FInfo* fi, string name) {
//	if (!fi->isDir()) {
//		return;
//	}
////	CurlBuilder curl;
////	curl.showDebug(1L);
////	curl.setUsrPwd(this->usrpwd);
////	curl.setUpload(0L);
//	curl.setURL(this->rurl + "/" + fi->cwd);
//	curl.appHeader("MKD " + name);
//	const char* res = curl.perform();
//	if (res) {
//		this->netstate = 500;
//		log.error("make remote directory error:%s", res);
//	} else {
//		this->netstate = 200;
//	}
//	curl.cleanHeader();
//	bsleep(300);
}

bool FtpAdapter::remove(FInfo* fi, string name) {
//	FInfo* child = fi->contain(name);
//	if (!child) {
//		return false;
//	}
//	curl.setURL(fi->absUrl());
//	if (child->isDir()) {
//		curl.appHeader("RMD " + name);
//	} else {
//		curl.appHeader("DELE " + name);
//	}
//	const char* res = curl.perform();
//	if (res) {
//		this->netstate = 500;
//		log.error("remove remote error:%s", res);
//	} else {
//		this->netstate = 200;
//	}
//	curl.cleanHeader();
//	fi->refreshSubs();
//	return fi->contain(name) == 0;
	return false;
}
FInfo* FtpAdapter::contain(FInfo*fi, string name) {
	vector<FInfo*> fis = fi->subs();
	if (fis.size() < 1) {
		return 0;
	}
	vector<FInfo*>::iterator it, end = fis.end();
	string cwd = fi->cwd + name;
	for (it = fis.begin(); it != end; it++) {
		if ((*it)->cwd == cwd) {
			return *it;
		}
	}
	return 0;
}
bool FtpAdapter::reinit() {
	this->noop();
	return this->netstate == 200;
}
void FtpAdapter::noop() {
	curl.reset();
	this->setUsrPwd();
	curl.setURL(this->rurl + "/");
	curl.setCustomRequest("NLST");
	curl.setWriteCallback(defaultStreamWriteCb, &curl.buf);
	const char* res = curl.perform();
	if (res) {
		this->netstate = 500;
		log.error("test noop error:%s", res);
	} else {
		this->netstate = 200;
	}
}
void FtpAdapter::setUsrPwd() {
	if (this->usr.size()) {
		curl.setUsrPwd(this->usrpwd());
	} else {
		curl.setAnonymous();
	}
}
} /* namespace centny */
