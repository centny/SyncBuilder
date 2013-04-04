/*
 * LogFactory.cpp
 *
 *  Created on: Jun 9, 2011
 *      Author: sco
 */

#include "LogFactory.h"
#include <iostream>
#include <string>
using namespace std;
Log::Log(){

}
Log& Log::debug(const char* format, ...) {
	va_list args;
	va_start(args, format);
	int res = vsnprintf(buf, 1000, format, args);
	va_end(args);
	buf[res] = 0;
#if USING_LOG4CXX
	log->debug(string(buf));
#else
	cout<<"DEBUG\t"<<string(buf)<<endl;
#endif
	return *this;
}
Log& Log::info(const char* format, ...) {
	va_list args;
	va_start(args, format);
	int res = vsnprintf(buf, 1000, format, args);
	va_end(args);
	buf[res] = 0;
#if USING_LOG4CXX
	log->info(string(buf));
#else
	cout<<"INFO\t"<<string(buf)<<endl;
#endif
	return *this;
}
Log& Log::warn(const char* format, ...) {
	va_list args;
	va_start(args, format);
	int res = vsnprintf(buf, 1000, format, args);
	va_end(args);
	buf[res] = 0;
#if USING_LOG4CXX
	log->warn(string(buf));
#else
	cout<<"WARN\t"<<string(buf)<<endl;
#endif
	return *this;
}
Log& Log::error(const char* format, ...) {
	va_list args;
	va_start(args, format);
	int res = vsnprintf(buf, 1000, format, args);
	va_end(args);
	buf[res] = 0;
#if USING_LOG4CXX
	log->error(string(buf));
#else
	cout<<"ERROR\t"<<string(buf)<<endl;
#endif
	return *this;
}
LogFactory::LogFactory() {

}

LogFactory::~LogFactory() {
}
//initial
string LogFactory::CONF_FILE = "";
Log LogFactory::createLogger(string name) {
	Log log;
#if USING_LOG4CXX
	log.log = Logger::getLogger(name);
#endif
	return log;
}
int LogFactory::init(string cf) {
	setlocale(LC_ALL, "");
	CONF_FILE = cf;
#if USING_LOG4CXX
	if (cf.length() > 4 && cf.substr(cf.length() - 4) == ".xml") {
		xml::DOMConfigurator::configure(cf);
	} else {
		PropertyConfigurator::configure(cf);
	}
#endif
	return 1;
}
int LogFactory::init() {
	setlocale(LC_ALL, "");
#if USING_LOG4CXX
	log4cxx::ConsoleAppenderPtr appender(new log4cxx::ConsoleAppender());
	log4cxx::PatternLayoutPtr layout(new log4cxx::PatternLayout());
#if LOG4CXX_LOGCHAR_IS_WCHAR
	layout->setConversionPattern(L"%d %5p [%c]  %m%n");
#else
	layout->setConversionPattern("%d %5p [%c]  %m%n");
#endif
	appender->setLayout(layout);
	log4cxx::helpers::Pool pool;
	appender->activateOptions(pool);
	log4cxx::Logger::getRootLogger()->addAppender(appender);
	LogManager::getLoggerRepository()->setConfigured(true);
#endif
	return 1;
}
