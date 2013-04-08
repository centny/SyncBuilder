/*
 * testSyncServer.cpp
 *
 *  Created on: Nov 30, 2012
 *      Author: Scorpion
 */

#include "SyncServer.h"
#include "FileCmdMgr.h"
#include "HttpCmdMgr.h"
#include "BindCmdMgr.h"
#include "ShellCmdMgr.h"
#include <boost/array.hpp>
using namespace centny;
void testSyncServer() {
#ifdef WIN32
	boost::shared_ptr<FileCmdMgr> fcm(
			new FileCmdMgr("SyncServer/users.cfg"));
#else
	boost::shared_ptr<FileCmdMgr> fcm(
			new FileCmdMgr("src/SyncServer/users.cfg"));
#endif
	io_service ios;
	SyncServer fss(ios, 10000, fcm.get(), "FCM");
	fcm->setServer(&fss);
	fss.accept();
	//
	//
	/*boost::shared_ptr<BindCmdMgr> bcm(new BindCmdMgr(fcm.get()));
	 SyncServer bss(ios, 10001, bcm.get(),"BCM");
	 bcm->setServer(&bss);
	 boost::shared_ptr<HttpCmdMgr> hcm(new HttpCmdMgr(bcm.get()));
	 hcm->setSeq("\r\n");
	 SyncServer hss(ios, 8080, hcm.get(),"HCM");
	 hcm->setServer(&hss);
	 bcm->setHttpCmdMgr(hcm.get());
	 hss.accept();
	 bss.accept();*/
	//
#ifdef WIN32
	boost::shared_ptr<ShellCmdMgr> scm(new ShellCmdMgr("SyncServer/users.cfg","SyncServer/WinWeb.cfg"));
#else
	boost::shared_ptr<ShellCmdMgr> scm(
			new ShellCmdMgr("src/SyncServer/users.cfg",
					"src/SyncServer/Web.cfg"));
#endif
	SyncServer sss(ios, 10001, scm.get(), "SCM");
	//	//
	sss.accept();
	//
	ios.run();
}
boost::shared_ptr<io_service> iosev;
boost::shared_ptr<ip::tcp::acceptor> acceptor;
set<boost::shared_ptr<tcp::socket> > sockets;
boost::array<char, 2048> buf;
void acceptHandler(boost::shared_ptr<tcp::socket> tcp,
		const boost::system::error_code& ec);
void readHandle(boost::shared_ptr<tcp::socket> tcp,
		const boost::system::error_code& ec, std::size_t bytes_transferred);
void startRead(boost::shared_ptr<tcp::socket> tcp);
void acceptSocket() {
	boost::shared_ptr<tcp::socket> tcp(new tcp::socket(*iosev.get()));
	sockets.insert(tcp);
	acceptor->async_accept(*tcp, boost::bind(&acceptHandler, tcp, _1));
	cout << "accept" << endl;
}
int acount = 0;
void acceptHandler(boost::shared_ptr<tcp::socket> tcp,
		const boost::system::error_code& ec) {
	startRead(tcp);
	acceptSocket();
	acount++;
	cout << "ahandler:" << acount << endl;
}
void startRead(boost::shared_ptr<tcp::socket> tcp) {
	tcp->async_read_some(buffer(buf), boost::bind(&readHandle, tcp, _1, _2));
	cout << "read" << endl;
}
void readHandle(boost::shared_ptr<tcp::socket> tcp,
		const boost::system::error_code& ec, std::size_t bytes_transferred) {
	cout << (ec.value() == ECANCELED) << endl;
	if (ec) {
		tcp->cancel();
		tcp->close();
		cout << "<" << ec.message() << ">" << endl;
		acount--;
		cout << "ahandler:" << acount << endl;
		return;
	}
	cout << "<---------------------------------------------->" << endl;
	cout.write(buf.c_array(), bytes_transferred);
	cout << endl;
	const char* data = "fsfsfklfkffdsfsdfsdfsfsf";
	tcp->write_some(buffer(data, strlen(data)));
	tcp->cancel();
	tcp->close();
	cout << "<close>" << endl;
	acount--;
	cout << "ahandler:" << acount << endl;
}
void testSocketServer() {
	iosev = boost::shared_ptr<io_service>(new io_service);
	acceptor = boost::shared_ptr<ip::tcp::acceptor>(
			new ip::tcp::acceptor(*iosev.get(),
					tcp::endpoint(tcp::v4(), 8080)));
	acceptSocket();
	iosev->run();
}
void testShellCmdServer() {
	ShellCmdMgr scm("SyncServer/users.cfg");
	io_service ios;
	SyncServer sss(ios, 10001, &scm, "SCM");
	sss.accept();
	ios.run();
}
