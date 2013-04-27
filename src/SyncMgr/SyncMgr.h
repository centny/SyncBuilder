/*
 * SyncMgr.h
 *
 *  Created on: Nov 14, 2012
 *      Author: Scorpion
 */

#ifndef SMGR_SYNCMGR_H_
#define SMGR_SYNCMGR_H_
#include "../ProtocolAdapter/AdapterBase.h"
#include "../CfgParser/CfgParser.h"
#include "../EventMgr/EventMgr.h"
#include "../log/LogFactory.h"
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include "../common.h"
#include "../ProtocolAdapter/NetCfg.h"
#include "../Notice/NoticeCenter.h"
namespace centny {
using namespace file;
using namespace boost;
//

class SyncMgr: public Noticeable {
private:
	time_t ctime;
	bool isUpload;
	bool isDownload;
	Log log;
private:
	AdapterBase *loc;
	NetAdapterBase *net;
	NetCfg *cfg;
//	thread *thr;
	bool stopped;
	boost::shared_mutex stop_mutex;
	boost::mutex end_mutex;
	bool noticed;
	boost::shared_mutex notice_mutex;
	EventMgr::EventMgrId emi;
	EventMgr *emgr;
public:
	SyncMgr(AdapterBase *loc, NetAdapterBase *net, NetCfg *cfg,
			EventMgr::EventMgrId emi);
	virtual ~SyncMgr();
	void sync();
//	void remove(FInfo * tf, vector<FInfo*>& fis);
	void receive(string name, DataPool::DId did);
private:
	void sync(FInfo* locf, NetFInfo* netf);
	void syncUp(FInfo* locf, NetFInfo* netf);
//	void syncUpNetSub(FInfo* locf, NetFInfo* parent);
	void syncDown(NetFInfo* netf, FInfo* locf);
	void syncRemoved(FInfo* locf, NetFInfo* netf);
//	void syncDownLocSub(NetFInfo* netf, FInfo* parent);
//	void syncRemove(FInfo* locf, NetFInfo* netf);
//	void syncRemove(NetFInfo* netf, FInfo* locf);
	int compare(FInfo* locf, NetFInfo* netf);
};
} /* namespace centny */
#endif /* SYNCMGR_H_ */
