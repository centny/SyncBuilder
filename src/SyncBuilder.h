/*
 * SyncBuilder.h
 *
 *  Created on: Nov 4, 2012
 *      Author: Scorpion
 */

#ifndef SYNCBUILDER_H_
#define SYNCBUILDER_H_
#include "Notice/NoticeCenter.h"
#include "xgetopt.h"
#include "SyncServer/SServerDemoCfg.h"
#include "SyncMgr/SMgrDemoCfg.h"
#include <boost/asio.hpp>
using namespace std;
using namespace centny;
namespace centny {
class SyncBuilder {
public:
	boost::asio::io_service ios;
public:
	SyncBuilder();
	virtual ~SyncBuilder();
	virtual void init(string cfg)=0;
	virtual void run()=0;
	virtual void stop()=0;
	//
public:
	static void help();
	static SyncBuilder* create(int argc, char** argv);
	static SyncBuilder* demo();
	static void fre();
};
class SMgrBuilder: public SyncBuilder {
private:
	SMgrDemoCfg* dcfg;
	Log log;
public:
	SMgrBuilder();
	virtual ~SMgrBuilder();
	virtual void init(string cfg);
	virtual void run();
	virtual void stop();
};
class SServeBuilder: public SyncBuilder {
private:
	SServerDemoCfg* dcfg;
	Log log;
public:
	SServeBuilder();
	virtual ~SServeBuilder();
	virtual void init(string cfg);
	virtual void run();
	virtual void stop();
};
}
//
//void initSyncBuilder(string cfg);
//void runSyncBuilder();
//void stopSyncBuilder();
////service interface.
//void printHelp();
#ifdef WIN32
int initService(int argc, char** argv);
int runService(int argc, char** argv);
int stopService(int argc, char** argv);
#endif
//
#endif /* SYNCBUILDER_H_ */
