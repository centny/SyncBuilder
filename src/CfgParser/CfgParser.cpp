/*
 * CfgParser.cpp
 *
 *  Created on: Nov 4, 2012
 *      Author: Scorpion
 */

#include "CfgParser.h"
#include <fstream>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
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
		this->kvs[name] = val;
		//		cout<<subs[0]<<":"<<subs[1]<<endl;
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
			assert(boost::filesystem::is_regular_file(boost::filesystem::path(cfgPath)));
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
} /* namespace centny */
