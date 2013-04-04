/*
 * DataPool.h
 *
 *  Created on: Nov 5, 2012
 *      Author: Scorpion
 */

#ifndef DATAPOOL_H_
#define DATAPOOL_H_

#include <map>
#include <boost/thread/mutex.hpp>
using namespace boost;
using namespace std;
namespace centny {

class DataPool {
public:
	typedef string DId;
	class DObj {
	public:
		DObj();
		virtual void fre()=0;
		virtual ~DObj();
	};
private:
	map<string, DObj*> containers;
	boost::mutex containers_mutex;
#define CONTAINERS_LOCK boost::mutex::scoped_lock lock(this->containers_mutex)
public:
	static DataPool& sharedPool();
	DId add(DObj* data);
	void fre(DId& id);
	DObj* data(DId& id);
	DataPool();
	virtual ~DataPool();
};

} /* namespace centny */
#endif /* DATAPOOL_H_ */
