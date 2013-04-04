/*
 * DataPool.cpp
 *
 *  Created on: Nov 5, 2012
 *      Author: Scorpion
 */

#include "DataPool.h"
#include <stdio.h>
namespace centny {
DataPool::DObj::DObj() {

}
DataPool::DObj::~DObj() {

}
//
static DataPool _pool;
DataPool& DataPool::sharedPool() {
	return _pool;
}
DataPool::DId DataPool::add(DataPool::DObj* data) {
	char tmp[256];
	int len = sprintf(tmp, "%p", data);
	tmp[len] = 0;
	string key(tmp);
	CONTAINERS_LOCK;
	this->containers[key] = data;
	return key;
}
void DataPool::fre(DataPool::DId& id) {
	CONTAINERS_LOCK;
	map<string, DObj*>::iterator it=this->containers.find(id);
	if(it!=this->containers.end()) {
		it->second->fre();
		this->containers.erase(it);
	}
}
DataPool::DObj* DataPool::data(DataPool::DId& id) {
	CONTAINERS_LOCK;
	map<string, DObj*>::iterator it=this->containers.find(id);
	if(it!=this->containers.end()) {
		return it->second;
	} else {
		return 0;
	}
}
DataPool::DataPool() {
}

DataPool::~DataPool() {
}

} /* namespace centny */
