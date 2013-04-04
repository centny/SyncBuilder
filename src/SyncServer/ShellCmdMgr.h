/*
 * ShellCmdMgr.h
 *
 *  Created on: Mar 27, 2013
 *      Author: Scorpion
 */

#ifndef SHELLCMDMGR_H_
#define SHELLCMDMGR_H_
#include "ConClient.h"
#include "HttpCmdMgr.h"
#include "FileCmdMgr.h"
#include <boost/thread.hpp>
#include "../CfgParser/CfgParser.h"
#include <map>
#include <set>
namespace centny {

class WebCfg: public CfgParser {

public:
	WebCfg(string& cfgPath);
	string rootPath();
	string contentType(string fpath);
};
//
class ShellCmdMgr: public LoginCmdBase {
private:
	Log log;
private:
	map<ConClient*, ConClient*> binded;
	map<string, ConClient*> binding;
	map<ConClient*, boost::timed_mutex*> cmutex;
	boost::mutex cmutex_lock;
	set<ConClient*> http;
	set<string> sessions;
	//map<string, ConClient*> n2c;
	char buf[R_BUF_SIZE];
	size_t blen;
	boost::array<char, R_BUF_SIZE> abuf;
	boost::system::error_code ec;
	WebCfg* wcfg;
	//FileCmdMgr *fcm;
	//map<ConClient*,size_t> tfsize;
public:
	//ShellCmdMgr(WebCfg* wcfg=0);
	ShellCmdMgr(string ucfg, string cfgPath = "");
	virtual ~ShellCmdMgr();
	virtual void sendCmd(vector<string>& cmds, ConClient* tar);
	virtual void execCmd(ConClient* c, std::istream* isbuf);
	virtual void shutdown(ConClient* c);
	virtual void writeMsg(ConClient* c, int code, string msg);
	virtual void response(ConClient* c, string name, int code, string msg);
	virtual void sendHeader(ConClient* c, int code, size_t len, string ctype =
			"text/plain");
	virtual void sendCookie(ConClient* c, int code, size_t len, string cookie,
			string ctype = "text/plain");
private:
	void httpBing(ConClient* c, ConClient* tar, vector<string> cmds);
	void thrHttpBing(ConClient* c, ConClient* tar, vector<string> cmds);
	void cmdBing(ConClient* c, ConClient* tar, vector<string> cmds);
	void thrCmdBing(ConClient* c, ConClient* tar, vector<string> cmds);
	boost::timed_mutex* conMutex(ConClient* c);
	void bind(ConClient* c, std::istream* isbuf);
	void bing(ConClient* c, std::istream* isbuf);
	void get(ConClient* c, std::istream* isbuf);
	void webfile(ConClient* c);
	void sendFile(ConClient* c,string tpath);
	void help(ConClient* c);
	//request remote data if return true ,or false.
	bool list(ConClient* c, vector<string>& args, std::istream* isbuf,
			bool byHtpp);
	void execBindedCmd(ConClient* c, ConClient* tar, std::istream* isbuf);
//	void startTransfter(ConClient* c,ConClient* tar);
//	void readHandle(ConClient* c,ConClient* tar,const boost::system::error_code& ec,std::size_t bytes_transferred);
};

} /* namespace centny */
#endif /* SHELLCMDMGR_H_ */
