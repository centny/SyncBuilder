/*
 * CfgParserTest.cpp
 *
 *  Created on: Nov 4, 2012
 *      Author: Scorpion
 */
#include "CfgParser.h"
using namespace centny;
//
void testParser(){
#ifdef WIN32
	string str="CfgParser\\e.cfg";
#else
	string str="src/CfgParser/e.cfg";
#endif
	CfgParser cp(str);
	if(!cp.valid){
		cout<<cp.msg;
	}
	printf("------------------------------testParser------------------------------\n");
}
