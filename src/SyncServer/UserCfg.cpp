/*
 * UserCfg.cpp
 *
 *  Created on: Dec 3, 2012
 *      Author: Scorpion
 */

#include "UserCfg.h"

namespace centny {

UserCfg::UserCfg(string& cfgPath) :
		CfgParser(cfgPath), log(C_LOG("UserCfg")) {
			this->initAllUser();
		}

UserCfg::~UserCfg() {
}
string UserCfg::pass(string user) {
	vector<string>::iterator it, end = this->validUsers.end();
	it = std::find(this->validUsers.begin(), end, user);
	if (it == end) {
		return "";
	}
	if (this->kvs.find("USER_" + user) == this->kvs.end()) {
		return "";
	} else {
		return this->kvs["USER_" + user];
	}
}
string UserCfg::root(string user) {
	if (this->kvs.find("ROOT_" + user) == this->kvs.end()) {
		return "";
	} else {
		return this->kvs["ROOT_" + user];
	}
}
void UserCfg::initAllUser() {
	//check the all event configure.
	map<string, string>::iterator it;
	for (it = this->kvs.begin(); it != this->kvs.end(); it++) {
		string en = it->first;
		if (en.size() < 6) {
			continue;
		}
		if ("USER_" == en.substr(0, 5)) {
			string name = en.substr(5);
			if (this->kvs.find("ROOT_" + name) == this->kvs.end()) {
				continue;
			}
			this->validUsers.push_back(name);
		}
	}
	log.info("initial %d user", this->validUsers.size());
}
} /* namespace centny */
