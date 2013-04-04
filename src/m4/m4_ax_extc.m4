dnl AX_CHECK_LOG4CXX([ ])
dnl
AC_DEFUN([AX_CHECK_EXTC],
[
	# Set Log4Cxx Properties
	AC_ARG_WITH(extc_prefix,
		[  --with-extc-prefix=PFX     prefix where extc is installed. (/usr/local)],
		[],
	        with_extc_prefix=${EXTC_PATH:-/usr/local})
	        
	EXTC_PATH="$with_extc_prefix"
	EXTC_CFLAGS="-I$with_extc_prefix/include"
	ac_extc_ldlib="-L$with_extc_prefix/lib"
	EXTC_LDFLAGS="-lextc $ac_EXTC_ldlib -L/opt/local/lib -I/opt/local/include"
	AC_CACHE_CHECK(whether the extc library is available,ax_cv_extc,
		[
			CXXFLAGS="$CXXFLAGS -L/opt/local/lib -I/opt/local/include"
			LIBS=-lextc
			AC_LANG_PUSH([C++])
			AC_COMPILE_IFELSE(
				[
					AC_LANG_PROGRAM(
						[@%:@include <ExtCaller.h>],
                   		[
                   			ExtCaller ec;
                   			ec.execute();
                          	return 0;
                       	])
              	],
				ax_cv_log4cxx=yes,
				ax_cv_log4cxx=no)
			AC_LINK_IFELSE(
				[
					AC_LANG_PROGRAM(
						[@%:@include <ExtCaller.h>],
                   		[
                   			ExtCaller ec;
                   			ec.execute();
                          	return 0;
                       	])
              	],
				ax_cv_extc=yes,
				ax_cv_extc=no)
        	AC_LANG_POP([C++])
        ],[],[]
	)
	if test "x$ax_cv_extc" != "xyes" ;then
		AC_MSG_ERROR(the extc library is not available)
	fi
	AC_SUBST(EXTC_PATH)
	AC_SUBST(EXTC_CFLAGS)
	AC_SUBST(EXTC_LDFLAGS)
	AC_SUBST(EXTC_LIBS)
])
