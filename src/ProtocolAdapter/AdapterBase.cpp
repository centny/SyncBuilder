/*
 * AdapterBase.cpp
 *
 *  Created on: Nov 10, 2012
 *      Author: Scorpion
 */

#include "AdapterBase.h"
#include "../Common/ReplaceAll.h"
namespace centny {
namespace file {
const char * MONTHES[MONTHES_SIZE] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
		"Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
//
FInfo::FInfo(AdapterBase *cb, FInfo *parent) {
	this->cb = cb;
	this->parent = parent;
	this->type = 0;
	this->mtime = 0;
	this->size = 0;
	this->cwd = F_SEQ;
	this->name = "";
//	if (!this->parent) {
//		this->cwd2f = new map<string, FInfo*>();
//		this->addToMap(this);
//	}
}
FInfo::~FInfo() {
	this->freSubs();
//	if (!this->parent) {
//		delete this->cwd2f;
//	}
}
vector<FInfo*>& FInfo::subs() {
	if (!this->isDir() || _subs.size()) {
		return _subs;
	}
	assert(this->cb);
	vector<FInfo*> fis;
	fis = this->cb->subs(this);
	vector<FInfo*>::iterator it;
	for (it = fis.begin(); it != fis.end(); it++) {
		this->_subs.push_back(*it);
//		printf("%s\n", (*it)->cwd.c_str());
//		(*it)->cwd2f = this->cwd2f;
//		this->addToMap(*it);
	}
//	printf("\n\n");
	return _subs;
}
void FInfo::showInfo() {
	cout << cwd << "\t" << type << "\t" << size << "\t" << mtime << "\t" << name
			<< endl;
}
void FInfo::showAll() {
	this->showInfo();
	vector<FInfo*> subs = this->subs();
	vector<FInfo*>::iterator it;
	for (it = subs.begin(); it != subs.end(); it++) {
		(*it)->showAll();
	}
}
bool FInfo::isDir() {
	return type == 'd' || type == 'D';
}
void FInfo::refreshSubs() {
//	this->cb->freSubs(this);
	this->freSubs();
	this->subs();
}
void FInfo::mkdir(string name) {
	if (!this->isDir()) {
		return;
	}
	cb->mkdir(this, name);
}
bool FInfo::remove(string name) {
//	FInfo* f = this->contain(name);
//	if (f) {
//		string cwd = f->cwd;
//		bool res = f->cb->remove(f);
//		cb->delDb(this, cwd);
//		return res;
//	} else {
//		return false;
//	}
	return cb->remove(this, name);
}
FInfo* FInfo::contain(string name) {
	if (!this->isDir()) {
		return 0;
	}
	return cb->contain(this, name);
}
int FInfo::update2Db() {
	return cb->update2Db(this);
}
int FInfo::update2Normal() {
	return cb->updateState2Db(this, 0);
}
int FInfo::remove2Db() {
	return cb->delDb(this, this->cwd);
}
void FInfo::delDb() {
	cb->delDb(this, cwd);
}
bool FInfo::isDbRemoved() {
	return cb->isDbRemoved(this, cwd);
}
set<pair<string, string> > FInfo::removedSubs() {
	return cb->snames2Db(this, 2);
}
bool FInfo::isNew() {
	return cb->isDbState(this, "3");
}
bool FInfo::isUpdated() {
	return cb->isDbState(this, "1");
}
bool FInfo::isNewOrUpdated() {
	return cb->isDbState(this, "1,3");
}
void FInfo::fre() {
	delete this;
}
//void FInfo::addToMap(FInfo* fi) {
//	cout << "add to map:" << fi->cwd << "\t" << fi->name << endl;
//	(*cwd2f)[fi->cwd] = fi;
//}
void FInfo::freSubs() {
	vector<FInfo*>::iterator it;
	for (it = _subs.begin(); it != _subs.end(); it++) {
//		this->cwd2f->erase((*it)->cwd);
		(*it)->fre();
	}
	this->_subs.clear();
}
//FInfo* FInfo::finfo(string cwd) {
//	map<string, FInfo*>::iterator it;
//	it = this->cwd2f->find(cwd);
//	if (it == this->cwd2f->end()) {
//		return 0;
//	} else {
//		return it->second;
//	}
//}
string FInfo::absUrl() {
	return cb->absUrl(this);
}
//void FInfo::showCwd2f() {
//	map<string, FInfo*>::iterator it;
//	cout << "cwd2f:" << this->cwd2f->size() << endl;
//	for (it = this->cwd2f->begin(); it != this->cwd2f->end(); it++) {
//		cout << it->second->name << "\t" << it->first << endl;
//	}
//}
//static int nfidx = 0;
//
NetFInfo::NetFInfo(NetAdapterBase *cb, NetFInfo *parent) :
		FInfo(cb, parent) {
//	this->fidx = nfidx++;
}
NetFInfo::~NetFInfo() {
#if SHOW_FRE_MSG
	printf("free NetFInfo\n");
#endif
}
const char* NetFInfo::upload(string lf, string name) {
	return ((NetAdapterBase*) cb)->upload(this, lf, name);
}
const char* NetFInfo::upload(istream& is, size_t len, string name) {
	return ((NetAdapterBase*) cb)->upload(this, is, len, name);
}
const char* NetFInfo::download(string of) {
	return ((NetAdapterBase*) cb)->download(this, of);
}
const char* NetFInfo::download(ostream& os) {
	return ((NetAdapterBase*) cb)->download(this, os);
}
int NetFInfo::netstate() {
	return ((NetAdapterBase*) cb)->netstate;
}
//

AdapterBase::AdapterBase(sqlite3 *db) :
		db(db) {
	this->rurl = "";
	this->last = 0;
}

AdapterBase::~AdapterBase() {
}
FInfo* AdapterBase::root() {
	if (last) {
		last->fre();
		last = 0;
	}
	last = this->createRootNode();
//	set<string>::iterator it, end;
//	for (it = p_cwd->begin(), end = p_cwd->end(); it != end; it++) {
//		cout << "cwd:" << *it << endl;
//	}
//	l_cwd->insert(last->cwd);
	return last;
}
//bool AdapterBase::isRemoved(FInfo* caller, FInfo* fi) {
//	return pre && pre->finfo(fi->cwd) != NULL && last
//			&& last->finfo(fi->cwd) == NULL;
//}
string AdapterBase::rootUrl() {
	return this->rurl;
}
vector<FInfo*> AdapterBase::subs(FInfo* parent) {
	vector<FInfo*> s = this->listSubs(parent);
	int len = s.size();
	string cwds = "";
	if (len) {
		s[0]->update2Db();
		cwds += "'" + s[0]->cwd + "'";
		for (int i = 1; i < len; i++) {
			FInfo* f = s[i];
			f->update2Db();
			cwds += ",'" + s[i]->cwd + "'";
		}
		this->updateState2DbNot(parent, cwds, 2);
	} else {
		this->updateState2DbNot(parent, "", 2);
	}
	return s;
}
//void AdapterBase::freSubs(FInfo* parent) {
////	vector<FInfo*>::iterator it, end;
////	for (it = parent->subs().begin(), end = parent->subs().end(); it != end;
////			it++) {
//////		this->l_cwd->erase((*it)->cwd);
////	}
//}
FInfo* AdapterBase::contain(FInfo*fi, string name) {
	vector<FInfo*> fis = fi->subs();
	if (fis.size() < 1) {
		return 0;
	}
	vector<FInfo*>::iterator it, end = fis.end();
	string cwd;
	if(fi->cwd.empty()){
		cwd= name;
	}else{
		cwd= fi->cwd +"/"+ name;
	}
	cout<<"tcwd:"<<cwd<<endl;
	for (it = fis.begin(); it != end; it++) {
		if ((*it)->cwd == cwd) {
			return *it;
		}
	}
	return 0;
}
int AdapterBase::update2Db(FInfo* caller) {
	return this->update2Db(caller, 0);
}
int AdapterBase::delDb(FInfo* caller, string cwd) {
	return this->delDb(caller, cwd, 0);
}
bool AdapterBase::isDbRemoved(FInfo* caller, string cwd) {
	return this->isDbRemoved(caller, cwd, 0);
}
int AdapterBase::updateState2DbNot(FInfo* caller, string cwds, int state) {
	return this->updateState2DbNot(caller, cwds, state, 0);
}
int AdapterBase::updateState2Db(FInfo* caller, int state) {
	return this->updateState2Db(caller, state, 0);
}
set<pair<string, string> > AdapterBase::snames2Db(FInfo* caller, int state) {
	return this->snames2Db(caller, state, 0);
}
bool AdapterBase::isDbState(FInfo* caller, string states) {
	return this->isDbState(caller, states, 0);
}
int AdapterBase::delDbParent(string cwd) {
	return this->delDbParent(cwd, 0);
}
int AdapterBase::update2Db(FInfo* caller, int location) {
	sqlite3_stmt *stmt;
	const char *isql = "INSERT INTO FINFO"
			" (PARENT,MTIME,NAME,TYPE,SIZE,STATE,LOCATION,CWD)"
			" VALUES"
			" (?,?,?,?,?,?,?,?)";
	const char *usql = "UPDATE FINFO SET"
			" PARENT=?,MTIME=?,"
			" NAME=?,TYPE=?,SIZE=?,STATE=?"
			" WHERE"
			" LOCATION=?"
			" AND CWD=?";
	int state;
	if (this->isDbExist(caller, caller->cwd, location ? 0 : 1)) {
		sqlite3_stmt *stmt2;
		const char *sql = "SELECT MTIME,SIZE,STATE FROM FINFO WHERE"
				" CWD=? AND LOCATION=?";
//		printf("SELECT MTIME,SIZE,STATE FROM FINFO WHERE"
//				" CWD=%s AND LOCATION=%d\n", caller->cwd.c_str(), location);
		sqlite3_prepare_v2(this->db, sql, -1, &stmt2, NULL);
		sqlite3_bind_text(stmt2, 1, caller->cwd.c_str(), -1, SQLITE_TRANSIENT );
		sqlite3_bind_int(stmt2, 2, location ? 0 : 1);
		int res = sqlite3_step(stmt2);
		assert(SQLITE_ROW==res);
		long mtime = atol((const char*) sqlite3_column_text(stmt2, 0));
		unsigned long long size = atol(
				(const char*) sqlite3_column_text(stmt2, 1));
//		int state = atoi((const char*) sqlite3_column_text(stmt2, 2));
		if (size != caller->size) {
			state = 1;
			sqlite3_finalize(stmt2);
		} else if (mtime != caller->mtime) {
			state = 1;
			sqlite3_finalize(stmt2);
		} else {
			state = 0;
			sqlite3_finalize(stmt2);
		}
	} else {
		state = 3;
	}
	if (this->isDbExist(caller, caller->cwd, location)) {
		sqlite3_prepare_v2(this->db, usql, -1, &stmt, NULL);
	} else {
		sqlite3_prepare_v2(this->db, isql, -1, &stmt, NULL);
	}
	if (caller->parent) {
		sqlite3_bind_text(stmt, 1, caller->parent->cwd.c_str(), -1,
				SQLITE_TRANSIENT );
	} else {
		sqlite3_bind_null(stmt, 1);
	};
	int blen;
	blen = sprintf(buf, "%ld", caller->mtime);
	buf[blen] = 0;
	sqlite3_bind_text(stmt, 2, buf, -1, SQLITE_TRANSIENT );
	sqlite3_bind_text(stmt, 3, caller->name.c_str(), -1, SQLITE_TRANSIENT );
	if (caller->isDir()) {
		sqlite3_bind_int(stmt, 4, 1);
	} else {
		sqlite3_bind_int(stmt, 4, 0);
	}
	blen = sprintf(buf, "%lld", caller->size);
	buf[blen] = 0;
	sqlite3_bind_text(stmt, 5, buf, -1, SQLITE_TRANSIENT );
	sqlite3_bind_int(stmt, 6, state);
	sqlite3_bind_int(stmt, 7, location);
	sqlite3_bind_text(stmt, 8, caller->cwd.c_str(), -1, SQLITE_TRANSIENT );
	int res = sqlite3_step(stmt);
	if (SQLITE_DONE != res && SQLITE_ROW != res) {
		caller->showInfo();
		printf("error sql:%s,msg:%s\n", sqlite3_sql(stmt),
				sqlite3_errmsg(this->db));
		assert(SQLITE_DONE==res||SQLITE_ROW==res);
	}
	sqlite3_finalize(stmt);
	return SQLITE_ROW == res;
}
int AdapterBase::delDb(FInfo* caller, string cwd, int location) {
	sqlite3_stmt *stmt;
	const char *sql = "DELETE FROM FINFO WHERE"
			" CWD=? AND LOCATION=?";
#if SHOW_SQL_LOG
	printf("delDb SQL:"
			" DELETE FROM FINFO WHERE"
			" CWD='%s' AND LOCATION=%d\n", caller->cwd.c_str(), location);
#endif
	sqlite3_prepare_v2(this->db, sql, -1, &stmt, NULL);
	sqlite3_bind_text(stmt, 1, cwd.c_str(), -1, SQLITE_TRANSIENT );
	sqlite3_bind_int(stmt, 2, location);
	int res = sqlite3_step(stmt);
	assert(SQLITE_DONE==res||SQLITE_ROW==res);
	int count = sqlite3_changes(this->db);
	sqlite3_finalize(stmt);
	return count;
}
int AdapterBase::delDbParent(string cwd, int location) {
	sqlite3_stmt *stmt;
	int blen = sprintf(buf, ""
			" DELETE FROM FINFO WHERE"
			" (PARENT LIKE '%s%s' OR CWD='%s')"
			" AND LOCATION=%d", cwd.c_str(), "%", cwd.c_str(), location);
	buf[blen] = 0;
#if SHOW_SQL_LOG
	printf("delDb SQL:%s\n", buf);
#endif
	sqlite3_prepare_v2(this->db, buf, -1, &stmt, NULL);
	int res = sqlite3_step(stmt);
	assert(SQLITE_DONE==res||SQLITE_ROW==res);
	int count = sqlite3_changes(this->db);
	sqlite3_finalize(stmt);
	return count;
}
bool AdapterBase::isDbRemoved(FInfo* caller, string cwd, int location) {
	sqlite3_stmt *stmt;
	const char *sql = "SELECT * FROM FINFO WHERE"
			" CWD=? AND LOCATION=? AND STATE=2";
	sqlite3_prepare_v2(this->db, sql, -1, &stmt, NULL);
	sqlite3_bind_text(stmt, 1, cwd.c_str(), -1, SQLITE_TRANSIENT );
	sqlite3_bind_int(stmt, 2, location);
	int res = sqlite3_step(stmt);
	assert(SQLITE_DONE==res||SQLITE_ROW==res);
	sqlite3_finalize(stmt);
	return SQLITE_ROW == res;
}
int AdapterBase::updateState2DbNot(FInfo* caller, string cwds, int state,
		int location) {
	sqlite3_stmt *stmt;
	int slen;
	if (cwds.empty()) {
		slen = sprintf(buf, ""
				" UPDATE FINFO SET"
				" STATE=?"
				" WHERE LOCATION=?"
				" AND PARENT=?");
	} else {
		slen = sprintf(buf, ""
				" UPDATE FINFO SET"
				" STATE=?"
				" WHERE LOCATION=?"
				" AND PARENT=?"
				" AND CWD NOT IN (%s)", cwds.c_str());
	}
	buf[slen] = 0;
#if SHOW_SQL_LOG
	printf("updateState2DbNot SQL:UPDATE FINFO SET"
			" STATE=%d"
			" WHERE LOCATION=%d"
			" AND PARENT='%s'"
			" AND CWD NOT IN (%s)\n", state, location, caller->cwd.c_str(),
			cwds.c_str());
#endif
	sqlite3_prepare_v2(this->db, buf, -1, &stmt, NULL);
	sqlite3_bind_int(stmt, 1, state);
	sqlite3_bind_int(stmt, 2, location);
	sqlite3_bind_text(stmt, 3, caller->cwd.c_str(), -1, SQLITE_TRANSIENT );
	int res = sqlite3_step(stmt);
	assert(SQLITE_DONE==res||SQLITE_ROW==res);
	int count = sqlite3_changes(this->db);
	sqlite3_finalize(stmt);
	return count;
}
int AdapterBase::updateState2Db(FInfo* caller, int state, int location) {
	sqlite3_stmt *stmt;
	const char* sql = ""
			" UPDATE FINFO SET"
			" STATE=?"
			" WHERE LOCATION=?"
			" AND CWD=?";
#if SHOW_SQL_LOG
	printf("updateState2Db SQL:"
			" UPDATE FINFO SET"
			" STATE=%d"
			" WHERE LOCATION=%d"
			" AND CWD='%s'\n", state, location, caller->cwd.c_str());
#endif
	sqlite3_prepare_v2(this->db, sql, -1, &stmt, NULL);
	sqlite3_bind_int(stmt, 1, state);
	sqlite3_bind_int(stmt, 2, location);
	sqlite3_bind_text(stmt, 3, caller->cwd.c_str(), -1, SQLITE_TRANSIENT );
	int res = sqlite3_step(stmt);
	assert(SQLITE_DONE==res||SQLITE_ROW==res);
	int count = sqlite3_changes(this->db);
	sqlite3_finalize(stmt);
	return count;
}
set<pair<string, string> > AdapterBase::snames2Db(FInfo* caller, int state,
		int location) {
	set<pair<string, string> > subs;
	sqlite3_stmt *stmt;
	const char *sql = "SELECT NAME,CWD FROM FINFO WHERE"
			" PARENT=? AND LOCATION=? AND STATE=?";
#if SHOW_SQL_LOG
	printf("snames2Db SQL:"
			" SELECT NAME FROM FINFO WHERE"
			" PARENT='%s' AND LOCATION=%d AND STATE=%d\n", caller->cwd.c_str(),
			location, state);
#endif
	sqlite3_prepare_v2(this->db, sql, -1, &stmt, NULL);
	sqlite3_bind_text(stmt, 1, caller->cwd.c_str(), -1, SQLITE_TRANSIENT );
	sqlite3_bind_int(stmt, 2, location);
	sqlite3_bind_int(stmt, 3, state);
	int res = sqlite3_step(stmt);
	assert(SQLITE_DONE==res||SQLITE_ROW==res);
	int count = sqlite3_data_count(stmt);
	for (int i = 0; i < count; i++) {
		subs.insert(
				pair<string, string>(
						string((const char*) sqlite3_column_text(stmt, 0)),
						string((const char*) sqlite3_column_text(stmt, 1))));
		sqlite3_next_stmt(this->db, stmt);
	}
	sqlite3_finalize(stmt);
	return subs;
}
bool AdapterBase::isDbState(FInfo* caller, string states, int location) {
	sqlite3_stmt *stmt;
	int slen = sprintf(buf, "SELECT * FROM FINFO WHERE"
			" CWD=? AND LOCATION=? AND STATE IN (%s)", states.c_str());
#if SHOW_SQL_LOG
	printf("isDbState SQL:SELECT * FROM FINFO WHERE"
			" CWD='%s' AND LOCATION=%d AND STATE IN (%s)\n",
			caller->cwd.c_str(), location, states.c_str());
#endif
	buf[slen] = 0;
	sqlite3_prepare_v2(this->db, buf, -1, &stmt, NULL);
	sqlite3_bind_text(stmt, 1, caller->cwd.c_str(), -1, SQLITE_TRANSIENT );
	sqlite3_bind_int(stmt, 2, location);
	int res = sqlite3_step(stmt);
	assert(SQLITE_DONE==res||SQLITE_ROW==res);
	sqlite3_finalize(stmt);
	return res == SQLITE_ROW;
}
bool AdapterBase::isDbExist(FInfo* caller, string cwd, int location) {
	sqlite3_stmt *stmt;
	const char *sql = "SELECT * FROM FINFO WHERE"
			" CWD=? AND LOCATION=?";
#if SHOW_SQL_LOG
	printf("isDbExist SQL:SELECT * FROM FINFO WHERE"
			" CWD='%s' AND LOCATION=%d\n", cwd.c_str(), location);
#endif
	sqlite3_prepare_v2(this->db, sql, -1, &stmt, NULL);
	sqlite3_bind_text(stmt, 1, cwd.c_str(), -1, SQLITE_TRANSIENT );
	sqlite3_bind_int(stmt, 2, location);
	int res = sqlite3_step(stmt);
	assert(SQLITE_DONE==res||SQLITE_ROW==res);
	sqlite3_finalize(stmt);
	return res == SQLITE_ROW;
}
NetAdapterBase::NetAdapterBase(sqlite3 *db) :
		AdapterBase(db) {
	this->netstate = 0;
}
const char* NetAdapterBase::upload(FInfo* fi, string lf, string name) {
	return 0;
}
const char* NetAdapterBase::upload(FInfo* fi, istream& is, size_t len,
		string name) {
	return 0;
}
const char* NetAdapterBase::download(FInfo* fi, string lf) {
	return 0;
}
const char* NetAdapterBase::download(FInfo* fi, ostream& os) {
	return 0;
}
string NetAdapterBase::absUrl(FInfo* tg) {
	string aurl = this->rurl + "/" + tg->cwd;
	replaceAll(aurl, "//", "/", 7);
	return aurl;
}
int NetAdapterBase::update2Db(FInfo* caller) {
	return AdapterBase::update2Db(caller, 1);
}
int NetAdapterBase::delDb(FInfo* caller, string cwd) {
	return AdapterBase::delDb(caller, cwd, 1);
}
bool NetAdapterBase::isDbRemoved(FInfo* caller, string cwd) {
	return AdapterBase::isDbRemoved(caller, cwd, 1);
}
int NetAdapterBase::updateState2DbNot(FInfo* caller, string cwds, int state) {
	return AdapterBase::updateState2DbNot(caller, cwds, state, 1);
}
int NetAdapterBase::updateState2Db(FInfo* caller, int state) {
	return AdapterBase::updateState2Db(caller, state, 1);
}
set<pair<string, string> > NetAdapterBase::snames2Db(FInfo* caller, int state) {
	return AdapterBase::snames2Db(caller, state, 1);
}
bool NetAdapterBase::isDbState(FInfo* caller, string states) {
	return AdapterBase::isDbState(caller, states, 1);
}
int NetAdapterBase::delDbParent(string cwd) {
	return AdapterBase::delDbParent(cwd, 1);
}
}
} /* namespace centny */
