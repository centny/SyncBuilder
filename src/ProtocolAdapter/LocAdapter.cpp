/*
 * LocAdapter.cpp
 *
 *  Created on: Nov 13, 2012
 *      Author: Scorpion
 */

#include "LocAdapter.h"
#include "CurlBuilder.h"
#include "../Common/ReplaceAll.h"
namespace centny {
LocFInfo::LocFInfo(LocAdapter *cb, LocFInfo *parent) :
		FInfo(cb, parent) {
}
LocFInfo::~LocFInfo() {
#if SHOW_FRE_MSG
	printf("free LocFInfo\n");
#endif
}
void LocFInfo::initByPath(const fs::path& p) {
//	printf("initByPath:%s\n", p.c_str());
	assert(fs::exists(p));
	if (parent) {
		this->name = p.filename().string();
		this->cwd = parent->cwd + F_SEQ + this->name;
		replaceAll(this->cwd, F_SEQ F_SEQ, F_SEQ);
	} else {
		this->name = "";
		this->cwd = "";
	}
	if (fs::is_directory(p)) {
		this->type = 'd';
		this->mtime = 0;
		this->size = 0;
	} else {
		this->type = '-';
		this->mtime = fs::last_write_time(p);
		this->size = fs::file_size(p);
		assert(parent);
	}
}
void LocFInfo::fre() {
	delete this;
}
void LocFInfo::refreshSubs() {
	this->initByPath(fs::path(this->absUrl()));
	FInfo::refreshSubs();
}
//
//
LocAdapter::LocAdapter(sqlite3 *db, string rurl) :
		AdapterBase(db), log(C_LOG("LocAdapter")) {
			this->rurl = rurl;
			this->checkRUrl(rurl);
		}
LocAdapter::LocAdapter(sqlite3 *db, string rurl, vector<string> sinc,
		vector<string> sexc) :
		AdapterBase(db), log(C_LOG("LocAdapter")) {
			this->rurl = rurl;
			this->checkRUrl(rurl);
			this->sinc=sinc;
			this->sexc=sexc;
		}
void LocAdapter::checkRUrl(string rurl) {
	fs::path rpath(rurl);
	if (!fs::exists(rpath)) {
		boost::system::error_code ec;
		fs::create_directories(rurl, ec);
		if (ec.value()) {
			log.error("create root directory %s error", rurl.c_str());
		} else {
			log.debug("create root directory %s error", rurl.c_str());
		}
	}
	assert(fs::exists(rpath));
}
LocAdapter::~LocAdapter() {
}
FInfo* LocAdapter::createRootNode() {
	last = new LocFInfo(this, 0);
	last->cwd = F_SEQ;
	last->type = 'd';
	return last;
}
vector<FInfo*> LocAdapter::listSubs(FInfo* parent) {
	assert(parent);
	vector<FInfo*> fsub;
	string rpath = this->rurl + parent->cwd;
	replaceAll(rpath, F_SEQ F_SEQ, F_SEQ, 0);
	fs::path ppath(rpath);
	assert(fs::exists(ppath));
	fs::directory_iterator it(ppath);
	fs::directory_iterator end;
	vector<string>::iterator fit, fend;
	for (; it != end; it++) {
		string cwd = string(it->path().c_str());
		bool exced = false, inced = false;
		if (this->sexc.size()) {
			for (fit = this->sexc.begin(), fend = this->sexc.end(); fit != fend;
					fit++) {
				if (boost::regex_match(cwd, boost::regex(*fit))) {
					exced = true;
					break;
				}
			}
			if (exced) {
				continue;
			}
		}
		if (this->sinc.size()) {
			for (fit = this->sinc.begin(), fend = this->sinc.end(); fit != fend;
					fit++) {
				if (boost::regex_match(cwd, boost::regex(*fit))) {
					inced = true;
					break;
				}
			}
			if (!inced) {
				continue;
			}
		}
		LocFInfo *lf = new LocFInfo(this, (LocFInfo*) parent);
		lf->initByPath(it->path());
		fsub.push_back(lf);
	}
	return fsub;
}
void LocAdapter::mkdir(FInfo* fi, string name) {
	string dpath = fi->absUrl() + F_SEQ + name;
	fs::path tp(dpath);
	bool ced = fs::create_directories(tp);
	if (!ced) {
		log.error("create local folder error:%s", dpath.c_str());
	}
}
FInfo* LocAdapter::contain(FInfo*fi, string name) {
	vector<FInfo*> fis = fi->subs();
	if (fis.size() < 1) {
		return 0;
	}
	vector<FInfo*>::iterator it, end = fis.end();
	string cwd = fi->cwd + F_SEQ + name;
	replaceAll(cwd, F_SEQ F_SEQ, F_SEQ);
	for (it = fis.begin(); it != end; it++) {
		if ((*it)->cwd == cwd) {
			return *it;
		}
	}
	return 0;
}
string LocAdapter::absUrl(FInfo* tg) {
	string aurl = this->rurl + F_SEQ + tg->cwd;
	replaceAll(aurl, "//", "/");
	return aurl;
}
bool LocAdapter::remove(FInfo* fi, string name) {
	system::error_code ec;
	FInfo *tf;
	if (name.empty()) {
		tf = fi;
		fi = fi->parent;
	} else {
		tf = fi->contain(name);
		if (tf == NULL) {
			log.error("can't find the local file %s in %s", name.c_str(),
					fi->absUrl().c_str());
			return false;
		}
	}
	string p = tf->absUrl();
	log.info("removing %s", p.c_str());
	fs::remove_all(fs::path(p), ec);
	fi->refreshSubs();
	if (fi->contain(name)) {
		log.error("remove %s error:%s", p.c_str(), ec.message().c_str());
		return false;
	} else {
		log.info("remove %s success", p.c_str());
		return true;
	}
}
}
/* namespace centny */
