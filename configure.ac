AC_INIT(libview, 0.6.4, view-devel@lists.sourceforge.net)
AC_PREREQ(2.50)
AC_CONFIG_SRCDIR(config.h.in)
AC_COPYRIGHT([Copyright 2005-2009 VMware, Inc.])

dnl ################################################################
dnl # libtool versioning
dnl ################################################################
dnl #
dnl # +1 :  0 : +1   == new interface that does not break old one.
dnl # +1 :  0 :  ?   == removed an interface. Breaks old apps.
dnl #  ? : +1 :  ?   == internal changes that doesn't break anything.
dnl #
dnl # CURRENT : REVISION : AGE
dnl #
LT_CURRENT=6
LT_REVISION=1
LT_AGE=4

AC_SUBST(LT_RELEASE)
AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)


dnl ################################################################
dnl # Initialize automake
dnl ################################################################
VERSION=0.6.4
PACKAGE=libview

AM_INIT_AUTOMAKE($PACKAGE, $VERSION)


dnl ################################################################
dnl # Specify a header configuration file
dnl ################################################################
AM_CONFIG_HEADER(config.h)


dnl ################################################################
dnl # Check for some standard stuff.
dnl ################################################################
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_MAKE_SET
AC_PROG_LN_S
AC_PROG_INSTALL
AM_PROG_LIBTOOL

AC_C_CONST
AC_TYPE_SIZE_T
AC_EXEEXT


PKG_CHECK_MODULES(VIEW, [gtk+-2.0 >= 2.4.0, gtkmm-2.4])
AC_SUBST(VIEW_CFLAGS)
AC_SUBST(VIEW_LIBS)


dnl # Use wall if we have GCC
if test "x$GCC" = "xyes"; then
   CFLAGS="$CFLAGS -Wall"
fi

AC_ARG_ENABLE(deprecated, [  --enable-deprecated    compile with deprecated API usage],,enable_deprecated=no)
if test "$enable_deprecated" = no; then
	DEBUG_CFLAGS="-DG_DISABLE_DEPRECATED -DGDK_DISABLE_DEPRECATED -DGDK_PIXBUF_DISABLE_DEPRECATED -DATK_DISABLED_DEPRECATED -DPANGO_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED"
fi

AC_SUBST(CFLAGS)
AC_SUBST(DEBUG_CFLAGS)

dnl ################################################################
dnl # Output the Makefiles
dnl ################################################################
AC_CONFIG_FILES([
Doxyfile
Makefile
libview.pc
docs/Makefile
docs/html/Makefile
libview/Makefile
scripts/Makefile
tests/Makefile
])

AC_OUTPUT

echo
echo $PACKAGE v$VERSION

echo
echo "Now type make to compile"
echo "Then su to root and type: make install"
echo
