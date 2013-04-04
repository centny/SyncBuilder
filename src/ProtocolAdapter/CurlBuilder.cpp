/*
 * Curl.cpp
 *
 *  Created on: Nov 10, 2012
 *      Author: Scorpion
 */

#include "CurlBuilder.h"
#include "../Common/ReplaceAll.h"
#include "../log/LogFactory.h"
#include <boost/filesystem.hpp>
namespace centny {
//
size_t defaultStreamWriteCb(void *ptr, size_t size, size_t nmemb, void *info) {
	ostream *buf = (ostream*) info;
	buf->write((char*) ptr, nmemb);
	return nmemb;
}
size_t defaultStreamReadCb(void *ptr, size_t size, size_t nmemb, void *info) {
	istream *buf = (istream*) info;
	buf->read((char*) ptr, nmemb);
	return (size_t) buf->gcount();
}
size_t defaultWriteCb(void *ptr, size_t size, size_t nmemb, void *info) {
	FName *fn = (FName*) info;
	deafultChunkBegin(info);
	fwrite(ptr, size, nmemb, fn->stream);
	return nmemb;
}
size_t defaultReadCb(void *ptr, size_t size, size_t nmemb, void *info) {
	FName *fn = (FName*) info;
	deafultChunkBegin(info);
	size_t rs = fread((char*) ptr, size, nmemb, fn->stream);
	if (rs < 1) {
		deafultChunkEnd(info);
	}
	return rs;
}
long deafultChunkBegin(void *info) {
	FName *fn = (FName*) info;
	if (fn->stream == 0) {
		fn->stream = fopen(fn->url.c_str(), "wrb");
		Log log = C_LOG("CurlBuilder");
		if (fn->stream) {
			log.debug("open file:%s", fn->url.c_str());
		} else {
			log.error("open file error:%s", fn->url.c_str());
			return 0;
		}
	}
	return CURL_CHUNK_END_FUNC_OK;
}
long deafultChunkEnd(void *info) {
	FName *fn = (FName*) info;
	if (fn->stream) {
		Log log = C_LOG("CurlBuilder");
		log.debug("close file:%s\n", fn->url.c_str());
		fclose(fn->stream);
		fn->stream = 0;
	}
	return CURL_CHUNK_END_FUNC_OK;
}
//
CURLcode CurlBuilder::init(long flags) {
	return curl_global_init(flags);
}
void CurlBuilder::clean() {
	curl_global_cleanup();
}
off_t CurlBuilder::fsize(const char* fp) {
	boost::system::error_code ec;
	off_t fs = (off_t) boost::filesystem::file_size(
			boost::filesystem::path(string(fp)), ec);
	if (ec.value()) {
		Log log = C_LOG("CurlBuilder");
		log.error("get file(%s) size error:%s", fp, ec.message().c_str());
		return 0;
	}
	return fs;
}

//
CurlBuilder::CurlBuilder() {
//	this->preHeader = 0;
	this->header = 0;
//	this->postHeader = 0;
	this->file.url = "";
	this->file.stream = 0;
	curl = curl_easy_init();
}
void CurlBuilder::showDebug(long sd) {
	curl_easy_setopt(curl, CURLOPT_VERBOSE, sd);
}
const char* CurlBuilder::perform() {
//	curl_easy_setopt(curl, CURLOPT_PREQUOTE, preHeader);
	curl_easy_setopt(curl, CURLOPT_QUOTE, header);
//	curl_easy_setopt(curl, CURLOPT_POSTQUOTE, postHeader);
	buf.clear();
//	curl_easy_setopt(curl, CURLOPT_FTP_CREATE_MISSING_DIRS, 1L);
	CURLcode res;
	/* Now run off and do what you've been told! */
	res = curl_easy_perform(curl);
	if (this->file.stream) {
		Log log = C_LOG("CurlBuilder");
		log.debug("close file:%s", file.url.c_str());
		fclose(this->file.stream);
		this->file.stream = 0;
	}
		/* Check for errors */
	if (res == CURLE_OK) {
		return 0;
	} else {
		return curl_easy_strerror(res);
	}
}
const char* CurlBuilder::finfo(time_t& mdtm, unsigned long long& fsize) {
	curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
	curl_easy_setopt(curl, CURLOPT_FILETIME, 1L);
	stringstream tmp;
	this->setWriteCallback(defaultStreamWriteCb, &tmp);
	CURLcode res;
	res = curl_easy_perform(curl);
	if (res == CURLE_OK) {
		res = curl_easy_getinfo(curl, CURLINFO_FILETIME, &mdtm);
		res = curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &fsize);
	}
	if (res == CURLE_OK) {
		return 0;
	} else {
		return curl_easy_strerror(res);
	}
}
//void CurlBuilder::appPreHeader(string h) {
//	preHeader = curl_slist_append(preHeader, h.c_str());
//}
void CurlBuilder::appHeader(string h) {
	header = curl_slist_append(header, h.c_str());
}
//void CurlBuilder::appPostHeader(string h) {
//	postHeader = curl_slist_append(postHeader, h.c_str());
//}
void CurlBuilder::cleanHeader() {
//	if (preHeader) {
//		curl_slist_free_all(preHeader);
//		preHeader = 0;
//	}
	if (header) {
		curl_slist_free_all(header);
		header = 0;
	}
//	if (postHeader) {
//		curl_slist_free_all(postHeader);
//		postHeader = 0;
//	}
}
void CurlBuilder::setCustomRequest(string req) {
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, req.c_str());
}
string CurlBuilder::text() {
	return buf.str();
}
//
void CurlBuilder::setReadCallback(CurlCallback rc, void* info) {
	/* we want to use our own read function */
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, rc);
	/* now specify which file to upload */
	if (info) {
		curl_easy_setopt(curl, CURLOPT_READDATA, info);
	} else {
		curl_easy_setopt(curl, CURLOPT_READDATA, &this->file);
	}
}
void CurlBuilder::setChunkEndCallback(CurlEvent rc, void* info) {
	curl_easy_setopt(curl, CURLOPT_CHUNK_END_FUNCTION, rc);
	if (info) {
		curl_easy_setopt(curl, CURLOPT_CHUNK_DATA, info);
	} else {
		curl_easy_setopt(curl, CURLOPT_CHUNK_DATA, &this->file);
	}
}
void CurlBuilder::setChunkBeginCallback(CurlEvent rc, void* info) {
	curl_easy_setopt(curl, CURLOPT_CHUNK_BGN_FUNCTION, rc);
	if (info) {
		curl_easy_setopt(curl, CURLOPT_CHUNK_DATA, info);
	} else {
		curl_easy_setopt(curl, CURLOPT_CHUNK_DATA, &this->file);
	}
}
void CurlBuilder::setWriteCallback(CurlCallback rc, void* info) {
	/* we want to use our own read function */
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, rc);
	/* now specify which file to upload */
	if (info) {
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, info);
	} else {
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &file);
	}
}
void CurlBuilder::setUpload(long iu) {
	curl_easy_setopt(curl, CURLOPT_UPLOAD, iu);
}
int CurlBuilder::setUFile(string uf) {
	if (this->file.stream) {
		fclose(this->file.stream);
		this->file.stream = 0;
	}
	if (uf.length()) {
		curl_off_t fz = (curl_off_t) fsize(uf.c_str());
		if (fz) {
			curl_easy_setopt(curl, CURLOPT_INFILE, fz);
			this->file.url = uf;
//			this->setChunkBeginCallback();
			this->setReadCallback();
//			this->setChunkEndCallback();
			return 1;
		}
	}
	this->file.url = "";
	this->setReadCallback(0, 0);
	return 0;
}
int CurlBuilder::setDFile(string df) {
	if (this->file.stream) {
		fclose(this->file.stream);
		this->file.stream = 0;
	}
	if (df.length()) {
		this->file.url = df;
//		this->setChunkBeginCallback();
		this->setWriteCallback();
//		this->setChunkEndCallback();
		return 1;
	}
	this->file.url = "";
	this->setWriteCallback(0, 0);
	return 0;
}
void CurlBuilder::setUsrPwd(string auth) {
	/* tell libcurl we can use "any" auth, which lets the lib pick one, but it
	 also costs one extra round-trip and possibly sending of all the PUT
	 data twice!!! */
	curl_easy_setopt(curl, CURLOPT_HTTPAUTH, (long)CURLAUTH_ANY);

	/* set user name and password for the authentication */
	curl_easy_setopt(curl, CURLOPT_USERPWD, auth.c_str());
}
void CurlBuilder::setAnonymous() {
	curl_easy_setopt(curl, CURLOPT_USERNAME, "anonymous");
}
void CurlBuilder::setURL(string url) {
	/* specify target */
	replaceAll(url, "//", "/", 6);
//	cout << "URL:" << url << endl;
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
}
void CurlBuilder::setOpt(CURLoption key, void* val) {
	curl_easy_setopt(curl, key, val);
}
void CurlBuilder::setOpt(CURLoption key, long val) {
	curl_easy_setopt(curl, key, val);
}
void CurlBuilder::reset() {
	curl_easy_reset(curl);
	buf.clear();
	if (header) {
		curl_slist_free_all(header);
		header = 0;
	}
}
void CurlBuilder::reinit() {
	this->reset();
	/* always cleanup */
	if (curl) {
		curl_easy_cleanup(curl);
		curl = 0;
	}

	/* close opened file */
	if (file.stream) {
		fclose(file.stream);
		file.stream = 0;
	}
	buf.clear();
	this->file.url = "";
	curl = curl_easy_init();
}
CurlBuilder::~CurlBuilder() {
	/* clean up the FTP commands list */
	if (header) {
		curl_slist_free_all(header);
		header = 0;
	}
}

} /* namespace centny */
