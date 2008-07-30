dnl Copyright (C) 2007,2008 the authors unless statet otherwise
dnl
dnl This file is part of the Fhtagn! C++ Library, and may be distributed under
dnl the following license terms and conditions as set forth in the COPYING file
dnl included in this source distribution.

dnl
dnl Checks whether the compiler's -pedantic flag is pedantic enough to produce
dnl errors on C++ code using the long long type.
dnl
AC_DEFUN([AX_TYPE_CXX_LONGLONG],
[
  AC_LANG_PUSH([C++])
  old_CXXFLAGS=${CXXFLAGS}
  CXXFLAGS="${CXXFLAGS} -std=c++98 -ansi -pedantic"
  AC_CACHE_CHECK([for long long type in pedantic C++], _have_cxx_pedantic_longlong,
    [AC_TRY_COMPILE([
       long long foo = (long long) 0;], ,
       _have_cxx_pedantic_longlong=yes,
       _have_cxx_pedantic_longlong=no)])
  if test x$_have_cxx_pedantic_longlong = xyes; then
    AC_DEFINE(HAVE_CXX_PEDANTIC_LONG_LONG, 1,
      [Define if you have the 'long long' type in C++ with -pedantic.])
  fi
  CXXFLAGS=${old_CXXFLAGS}

  AM_CONDITIONAL([HAVE_CXX_PEDANTIC_LONG_LONG],
    [test x$_have_cxx_pedantic_longlong = xyes])
  AC_LANG_POP([C++])
])

dnl
dnl Defines and exports PACKAGE_MAJOR and PACKAGE_MINOR
dnl
AC_DEFUN([AM_FHTAGN_VERSION],
[
  AC_DEFINE(PACKAGE_MAJOR,$1,[Define to the major version number of this package.])
  AC_DEFINE(PACKAGE_MINOR,$2,[Define to the minor version number of this package.])
  PACKAGE_MAJOR="$1"
  PACKAGE_MINOR="$2"
  AC_SUBST([PACKAGE_MAJOR])
  AC_SUBST([PACKAGE_MINOR])
])


dnl
dnl Two macros to collect information on optional parts of the build, and
dnl to display it later on.
dnl
AC_DEFUN([AM_FHTAGN_HAVE],
[
  if echo "$1" | grep -q ':,' ; then
    AC_MSG_ERROR([Cannot use the characters ':' and ',' in parameters passed to
      the FHTAGN_HAVE macro])
  fi
  if eval $2 ; then
    am_fhtagn_have_res=yes
  else
    am_fhtagn_have_res=no
  fi
  am_fhtagn_have_string="$am_fhtagn_have_string $1:$am_fhtagn_have_res"
])

AC_DEFUN([AM_FHTAGN_HAVE_DISPLAY],
[
  echo "**************************************************"
  echo ""
  echo "  Building optional components dependent on the"
  echo "  following external libraries:"
  echo ""
  for am_fhtagn_have_lib in $am_fhtagn_have_string ; do
    am_fhtagn_libname=$(echo $am_fhtagn_have_lib | cut -d: -f1)
    am_fhtagn_have=$(echo $am_fhtagn_have_lib | cut -d: -f2)

    echo -e "  $am_fhtagn_libname: $am_fhtagn_have"
  done
  echo ""
  echo "**************************************************"
])


