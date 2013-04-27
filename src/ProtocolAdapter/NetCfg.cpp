/*
 * NetCfg.cpp
 *
 *  Created on: Nov 17, 2012
 *      Author: Scorpion
 */

#include "NetCfg.h"
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
namespace centny {
namespace fs = boost::filesystem;
NetCfg::NetCfg(string& cfgPath) :
		CfgParser(cfgPath) {
	if (!this->valid) {
		return;
	}
	fs::path fp(this->locSyncDir());
	this->valid = exists(fp) && fs::is_directory(fp);
	if (!this->valid) {
		this->msg = "local sync directory (" + this->locSyncDir()
				+ ") is not exist or not a directory";
		return;
	}
	this->isNoticeSync = false;
	this->isTimeSync = false;
	vector<string> stype = this->syncChkType();
	for (size_t i = 0; i < stype.size(); i++) {
		if ("NOTICE" == stype[i]) {
			this->isNoticeSync = true;
			continue;
		}
		if ("TIME" == stype[i]) {
			this->isTimeSync = true;
			continue;
		}
	}
}
NetCfg::~NetCfg() {
}
string NetCfg::host() {
	CFG_SLOCK;
	return this->kvs["SERVER_HOST"];
}
string NetCfg::port() {
	CFG_SLOCK;
	return this->kvs["SERVER_PORT"];
}
string NetCfg::bhost() {
	CFG_SLOCK;
	return this->kvs["SERVER_BHOST"];
}
string NetCfg::bport() {
	CFG_SLOCK;
	return this->kvs["SERVER_BPORT"];
}
string NetCfg::path() {
	CFG_SLOCK;
	return this->kvs["SERVER_PATH"];
}
bool NetCfg::ssl() {
	CFG_SLOCK;
	return this->kvs["SERVER_SSL"]=="YES";
}
string NetCfg::username() {
	CFG_SLOCK;
	return this->kvs["USERNAME"];
}
string NetCfg::busername() {
	CFG_SLOCK;
	return this->kvs["B_USERNAME"];
}
string NetCfg::password() {
	CFG_SLOCK;
	return this->kvs["PASSWORD"];
}
string NetCfg::bpassword() {
	CFG_SLOCK;
	return this->kvs["B_PASSWORD"];
}
string NetCfg::usrpwd() {
	CFG_SLOCK;
	string usr = this->kvs["USERNAME"];
	string pwd = this->kvs["PASSWORD"];
	if (usr.length() < 1 || pwd.length() < 1) {
		return "";
	}
	return usr + ":" + pwd;
}
string NetCfg::cname() {
	CFG_SLOCK;
	return this->kvs["CNAME"];
}
string NetCfg::bcname() {
	CFG_SLOCK;
	return this->kvs["B_CNAME"];
}
bool NetCfg::isUpload() {
	CFG_SLOCK;
	return this->kvs["UPLOAD"] == "YES";
}
bool NetCfg::isDownload() {
	CFG_SLOCK;
	return this->kvs["DOWNLOAD"] == "YES";
}
time_t NetCfg::syncChkTime() {
	CFG_SLOCK;
	string tmp = this->kvs["SYNC_CHK_TIME"];
	if(tmp.empty()) {
		return 50000;
	} else {
		return atol(tmp.c_str());
	}
}
time_t NetCfg::bindReconnectTime() {
	CFG_SLOCK;
	string tmp = this->kvs["BIND_RECONNECT_TIME"];
	if(tmp.empty()) {
		return 20000;
	} else {
		return atol(tmp.c_str());
	}
}
string NetCfg::syncChkNotice() {
	CFG_SLOCK;
	return this->kvs["SYNC_CHK_NOTICE"];
}
vector<string> NetCfg::syncChkType() {
	CFG_SLOCK;
	string tmp=this->kvs["SYNC_CHK_TYPES"];
	vector<string> types;
	boost::split(types,tmp,boost::is_any_of("|"));
	return types;
}
string NetCfg::type() {
	CFG_SLOCK;
	return this->kvs["SERVER_TYPE"];
}
string NetCfg::locSyncDir() {
	CFG_SLOCK;
	return this->kvs["LOC_SYNC_DIR"];
}
string NetCfg::dbPath() {
	CFG_SLOCK;
	return this->kvs["LOC_DB_PATH"];
}
vector<string> NetCfg::sinc() {
	CFG_SLOCK;
	string tmp=this->kvs["SYNC_INC_FILTER"];
	vector<string> filters;
	boost::split(filters,tmp,boost::is_any_of(";"));
	return this->rempty(filters);
}
vector<string> NetCfg::sexc() {
	CFG_SLOCK;
	string tmp=this->kvs["SYNC_EXC_FILTER"];
	vector<string> filters;
	boost::split(filters,tmp,boost::is_any_of(";"));
	return this->rempty(filters);
}
vector<string> NetCfg::rempty(vector<string>& tar) {
	vector<string>::iterator it, end;
	for (it = tar.begin(), end = tar.end(); it != end; it++) {
		if (it->empty()) {
			it = tar.erase(it);
			end = tar.end();
			if(it==end){
				break;
			}
		}
	}
	return tar;
}
//
} /* namespace centny */
