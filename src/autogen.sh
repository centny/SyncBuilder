#!/bin/bash
echo 0
./automakeam.sh
oname=`uname`
if [ "x$oname" = "xDarwin" ];then
 cp om4/m4_ax_boost_filesystem_Darwin.m4 m4/m4_ax_boost_filesystem.m4
 echo configure by Mac OS X
else
 cp om4/m4_ax_boost_filesystem_Other.m4 m4/m4_ax_boost_filesystem.m4
 echo configure by Linux
fi
echo 1
aclocal --force -I m4
echo 2
autoheader
echo 3
touch NEWS README AUTHORS ChangeLog
echo 4
automake --add-missing
echo 5
automake
echo 6
autoconf
echo all end
