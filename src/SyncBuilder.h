/*
 * SyncBuilder.h
 *
 *  Created on: Nov 4, 2012
 *      Author: Scorpion
 */

#ifndef SYNCBUILDER_H_
#define SYNCBUILDER_H_
#include <string>
using namespace std;
//
void initSyncBuilder(string cfg);
void runSyncBuilder();
void stopSyncBuilder();
//service interface.
void printHelp();
int initService(int argc,char** argv);
int runService(int argc,char** argv);
int stopService(int argc,char** argv);
//
#endif /* SYNCBUILDER_H_ */
