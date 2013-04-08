/*
 * ConClient.cpp
 *
 *  Created on: Nov 30, 2012
 *      Author: Scorpion
 */

#include "ConClient.h"
#include <stdlib.h>
#include <cstdlib>
#include <stdio.h>
namespace centny {
#define SHOW_REL_LOG 1
ConClient::ConClient(SyncServer& server, CmdBase* cmd) :
		log(C_LOG("ConClient")),server(server) {
			this->cmdb=cmd;
			this->psocket=boost::shared_ptr<tcp::socket>(new tcp::socket(server.iosev));
			this->inited=false;
			this->locked=false;
			this->rtimer=this->server.deadlineTimer();
//			this->wtimer=this->server.deadlineTimer();
		}

ConClient::~ConClient() {
	this->shutdown();
#if SHOW_REL_LOG
	printf("ConClient release...\n");
#endif
}
void ConClient::setName(string n) {
	this->cname = n;
}
string ConClient::name() {
	return this->cname;
}
void ConClient::setSession(string n) {
	this->csession = n;
}
string ConClient::session() {
	return this->csession;
}
void ConClient::setSp(boost::shared_ptr<ConClient>& sp) {
	this->csp = sp;
}
boost::shared_ptr<ConClient>& ConClient::sp() {
	return csp;
}
SyncServer& ConClient::tServer() {
	return server;
}
//void ConClient::fre() {
//	this->shutdown();
//	this->server.remove(this);
//}
void ConClient::initAdr() {
	this->radr = psocket->remote_endpoint().address();
	this->ladr = psocket->local_endpoint().address();
}
bool ConClient::isInited() {
	return this->inited;
}
bool ConClient::isLocked() {
	return this->locked;
}
void ConClient::lock() {
	this->lmutex.lock();
	assert(!this->locked);
	this->locked = true;
}
void ConClient::unlock() {
	if (!this->locked) {
		return;
	}
	this->locked = false;
	this->unlock();
}
void ConClient::shutdown() {
	if (!this->inited) {
		return;
	}
	log.debug("shutdown client:%s", this->address().c_str());
	this->psocket->cancel();
	this->rtimer->cancel();
	boost::system::error_code ignored_ec;
	this->psocket->shutdown(boost::asio::ip::tcp::socket::shutdown_both,
			ignored_ec);
	this->inited = false;
	this->cmdb->shutdown(this);
	this->csp = boost::shared_ptr<ConClient>();
}
void ConClient::startRead() {
	this->inited = true;
	//cout << this->psocket->is_open() << endl;
	this->rtimer->expires_from_now(
			boost::posix_time::milliseconds(this->server.stimeout));
	this->rtimer->async_wait(
			boost::bind(&ConClient::timeoutHandler, this, csp, _1));
	boost::asio::async_read_until(*this->psocket, buf, DEFAULT_EOC,
			boost::bind(&ConClient::readHandle, this, csp, _1, _2));
}
int ConClient::asyncWrite(const char* data, size_t len) {
	this->psocket->async_write_some(buffer(data, len),
			boost::bind(&ConClient::writeHandler, this, _1, _2));
	return 0;
}
int ConClient::syncWrite(const char* data, size_t len) {
	boost::system::error_code ec;
	this->psocket->write_some(buffer(data, len), ec);
	if (ec.value()) {
		log.error("send data error(%s) to:%s", ec.message().c_str(),
				this->address().c_str());
	}
	return ec.value();
}
int ConClient::syncWrite(const char* data, size_t len,
		boost::system::error_code ec) {
	return this->psocket->write_some(buffer(data, len), ec);
}
size_t ConClient::syncRead(boost::array<char, R_BUF_SIZE>& buf,
		boost::system::error_code& ec) {
	return this->psocket->read_some(buffer(buf), ec);
}
string ConClient::address() {
	return this->radr.to_string();
}
void ConClient::writeHandler(const boost::system::error_code& ec,
		size_t bytes_transferred) {
	if (ec.value() == ECANCELED) {
		return;
	}
	if (ec.value()) {
		log.error("write data error to:%s", this->address().c_str());
		this->shutdown();
		return;
	}
}
void ConClient::readHandle(boost::shared_ptr<ConClient> sp,
		const boost::system::error_code& ec, std::size_t bytes_transferred) {
	if (ec.value() == ECANCELED) {
		return;
	}
	this->rtimer->cancel();
	if (ec.value()) {
		//cout << "val:" << ec.value() << ":" << ec.message() << endl;
		log.error("read data error(%s) from:%s", ec.message().c_str(),
				this->address().c_str());
		this->shutdown();
		return;
	}
	//check if valid to received data.
	if (bytes_transferred > BUF_SIZE) {
		string emsg("invalid command\n");
		this->syncWrite(emsg.c_str(), emsg.size());
		this->shutdown();
		return;
	}
	log.debug("bytes_transferred:%ld", bytes_transferred);
	//copy new buf to data.
	std::istream ibuf(&this->buf);
	ibuf.read(cbuf, bytes_transferred);
	cbuf[bytes_transferred] = 0;
	string data(cbuf);

	//if not receive intact command.
	if (!cmdb->initCmd(data)) {
		this->startRead();
		return;
	}
	log.debug("receive intact command (%s) from:%s", cmdb->cmd().c_str(),
			this->address().c_str());
	//if is valid command,execute command.
	cmdb->execCmd(this, &ibuf);
}
void ConClient::timeoutHandler(boost::shared_ptr<ConClient> sp,
		const boost::system::error_code& ec) {
	if (ec) {
		return;
	}
	this->log.debug("timeout client:%s", this->address().c_str());
	this->shutdown();
}

//int ConClient::executeCmd() {
//	this->asyncWrite("received\n", 10);
//	if (this->data.str() == "exit") {
//		this->shutdown();
//	} else {
//		this->data.str("");
//		this->startRead();
//	}
//	return 0;
//}
CmdBase::CmdBase() {
	this->seq = " \n\t\r";
	this->eoc = DEFAULT_EOC;
}

CmdBase::~CmdBase() {
}
void CmdBase::shutdown(ConClient* c) {

}
} /* namespace centny */
