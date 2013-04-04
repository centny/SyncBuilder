/*
 * LoginCmdBase.h
 *
 *  Created on: Apr 2, 2013
 *      Author: Scorpion
 */

#ifndef LOGINCMDBASE_H_
#define LOGINCMDBASE_H_

#include "ConClient.h"
#include "UserCfg.h"

namespace centny {
//
#define LOGIN_USG "LOGIN <user> <pass> \t\t login by username password"
//
class LoginCmdBase:public CmdBase{
private:
	Log log;
protected:
	map<ConClient*, string> c2n; //client to name map.
	map<string, ConClient*> n2c; //name to client map.
	map<string, ConClient*> s2c; //session to client map.
	boost::mutex session_mutex;
#define SESSION_LOCK boost::mutex::scoped_lock sessionlock(session_mutex)
	UserCfg *users;
	char cbuf[R_BUF_SIZE];
	size_t blen;
public:
	LoginCmdBase(string ucfg);
	virtual ~LoginCmdBase();
	static string newSession();
	virtual bool isLogin(ConClient* c);
	virtual void shutdown(ConClient* c);
	virtual ConClient* nameClient(string name);
	virtual ConClient* sessionClient(string session);
	virtual string rdir(ConClient* c);
	virtual size_t logout(ConClient* c);
	virtual size_t login(ConClient* c, string user, string pass, string name);
	virtual size_t login(ConClient* c,vector<string>& rcmds);
	virtual void execCmd(ConClient* c,std::istream* isbuf)=0;
};


} /* namespace centny */
#endif /* LOGINCMDBASE_H_ */
