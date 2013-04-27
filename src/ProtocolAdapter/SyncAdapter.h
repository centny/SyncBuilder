/*
 * SyncAdapter.h
 *
 *  Created on: Dec 4, 2012
 *      Author: Scorpion
 */

#ifndef SYNCADAPTER_H_
#define SYNCADAPTER_H_

#include "AdapterBase.h"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/filesystem.hpp>
#include "../common.h"
#include <boost/filesystem.hpp>
#include <boost/date_time.hpp>
#include "NetCfg.h"
#include "../log/LogFactory.h"
#include "../SyncServer/ConClient.h"
namespace centny {
using namespace boost;
using namespace boost::asio;
using namespace std;
using namespace file;
class SyncAdapter: public NetAdapterBase {
public:
	static void setFileInfo(FInfo* fi, string fp);
private:
	Log log;
	io_service iosev;
	boost::shared_ptr<ip::tcp::socket> socket;
	boost::system::error_code ec;
	NetCfg *ncfg;
	char buf[R_BUF_SIZE];
//	boost::array<char, BUF_SIZE> rbuf;
	size_t blen;
	int logined;
	vector<string> sinc;
	vector<string> sexc;
public:
	SyncAdapter(sqlite3 *db, NetCfg *ncfg);
	virtual ~SyncAdapter();
	virtual bool isLogined();
	virtual bool reinit();
	virtual bool uploadSupported();
	virtual bool downloadSupported();
	virtual void login();
	virtual bool quit();
	virtual FInfo* createRootNode();
	//the return FInfo* object will be auto free by call fre().
	virtual vector<FInfo*> listSubs(FInfo* parent);
	vector<FInfo*> convert(FInfo* parent, stringstream& dbuf);
	FInfo* convertOne(FInfo* parent, string line);
	virtual void mkdir(FInfo* fi, string name);
	virtual bool remove(FInfo* fi, string name = "");
	virtual const char* upload(FInfo* fi, string lf, string name);
	virtual const char* upload(FInfo* fi, istream& is, size_t len, time_t mtime,
			string name);
	virtual const char* download(FInfo* fi, string lf);
	virtual const char* download(FInfo* fi, ostream& os);
	virtual string absUrl(FInfo* tg);
};
} /* namespace centny */
#endif /* SYNCADAPTER_H_ */
