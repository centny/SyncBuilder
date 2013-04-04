dnl AX_CHECK_LOG4CXX([ ])
dnl
AC_DEFUN([AX_CHECK_LOG4CXX],
[
	# Set Log4Cxx Properties
	AC_ARG_WITH(log4cxx_prefix,
		[  --with-log4cxx-prefix=PFX     prefix where Log4Cxx is installed. (/opt/local)],
		[],
	        with_log4cxx_prefix=${LO4CXX_PATH:-/opt/local/})
	        
	LOG4CXX_PATH="$with_log4cxx_prefix"
	LOG4CXX_CFLAGS="-I$with_log4cxx_prefix/include"
	ac_log4cxx_ldlib="-L$with_log4cxx_prefix/lib"
	LOG4CXX_LDFLAGS="-llog4cxx $ac_log4cxx_ldlib -L/opt/local/lib -I/opt/local/include"
	AC_CACHE_CHECK(whether the log4cxx library is available,ax_cv_log4cxx,
		[
			CXXFLAGS="$CXXFLAGS -L/opt/local/lib -I/opt/local/include"
			LIBS=-llog4cxx
			AC_LANG_PUSH([C++])
			AC_COMPILE_IFELSE(
				[
					AC_LANG_PROGRAM(
						[@%:@include <log4cxx/logger.h>],
                   		[
                   			using namespace log4cxx;
                           	Logger::getLogger("A");
                          	return 0;
                       	])
              	],
				ax_cv_log4cxx=yes,
				ax_cv_log4cxx=no)
			AC_LINK_IFELSE(
				[
					AC_LANG_PROGRAM(
						[@%:@include <log4cxx/logger.h>],
                   		[
                   			using namespace log4cxx;
                           	Logger::getLogger("A");
                          	return 0;
                       	])
              	],
				ax_cv_log4cxx=yes,
				ax_cv_log4cxx=no)
        	AC_LANG_POP([C++])
        ],[],[]
	)
	if test "x$ax_cv_log4cxx" != "xyes" ;then
		AC_MSG_ERROR(the log4cxx library is not available)
	fi
	AC_SUBST(LOG4CXX_PATH)
	AC_SUBST(LOG4CXX_CFLAGS)
	AC_SUBST(LOG4CXX_LDFLAGS)
	AC_SUBST(LOG4CXX_LIBS)
])
