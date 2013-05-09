/*
 * CfgParser.cpp
 *
 *  Created on: Nov 4, 2012
 *      Author: Scorpion
 */

#include "CfgParser.h"
#include <fstream>
using namespace boost;

namespace centny {
//CfgParser implementation.
void CfgParser::init(basic_istream<char>& s) {
	//read all key value.
	int row = 0;
	while (!s.eof()) {
		string line;
		getline(s, line, '\n');
		row++;
		vector<string> subs;
		boost::split(subs, line, is_any_of("=#"));
		if (subs.size() < 2) {
			continue;
		}
		if (subs[0].size() < 1) {
			continue;
		}
		string name = subs[0];
		string val = subs[1];
		boost::trim(name);
		boost::trim(val);
		if (this->kvs[name].size() > 1) {
			log.warn("the configure \"%s\" in line %d already exist",
					name.c_str(), row);
			continue;
		}
		if (name.size() > 4 && name.substr(0, 4) == "ENV_") {
			string vname = name.substr(4);
			setenv(vname.c_str(), envVal(val).c_str(), true);
		} else {
			this->kvs[name] = envVal(val);
		}
	}
	this->valid = true;
}
//
CfgParser::CfgParser(basic_istream<char>& cfgStream) :
		log(C_LOG("CfgParser")) {
			this->valid = false;
			this->init(cfgStream);
		}
CfgParser::CfgParser(string& cfgPath) :
		log(C_LOG("CfgParser")) {
			this->cfgPath = cfgPath;
			this->valid = false;
			if(!boost::filesystem::is_regular_file(boost::filesystem::path(cfgPath))){
				this->log.error("the configure file %s is not a regular file.",cfgPath.c_str());
				assert("invalid configure file"==0);
			}
			std::ifstream fs(this->cfgPath.c_str());
			if (!fs.is_open()) {
				this->valid = false;
				this->msg = "open configure file:" + this->cfgPath + " error!";
				return;
			}
			this->init(fs);
			fs.close();
			log.info("read %d pair key value in file:%s",this->kvs.size(),cfgPath.c_str());
		}

CfgParser::~CfgParser() {

}
string CfgParser::value(string key) {
	if (this->kvs.find(key) != this->kvs.end()) {
		return this->kvs[key];
	} else {
		return "";
	}
}
string CfgParser::envVal(string val) {
	return boost::regex_replace(val, boost::regex("\\$\\([^\\)]*\\)"),
			CfgParser::envBack);
}
string CfgParser::envBack(
		boost::match_results<std::string::const_iterator> match) {
	string ms = match[0].str();
	ms = boost::regex_replace(ms, boost::regex("[\\$\\(\\)]"), "");
//	cout << "name:" << ms << endl;
	const char* val = getenv(ms.c_str());
//	printf("%s\n", val);
	if (val) {
		return string(val) + "";
	} else {
		return "";
	}
}
} /* namespace centny */
