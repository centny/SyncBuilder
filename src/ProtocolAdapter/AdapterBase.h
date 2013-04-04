/*
 * AdapterBase.h
 *
 *  Created on: Nov 10, 2012
 *      Author: Scorpion
 */

#ifndef AdapterBase_H_
#define AdapterBase_H_
#include <errno.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <assert.h>
#include <map>
#include <set>
#include <sqlite3.h>
#include "../Common/ComDef.h"
using namespace std;
namespace centny {
namespace file {
//
const int MONTHES_SIZE = 12;
extern const char * MONTHES[MONTHES_SIZE];
//
class AdapterBase;
class NetAdapterBase;
//
class FInfo {
public:
	char type;
	unsigned long long size;
	time_t mtime;
	string name;
	string cwd;
	AdapterBase *cb;
	FInfo *parent;
protected:
	vector<FInfo*> _subs;
private:
//	map<string, FInfo*>* cwd2f;
public:
	FInfo(AdapterBase *cb, FInfo *parent = 0);
	virtual ~FInfo();
	//
	virtual vector<FInfo*>& subs();
	virtual void showInfo();
	virtual void showAll();
	virtual bool isDir();
	//refresh the sub file info.
	virtual void refreshSubs();
	virtual void mkdir(string name);
	virtual bool remove(string name);
	virtual FInfo* contain(string name);
	virtual int update2Db();
	virtual int update2Normal();
	virtual int remove2Db();
	virtual void delDb();
	virtual bool isDbRemoved();
	virtual set<pair<string, string> > removedSubs();
	virtual bool isNew();
	virtual bool isUpdated();
	virtual bool isNewOrUpdated();
	//the subclass must inherit fre() function.
	//it will be used to free this object.
	virtual void fre();
//	virtual void addToMap(FInfo* fi);
	virtual void freSubs();
//	virtual FInfo* finfo(string cwd = F_SEQ);
	//
	virtual string absUrl();
//	virtual void showCwd2f();
};
//
class NetFInfo: public FInfo {
public:
//	int fidx;
public:
	NetFInfo(NetAdapterBase *cb, NetFInfo *parent = 0);
	virtual ~NetFInfo();
	virtual const char* upload(string lf, string name);
	virtual const char* upload(istream& is, size_t len, string name);
	virtual const char* download(string of);
	virtual const char* download(ostream& os);
	virtual int netstate();
};
//
class AdapterBase {
protected:
	string rurl;
	FInfo* last;
//	set<string> should_removed;
//	set<string> set_a, set_b;
//	set<string>* p_cwd;
//	set<string>* l_cwd;
	sqlite3 *db;
	char buf[10240];
//	FInfo* pre;
protected:
public:
	AdapterBase(sqlite3 *db);
	virtual ~AdapterBase();
	//create the root FInfo by root URL.
	//it must free all old FInfo* in cwd2f map by calling fre().
	FInfo* root();
	string rootUrl();
	vector<FInfo*> subs(FInfo* parent);
	//
//	virtual void freSubs(FInfo* parent);
	virtual FInfo* contain(FInfo* fi, string name);
	//
	virtual int update2Db(FInfo* caller);
	virtual int delDb(FInfo* caller, string cwd);
	virtual bool isDbRemoved(FInfo* caller, string cwd);
	virtual int updateState2DbNot(FInfo* caller, string cwds, int state);
	virtual int updateState2Db(FInfo* caller, int state);
	virtual set<pair<string, string> > snames2Db(FInfo* caller, int state);
	virtual bool isDbState(FInfo* caller, string states);
	virtual int delDbParent(string cwd);
	//
	int update2Db(FInfo* caller, int location);
	bool isDbExist(FInfo* caller, string cwd, int location);
	int delDb(FInfo* caller, string cwd, int location);
	int delDbParent(string cwd, int location);
	bool isDbRemoved(FInfo* caller, string cwd, int location);
	int updateState2DbNot(FInfo* caller, string cwds, int state, int location);
	int updateState2Db(FInfo* caller, int state, int location);
	set<pair<string, string> > snames2Db(FInfo* caller, int state,
			int location);
	bool isDbState(FInfo* caller, string states, int location);
	//
	virtual FInfo* createRootNode()=0;
	//the return FInfo* object will be auto free by call fre().
	virtual vector<FInfo*> listSubs(FInfo* parent)=0;
	virtual void mkdir(FInfo* fi, string name)=0;
	virtual bool remove(FInfo* fi, string name = "")=0;
	virtual string absUrl(FInfo* tg)=0;
};
//
class NetAdapterBase: public AdapterBase {
public:
	string usr;
	string pwd;
	string name;
	string session;
	int netstate;	//200 is OK,500 is error,other is same of HTTP
public:
	//virtual vector<FInfo*> convert(FInfo* parent, stringstream& buf);
	//virtual FInfo* convertOne(FInfo* parent, string line);
	//if net state is error,reinit will be called.
	NetAdapterBase(sqlite3 *db);
	virtual bool reinit()=0;
	virtual bool uploadSupported()=0;
	virtual bool downloadSupported()=0;
	virtual const char* upload(FInfo* fi, string lf, string name);
	virtual const char* upload(FInfo* fi, istream& is, size_t len, string name);
	virtual const char* download(FInfo* fi, string lf);
	virtual const char* download(FInfo* fi, ostream& os);
	virtual string absUrl(FInfo* tg);
	virtual int update2Db(FInfo* caller);
	virtual int delDb(FInfo* caller, string cwd);
	virtual bool isDbRemoved(FInfo* caller, string cwd);
	virtual int updateState2DbNot(FInfo* caller, string cwds, int state);
	virtual int updateState2Db(FInfo* caller, int state);
	virtual set<pair<string, string> > snames2Db(FInfo* caller, int state);
	virtual bool isDbState(FInfo* caller, string states);
	virtual int delDbParent(string cwd);
};
}
} /* namespace centny */
#endif /* AdapterBase_H_ */
