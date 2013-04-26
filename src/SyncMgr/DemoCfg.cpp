/*
 * DemoCfg.cpp
 *
 *  Created on: Apr 26, 2013
 *      Author: Scorpion
 */

#include "DemoCfg.h"

namespace centny {
namespace SMgr {
#define DEMO_SERVE_CFG "DEMO_SERVE_CFG_"
#define DEMO_EVENT_CFG "DEMO_EVENT_CFG_"
DemoCfg::DemoCfg(string& cfgPath) :
		CfgParser(cfgPath), log(C_LOG("DemoCfg")) {
			map<string, string>::iterator it, fit, end;
			end = this->kvs.end();
			size_t clen = std::strlen(DEMO_SERVE_CFG);
			for (it = this->kvs.begin(); it != end; it++) {
				if (it->first.size() <= clen) {
					continue;
				}
				string w, name;
				w = it->first.substr(0, clen);
				if (w != DEMO_SERVE_CFG) {
					continue;
				}
				name = it->first.substr(clen);
				fit = this->kvs.find(DEMO_EVENT_CFG + name);
				if (fit == end) {
					continue;
				}
				this->demo_cfgs[name] = pair<string, string>(it->second, fit->second);
				names.push_back(name);
			}
			log.info("run %d demo for configure;%s",names.size(),cfgPath.c_str());
}
string DemoCfg::serveCfg(string name) {
	pair<string, string> p = this->demo_cfgs[name];
	return p.first;
}
string DemoCfg::eventCfg(string name) {
	pair<string, string> p = this->demo_cfgs[name];
	return p.second;
}
}
} /* namespace centny */
