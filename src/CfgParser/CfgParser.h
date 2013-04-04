/*
 * CfgParser.h
 *
 *  Created on: Nov 4, 2012
 *      Author: Scorpion
 */

#ifndef CFGPARSER_H_
#define CFGPARSER_H_
#include <iostream>
#include <map>
#include <vector>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include "../common.h"
using namespace std;

namespace centny {

//the configure parser to parser the event configure
//and server configure.
class CfgParser {
protected:
	//private fields.
	Log log;
	map<string, string> kvs;		//the all key/value map.
	boost::mutex cfgLock;
#define CFG_SLOCK boost::mutex::scoped_lock l(cfgLock);
public:
	//public fields.
	string cfgPath;				//the configure file path.
	bool valid;					//if the configure valid.
	string msg;					//the configure error message.
private:
	void init(basic_istream<char>& s);
public:
	CfgParser(string& cfgPath);
	CfgParser(basic_istream<char>& cfgStream);
	virtual ~CfgParser();
	string value(string key);
};

} /* namespace centny */
#endif /* CFGPARSER_H_ */
