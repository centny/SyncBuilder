/*
 * testCurl.cpp
 *
 *  Created on: Nov 15, 2012
 *      Author: Scorpion
 */
#include "CurlBuilder.h"
using namespace centny;
void testCurl() {
	CurlBuilder::init();
	CurlBuilder curl;
	curl.showDebug(1L);
//	curl.setURL("ftp://192.168.17.183/sbt/1/");
	curl.setURL("ftp://10.211.55.10:21/");
//	curl.setURL("ftp://10.211.55.10/sss4.txt");
//	curl.setDFile("/tmp/sss4.txt");
//	curl.appHeader("MDTM a.txt 20121115200000");
	curl.setUsrPwd("sco:sco");
//	curl.appHeader("MKD aasfsk");
//	curl.appHeader("CWD 1");
//	curl.setCustomRequest("NLST");
//	curl.perform();
//	curl.cleanHeader();
//	curl.appHeader("CWD ~");
//	curl.appHeader("MDTM 2.txt");
//	curl.appHeader("MDTM 4.txt");
//	curl.appPreHeader("MDTM www.h");
	curl.setCustomRequest("NLST");
//	time_t mdtm=0;
//	unsigned long long fsize=0;
//	curl.finfo(mdtm, fsize);
//	cout << "mdtm:" << mdtm << ",fsize:" << fsize << endl;
//	curl.reset();
//	curl.setURL("ftp://10.211.55.10/4.txt");
//	curl.setUsrPwd("sco:sco");
//	curl.finfo(mdtm, fsize);
//	cout << "mdtm:" << mdtm << ",fsize:" << fsize << endl;
//	curl.setReadCallback();
//	curl.setWriteCallback();
	cout << curl.perform() << endl;
//	cout << curl.text() << endl;
	CurlBuilder::clean();
}

