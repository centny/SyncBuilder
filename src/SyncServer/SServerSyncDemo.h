/*
 * SyncDemo.h
 *
 *  Created on: Apr 26, 2013
 *      Author: Scorpion
 */

#ifndef SSERVER_SYNCDEMO_H_
#define SSERVER_SYNCDEMO_H_
#include "SyncServer.h"
#include "FileCmdMgr.h"
#include "HttpCmdMgr.h"
#include "BindCmdMgr.h"
#include "ShellCmdMgr.h"
#include <boost/array.hpp>
#include <boost/filesystem.hpp>
namespace centny {
struct SServerDemo {
	string name;
	unsigned int fport;
	unsigned int sport;
	string fucfg;
	string sucfg;
	string swcfg;
};
class SServerSyncDemo {
private:
	FileCmdMgr* fcm;
	SyncServer* fss;
	ShellCmdMgr* scm;
	SyncServer* sss;
public:
	static SServerSyncDemo* createDemo(io_service& ios, SServerDemo* d);
	static int demoes();
	static void fre(SServerSyncDemo* sd = 0);
public:
	SServerSyncDemo(io_service& ios, SServerDemo* d);
	virtual ~SServerSyncDemo();
};
} /* namespace centny */
#endif /* SYNCDEMO_H_ */
