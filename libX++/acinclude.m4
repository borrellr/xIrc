dnl Copyright (c) 1998 N. D. Bellamy

## ------------------------------------------------------------------------
## Find a file (or one of more files in a list of dirs)
## ------------------------------------------------------------------------
##
AC_DEFUN(AC_FIND_FILE,
[
$3=NO
for i in $2;
do
  for j in $1;
  do
    if test -r "$i/$j"; then
      $3=$i
      break 2
    fi
  done
done
])


AC_DEFUN(AC_PATH_QT_LIB,
[
  AC_REQUIRE_CPP()
  AC_REQUIRE([AC_PATH_X])
  AC_MSG_CHECKING(for QT libraries)
  
  ac_qt_libraries="no"
  
  AC_ARG_WITH(qt-libraries,
    [  --with-qt-libraries     where the QT libraries are located. ],
    [  ac_qt_libraries="$withval" ])

  AC_CACHE_VAL(ac_cv_lib_qtlib, [
    
    dnl Did the user give --with-qt-libraries?
    
    if test "$ac_qt_libraries" = no; then

      dnl No they didn't, so lets look for them...
    
      dnl If you need to add extra directories to check, add them here.
      
      qt_library_dirs="\
        /usr/lib/qt-2.1.1/lib \
        /usr/lib/qt2/lib \
        /usr/lib \
        /usr/local/lib \
        /usr/lib/qt \
        /usr/lib/qt/lib \
        /usr/local/lib/qt \
        /usr/local/qt/lib \
        /usr/X11/lib \
        /usr/X11/lib/qt \
        /usr/X11R6/lib \
        /usr/X11R6/lib/qt"
  
      if test "x$QTDIR" != x; then
        qt_library_dirs="$qt_library_dirs $QTDIR/lib"
      fi
  
      if test "x$QTLIB" != x; then
        qt_library_dirs="$qt_library_dirs $QTLIB"
      fi

      for qt_dir in $qt_library_dirs; do
        for qt_check_lib in $qt_dir/libqt.so*; do
          if test -r $qt_check_lib; then
            ac_qt_libraries=$qt_dir
            break 2
          fi
        done
      done
    fi

    ac_cv_lib_qtlib=$ac_qt_libraries
  ])

  dnl Define a shell variable for later checks

  if test "$ac_cv_lib_qtlib" = no; then
    have_qt_lib="no"
  else
    have_qt_lib="yes"
  fi
  
  AC_MSG_RESULT([$ac_cv_lib_qtlib])
  QT_LDFLAGS="-L$ac_cv_lib_qtlib"
  QT_LIBDIR="$ac_cv_lib_qtlib"
  AC_SUBST(QT_LDFLAGS)
  AC_SUBST(QT_LIBDIR)
])

AC_DEFUN(AC_PATH_QT_INC,
[
  AC_REQUIRE_CPP()
  AC_REQUIRE([AC_PATH_X])
  AC_MSG_CHECKING(for QT includes)
  
  ac_qt_includes="no"
  
  AC_ARG_WITH(qt-includes,
    [  --with-qt-includes      where the QT headers are located. ],
    [  ac_qt_includes="$withval" ])
  
  AC_CACHE_VAL(ac_cv_header_qtinc, [
    
    dnl Did the user give --with-qt-includes?
    
    if test "$ac_qt_includes" = no; then

      dnl No they didn't, so lets look for them...

      dnl If you need to add extra directories to check, add them here.
      
      qt_include_dirs="\
        /usr/lib/qt-2.1.1/include \
        /usr/lib/qt2/include \
        /usr/lib/qt/include \
        /usr/include/qt \
        /usr/local/qt/include \
        /usr/local/include/qt \
        /usr/X11/include/qt \
        /usr/X11/include/X11/qt \
        /usr/X11R6/include \        
        /usr/X11R6/include/qt \
        /usr/X11R6/include/X11/qt"

      if test "x$QTDIR" != x; then
        qt_include_dirs="$qt_include_dirs $QTDIR/include"
      fi
      
      if test "x$QTINC" != x; then
        qt_include_dirs="$qt_include_dirs $QTINC"
      fi
      
      for qt_dir in $qt_include_dirs; do
        if test -r "$qt_dir/qwidget.h"; then
          ac_qt_includes=$qt_dir
          break
        fi
      done

      if test -r "$qt_dir/qtabwidget.h"; then
        ac_qt2="yes"
      else
        ac_qt2="no"
      fi
    fi

    ac_cv_header_qtinc=$ac_qt_includes
  
  ])

  if test "$ac_cv_header_qtinc" = no; then
    have_qt_inc="no"
  else
    have_qt_inc="yes"
  fi

  AC_MSG_RESULT([$ac_cv_header_qtinc])
  QT_INCLUDES="-I$ac_cv_header_qtinc"
  QT_INCDIR="$ac_cv_header_qtinc"

  AC_SUBST(QT_INCLUDES)
  AC_SUBST(QT_INCDIR)
])

