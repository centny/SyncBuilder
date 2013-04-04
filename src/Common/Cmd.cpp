/*
 * Cmd.cpp
 *
 *  Created on: Mar 27, 2013
 *      Author: Scorpion
 */

#include "Cmd.h"
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/array.hpp>
namespace centny {

Cmd::Cmd() {
	this->seq = " \n\t\r";
	this->eoc = DEFAULT_EOC;
}

Cmd::~Cmd() {
}
void Cmd::setSeq(string s) {
	this->seq = s;
}
void Cmd::setEoc(string e) {
	this->eoc = e;
}
bool Cmd::initCmd(string data) {
	cmds.clear();
	if (data.size() < 3) {
		return false;
	}
	string cend = data.substr(data.size() - 2);
	if (cend != this->eoc) {
		return false;
	}
	string cbody = data.substr(0, data.size() - 2);
	vector<string> subs;
	boost::split(subs, cbody, is_any_of(this->seq));
	for (size_t i = 0; i < subs.size(); i++) {
		if (subs[i].size()) {
			cmds.push_back(subs[i]);
		}
	}
	return cmds.size()>0;
}
string Cmd::cmd() {
	string cmd;
	for (size_t i = 0; i < cmds.size(); i++) {
		cmd.append(cmds[i] + " ");
	}
	return cmd;
}
} /* namespace centny */
