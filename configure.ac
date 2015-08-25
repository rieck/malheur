# MALHEUR - Automatic Analysis of Malware Behavior
# Copyright (c) 2009-2015 Konrad Rieck (konrad@mlsec.org)
# University of Goettingen, Berlin Institute of Technology
#

# Malheur version
AC_INIT([malheur], [0.6.0], [konrad@mlsec.org])
AC_PREFIX_DEFAULT("/usr/local")

# Defines (suitable for loading and saving versioned files)
AC_DEFINE(MALHEUR_MAJOR, 0, Major version number)
AC_DEFINE(MALHEUR_MINOR, 6, Minor version number)
AC_DEFINE(MALHEUR_PATCH, 0, Patch version number)


echo
echo " > MALHEUR - Automatic Analysis of Malware Behavior"
echo "   Copyright (c) 2009-2015 Konrad Rieck (konrad@mlsec.org)"
echo "   University of Goettingen, Berlin Institute of Technology"
echo

# Init automake
AM_INIT_AUTOMAKE
AC_CONFIG_HEADER([config.h])

# Check for important programs
AC_PROG_CC
AC_PROG_LD
AC_PROG_INSTALL

# Libtool stuff
AC_CONFIG_MACRO_DIR(m4)
AC_PROG_LIBTOOL

# By default remove assert statements
CFLAGS="$CFLAGS -DNDEBUG"

# Set GCC and C99 flags if present
if test "$GCC" = "yes" ; then
   CFLAGS="$CFLAGS -std=c99 -fgnu89-inline -Wall -fPIC"
fi

# Optional packages
AC_ARG_WITH([libarchive], [AS_HELP_STRING([--with-libarchive],
            [support for reading archives @<:@default=check@:>@])],
            [], [with_libarchive=check])
AC_ARG_WITH([openmp], [AS_HELP_STRING([--with-openmp],
            [support for multi-processing @<:@default=check@:>@])],
            [], [with_openmp=check])


# Check for zlib (required)
AC_CHECK_HEADERS([zlib.h], HEADER_ZLIB="yes")
AC_CHECK_LIB([z], gzopen, LIBRARY_ZLIB="yes")
if test "x$LIBRARY_ZLIB" != "x" && test "x$HEADER_ZLIB" != "x" ; then
    LIBS="-lz $LIBS"
    AC_DEFINE([HAVE_ZLIB], [1], [Define if you have zlib])
    HAVE_ZLIB=yes
else
    HAVE_ZLIB=no
    AC_MSG_FAILURE([libz not found. see README.md])
fi

# Check for libarchive (optional)
AC_CHECK_HEADERS([archive.h], HEADER_LIBARCHIVE="yes")
AC_CHECK_LIB([archive], archive_read_new, LIBRARY_LIBARCHIVE="yes")
if test "x$LIBRARY_LIBARCHIVE" != "x" && \
   test "x$HEADER_LIBARCHIVE" != "x" && \
   test "x$with_libarchive" != "xno" ; then
    AC_DEFINE([HAVE_LIBARCHIVE], [1], [Define if you have libarchive])
    LIBS="-larchive $LIBS"
    HAVE_LIBARCHIVE=yes
else
    HAVE_LIBARCHIVE=no
    if test "x$with_libarchive" == "xyes" ; then
        AC_MSG_FAILURE([libarchive not found. see README.md])
    fi
fi

# Check for libconfig (required)
AC_CHECK_HEADERS([libconfig.h], HEADER_LIBCONFIG="yes")
PKG_CHECK_MODULES([PKGCONFIG], [libconfig >= 1.3.2], LIBRARY_LIBCONFIG="yes")
if test "x$LIBRARY_LIBCONFIG" != "x" && \
   test "x$HEADER_LIBCONFIG" != "x" ; then
    CFLAGS="$CFLAGS $PKGCONFIG_CFLAGS"
    LIBS="$LIBS $PKGCONFIG_LIBS"
    AM_CPPFLAGS="$AM_CPPFLAGS `pkg-config --cflags-only-I libconfig`"
    HAVE_LIBCONFIG=yes
    AC_DEFINE([HAVE_LIBCONFIG], [1], [Define if you have libconfig])
else
    HAVE_LIBCONFIG=no
    AC_MSG_FAILURE([libconfig not found. see README.md])
fi

# Check for OpenMP (optional)
AC_CHECK_HEADERS([omp.h], HEADER_OPENMP="yes")
AX_OPENMP(LIBRARY_OPENMP="yes")
if test "x$LIBRARY_OPENMP" != "x" && \
   test "x$HEADER_OPENMP" != "x" && \
   test "x$with_openmp" != "xno" ; then
    CFLAGS="$CFLAGS $OPENMP_CFLAGS"
    HAVE_OPENMP=yes
    AC_DEFINE([HAVE_OPENMP], [1], [Define if you have OpenMP])
else
    HAVE_OPENMP=no
    if test "x$with_openmp" == "xyes" ; then
        AC_MSG_FAILURE([no openmp support. see README.md])
    fi
fi

# Check headers
AC_CHECK_HEADERS([getopt.h string.h strings.h])
AC_CHECK_HEADERS([uthash.h uthash/uthash.h], HEADER_UTHASH="yes")

# Check functions
AC_CHECK_FUNC(round, AC_DEFINE(HAVE_FUNC_ROUND, 1,
       [Define to 1 if you have the function round]))
AC_CHECK_FUNC(log2, AC_DEFINE(HAVE_FUNC_LOG2, 1,
       [Define to 1 if you have the function log2]))

AC_SUBST([AM_CPPFLAGS])
AC_CONFIG_FILES([
   Makefile \
   src/Makefile \
   doc/Makefile \
   tests/Makefile \
])
AC_OUTPUT

echo
echo " > Optional packages"
echo "   Support reading archives (--with-libarchive):  $HAVE_LIBARCHIVE"
echo "   Support for multi-processing (--with-openmp):  $HAVE_OPENMP"
