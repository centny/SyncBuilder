/*
 * LoginCmdBase.cpp
 *
 *  Created on: Apr 2, 2013
 *      Author: Scorpion
 */

#include "LoginCmdBase.h"

namespace centny {

LoginCmdBase::LoginCmdBase(string ucfg):
	log(C_LOG("LoginCmdBase")){
	this->users=new UserCfg(ucfg);
	this->blen=0;
}
LoginCmdBase::~LoginCmdBase() {
	if (this->users) {
		delete this->users;
	}
}
static long long sessionCid = 1;
string LoginCmdBase::newSession() {
	string ss;
	srand(time(0));
	for (int i = 0; i < 10; i++) {
		char rc = rand() % 26 + 'A';
		ss += rc;
	}
	char buf[100] = { 0 };
	sprintf(buf, "_%lld", sessionCid++);
	ss = ss + string(buf);
	sessionCid++;
	return ss;
}
void LoginCmdBase::shutdown(ConClient* c) {
	SESSION_LOCK;
	this->n2c.erase(c->name());
	this->s2c.erase(c->session());
	this->c2n.erase(c);
}
bool LoginCmdBase::isLogin(ConClient* c) {
	SESSION_LOCK;
	return this->c2n.find(c) != this->c2n.end();
}
ConClient* LoginCmdBase::nameClient(string name) {
	SESSION_LOCK;
	map<string,ConClient*>::iterator it;
	it=this->n2c.find(name);
	if(it!=this->n2c.end()) {
		return it->second;
	} else {
		return 0;
	}
}
ConClient* LoginCmdBase::sessionClient(string session) {
	SESSION_LOCK;
	map<string,ConClient*>::iterator it;
	it=this->s2c.find(session);
	if(it!=this->s2c.end()) {
		return it->second;
	} else {
		return 0;
	}
}
size_t LoginCmdBase::logout(ConClient* c) {
	SESSION_LOCK;
	log.debug("start execute logout command for:%s",c->name().c_str());
	map<ConClient*, string>::iterator it;
	it = this->c2n.find(c);
	if (it == this->c2n.end()) {
		blen = sprintf(cbuf, "%d\n%s"DEFAULT_EOC, 202, "Not Login");
	} else {
		this->c2n.erase(it);
		if(c->name().size()) {
			this->n2c.erase(c->name());
		}
		if(c->session().size()) {
			this->s2c.erase(c->session());
		}
		blen = sprintf(cbuf, "%d\n%s"DEFAULT_EOC, 200, "Logout success");
	}
	c->asyncWrite(cbuf, blen);
	return blen;
}
size_t LoginCmdBase::login(ConClient* c, string user, string pass, string name) {
	SESSION_LOCK;
	log.debug("start execute login command by name:%s",name.c_str());
	string tpass=this->users->pass(user);
	if (tpass.size()&&pass == tpass) {
		if(this->n2c.find(name)!=this->n2c.end()) {
			this->c2n[c] = user;
			blen = sprintf(cbuf, "%d\n%s"DEFAULT_EOC, 202, "name already in using");
		} else {
			this->c2n[c] = user;
			string ss=newSession();
			this->s2c[ss]=c;
			c->setSession(ss);
			if(name.size()) {
				this->n2c[name]=c;
				c->setName(name);
			}
			blen = sprintf(cbuf, "%d\n%s %s"DEFAULT_EOC, 200, "Login success",ss.c_str());
		}
	} else {
		blen = sprintf(cbuf, "%d\n%s"DEFAULT_EOC, 403, "Login failed");
	}
	c->asyncWrite(cbuf, blen);
	log.debug("execute login command end,send:%ld",blen);
	return blen;
}
size_t LoginCmdBase::login(ConClient* c,vector<string>& rcmds){
	//
#define P_LOGIN_ERR blen=sprintf(cbuf, "%d\n%s\n%s"DEFAULT_EOC, 500, "invalid command","Usage:"LOGIN_USG)\
	,c->asyncWrite(cbuf, blen)
	///
	if (rcmds.size() < 3 && rcmds.size() > 4) {
		P_LOGIN_ERR;
		c->startRead();
		return 0;
	}
	if (rcmds[1].size() < 1 || rcmds[2].size() < 1) {
		P_LOGIN_ERR;
		c->startRead();
		return 0;
	}
	size_t  s=this->login(c, rcmds[1], rcmds[2], rcmds.size() == 4 ? rcmds[3] : "");
	c->startRead();
	return s;
}
string LoginCmdBase::rdir(ConClient* c) {
	SESSION_LOCK;
	string r=this->users->root(this->c2n[c]);
	return r;
}
//
} /* namespace centny */
