//============================================================================
// Name        : SyncBuilder.cpp
// Author      : Centny
// Version     :
// Copyright   : Sync Builder
// Description : Hello World in C++, Ansi-style
//============================================================================
#include "SyncBuilder.h"
using namespace centny;
namespace centny {
	SyncBuilder* __sb__ = 0;
	SyncBuilder::SyncBuilder() {

	}
	SyncBuilder::~SyncBuilder() {

	}
	void SyncBuilder::help() {
		printf("Usage:cmd [options]\n"
			"\t-C \t\t run as client.\n"
			"\t-S \t\t run as server.\n"
			"\t-l path \t the log configure file.\n"
			"\t-c path \t the normal configure file\n"
			"\t-h \t\t show this useage\n");
	}
	SyncBuilder* SyncBuilder::create(int argc, char** argv) {
		//fstream fs;
		//fs.open("D:\\t.log",ios::out);
		//for(int i=0;i<argc;i++){
		//	fs<<string(argv[i])<<endl;
		//}
		//fs.close();
		fre();
		string lcfg;
		string ncfg;
		bool hc, hs;
		hc = hs = false;
		if (argc > 1) {
			xoptind = 0;
			int ch = 0;
			while ((ch = xgetopt(argc, argv, "CShn:l:c:")) != EOF) {
				switch (ch) {
				case 'l':
					lcfg = string(xoptarg);
					break;
				case 'c':
					ncfg = string(xoptarg);
					break;
				case 'C':
					hc = true;
					break;
				case 'S':
					hs = true;
					break;
				case 'h':
					help();
					exit(0);
					break;
				}
			}
		} else {
			help();
			exit(0);
		}
		if (ncfg.length() < 1) {
			help();
			exit(0);
		}
		if (lcfg.length() < 1) {
			LogFactory::init();
		} else {
			LogFactory::init(lcfg);
		}
		if (hc) {
			__sb__ = new SMgrBuilder();
		} else if (hs) {
			__sb__ = new SServeBuilder();
		} else {
			help();
			exit(0);
		}
		__sb__->init(ncfg);
		return __sb__;
	}
	SyncBuilder* SyncBuilder::demo() {
		return __sb__;
	}
	void SyncBuilder::fre() {
		if (__sb__) {
			delete __sb__;
		}
	}
	//
	SMgrBuilder::SMgrBuilder() :
		dcfg(0), log(C_LOG("SMgrBuilder")) {

	}
	SMgrBuilder::~SMgrBuilder() {
		if (dcfg) {
			delete dcfg;
		}
	}
	void SMgrBuilder::init(string cfg) {
		dcfg = new SMgrDemoCfg(cfg);
		if (!dcfg->valid) {
			log.error("demo configure error:%s", dcfg->msg.c_str());
			delete dcfg;
			dcfg = 0;
			assert("invalid demo configure"==0);
		}
		if (dcfg->names.size() < 1) {
			log.error("can't find demo configure");
			delete dcfg;
			dcfg = 0;
			assert("invalid demo configure"==0);
		}
		this->log.info("initial success");
	}
	void SMgrBuilder::run() {
		if (this->dcfg == NULL) {
			assert( "the demo configure is not initialed,"
				"please run initSyncBuilder first"==0);
		}
		size_t dsize = this->dcfg->names.size();
		if (dsize < 1) {
			log.error("at last specified one demo");
			return;
		} else {
			log.info("find %d demo configure", dsize);
		}
		NoticeTimer::defaultNoticeTimer().initTimer(this->ios);
		NoticeCenter::defaultCenter();
		for (size_t i = 0; i < dsize; i++) {
			string dname = this->dcfg->names[i];
			SMgrSyncDemo* sd;
			sd=SMgrSyncDemo::createDemo(this->dcfg->serveCfg(dname),
				this->dcfg->eventCfg(dname));
			sd->initBCmd(this->ios);
			NoticeTimer::defaultNoticeTimer().reg(sd->gbcm());
		}
		this->ios.run();
		log.info("service will stop");
		NoticeTimer::fre();
		NoticeCenter::fre();
		SMgrSyncDemo::fre();
		delete this->dcfg;
		this->dcfg = 0;
		log.info("service stopped");
	}
	void SMgrBuilder::stop() {
		this->ios.stop();
	}
	SServeBuilder::SServeBuilder() :
		dcfg(0), log(C_LOG("SServeBuilder")) {

	}
	SServeBuilder::~SServeBuilder() {
		if (dcfg) {
			delete dcfg;
		}
	}
	void SServeBuilder::init(string cfg) {
		dcfg = new SServerDemoCfg(cfg);
		if (!dcfg->valid) {
			log.error("demo configure error:%s", dcfg->msg.c_str());
			delete dcfg;
			dcfg = 0;
			assert("invalid demo configure"==0);
		}
		if (dcfg->names.size() < 1) {
			log.error("can't find demo configure");
			delete dcfg;
			dcfg = 0;
			assert("invalid demo configure"==0);
		}
		this->log.info("initial success");
	}
	void SServeBuilder::run() {
		if (this->dcfg == NULL) {
			assert( "the demo configure is not initialed,"
				"please run initSyncBuilder first"==0);
		}
		size_t dsize = this->dcfg->names.size();
		if (dsize < 1) {
			log.error("at last specified one demo");
			return;
		} else {
			log.info("find %d demo configure", dsize);
		}
		for (size_t i = 0; i < dsize; i++) {
			string dname = this->dcfg->names[i];
			SServerSyncDemo::createDemo(this->ios, this->dcfg->demo(dname));
		}
		try {
			this->ios.run();
		} catch (...) {

		}
		log.info("service will stop");
		SServerSyncDemo::fre();
		delete this->dcfg;
		this->dcfg = 0;
		log.info("service stopped");
	}
	void SServeBuilder::stop() {
		this->ios.stop();
	}
}
#ifdef WIN32
int initService(int argc, char** argv){
	SyncBuilder::create(argc,argv);
	return 0;
}
int runService(int argc, char** argv){
	SyncBuilder::demo()->run();
	SyncBuilder::fre();
	return 0;
}
int stopService(int argc, char** argv){
	SyncBuilder::demo()->stop();
	return 0;
}
#endif
