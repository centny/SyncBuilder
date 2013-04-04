/*
 * HttpCmdMgr.cpp
 *
 *  Created on: Dec 4, 2012
 *      Author: Scorpion
 */
/*
#include "HttpCmdMgr.h"
#include "../Common/ReplaceAll.h"
#include <sstream>
#include <string>
using namespace std;
namespace centny {

HttpCmdMgr::HttpCmdMgr(BindCmdMgr* bcm) {
	this->bcm = bcm;
	this->server = 0;
}

HttpCmdMgr::~HttpCmdMgr() {
}
void HttpCmdMgr::execCmd(ConClient* c,std::istream* isbuf) {
	stringstream sbuf(cmds[0]);
	string tmp;
	sbuf >> tmp;
	if (tmp != "GET" || sbuf.eof()) {
		this->response(c, "EventServer", 500,
				"invalid protocol,please using GET");
		c->shutdown();
		return;
	}
	sbuf >> tmp;
	vector<string> args;
	boost::split(args, tmp, is_any_of("/"));
	vector<string>::iterator it;
	for (it = args.begin(); it != args.end(); it++) {
		if (it->size() < 1) {
			it=args.erase(it);
		}
	}
	if (args.size() < 2) {
		this->response(c, "EventServer", 500,
				"invalid command.</br>"
						"Usage:/&lt;Client Name&gt;/&lt;Event Name&gt;/&lt;Other Parameter&gt;...");
		c->shutdown();
		return;
	}
	//this->server->thrGrps().create_thread(
			//boost::bind(&BindCmdMgr::sendCmd, this->bcm, args, c));
	this->bcm->sendCmd(args,c);
}
void HttpCmdMgr::setServer(SyncServer* s) {
	this->server = s;
}
void HttpCmdMgr::response(ConClient* c, string name, int code, string msg) {
	stringstream body;
	body << "<html>" << endl;
	body << "<header>" << endl;
	body
			<< "\
			<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\
			<meta charset=\"utf-8\">\
			<meta name=\"viewport\" content=\"width=device-width,initial-scale=1.0,maximum-scale=1.0,user-scalable=0\">"
			<< endl;
	body << "<title>" << name << "</title>" << endl;
	body
			<< "\
			<meta name=\"keywords\" content=\"\">\
			<meta name=\"description\" content=\"\">\
			<meta name=\"author\" content=\"zhuanghuanbin_gz\">\
			<meta name=\"apple-mobile-web-app-capable\" content=\"yes\">\
			<meta name=\"apple-mobile-web-app-status-bar-style\" content=\"black\">\
			<meta name=\"format-detection\" content=\"telephone=no\">\
			<meta http-equiv=\"Content-Type\" content=\"text/html;charset=UTF-8\" />"
			<< endl;
	body << "<html><body><div>" << msg << "</div></body></html>";
	stringstream buf;
	buf << "HTTP/1.1 " << code << " OK\r\n";
	buf << "Cache-Control:no-cache\r\n";
	buf << "Expires:0\r\n";
	buf << "Pragma:no-cache\r\n";
	buf << "Connection:Keep-Alive\r\n";
	buf << "Content-Type:text/html;charset=UTF-8\r\n";
	buf << "Connection:Keep-Alive\r\n";
	buf << "Server:ShortcutServer\r\n";
	buf << "Content-Length:" << body.str().size() << "\r\n\n";
	buf << body.str() << endl;
	c->syncWrite(buf.str().c_str(), buf.str().size());
}
void HttpCmdMgr::sendHeader(ConClient* c, int code, size_t len) {
	stringstream buf;
	buf << "HTTP/1.1 " << code << " OK\r\n";
	buf << "Cache-Control:no-cache\r\n";
	buf << "Expires:0\r\n";
	buf << "Pragma:no-cache\r\n";
	buf << "Connection:Keep-Alive\r\n";
	buf << "Content-Type:text/plain;charset=UTF-8\r\n";
	buf << "Server:ShortcutServer\r\n";
	buf << "Content-Length:" << len << "\r\n\n";
	c->syncWrite(buf.str().c_str(), buf.str().size());
}
}
*/
 /* namespace centny */
