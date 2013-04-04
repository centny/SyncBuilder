///*
// * HttpCmdMgr.h
// *
// *  Created on: Dec 4, 2012
// *      Author: Scorpion
// */
//
//#ifndef HTTPCMDMGR_H_
//#define HTTPCMDMGR_H_
//
//#include "ConClient.h"
//#include "BindCmdMgr.h"
//namespace centny {
//
//class HttpCmdMgr: public CmdBase {
//private:
//	SyncServer *server;
//	BindCmdMgr* bcm;
//public:
//	HttpCmdMgr(BindCmdMgr* bcm);
//	virtual ~HttpCmdMgr();
//	virtual void execCmd(ConClient* c,std::istream* isbuf);
//	virtual void setServer(SyncServer* s);
//	virtual void response(ConClient* c, string name, int code, string msg);
//	virtual void sendHeader(ConClient* c, int code, size_t len);
//};
//
//} /* namespace centny */
//#endif /* HTTPCMDMGR_H_ */
