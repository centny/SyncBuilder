///*
// * BindCmdMgr.h
// *
// *  Created on: Dec 5, 2012
// *      Author: Scorpion
// */
//
//#ifndef BINDCMDMGR_H_
//#define BINDCMDMGR_H_
//
//#include "ConClient.h"
//#include "FileCmdMgr.h"
//namespace centny {
//class HttpCmdMgr;
//class BindCmdMgr: public CmdBase {
//	SyncServer *server;
//	FileCmdMgr *fcm;
//	HttpCmdMgr *hcm;
//	map<string, ConClient*> s2bc; //session to bind client map.
//public:
//	BindCmdMgr(FileCmdMgr* fcm);
//	virtual ~BindCmdMgr();
//	virtual void setHttpCmdMgr(HttpCmdMgr* hcm);
//	virtual ConClient* client(string s);
//	virtual void setServer(SyncServer* s);
//	virtual void shutdown(ConClient* c);
//	virtual void execCmd(ConClient* c,std::istream* isbuf);
//	virtual void sendCmd(vector<string> cmds, ConClient* http);
////	virtual size_t log(ConClient* c, string name, string event, size_t len);
//};
//
//} /* namespace centny */
//#endif /* BINDCMDMGR_H_ */
