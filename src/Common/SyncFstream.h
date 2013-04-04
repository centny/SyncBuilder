/*
 * SyncFstream.h
 *
 *  Created on: Nov 6, 2012
 *      Author: Scorpion
 */

#ifndef SYNCFSTREAM_H_
#define SYNCFSTREAM_H_
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <fstream>
using namespace std;
namespace centny {

class SyncFstream: public std::fstream {
private:
	boost::mutex fmutex;
public:
	SyncFstream& write(const char_type* __s, streamsize __n);
	SyncFstream();
	SyncFstream(const char *_Filename,
		ios_base::openmode _Mode = ios_base::in | ios_base::out);
	virtual ~SyncFstream();
};

} /* namespace centny */
#endif /* SYNCFSTREAM_H_ */
