//============================================================================
// Name        : SyncBuilder.cpp
// Author      : Centny
// Version     :
// Copyright   : Sync Builder
// Description : Hello World in C++, Ansi-style
//============================================================================
#include "SyncBuilder.h"
#include "CfgParser/CfgParser.h"
#include "SyncMgr/SyncDemo.h"
#include "Notice/NoticeCenter.h"
#include "xgetopt.h"
#include "SyncMgr/DemoCfg.h"
using namespace centny;

DemoCfg *_dcfg = 0;
void initSyncBuilder(string cfg) {
	Log log = C_LOG("InitSyncBuilder");
	_dcfg = new DemoCfg(cfg);
	if(!_dcfg->valid) {
		log.error("demo configure error:%s",_dcfg->msg.c_str());
		delete _dcfg;
		_dcfg = 0;
		assert("invalid demo configure"==0);
	}
	if (_dcfg->names.size() < 1) {
		log.error("can't find demo configure");
		delete _dcfg;
		_dcfg = 0;
		assert("invalid demo configure"==0);
	}
	log.info("initial SyncBuilder success");
}

io_service* ios__ = 0;
void runSyncBuilder() {
	if (_dcfg == NULL) {
		assert( "the demo configure is not initialed,"
		"please run initSyncBuilder first"==0);
	}
	Log log = C_LOG("SyncBuilder");
	size_t dsize = _dcfg->names.size();
	if (dsize < 1) {
		log.error("at last specified one demo");
		return;
	} else {
		log.info("find %d demo configure", dsize);
	}
	ios__ = new io_service();
	for (size_t i = 0; i < dsize; i++) {
		string dname = _dcfg->names[i];
		SyncDemo::createDemo(_dcfg->serveCfg(dname), _dcfg->eventCfg(dname))->initBCmd(
				*ios__);
	}
	NoticeTimer::defaultNoticeTimer().initTimer(*ios__);
	NoticeCenter::defaultCenter();
	ios__->run();
	log.info("SyncBuilder service will stop");
	NoticeTimer::fre();
	NoticeCenter::fre();
	SyncDemo::fre();
	delete _dcfg;
	_dcfg = 0;
	log.info("SyncBuilder service stopped");
}

void stopSyncBuilder() {
	if (ios__) {
		ios__->stop();
	}
}

void printHelp() {
	printf("Usage:cmd [options]\n"
			"\t-l path \t the log configure file.\n"
			"\t-c path \t the normal configure file\n"
			"\t-h \t\t show this useage\n");
}
int initService(int argc, char** argv) {
	string lcfg;
	string ncfg;
	if (argc > 1) {
		xoptind = 0;
		int ch = 0;
		while ((ch = xgetopt(argc, argv, "hn:l:c:")) != EOF) {
			switch (ch) {
			case 'l':
				lcfg = string(xoptarg);
				break;
			case 'c':
				ncfg = string(xoptarg);
				break;
			case 'h':
				printHelp();
				break;
			}
		}
	} else {
		printHelp();
	}
	if (ncfg.length() < 1) {
		printHelp();
		assert("normal configure not specified"==0);
	}
	if (lcfg.length() < 1) {
		LogFactory::init();
	} else {
		LogFactory::init(lcfg);
	}
	initSyncBuilder(ncfg);
	return 1;
}

int runService(int argc, char** argv) {
	runSyncBuilder();
	return 0;
}
int stopService(int argc, char** argv) {
	stopSyncBuilder();
	return 0;
}
