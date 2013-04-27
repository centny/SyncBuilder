/*
 * DemoCfg.h
 *
 *  Created on: Apr 26, 2013
 *      Author: Scorpion
 */

#ifndef SSERVER_DEMOCFG_H_
#define SSERVER_DEMOCFG_H_
#include <string>
#include <vector>
#include <map>
#include "../CfgParser/CfgParser.h"
#include "SServerSyncDemo.h"
namespace centny {
class SServerDemoCfg: public CfgParser {
public:
	vector<string> names;
private:
	map<string, SServerDemo*> demo_cfgs;
	Log log;
public:
	SServerDemoCfg(string& cfgPath);
	SServerDemo* demo(string& name);
	~SServerDemoCfg();
};
} /* namespace centny */
#endif /* DEMOCFG_H_ */