AC_DEFUN(AC_PATH_QT_MOC,
[
  AC_PATH_PROG(
    MOC,
    moc,
    $QTDIR/bin/moc,
    $QTDIR/bin:/usr/bin:/usr/X11R6/bin:/usr/lib/qt/bin:/usr/local/qt/bin:$PATH)
  AC_SUBST(MOC)
])

AC_DEFUN(AC_PATH_QT_FINDTR,
[
  AC_PATH_PROG(
    QT_FINDTR,
    findtr,
    $QTDIR/bin/findtr,
    $QTDIR/bin:/usr/bin:/usr/X11R6/bin:/usr/lib/qt/bin:/usr/local/qt/bin:$PATH)
])

AC_DEFUN(AC_PATH_QT_MSGTOQM,
[
  AC_PATH_PROG(
    QT_MSG2QM,
    msg2qm,
    $QTDIR/bin/msg2qm,
    $QTDIR/bin:/usr/bin:/usr/X11R6/bin:/usr/lib/qt/bin:/usr/local/qt/bin:$PATH)
])

AC_DEFUN(AC_PATH_QT_MERGETR,
[
  AC_PATH_PROG(
    QT_MERGETR,
    mergetr,
    $QTDIR/bin/mergetr,
    $QTDIR/bin:/usr/bin:/usr/X11R6/bin:/usr/lib/qt/bin:/usr/local/qt/bin:$PATH)
])

## ------------------------------------------------------------------------
## Now, the same with KDE
## $(KDE_LDFLAGS) will be the kdeliblocation (if needed)
## and $(kde_includes) will be the kdehdrlocation (if needed)
## ------------------------------------------------------------------------
##
AC_DEFUN(AC_PATH_KDE,
[
AC_REQUIRE([AC_PATH_QT_LIB])
AC_REQUIRE([AC_PATH_QT_INC])

AC_ARG_WITH(kde,
  [  --with-kde       compile with KDE support. ],
  [  ac_kde="$withval" ])


AC_MSG_CHECKING([for KDE])
dnl if test "${prefix}" != NONE; then
dnl   kde_libraries=${prefix}/lib
dnl   kde_includes=${prefix}/include
dnl   AC_MSG_RESULT(["will be installed in" $prefix])
dnl else
ac_kde_includes=NO ac_kde_libraries=NO
kde_libraries=""
kde_includes=""
AC_CACHE_VAL(ac_cv_have_kde,
[#try to guess kde locations

kde_incdirs="/usr/lib/kde/include /usr/local/kde/include /usr/kde/include /usr/include/kde /usr/include $x_includes $qt_includes"

test -n "$KDEDIR" && kde_incdirs="$KDEDIR/include $KDEDIR $kde_incdirs"
AC_FIND_FILE(kthemestyle.h, $kde_incdirs, kde_incdir)
ac_kde_includes=$kde_incdir

kde_libdirs="/usr/lib/kde/lib /usr/local/kde/lib /usr/kde/lib /usr/lib/kde /usr/lib /usr/X11R6/lib /usr/X11R6/kde/lib"
test -n "$KDEDIR" && kde_libdirs="$KDEDIR/lib $KDEDIR $kde_libdirs"
AC_FIND_FILE(libkdecore.la, $kde_libdirs, kde_libdir)
ac_kde_libraries=$kde_libdir

if test "$ac_kde_includes" = NO || test "$ac_kde_libraries" = NO; then
  ac_cv_have_kde="have_kde=no"
else
  ac_cv_have_kde="have_kde=yes \
    ac_kde_includes=$ac_kde_includes ac_kde_libraries=$ac_kde_libraries"
fi])

eval "$ac_cv_have_kde"

if test "$have_kde" != yes; then
  if test "${prefix}" = NONE; then
    ac_kde_prefix=$ac_default_prefix
  else
    ac_kde_prefix=$prefix
  fi
  dnl  AC_MSG_RESULT(["will be installed in" $ac_kde_prefix])
  kde_libraries=${ac_kde_prefix}/lib
  kde_includes=${ac_kde_prefix}/include
else
  ac_cv_have_kde="have_kde=yes \
    ac_kde_includes=$ac_kde_includes ac_kde_libraries=$ac_kde_libraries"
  dnl   AC_MSG_RESULT([libraries $ac_kde_libraries, headers $ac_kde_includes])
  kde_libraries=$ac_kde_libraries
  kde_includes=$ac_kde_includes
fi
dnl fi
AC_SUBST(kde_libraries)
AC_SUBST(kde_includes)

if test "$kde_includes" = "$x_includes" || test "$kde_includes" = "$qt_includes" ; then
 KDE_INCLUDES=""
else
 KDE_INCLUDES="-I$kde_includes"
 all_includes="$all_includes $KDE_INCLUDES"
fi

if test "$kde_libraries" = "$x_libraries" || test "$kde_libraries" = "$qt_libraries" ; then
 KDE_LDFLAGS=""
else
 KDE_LDFLAGS="-L$kde_libraries"
 all_libraries="$all_libraries $KDE_LDFLAGS"
fi

AC_SUBST(KDE_LDFLAGS)
AC_SUBST(KDE_INCLUDES)
AC_SUBST(all_includes)
AC_SUBST(all_libraries)

test -n "$ac_kde" && have_kde=$ac_kde
test -n "$ac_kde" || have_kde=no
if test "$have_kde" = yes; then
  AC_MSG_RESULT([libraries $ac_kde_libraries, headers $ac_kde_includes])
else
  AC_MSG_RESULT(["no"])
fi

])