dnl AX_BOOST_BASE Copyright (c) 2006 Thomas Porschberg <thomas@randspringer.de>
dnl               Copyright (c) 2007 Jens Finkhaeuser <unwesen@users.sourceforge.net>
dnl Modified to not abort if boost is not found by Jens Finkhaeuser. Also makes
dnl HAVE_BOOST an automake conditional, in order to perform conditional builds.
AC_DEFUN([AX_BOOST_BASE],
[
AC_ARG_WITH([boost],
        AS_HELP_STRING([--with-boost@<:@=DIR@:>@], [use boost (default is No) - it is possible to specify the root directory for boost (optional)]),
        [
    if test "$withval" = "no"; then
                want_boost="no"
    elif test "$withval" = "yes"; then
        want_boost="yes"
        ac_boost_path=""
    else
            want_boost="yes"
        ac_boost_path="$withval"
        fi
    ],
    [want_boost="yes"])

if test "x$want_boost" = "xyes"; then
        boost_lib_version_req=ifelse([$1], ,1.20.0,$1)
        boost_lib_version_req_shorten=`expr $boost_lib_version_req : '\([[0-9]]*\.[[0-9]]*\)'`
        boost_lib_version_req_major=`expr $boost_lib_version_req : '\([[0-9]]*\)'`
        boost_lib_version_req_minor=`expr $boost_lib_version_req : '[[0-9]]*\.\([[0-9]]*\)'`
        boost_lib_version_req_sub_minor=`expr $boost_lib_version_req : '[[0-9]]*\.[[0-9]]*\.\([[0-9]]*\)'`
        if test "x$boost_lib_version_req_sub_minor" = "x" ; then
                boost_lib_version_req_sub_minor="0"
        fi
        WANT_BOOST_VERSION=`expr $boost_lib_version_req_major \* 100000 \+  $boost_lib_version_req_minor \* 100 \+ $boost_lib_version_req_sub_minor`
        AC_MSG_CHECKING(for boostlib >= $boost_lib_version_req)
        succeeded=no

        dnl first we check the system location for boost libraries
        dnl this location ist chosen if boost libraries are installed with the --layout=system option
        dnl or if you install boost with RPM
        if test "$ac_boost_path" != ""; then
                BOOST_LDFLAGS="-L$ac_boost_path/lib"
                BOOST_CPPFLAGS="-I$ac_boost_path/include"
        else
                for ac_boost_path_tmp in /usr /usr/local /opt ; do
                        if test -d "$ac_boost_path_tmp/include/boost" && test -r "$ac_boost_path_tmp/include/boost"; then
                                BOOST_LDFLAGS="-L$ac_boost_path_tmp/lib"
                                BOOST_CPPFLAGS="-I$ac_boost_path_tmp/include"
                                break;
                        fi
                done
        fi

        CPPFLAGS_SAVED="$CPPFLAGS"
        CPPFLAGS="$CPPFLAGS $BOOST_CPPFLAGS"
        export CPPFLAGS

        LDFLAGS_SAVED="$LDFLAGS"
        LDFLAGS="$LDFLAGS $BOOST_LDFLAGS"
        export LDFLAGS

        AC_LANG_PUSH(C++)
        AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
        @%:@include <boost/version.hpp>
        ]], [[
        #if BOOST_VERSION >= $WANT_BOOST_VERSION
        // Everything is okay
        #else
        #  error Boost version is too old
        #endif
        ]])],[
        AC_MSG_RESULT(yes)
        succeeded=yes
        found_system=yes
        ],[
        ])
        AC_LANG_POP([C++])



        dnl if we found no boost with system layout we search for boost libraries
        dnl built and installed without the --layout=system option or for a staged(not installed) version
        if test "x$succeeded" != "xyes"; then
                _version=0
                if test "$ac_boost_path" != ""; then
                        BOOST_LDFLAGS="-L$ac_boost_path/lib"
                        if test -d "$ac_boost_path" && test -r "$ac_boost_path"; then
                                for i in `ls -d $ac_boost_path/include/boost-* 2>/dev/null`; do
                                        _version_tmp=`echo $i | sed "s#$ac_boost_path##" | sed 's/\/include\/boost-//' | sed 's/_/./'`
                                        V_CHECK=`expr $_version_tmp \> $_version`
                                        if test "$V_CHECK" = "1" ; then
                                                _version=$_version_tmp
                                        fi
                                        VERSION_UNDERSCORE=`echo $_version | sed 's/\./_/'`
                                        BOOST_CPPFLAGS="-I$ac_boost_path/include/boost-$VERSION_UNDERSCORE"
                                done
                        fi
                else
                        for ac_boost_path in /usr /usr/local /opt ; do
                                if test -d "$ac_boost_path" && test -r "$ac_boost_path"; then
                                        for i in `ls -d $ac_boost_path/include/boost-* 2>/dev/null`; do
                                                _version_tmp=`echo $i | sed "s#$ac_boost_path##" | sed 's/\/include\/boost-//' | sed 's/_/./'`
                                                V_CHECK=`expr $_version_tmp \> $_version`
                                                if test "$V_CHECK" = "1" ; then
                                                        _version=$_version_tmp
                                                        best_path=$ac_boost_path
                                                fi
                                        done
                                fi
                        done

                        VERSION_UNDERSCORE=`echo $_version | sed 's/\./_/'`
                        BOOST_CPPFLAGS="-I$best_path/include/boost-$VERSION_UNDERSCORE"
                        BOOST_LDFLAGS="-L$best_path/lib"

                        if test "x$BOOST_ROOT" != "x"; then
                                if test -d "$BOOST_ROOT" && test -r "$BOOST_ROOT" && test -d "$BOOST_ROOT/stage/lib" && test -r "$BOOST_ROOT/stage/lib"; then
                                        version_dir=`expr //$BOOST_ROOT : '.*/\(.*\)'`
                                        stage_version=`echo $version_dir | sed 's/boost_//' | sed 's/_/./g'`
                                        stage_version_shorten=`expr $stage_version : '\([[0-9]]*\.[[0-9]]*\)'`
                                        V_CHECK=`expr $stage_version_shorten \>\= $_version`
                                        if test "$V_CHECK" = "1" ; then
                                                AC_MSG_NOTICE(We will use a staged boost library from $BOOST_ROOT)
                                                BOOST_CPPFLAGS="-I$BOOST_ROOT"
                                                BOOST_LDFLAGS="-L$BOOST_ROOT/stage/lib"
                                        fi
                                fi
                        fi
                fi

                CPPFLAGS="$CPPFLAGS $BOOST_CPPFLAGS"
                export CPPFLAGS
                LDFLAGS="$LDFLAGS $BOOST_LDFLAGS"
                export LDFLAGS

                AC_LANG_PUSH(C++)
                AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
                @%:@include <boost/version.hpp>
                ]], [[
                #if BOOST_VERSION >= $WANT_BOOST_VERSION
                // Everything is okay
                #else
                #  error Boost version is too old
                #endif
                ]])],[
                AC_MSG_RESULT([$_version])
                succeeded=yes
                found_system=yes
                ],[
                ])
                AC_LANG_POP([C++])
        fi

        if test "$succeeded" != "yes" ; then
                if test "$_version" = "0" ; then
                        AC_MSG_RESULT([no])
                else
                        AC_MSG_RESULT([no (found version $_version)])
                fi
                BOOST_CPPFLAGS=""
                BOOST_LDFLAGS=""
        else
                AC_SUBST(BOOST_CPPFLAGS)
                AC_SUBST(BOOST_LDFLAGS)
                AC_DEFINE(HAVE_BOOST,1,[define if the Boost library is available])
        fi
        AM_CONDITIONAL([HAVE_BOOST], [test x$succeeded = xyes])
        AM_FHTAGN_HAVE([boost], [test x$succeeded = xyes])

        CPPFLAGS="$CPPFLAGS_SAVED"
        LDFLAGS="$LDFLAGS_SAVED"
