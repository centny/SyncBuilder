#!/bin/sh
sfs=`find . -name '*.cpp'`
echo >Makefile.am
echo AM_CXXFLAGS=-DUSING_LOG4CXX=1 -DDEV_NO_SERVICE=0 -L/opt/local/lib -I/opt/local/include>>Makefile.am
echo bin_PROGRAMS=SyncBuilder>>Makefile.am
echo "LIBS=\$(SQLITE3_LDFLAGS) \$(BOOST_DATE_TIME_LIB) \$(BOOST_FILESYSTEM_LIB) \$(BOOST_REGEX_LIB) \$(BOOST_THREAD_LIB) \$(BOOST_SYSTEM_LIB) \$(LOG4CXX_LIBS) \$(EXTC_LDFLAGS) \$(LOG4CXX_LDFLAGS) -lcurl">>Makefile.am
echo SyncBuilder_SOURCES=$sfs>>Makefile.am

echo >>Makefile.am
echo >>Makefile.am
echo CFG_DIR=/etc/sbuilder>>Makefile.am

echo 'install-exec-hook:
	(\
if [ ! -d $(CFG_DIR) ] ; \
then \
mkdir $(CFG_DIR);\
fi;\
cp ProtocolAdapter/Net.cfg $(CFG_DIR)/Net.cfg_s;\
cp EventMgr/e.cfg $(CFG_DIR)/e.cfg_s;\
cp log/log.conf $(CFG_DIR)/log.conf_s;\
cp SyncServer/Demoes.cfg $(CFG_DIR)/Demoes.cfg_s;\
cp SyncServer/users.cfg $(CFG_DIR)/users.cfg_s;\
cp SyncServer/web.cfg $(CFG_DIR)/web.cfg_s;\
cp sbuilder $(prefix)/bin;\
chmod +x $(prefix)/bin/sbuilder;\
)'>>Makefile.am

###
echo 'uninstall-hook:
	(rm $(prefix)/bin/sbuilder;)'>>Makefile.am