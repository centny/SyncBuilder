/*
 * SyncDemo.h
 *
 *  Created on: Nov 18, 2012
 *      Author: Scorpion
 */

#ifndef SMGR_SYNCDEMO_H_
#define SMGR_SYNCDEMO_H_
#include "../ProtocolAdapter/FtpAdapter.h"
#include "../ProtocolAdapter/SyncAdapter.h"
#include "../ProtocolAdapter/LocAdapter.h"
#include "../ProtocolAdapter/NetCfg.h"
#include "../ProtocolAdapter/SyncBindCmd.h"
#include "../EventMgr/EventCfg.h"
#include "SyncMgr.h"
#include <sqlite3.h>
namespace centny {
namespace SMgr {
//
#define TNAME "FINFO"
//location 0:local,1:remote.
//type 0:normal file,1:folder.
//state 0:normal,1:update,2:delete,3:new
#define CREATE_TABLE "\
CREATE TABLE IF NOT EXISTS FINFO(\
 CWD VARCHAR(255),\
 LOCATION INT,\
 PARENT VARCHAR(255),\
 MTIME VARCHAR(20) NOT NULL,\
 NAME VARCHAR(255) NOT NULL,\
 TYPE INT NOT NULL,\
 SIZE VARCHAR(20) NOT NULL,\
 STATE INT NOT NULL,\
 PRIMARY KEY (CWD,LOCATION)\
)"

class SyncDemo {
private:
	NetCfg *ncfg;
	EventCfg *ecfg;
	NetAdapterBase *net;
	LocAdapter *loc;
	SyncBindCmd *bcmd;
	EventMgr::EventMgrId mid;
	SyncMgr *mgr;
	Log log;
	boost::thread *thr;
	sqlite3 *db;
	string ncf;
	string ecf;
public:
	SyncDemo(string ncf, string ecf);
	virtual ~SyncDemo();
public:
	static SyncDemo* createDemo(string ncf, string ecf);
	static int demoes();
	static void fre(SyncDemo* sd = 0);
private:
	void checkDbTable();
public:
	void run();
	void initBCmd(io_service& ios);
};
}
} /* namespace centny */
#endif /* SYNCDEMO_H_ */