fi

])




dnl
dnl AM_PATH_CPPUNIT(MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl
AC_DEFUN([AM_PATH_CPPUNIT],
[

AC_ARG_WITH(cppunit-prefix,[  --with-cppunit-prefix=PFX   Prefix where CppUnit is installed (optional)],
            cppunit_config_prefix="$withval", cppunit_config_prefix="")
AC_ARG_WITH(cppunit-exec-prefix,[  --with-cppunit-exec-prefix=PFX  Exec prefix where CppUnit is installed (optional)],
            cppunit_config_exec_prefix="$withval", cppunit_config_exec_prefix="")

  if test x$cppunit_config_exec_prefix != x ; then
     cppunit_config_args="$cppunit_config_args --exec-prefix=$cppunit_config_exec_prefix"
     if test x${CPPUNIT_CONFIG+set} != xset ; then
        CPPUNIT_CONFIG=$cppunit_config_exec_prefix/bin/cppunit-config
     fi
  fi
  if test x$cppunit_config_prefix != x ; then
     cppunit_config_args="$cppunit_config_args --prefix=$cppunit_config_prefix"
     if test x${CPPUNIT_CONFIG+set} != xset ; then
        CPPUNIT_CONFIG=$cppunit_config_prefix/bin/cppunit-config
     fi
  fi

  AC_PATH_PROG(CPPUNIT_CONFIG, cppunit-config, no)
  cppunit_version_min=$1

  if test x$cppunit_version_min = x ; then
    AC_MSG_CHECKING(for Cppunit)
  else
    AC_MSG_CHECKING(for Cppunit - version >= $cppunit_version_min)
  fi
  no_cppunit=""
  if test "$CPPUNIT_CONFIG" = "no" ; then
    AC_MSG_RESULT(no)
    no_cppunit=yes
  else
    CPPUNIT_CFLAGS=`$CPPUNIT_CONFIG --cflags`
    CPPUNIT_LIBS=`$CPPUNIT_CONFIG --libs`
    cppunit_version=`$CPPUNIT_CONFIG --version`

    cppunit_major_version=`echo $cppunit_version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    cppunit_minor_version=`echo $cppunit_version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    cppunit_micro_version=`echo $cppunit_version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`

    cppunit_major_min=`echo $cppunit_version_min | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    if test "x${cppunit_major_min}" = "x" ; then
       cppunit_major_min=0
    fi
    
    cppunit_minor_min=`echo $cppunit_version_min | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    if test "x${cppunit_minor_min}" = "x" ; then
       cppunit_minor_min=0
    fi
    
    cppunit_micro_min=`echo $cppunit_version_min | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
    if test "x${cppunit_micro_min}" = "x" ; then
       cppunit_micro_min=0
    fi

    cppunit_version_proper=`expr \
        $cppunit_major_version \> $cppunit_major_min \| \
        $cppunit_major_version \= $cppunit_major_min \& \
        $cppunit_minor_version \> $cppunit_minor_min \| \
        $cppunit_major_version \= $cppunit_major_min \& \
        $cppunit_minor_version \= $cppunit_minor_min \& \
        $cppunit_micro_version \>= $cppunit_micro_min `

    if test "$cppunit_version_proper" = "1" ; then
      AC_MSG_RESULT([$cppunit_major_version.$cppunit_minor_version.$cppunit_micro_version])
      AC_DEFINE(HAVE_CPPUNIT,1,[define if the cppunit library is available])
    else
      AC_MSG_RESULT(no)
      no_cppunit=yes
    fi
  fi

  if test "x$no_cppunit" = x ; then
     ifelse([$2], , :, [$2])     
  else
     CPPUNIT_CFLAGS=""
     CPPUNIT_LIBS=""
     ifelse([$3], , :, [$3])
  fi

  AM_CONDITIONAL([HAVE_CPPUNIT], [test x$no_cppunit = x])
  AM_FHTAGN_HAVE([cppunit], [test x$no_cppunit = x])

  AC_SUBST(CPPUNIT_CFLAGS)
  AC_SUBST(CPPUNIT_LIBS)
])




dnl
dnl Extra code checks
dnl
AC_DEFUN([AM_FHTAGN_ENABLE_EXTRA_CHECKS],
[
AC_MSG_CHECKING([whether extra static code checking is enabled])
AC_ARG_ENABLE([extra-checks],
  [AS_HELP_STRING([--enable-extra-checks],
    [(default is no) Turn on extra static code checking (`-Weffc++' and
     `-Wold-style-cast' compiler options). This will lead to spam from headers
     included from the STL and/or boost, but may help finding flaws in Fhtagn!])],
[case "${enableval}" in
  yes) enable_extra_checks=true ; result_extra_checks=yes ;;
  no)  enable_extra_checks=false ; result_extra_checks=no ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --enable-extra-checks]) ;;
esac],[enable_extra_checks=false ; result_extra_checks=no])
AM_CONDITIONAL([ENABLE_EXTRA_CHECKS], [test x$enable_extra_checks = xtrue])
AC_MSG_RESULT([${result_extra_checks}])
])

dnl
dnl Debug/release builds
dnl
AC_DEFUN([AM_FHTAGN_ENABLE_DEBUG],
[
AC_MSG_CHECKING([whether we're creating a debug build])
AC_ARG_ENABLE([debug],
  [AS_HELP_STRING([--enable-debug],
    [(default is yes) Switch off debug builds, i.e. pass the -DNDEBUG flag and
     use more highly optimized build flags. Use `--disable-debug' for production
     builds.])],