dnl Like AC_CHECK_HEADER, but it uses the already-computed -I directories.

AC_DEFUN(AC_CHECK_X_HEADER, [
  ac_save_CPPFLAGS="$CPPFLAGS"
  if test \! -z "$includedir" ; then
    CPPFLAGS="$CPPFLAGS -I$includedir"
  fi
  CPPFLAGS="$CPPFLAGS $X_CFLAGS"
  AC_CHECK_HEADER([$1],[$2],[$3])
  CPPFLAGS="$ac_save_CPPFLAGS"
])
  
dnl Like AC_CHECK_LIB, but it used the -L dirs set up by the X checks.

AC_DEFUN(AC_CHECK_X_LIB, [
  ac_save_CPPFLAGS="$CPPFLAGS"
  ac_save_LDFLAGS="$LDFLAGS"

  if test \! -z "$includedir" ; then
    CPPFLAGS="$CPPFLAGS -I$includedir"
  fi
  
  dnl note: $X_CFLAGS includes $x_includes
  CPPFLAGS="$CPPFLAGS $X_CFLAGS"

  if test \! -z "$libdir" ; then
    LDFLAGS="$LDFLAGS -L$libdir"
  fi
  
  dnl note: $X_LIBS includes $x_libraries
  
  LDFLAGS="$LDFLAGS $X_LIBS"
  AC_CHECK_LIB([$1], [$2], [$3], [$4], [$5])
  CPPFLAGS="$ac_save_CPPFLAGS"
  LDFLAGS="$ac_save_LDFLAGS"]
)

dnl Check if it is possible to turn off run time type information (RTTI)
AC_DEFUN(AC_PROG_CXX_FNO_RTTI,
[AC_CACHE_CHECK(whether ${CXX-g++} accepts -fno-rtti, ac_cv_prog_cxx_fno_rtti,
[echo 'void f(){}' > conftest.cc
if test -z "`${CXX-g++} -fno-rtti -c conftest.cc 2>&1`"; then
  ac_cv_prog_cxx_fno_rtti=yes
  CXXFLAGS="${CXXFLAGS} -fno-rtti"
else
  ac_cv_prog_cxx_fno_rtti=no
fi
rm -f conftest*
])])

dnl Check if the type socklen_t is defined anywhere
AC_DEFUN(AC_C_SOCKLEN_T,
[AC_CACHE_CHECK(for socklen_t, ac_cv_c_socklen_t,
[ AC_TRY_COMPILE([
#include <sys/types.h>
#include <sys/socket.h>
],[
socklen_t foo;
],[
  ac_cv_c_socklen_t=yes
],[
  ac_cv_c_socklen_t=no
  AC_DEFINE(socklen_t,int)
])])])

dnl Check for sys_errlist[] and sys_nerr, check for declaration
dnl Check nicked from aclocal.m4 from GNU bash 2.01
AC_DEFUN(AC_SYS_ERRLIST,
[AC_MSG_CHECKING([for sys_errlist and sys_nerr])
AC_CACHE_VAL(ac_cv_sys_errlist,
[AC_TRY_LINK([#include <errno.h>],
[extern char *sys_errlist[];
 extern int sys_nerr;
 char *msg = sys_errlist[sys_nerr - 1];],
    ac_cv_sys_errlist=yes, ac_cv_sys_errlist=no)])dnl
AC_MSG_RESULT($ac_cv_sys_errlist)
if test $ac_cv_sys_errlist = yes; then
AC_DEFINE(HAVE_SYS_ERRLIST)
fi
])

dnl Generate the moc files
AC_DEFUN(AC_MAKE_MOC_FILES,
[
  list=`grep -l Q_OBJECT *.h`
  ac_moc_list=`echo $list | sed -e s/^/moc_/ -e s/h$/cpp/`
  MOC_FILES=$ac_moc_list
  AC_SUBST(MOC_FILES)
])