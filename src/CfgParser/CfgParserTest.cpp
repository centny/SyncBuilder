/*
 * CfgParserTest.cpp
 *
 *  Created on: Nov 4, 2012
 *      Author: Scorpion
 */
#include "CfgParser.h"
using namespace centny;
//
void testParser() {
#ifdef WIN32
	string str="CfgParser\\e.cfg";
#else
	string str = "src/CfgParser/e.cfg";
#endif
	CfgParser cp(str);
	if (!cp.valid) {
		cout << cp.msg;
	}
//	const char* val = getenv("WWS");
//	printf("%s\n", val);
	cout << cp.value("ccff") << endl;
	printf(
			"------------------------------testParser------------------------------\n");
}