[case "${enableval}" in
  yes)
    enable_debug=true
    if test x$enable_coverage = xtrue ; then
      result_debug="yes, forced by --enable-coverage"
    else
      result_debug=yes
    fi
    ;;
  no)  enable_debug=false ; result_debug=no ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
esac],[enable_debug=true ; result_debug=yes])
AM_CONDITIONAL([ENABLE_DEBUG], [test x$enable_debug = xtrue])
AC_MSG_RESULT([${result_debug}])
])


dnl
dnl Extra coverage information (test coverage, etc)
dnl
AC_DEFUN([AM_FHTAGN_ENABLE_COVERAGE],
[
AC_CHECK_PROG([have_gcov], [gcov], [yes], [no])
AC_MSG_CHECKING([whether coverage information is to be produced])
AC_ARG_ENABLE([coverage],
  [AS_HELP_STRING([--enable-coverage],
    [(default is no) Embed coverage information into the build, that can be
     be used for test coverage analysis. Coverage information is extracted by
     `gcov'; if that program cannot be found, `--enable-coverage' has no effect.
     Implies `--enable-debug=yes'])],
[
  if test x"$have_gcov" = xyes ; then
    case "${enableval}" in
      yes) enable_coverage=true ; result_coverage=yes ; enable_debug=yes ;;
      no)  enable_coverage=false ; result_coverage=no ;;
      *) AC_MSG_ERROR([bad value ${enableval} for --enable-coverage]) ;;
    esac
  else
    enable_coverage=false
    result_coverage="no, gcov not found"
  fi
],[enable_coverage=false ; result_coverage=no])
AM_CONDITIONAL([ENABLE_COVERAGE], [test x$enable_coverage = xtrue])
AC_MSG_RESULT([${result_coverage}])
])


