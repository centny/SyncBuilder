/*
 * SyncFstream.cpp
 *
 *  Created on: Nov 6, 2012
 *      Author: Scorpion
 */

#include "SyncFstream.h"

namespace centny {
SyncFstream& SyncFstream::write(const char_type* __s, streamsize __n) {
	boost::mutex::scoped_lock lock(this->fmutex);
	ostream::write(__s, __n);
	return *this;
}
SyncFstream::SyncFstream(const char *_Filename, ios_base::openmode _Mode) :
		fstream(_Filename, _Mode) {
}
SyncFstream::SyncFstream() {
}

SyncFstream::~SyncFstream() {
}

} /* namespace centny */
