/*
 * SyncServer.cpp
 *
 *  Created on: Nov 30, 2012
 *      Author: Scorpion
 */

#include "SyncServer.h"
#include "ConClient.h"
namespace centny {
using namespace boost;
using namespace boost::asio;
#define CLEAR_TIMEOUT 5000
SyncServer::SyncServer(io_service& ios, int port, CmdBase* cmd, string name) :
		log(C_LOG("SyncServer."+name)),iosev(ios) {
			this->stimeout = 300000;
			this->cmdb=cmd;
			log.info("initial SyncServer by port:%d",port);
			this->acceptor=new ip::tcp::acceptor(iosev, tcp::endpoint(tcp::v4(),port));
			this->ctimer=this->deadlineTimer();
			this->ctimer->expires_from_now(
					boost::posix_time::milliseconds(CLEAR_TIMEOUT));
			this->ctimer->async_wait(boost::bind(&SyncServer::ctimerHandler,this, _1));
		}

SyncServer::~SyncServer() {
}
boost::shared_ptr<deadline_timer> SyncServer::deadlineTimer() {
	boost::mutex::scoped_lock lock(this->smutex);
	return boost::shared_ptr<deadline_timer>(new deadline_timer(this->iosev));
}
long SyncServer::socketTimeout() {
	boost::mutex::scoped_lock lock(this->smutex);
	return this->stimeout;
}
void SyncServer::setSocketTimeout(long sto) {
	boost::mutex::scoped_lock lock(this->smutex);
	this->stimeout = sto;
}
boost::thread_group& SyncServer::thrGrps() {
	return this->tgrps;
}
void SyncServer::accept() {
	boost::shared_ptr<ConClient> client = boost::shared_ptr<ConClient>(
			new ConClient(*this, this->cmdb));
	boost::shared_ptr<tcp::socket>& rsoc = client->psocket;
	this->cfgSocketTimeouts(*(client->psocket), this->stimeout);
	acceptor->async_accept(*rsoc,
			boost::bind(&SyncServer::acceptHandler, this, client, _1));
}
//void SyncServer::run() {
//	this->iosev.run();
//}
//void SyncServer::stop() {
//	vector<ConClient*>::iterator it, end;
//	cout << "client size:" << this->clients.size() << endl;
//	for (it = this->clients.begin(); it != end; it++) {
//		delete *it;
//	}
//	this->clients.clear();
//	this->iosev.stop();
//}
//void SyncServer::remove(shared_ptr<ConClient> client) {
//	vector<shared_ptr<ConClient>>::iterator it, end;
//	end = this->clients.end();
//	it = std::find(this->clients.begin(), end, client);
//	if (it != end) {
//		this->clients.erase(it);
//	}
//}
void SyncServer::acceptHandler(boost::shared_ptr<ConClient> client,
							   const boost::system::error_code& ec) {
	this->clients.push_back(client);
	if (ec.value()) {
		log.error("accepting error:%s", ec.message().c_str());
		this->accept();
		return;
	} else {
		log.debug("accepting one connect:%s", client->address().c_str());
		client->setSp(client);
		client->startRead();
		this->accept();
	}
}

void SyncServer::cfgSocketTimeouts(tcp::socket& socket, long timeout) {
#if defined OS_WINDOWS
	int32_t timeout =(int32_t)timeout;
	setsockopt(socket.native(), SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
	setsockopt(socket.native(), SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));
#else
	struct timeval tv;
	tv.tv_sec = timeout / 1000;
	tv.tv_usec = 0;
	setsockopt(socket.native(), SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
	setsockopt(socket.native(), SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof(tv));
#endif
}
void SyncServer::ctimerHandler(const boost::system::error_code& ec) {
	if (ec.value() == ECANCELED) {
		return;
	}
	this->ctimer->expires_from_now(
			boost::posix_time::milliseconds(CLEAR_TIMEOUT));
	this->ctimer->async_wait(boost::bind(&SyncServer::ctimerHandler, this, _1));
	vector<boost::shared_ptr<ConClient> >::iterator it;
	size_t size = this->clients.size();
	if (size < 1) {
		return;
	}
	it = this->clients.begin();
	for (;it!=this->clients.end();it++) {
		if (it->use_count() < 2) {
			it=this->clients.erase(it);
			if(it==this->clients.end()){
				break;
			}
		}
	}
	log.debug("current %d clients,erase %d client",this->clients.size(),
			size - this->clients.size());
}
}
/* namespace centny */
