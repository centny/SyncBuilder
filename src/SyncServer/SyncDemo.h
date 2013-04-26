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
namespace SServer {
struct Demo {
	string name;
	unsigned int fport;
	unsigned int sport;
	string fucfg;
	string sucfg;
	string swcfg;
};
class SyncDemo {
private:
	FileCmdMgr* fcm;
	SyncServer* fss;
	ShellCmdMgr* scm;
	SyncServer* sss;
public:
	static SyncDemo* createDemo(io_service& ios, Demo* d);
	static int demoes();
	static void fre(SyncDemo* sd = 0);
public:
	SyncDemo(io_service& ios, Demo* d);
	virtual ~SyncDemo();
};

} /* namespace SServer */
} /* namespace centny */
#endif /* SYNCDEMO_H_ */
