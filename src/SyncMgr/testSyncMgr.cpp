/*
 * testSyncMgr.cpp
 *
 *  Created on: Nov 14, 2012
 *      Author: Scorpion
 */
#include "SyncDemo.h"
using namespace centny;
void handler() {
	cout << "handle" << endl;
}
void testSyncMgr() {
//	string ecfp = "src/EventMgr/e.cfg";
//	string ncfp = "src/ProtocolAdapter/Net.cfg";
//	NetCfg cp(ncfp);
//	EventCfg ep(ecfp);
//	FtpAdapter ftp(&cp);
//	LocAdapter loc("/Users/Scorpion/sbtmp");
//	EventMgr::EventMgrId mid = EventMgr::createDemo(&ep);
//	SyncMgr sm(&loc, &ftp, &cp, mid);
//	bsleep(100000);
//	EventMgr::fre();
//	cout << "--------------------------------" << "all end"
//			<< "--------------------------------" << endl;
}
void testSyncDemo() {
	namespace fs=boost::filesystem;
#ifdef WIN32
	string ecfp = "EventMgr/Wine.cfg";
	string ncfp = "ProtocolAdapter/WinNet.cfg";
	if(!fs::exists(fs::path("F:\\Temp\\sbtemp"))){
		fs::create_directories(fs::path("F:\\Temp\\sbtemp"));
	}
#else
	string ecfp = "src/EventMgr/e.cfg";
	string ncfp = "src/ProtocolAdapter/Net.cfg";
	if(!fs::exists(fs::path("/tmp/sync"))){
		fs::create_directories(fs::path("/tmp/sync"));
	}
#endif
//	string ncfp = "src/ProtocolAdapter/Ftp.cfg";
	io_service ios;
	NoticeCenter::defaultCenter().initTimer(ios);
	SyncDemo::createDemo(ncfp, ecfp)->initBCmd(ios);
//	while (1)
//		bsleep(50000);
	ios.run();
	SyncDemo::fre();
	cout << "--------------------------------" << "all end"
			<< "--------------------------------" << endl;
}

