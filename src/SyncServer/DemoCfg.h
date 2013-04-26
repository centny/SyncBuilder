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
#include "SyncDemo.h"
namespace centny {
namespace SServer {
class DemoCfg: public CfgParser {
public:
	vector<string> names;
private:
	map<string, Demo*> demo_cfgs;
	Log log;
public:
	DemoCfg(string& cfgPath);
	Demo* demo(string& name);
	~DemoCfg();
};

} /* namespace SyncServer */
} /* namespace centny */
#endif /* DEMOCFG_H_ */
