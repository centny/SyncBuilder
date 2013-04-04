/*
 * common.h
 *
 *  Created on: Nov 5, 2012
 *      Author: Scorpion
 */

#ifndef COMMON_H_
#define COMMON_H_
#include "log/LogFactory.h"
#include "DataPool/DataPool.h"
#include <iostream>
#include <string>
#include <vector>
using namespace std;
using namespace centny;
#define bsleep(x) boost::this_thread::sleep(boost::posix_time::milliseconds(x))
#define BUF_SIZE 1024
#define R_BUF_SIZE 2048
#define DEFAULT_EOC "\r\n"
#endif /* COMMON_H_ */
