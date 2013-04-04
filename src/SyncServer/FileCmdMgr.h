/*
 * FileCmdMgr.h
 *
 *  Created on: Dec 1, 2012
 *      Author: Scorpion
 */

#ifndef FILECMDMGR_H_
#define FILECMDMGR_H_
#include <vector>
#include <boost/thread/mutex.hpp>
#include "SyncServer.h"
#include "LoginCmdBase.h"
using namespace std;
namespace centny {

class FileCmdMgr: public LoginCmdBase {
public:
	static string newSession();
private:
	vector<string> wait_store;
	SyncServer *server;
	Log log;
public:
	FileCmdMgr(string ucfg);
	virtual ~FileCmdMgr();
	virtual void setServer(SyncServer* s);
	//command.
	virtual size_t quit(ConClient* c);
	virtual size_t help(ConClient* c);
	virtual size_t list(ConClient* c, string path);
	virtual size_t mkd(ConClient* c, string path);
	virtual size_t rmd(ConClient* c, string path);
	virtual size_t stor(ConClient* c, string path, long flen, time_t mtime);
	virtual size_t down(ConClient* c, string path);
	virtual size_t contain(ConClient* c, string path, string name);
	virtual void execCmd(ConClient* c,std::istream* isbuf);
};

} /* namespace centny */
#endif /* FILECMDMGR_H_ */
