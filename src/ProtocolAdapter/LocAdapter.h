/*
 * LocAdapter.h
 *
 *  Created on: Nov 13, 2012
 *      Author: Scorpion
 */

#ifndef LOCADAPTER_H_
#define LOCADAPTER_H_
#include "AdapterBase.h"
#include <boost/filesystem.hpp>
#include "../log/LogFactory.h"
#include <set>
namespace centny {
namespace fs = boost::filesystem;
using namespace file;
using namespace boost;
using namespace std;
class LocAdapter;

class LocFInfo: public FInfo {
public:
	LocFInfo(LocAdapter *cb, LocFInfo *parent = 0);
	virtual ~LocFInfo();
	virtual void initByPath(const fs::path& p);
	virtual void fre();
	virtual void refreshSubs();
};
//
class LocAdapter: public AdapterBase {
private:
	Log log;
public:
	LocAdapter(sqlite3 *db, string rurl);
	virtual ~LocAdapter();
	virtual FInfo* createRootNode();
	virtual vector<FInfo*> listSubs(FInfo* parent);
	virtual void mkdir(FInfo* fi, string name);
	FInfo* contain(FInfo*fi, string name);
	virtual string absUrl(FInfo* tg);
	virtual bool remove(FInfo* fi, string name="");
};

} /* namespace centny */
#endif /* LOCADAPTER_H_ */
