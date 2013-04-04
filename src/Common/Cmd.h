/*
 * Cmd.h
 *
 *  Created on: Mar 27, 2013
 *      Author: Scorpion
 */

#ifndef CMD_H_
#define CMD_H_
#include <string>
#include <vector>
#include "../common.h"
using namespace std;
namespace centny {

class Cmd {
public:
	vector<string> cmds;
	string seq;	//default is " \n\t\r".
	string eoc; //default is DEFAULT_EOC.
public:
	Cmd();
	virtual ~Cmd();
	void setSeq(string s);
	void setEoc(string e);
	virtual bool initCmd(string data);
	virtual string cmd();
};

} /* namespace centny */
#endif /* CMD_H_ */
