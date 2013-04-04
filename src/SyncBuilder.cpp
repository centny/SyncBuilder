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
using namespace centny;
#define DEMO_SERVE_CFG "DEMO_SERVE_CFG_"
#define DEMO_EVENT_CFG "DEMO_EVENT_CFG_"
class DemoCfg: public CfgParser {
public:
	vector<string> names;
private:
	map<string, pair<string, string> > demo_cfgs;
	Log log;
public:
	DemoCfg(string& cfgPath);
	string serveCfg(string name);
	string eventCfg(string name);
};
DemoCfg::DemoCfg(string& cfgPath) :
		CfgParser(cfgPath), log(C_LOG("DemoCfg")) {
			map<string, string>::iterator it, fit, end;
			end = this->kvs.end();
			size_t clen = std::strlen(DEMO_SERVE_CFG);
			for (it = this->kvs.begin(); it != end; it++) {
				if (it->first.size() <= clen) {
					continue;
				}
				string w, name;
				w = it->first.substr(0, clen);
				if (w != DEMO_SERVE_CFG) {
					continue;
				}
				name = it->first.substr(clen);
				fit = this->kvs.find(DEMO_EVENT_CFG + name);
				if (fit == end) {
					continue;
				}
				this->demo_cfgs[name] = pair<string, string>(it->second, fit->second);
			names.push_back(name);
		}
		log.info("run %d demo for configure;%s",names.size(),cfgPath.c_str());
}
string DemoCfg::serveCfg(string name) {
	pair<string, string> p = this->demo_cfgs[name];
	return p.first;
}
string DemoCfg::eventCfg(string name) {
	pair<string, string> p = this->demo_cfgs[name];
	return p.second;
}

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
//void postTestNotice() {
//	bsleep(5000);
//	NoticeCenter::defaultCenter().post("ABC");
//}
int MAIN_THR_STOP = 1;
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
	for (size_t i = 0; i < dsize; i++) {
		string dname = _dcfg->names[i];
		SyncDemo::createDemo(_dcfg->serveCfg(dname), _dcfg->eventCfg(dname));
	}
	//thread thr(postTestNotice);
	MAIN_THR_STOP = 0;
	while (SyncDemo::demoes()) {
		if (MAIN_THR_STOP) {
			break;
		}
		if (NoticeCenter::defaultCenter().handle()) {
			continue;
		} else {
			bsleep(500);
		}
	}
	MAIN_THR_STOP = 1;
	log.info("SyncBuilder service will stop");
	SyncDemo::fre();
	log.info("SyncBuilder service stopped");
}

void stopSyncBuilder() {
	MAIN_THR_STOP = 1;
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
