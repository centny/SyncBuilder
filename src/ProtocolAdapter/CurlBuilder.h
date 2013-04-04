/*
 * Curl.h
 *
 *  Created on: Nov 10, 2012
 *      Author: Scorpion
 */

#ifndef CURL_H_
#define CURL_H_
#include <curl/curl.h>
#include <errno.h>
#include <string>
#include <iostream>
#include <vector>
#include <sstream>
using namespace std;
namespace centny {
//

size_t defaultStreamWriteCb(void *ptr, size_t size, size_t nmemb, void *info);
size_t defaultStreamReadCb(void *ptr, size_t size, size_t nmemb, void *info);
size_t defaultWriteCb(void *ptr, size_t size, size_t nmemb, void *info);
//
size_t defaultReadCb(void *ptr, size_t size, size_t nmemb, void *info);
long deafultChunkBegin(void *info);
long deafultChunkEnd(void *info);

//define.
typedef size_t (*CurlCallback)(void *ptr, size_t size, size_t nmemb,
		void *info);
typedef long CurlEvent(void *ptr);

struct FName {
	string url;
	FILE *stream;
};

//
class CurlBuilder {
public:
	static CURLcode init(long flags = CURL_GLOBAL_ALL);
	static void clean();
	static off_t fsize(const char* fp);
protected:
//	struct curl_slist *preHeader;
	struct curl_slist *header;
//	struct curl_slist *postHeader;
	CURL *curl;
	FName file;
public:
	stringstream buf;
public:
	CurlBuilder();
	virtual void showDebug(long sd = 0);
	virtual const char* perform();
	virtual const char* finfo(time_t& mdtm, unsigned long long& fsize);
//	virtual void appPreHeader(string h);
	virtual void appHeader(string h);
//	virtual void appPostHeader(string h);
	virtual void cleanHeader();
	virtual void setCustomRequest(string req);
	virtual string text();
	//common method
	//if the info is NULL,it will setting the "FName file".
	virtual void setReadCallback(CurlCallback rc = &defaultReadCb, void* info =
			0);
	virtual void setChunkEndCallback(CurlEvent rc = &deafultChunkEnd,
			void* info = 0);
	virtual void setChunkBeginCallback(CurlEvent rc = &deafultChunkBegin,
			void* info = 0);
	virtual void setWriteCallback(CurlCallback rc = &defaultWriteCb,
			void* info = 0);
	//
	virtual void setUpload(long iu = 0);
	virtual int setUFile(string uf);
	virtual int setDFile(string df);
	virtual void setUsrPwd(string auth);
	virtual void setAnonymous();
	virtual void setURL(string url);
	virtual void setOpt(CURLoption key, void* val);
	virtual void setOpt(CURLoption key, long val);
	virtual void reset();
	virtual void reinit();
	virtual ~CurlBuilder();
};

} /* namespace centny */
#endif /* CURL_H_ */
