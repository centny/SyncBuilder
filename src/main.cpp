/*
* main.cpp
*
*  Created on: Nov 4, 2012
*      Author: Scorpion
*/

#include <iostream>
#include <fstream>
#include "log/LogFactory.h"
#include "Common/SyncFstream.h"
#include "SyncBuilder.h"
#include <signal.h>
#include "common.h"
#include "xgetopt.h"
#include <signal.h>
#ifdef WIN32
#include "WinServcie/WinService.h"
#include "WinServcie/WinTools.h"
#endif
using namespace std;
//
#if DEV_NO_SERVICE
void testParser();
void testDataPool();
void testEventMgr();
void testCurl();
void testAdapter();
void testSyncMgr();
void testSyncDemo();
void testSyncBuilder();
void testSyncServer();
void testShellCmdServer();
void testCmdBase();
void testSocketServer();
void devTest(int argc, char** argv);
#endif
using namespace centny;
void printMainHelp() {
#ifdef WIN32
	printf("Usage:cmd <option>\n"
		"\t-i  install service\n"
		"\t-u  uninstall service\n"
		"\t-n name  service name\n"
		"\t-l path  the log configure file path\n"
		"\t-c path  the demo configure  file path\n"
		"\t-h show this message\n");

#else
	SyncBuilder::help();
#endif
}
#ifndef WIN32
void receiveKillSignal(int s) {
	SyncBuilder::demo()->stop();
	printf("receive kill signal:%d\n", s);
}
void registerKillSignal() {
	signal(SIGTERM, receiveKillSignal);
	signal(SIGINT, receiveKillSignal);
	signal(SIGKILL, receiveKillSignal);
	signal(SIGSTOP, receiveKillSignal);
	signal(SIGHUP, receiveKillSignal);
	signal(SIGQUIT, receiveKillSignal);
}
#endif
int main(int argc, char** argv) {
#if DEV_NO_SERVICE
	devTest(argc, argv);
#else

#ifdef WIN32
	string lcfg,ncfg,sname;
	bool hc,hs;
	hc=hs=false;
	int cmd_action=0;
	int ch;
	while ((ch = xgetopt(argc, argv, "iuhn:l:c:CS")) != EOF) {
		switch (ch) {
		case 'l':
			lcfg = string(xoptarg);
			break;
		case 'c':
			ncfg = string(xoptarg);
			break;
		case 'n':
			sname=string(xoptarg);
			break;
		case 'i':
			cmd_action=1;
			break;
		case 'u':
			cmd_action=2;
			break;
		case 'C':
			break;
		case 'h':
			printMainHelp();
			return 0;
		}
	}
	switch(cmd_action) {
	case 1: //install
		{
			if(lcfg.size()<1||ncfg.size()<1||sname.size()<1) {
				printMainHelp();
				return 0;
			}
			vector<string> args;
			string epath=ModuleFullPath();
			if(hc){
				args.push_back("-C");
			}else if(hs){
				args.push_back("-S");
			}else{
				printf("-C or -S must be setted\n");
				exit(0);
			}
			args.push_back("-n");
			args.push_back(sname);
			args.push_back("-l");
			args.push_back(lcfg);
			args.push_back("-c");
			args.push_back(ncfg);
			InstallService(sname,epath,args);
			break;
		}
	case 2: //uninstall
		{
			if(sname.size()<1) {
				printMainHelp();
				return 0;
			}
			UnInstallService(sname);
			break;
		}
	case 0:
		{
			if(lcfg.size()<1||ncfg.size()<1||sname.size()<1) {
				printMainHelp();
				return 0;
			}
			SetServicesInitMethod(&initService);
			SetServicesMainMethod(&runService);
			SetServicesStopMethod(&stopService);
			RunService(sname,argc,argv);
		}
		break;
	}
#else
	if (argc < 4) {
		printMainHelp();
		exit(0);
	}
#ifndef DEV_IN_ECLIPSE
	int pid = fork();
	if (pid > 0) {
		printf("%d", pid);
		exit(0);
	}
#endif
	fstream lfs("/tmp/SyncBuilderLock", ios::out);
	assert(lfs.is_open());
	lfs << getpid() << endl;
	registerKillSignal();
	SyncBuilder::create(argc, argv);
	SyncBuilder::demo()->run();
	lfs.close();
	std::system("rm -f /tmp/SyncBuilderLock");
#endif
#endif
	return 0;
}
#if DEV_NO_SERVICE
void killsig(int s) {
	printf("receive kill signal:%d\n", s);
	SyncBuilder::demo()->stop();
}
void registerSignal() {
	signal(SIGTERM, killsig);
	signal(SIGINT, killsig);
#ifdef WIN32
	signal(SIGBREAK,killsig);
#else
	signal(SIGKILL, killsig);
	signal(SIGSTOP, killsig);
	signal(SIGHUP, killsig);
	signal(SIGQUIT, killsig);
#endif
}
void devTest(int argc, char** argv) {
	LogFactory::init();
	//	registerSignal();
	//	initService(argc,argv);
	//	runService(argc,argv);

	//	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	//	testParser();
	//	testDataPool();
	//	testEventMgr();
	//	testCurl();
	//	testSocketServer();
	//	testShellCmdServer();
	//	testAdapter();
	//	testSyncMgr();
	//	testSyncBuilder();
	//	testCmdBase();
#if RUN_SERVER
	testSyncServer();
#else
	testSyncDemo();
#endif
}
void t() {
	testParser();
	testDataPool();
	testEventMgr();
	testCurl();
	testAdapter();
	testSyncMgr();
	testSyncDemo();
	testSyncBuilder();
	testSyncServer();
	testCmdBase();
	testShellCmdServer();
	testSocketServer();
}

void testSyncBuilder() {
#ifdef WIN32
	//	initSyncBuilder("Demoes.cfg");
#else
	//	initSyncBuilder("src/Demoes.cfg");
#endif
	//	runSyncBuilder();
	bsleep(100000);
	cout << "-----------run end-----------" << endl;
}
#endif

