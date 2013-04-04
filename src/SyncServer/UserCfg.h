/*
 * UserCfg.h
 *
 *  Created on: Dec 3, 2012
 *      Author: Scorpion
 */

#ifndef USERCFG_H_
#define USERCFG_H_

#include "../CfgParser/CfgParser.h"

namespace centny {

class UserCfg: public CfgParser {
private:
	Log log;
public:
	vector<string> validUsers;
	map<string, string> n2u; //name to user map.
public:
	UserCfg(string& cfgPath);
	virtual ~UserCfg();
	string pass(string user);
	string root(string user);
private:
	void initAllUser();
};

} /* namespace centny */
#endif /* USERCFG_H_ */
