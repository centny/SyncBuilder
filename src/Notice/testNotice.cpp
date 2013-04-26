/*
 * testNotice.cpp
 *
 *  Created on: Apr 26, 2013
 *      Author: Scorpion
 */
#include "NoticeCenter.h"
using namespace centny;
class TNotice: public Noticeable {
	void receive(string name, DataPool::DId did) {
		cout << name << endl;
	}
};

void testNotice() {
	TNotice* tn = new TNotice();
	NoticeCenter::defaultCenter().reg("abc", tn);
}
