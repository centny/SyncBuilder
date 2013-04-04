/*
 * FtpAdapter.h
 *
 *  Created on: Nov 10, 2012
 *      Author: Scorpion
 */

#ifndef FTPADAPTER_H_
#define FTPADAPTER_H_
#include "AdapterBase.h"
#include "CurlBuilder.h"
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include "../common.h"
#include <boost/filesystem.hpp>
#include <boost/date_time.hpp>
#include "NetCfg.h"
#include "../log/LogFactory.h"
namespace centny {
namespace fs = boost::filesystem;
using namespace file;
using namespace boost::posix_time;
class FtpAdapter: public NetAdapterBase {
public:
	static void setFileInfo(FInfo* fi, string fp);
private:
	CurlBuilder curl;
	Log log;
public:
public:
	FtpAdapter(sqlite3 *db, string rurl, string usr = "", string pwd = "");
	FtpAdapter(sqlite3 *db, NetCfg *ncfg);
	virtual ~FtpAdapter();
	void init(string rurl, string usr = "", string pwd = "");
	virtual string usrpwd();
	virtual FInfo* createRootNode();
	virtual vector<FInfo*> listSubs(FInfo* parent);
	virtual vector<FInfo*> convert(FInfo* parent, stringstream& buf);
	virtual FInfo* convertOne(FInfo* parent, string line);
	virtual bool uploadSupported();
	virtual bool downloadSupported();
//	virtual const char* upload(FInfo* fi, string lf, string name);
//	virtual const char* upload(FInfo* fi, istream& is, string name);
	virtual const char* download(FInfo* fi, string lf);
	virtual const char* download(FInfo* fi, ostream& os);
	virtual void mkdir(FInfo* fi, string name);
	virtual bool remove(FInfo* fi, string name = "");
	virtual FInfo* contain(FInfo*fi, string name);
	virtual bool reinit();
	virtual void noop();
private:
	void setUsrPwd();
};

} /* namespace centny */
#endif /* FTPADAPTER_H_ */
