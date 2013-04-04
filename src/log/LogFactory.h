/*
 * LogFactory.h
 *
 *  Created on: Jun 9, 2011
 *      Author: sco
 */
#ifndef LOGFACTORY_H_
#define LOGFACTORY_H_
#include <stdlib.h>
#include <iostream>
#include <locale.h>
#include <stdarg.h>
#include <stdio.h>
#if USING_LOG4CXX
#include <log4cxx/logger.h>
#include <log4cxx/consoleappender.h>
#include <log4cxx/simplelayout.h>
#include <log4cxx/patternlayout.h>
#include <log4cxx/logmanager.h>
#include <log4cxx/xml/domconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
using namespace log4cxx;
#endif

using namespace std;
class Log {
private:
	char buf[1000];
public:
//	int i;
#if USING_LOG4CXX
	LoggerPtr log;
#endif
	Log();
	Log& debug(const char* format, ...);
	Log& info(const char* format, ...);
	Log& warn(const char* format, ...);
	Log& error(const char* format, ...);
};
class LogFactory {
public:
	static string CONF_FILE;
	static int init(string cf);
	static int init();
	static Log createLogger(string name);
	LogFactory();
	virtual ~LogFactory();
};
#define C_LOG(X) LogFactory::createLogger(X)
#endif /* LOGFACTORY_H_ */
