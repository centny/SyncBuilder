/*
 * DemoCfg.h
 *
 *  Created on: Apr 26, 2013
 *      Author: Scorpion
 */

#ifndef DEMOCFG_H_
#define DEMOCFG_H_
#include <string>
#include <vector>
#include <map>
#include "../CfgParser/CfgParser.h"

using namespace std;
namespace centny {

class DemoCfg: public CfgParser {
public:
	vector<string> names;
private:
	map<string, pair<string, string> > demo_cfgs;
	Log log;
public:
	DemoCfg(string& cfgPath);
	string serveCfg(string name);
	string eventCfg(string name);
};
} /* namespace centny */
#endif /* DEMOCFG_H_ */
