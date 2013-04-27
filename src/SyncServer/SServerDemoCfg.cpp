/*
 * DemoCfg.cpp
 *
 *  Created on: Apr 26, 2013
 *      Author: Scorpion
 */

#include "SServerDemoCfg.h"

namespace centny {
#define DEMO_FSERVE_PORT "DEMO_FSERVE_PORT_"
#define DEMO_FSERVE_USER "DEMO_FSERVE_USER_"
#define DEMO_SSERVE_PORT "DEMO_SSERVE_PORT_"
#define DEMO_SSERVE_USER "DEMO_SSERVE_USER_"
#define DEMO_SSERVE_WEB  "DEMO_SSERVE_WEB_"
SServerDemoCfg::SServerDemoCfg(string& cfgPath) :
		CfgParser(cfgPath), log(C_LOG("DemoCfg")) {
			map<string, string>::iterator it,fu,sp,su,sw, end;
			end = this->kvs.end();
			size_t clen = std::strlen(DEMO_FSERVE_PORT);
			for (it = this->kvs.begin(); it != end; it++) {
				if (it->first.size() <= clen) {
					continue;
				}
				string w, name;
				w = it->first.substr(0, clen);
				if (w != DEMO_FSERVE_PORT) {
					continue;
				}
				name = it->first.substr(clen);
				fu = this->kvs.find(DEMO_FSERVE_USER + name);
				if (fu == end) {
					continue;
				}
				sp = this->kvs.find(DEMO_SSERVE_PORT + name);
				if (sp == end) {
					continue;
				}
				su = this->kvs.find(DEMO_SSERVE_USER + name);
				if (su == end) {
					continue;
				}
				sw = this->kvs.find(DEMO_SSERVE_WEB + name);
				if (sw == end) {
					continue;
				}
				SServerDemo* d=new SServerDemo();
				d->name=name;
				d->fport=atoi(it->second.c_str());
				d->fucfg=fu->second;
				d->sport=atoi(sp->second.c_str());
				d->sucfg=su->second;
				d->swcfg=sw->second;
				this->demo_cfgs[name] = d;
				names.push_back(name);
			}
			log.info("run %d demo for configure;%s",names.size(),cfgPath.c_str());
		}

SServerDemoCfg::~SServerDemoCfg() {
	map<string, SServerDemo*>::iterator it;
	it = this->demo_cfgs.begin();
	for (; it != this->demo_cfgs.end(); it++) {
		delete it->second;
	}
}
SServerDemo* SServerDemoCfg::demo(string& name) {
	map<string, SServerDemo*>::iterator it = this->demo_cfgs.find(name);
	if (it == this->demo_cfgs.end()) {
		return 0;
	} else {
		return it->second;
	}
}
} /* namespace centny */
