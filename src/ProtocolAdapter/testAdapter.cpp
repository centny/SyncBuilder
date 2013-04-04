/*
 * testAdapter.cpp
 *
 *  Created on: Nov 15, 2012
 *      Author: Scorpion
 */
#include "LocAdapter.h"
#include "FtpAdapter.h"
#include "SyncAdapter.h"
using namespace centny;
void showFInfo(FInfo* fi) {
//	if (fi->name != "abc") {
	cout << fi->name << "\t" << fi->cwd << endl;
//	}
	if (!fi->isDir()) {
		return;
	}
	vector<FInfo*>::iterator it, end;
	end = fi->subs().end();
	for (it = fi->subs().begin(); it != end; it++) {
		showFInfo(*it);
	}
}
void testLocAdapter() {
//	LocAdapter loc("/Users/Scorpion/sbtmp");
//	FInfo *fi = loc.root();
//	showFInfo(fi);
//	string name = "11111111";
//	fi->mkdir(name);
//	FInfo *s = fi->contain(name);
//	if (s) {
//		cout << "created:" << s->cwd << endl;
//	}
//	showFInfo(fi);
}
void testFtpAdapter() {
//	string ncfp = "src/ProtocolAdapter/Net.cfg";
//	NetCfg cp(ncfp);
//	FtpAdapter ftp(&cp);
//	FInfo *fi = ftp.root();
//	fi->subs();
//	showFInfo(fi);
//	string name = "11111111";
//	fi->mkdir(name);
//	FInfo *s = fi->contain(name);
//	if (s) {
//		cout << "created:" << s->cwd << endl;
//	}
//	showFInfo(fi);
}
void testSyncAdapter() {
//	time_t t;
//	t = boost::filesystem::last_write_time(
//			boost::filesystem::path("/Users/Scorpion/sbtmp/4.txt"));
//	cout << "t:" << t << endl;
//	t = boost::filesystem::last_write_time(
//			boost::filesystem::path("/Users/Scorpion/tmp/4.txt"));
//	cout << "t:" << t << endl;
//	string ncfp = "src/ProtocolAdapter/Net.cfg";
//	NetCfg cp(ncfp);
//	SyncAdapter sa(&cp);
//	sa.login();
//	NetFInfo *fi = (NetFInfo*) sa.root();
//	vector<FInfo*> sub = fi->subs();
//	vector<FInfo*>::iterator it, end;
//	for (it = sub.begin(), end = sub.end(); it != end; it++) {
//		NetFInfo *s = (NetFInfo*) (*it);
////		cout << s->name << endl;
//		if (s->type == 'd') {
//			continue;
//		}
//		s->download("/Users/Scorpion/tmp/" + s->name);
//	}
//	fi->upload("src/ProtocolAdapter/Net.cfg", "n.cfg");

//	showFInfo(fi);
	//sleep(10);
}
void testAdapter() {
//	testLocAdapter();
//	testFtpAdapter();
	testSyncAdapter();
}

