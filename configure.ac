AC_INIT([MATE Disk Utility],[1.8.0],[https://bugzilla.mate.org/enter_bug.cgi?product=mate-disk-utility],[mate-disk-utility])

AM_INIT_AUTOMAKE([1.9 foreign dist-xz no-dist-gzip])

AC_CONFIG_SRCDIR([src])
AC_CONFIG_HEADERS([config.h])

AM_MAINTAINER_MODE

# libtool versioning - this applies to all libraries in this package
#
# See http://sources.redhat.com/autobook/autobook/autobook_91.html#SEC91 for details
#
LT_CURRENT=0
LT_REVISION=0
LT_AGE=0
AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)

AC_PROG_CC
AC_ISC_POSIX
AC_HEADER_STDC
AC_PROG_LIBTOOL

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

#### gcc warning flags

AC_ARG_ENABLE(ansi, AS_HELP_STRING([--enable-ansi],
  [enable -ansi -pedantic gcc flags]),
  enable_ansi=$enableval,enable_ansi=no)

if test "x$GCC" = "xyes"; then
  changequote(,)dnl
  case " $CFLAGS " in
  *[\ \	]-Wall[\ \	]*) ;;
  *) CFLAGS="$CFLAGS -Wall" ;;
  esac

  case " $CFLAGS " in
  *[\ \	]-Wchar-subscripts[\ \	]*) ;;
  *) CFLAGS="$CFLAGS -Wchar-subscripts" ;;
  esac

  case " $CFLAGS " in
  *[\ \	]-Wmissing-declarations[\ \	]*) ;;
  *) CFLAGS="$CFLAGS -Wmissing-declarations" ;;
  esac

  case " $CFLAGS " in
  *[\ \	]-Wnested-externs[\ \	]*) ;;
  *) CFLAGS="$CFLAGS -Wnested-externs" ;;
  esac

  case " $CFLAGS " in
  *[\ \	]-Wpointer-arith[\ \	]*) ;;
  *) CFLAGS="$CFLAGS -Wpointer-arith" ;;
  esac

  case " $CFLAGS " in
  *[\ \	]-Wcast-align[\ \	]*) ;;
  *) CFLAGS="$CFLAGS -Wcast-align" ;;
  esac

  case " $CFLAGS " in
  *[\ \	]-Wsign-compare[\ \	]*) ;;
  *) CFLAGS="$CFLAGS -Wsign-compare" ;;
  esac

  case " $CFLAGS " in
  *[\ \	]-Wformat[\ \	]*) ;;
  *) CFLAGS="$CFLAGS -Wformat" ;;
  esac

  case " $CFLAGS " in
  *[\ \	]-Wformat-security[\ \	]*) ;;
  *) CFLAGS="$CFLAGS -Wformat-security" ;;
  esac

  if test "x$enable_ansi" = "xyes"; then
    case " $CFLAGS " in
    *[\ \	]-ansi[\ \	]*) ;;
    *) CFLAGS="$CFLAGS -ansi" ;;
    esac

    case " $CFLAGS " in
    *[\ \	]-D_POSIX_C_SOURCE*) ;;
    *) CFLAGS="$CFLAGS -D_POSIX_C_SOURCE=199309L" ;;
    esac

    case " $CFLAGS " in
    *[\ \	]-D_BSD_SOURCE[\ \	]*) ;;
    *) CFLAGS="$CFLAGS -D_BSD_SOURCE" ;;
    esac

    case " $CFLAGS " in
    *[\ \	]-pedantic[\ \	]*) ;;
    *) CFLAGS="$CFLAGS -pedantic" ;;
    esac
  fi
  changequote([,])dnl
fi

# *****
# MATE
# *****

MATE_COMMON_INIT
MATE_DEBUG_CHECK
MATE_COMPILE_WARNINGS([maximum])
MATE_MAINTAINER_MODE_DEFINES

# User documentation
YELP_HELP_INIT

AC_ARG_ENABLE(gtk-doc, AS_HELP_STRING([--enable-gtk-doc],
  [use gtk-doc to build documentation [default=no]]),,
  enable_gtk_doc=no)
GTK_DOC_CHECK([1.3])

# ***************************
# Check for required packages
# ***************************

GLIB2_REQUIRED=2.22
GOBJECT2_REQUIRED=2.16
GIO2_REQUIRED=2.22
DBUS_GLIB_REQUIRED=0.74
MATE_KEYRING_REQUIRED=1.6.0
LIBSECRET_REQUIRED=0.16
GTK2_REQUIRED=2.20.0
UNIQUE_REQUIRED=1.0
LIBNOTIFY_REQUIRED=0.6.1
CAJA_REQUIRED=1.2.0
AVAHI_UI_REQUIRED=0.6.25

UDISKS_REQUIRED=1.0.0
UDISKS_NEXT_ABI_INCOMPATIBLE_VERSION=1.1.0

INTLTOOL_REQUIRED=0.35.0

PKG_CHECK_MODULES(GLIB2, [glib-2.0 >= $GLIB2_REQUIRED])
PKG_CHECK_MODULES(GOBJECT2, [gobject-2.0 >= $GOBJECT2_REQUIRED])
PKG_CHECK_MODULES(GIO2, [gio-2.0 >= $GIO2_REQUIRED])
PKG_CHECK_MODULES(GIO_UNIX2, [gio-unix-2.0 >= $GIO2_REQUIRED])
PKG_CHECK_MODULES(GTHREAD2, [gthread-2.0 >= $GLIB2_REQUIRED])
PKG_CHECK_MODULES(DBUS_GLIB, [dbus-glib-1 >= $DBUS_GLIB_REQUIRED])
PKG_CHECK_MODULES(GTK2, [gtk+-2.0 >= $GTK2_REQUIRED])

#============================================================================
# libunique
#============================================================================
PKG_CHECK_MODULES(UNIQUE, unique-$UNIQUE_REQUIRED, enable_libunique=yes, enable_libunique=no)

AC_SUBST([UNIQUE_CFLAGS])
AC_SUBST([UNIQUE_LIBS])

#AC_ARG_ENABLE(unique, [ --enable-unique enable the use of libunique instead of g_application or gtk_application], enable_libunique=yes,)

if test "x$enable_libunique" = "xyes"; then
    UNIQUE_CFLAGS="$UNIQUE_CFLAGS -DUSE_UNIQUE=1"
    # fix for deprecated on 2.26
    # unique, on 1.0 version, has deprecated symbols in glib 2.26, but
    # we avoid problems using -DG_CONST_RETURN=const
    if $PKG_CONFIG --atleast-version 2.26 glib-2.0; then
        UNIQUE_CFLAGS="$UNIQUE_CFLAGS -DG_CONST_RETURN=const"
    fi

    EXTRA_CORE_MODULES="$EXTRA_CORE_MODULES unique-$UNIQUE_REQUIRED"
else
    AC_ERROR([LIBUNIQUE ${UNIQUE_REQUIRED} not found])
fi

PKG_CHECK_MODULES(LIBNOTIFY, [libnotify  >= $LIBNOTIFY_REQUIRED])
PKG_CHECK_MODULES(UDISKS, [udisks  >= $UDISKS_REQUIRED udisks < $UDISKS_NEXT_ABI_INCOMPATIBLE_VERSION])
PKG_CHECK_MODULES(X11, [x11])
PKG_CHECK_MODULES(LIBATASMART, [libatasmart >= 0.14])
PKG_CHECK_MODULES(AVAHI_UI, [avahi-ui >= $AVAHI_UI_REQUIRED])

# *************
# Remote Access
# *************
have_remote_access=no
AC_ARG_ENABLE(remote_access, AS_HELP_STRING([--disable-remote-access], [disable access to remote udisks daemons]))
if test "x$enable_remote_access" != "xno"; then
   AC_DEFINE(HAVE_REMOTE_ACCESS, 1, [Defined if remote access is enabled])
   have_remote_access=yes
fi
AM_CONDITIONAL(HAVE_REMOTE_ACCESS, [test "$have_remote_access" = "yes"])

# *************************
# Libsecret or MATE Keyring
# *************************
have_libsecret=no
have_mate_keyring=no
selected_keyring=none
FORCED_KEYRING=none
AC_ARG_ENABLE([keyring], [AS_HELP_STRING([--enable-keyring], [compile for specified secret storage, mate or libsecret (default: autodetect, prefer libsecret)])], [FORCED_KEYRING=$enableval])
PKG_CHECK_MODULES([LIBSECRET], [libsecret-1 >= $LIBSECRET_REQUIRED], [AC_DEFINE(HAVE_LIBSECRET, 1, [Define if libsecret is available]) have_libsecret=yes], [have_libsecret=no])
PKG_CHECK_MODULES([MATE_KEYRING], [mate-keyring-1 >= $MATE_KEYRING_REQUIRED], [AC_DEFINE(HAVE_MATE_KEYRING, 1, [Define if mate-keyring is available]) have_mate_keyring=yes], [have_mate_keyring=no])

if test "x$FORCED_KEYRING" = xlibsecret; then
        if test "x$have_libsecret" = xno; then
                AC_MSG_ERROR([libsecret support requested but libraries not found])
        else
                selected_keyring=$FORCED_KEYRING
        fi
elif test "x$FORCED_KEYRING" = xlibsecret; then
        if test "x$have_mate_keyring" = xno; then
                AC_MSG_ERROR([mate-keyring support requested but libraries not found])
        else
                selected_keyring=$FORCED_KEYRING
        fi
else
        if test "x$have_libsecret" = xyes; then
                selected_keyring=libsecret
        elif test "x$have_mate_keyring" = xyes; then
                selected_keyring=mate
        fi
fi

if test "x$selected_keyring" = xlibsecret; then
        MATE_KEYRING_LIBS=""
        MATE_KEYRING_CFLAGS=""
elif test "x$selected_keyring" = xmate; then
        LIBSECRET_LIBS=""
        LIBSECRET_CFLAGS=""
fi

AC_SUBST([LIBSECRET_LIBS])
AC_SUBST([LIBSECRET_CFLAGS])
AC_SUBST([MATE_KEYRING_LIBS])
AC_SUBST([MATE_KEYRING_CFLAGS])
AM_CONDITIONAL(ENABLE_LIBSECRET, [test "x$selected_keyring" = "xlibsecret"])
AM_CONDITIONAL(ENABLE_MATE_KEYRING, [test "x$selected_keyring" = "xmate"])

# ********************
# Internationalization
# ********************

IT_PROG_INTLTOOL([$INTLTOOL_REQUIRED])

GETTEXT_PACKAGE=mate-disk-utility
AC_SUBST([GETTEXT_PACKAGE])
AM_GLIB_GNU_GETTEXT
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE],["$GETTEXT_PACKAGE"],[gettext domain])

# ******************
# Caja extension
# ******************

have_caja=no
AC_ARG_ENABLE(caja, AS_HELP_STRING([--disable-caja], [disable caja extension]))

if test "x$enable_caja" != "xno"; then
	PKG_CHECK_MODULES(CAJA, libcaja-extension >= $CAJA_REQUIRED,
			  [AC_DEFINE(HAVE_CAJA, 1, [Define if caja is available])
			  have_caja=yes], have_caja=no)
	if test "x$have_caja" = xno -a "x$enable_caja" = xyes; then
		AC_MSG_ERROR([caja support requested but libraries not found])
	fi
fi

AC_SUBST(CAJA_LIBS)
AC_SUBST(CAJA_CFLAGS)
AC_SUBST(CAJA_EXTENSION_DIR)

AM_CONDITIONAL(ENABLE_CAJA, [test "$have_caja" = "yes"])

# *************************************
# *************************************

AC_SUBST([AM_CPPFLAGS])
AC_SUBST([AM_CFLAGS])
AC_SUBST([AM_CXXFLAGS])
AC_SUBST([AM_LDFLAGS])

AC_CONFIG_FILES([
Makefile
src/Makefile
src/mdu/Makefile
src/mdu/mdu.pc
src/mdu-gtk/Makefile
src/mdu-gtk/mdu-gtk.pc
src/mate-disk/Makefile
src/notification/Makefile
src/format-tool/Makefile
src/caja-extension/Makefile
po/Makefile.in
data/Makefile
data/mdu-notification-daemon.desktop.in.in
data/icons/Makefile
data/icons/16x16/Makefile
data/icons/22x22/Makefile
data/icons/24x24/Makefile
data/icons/48x48/Makefile
data/icons/scalable/Makefile
help/Makefile
doc/Makefile
doc/version.xml
])

AC_OUTPUT

echo "
                  mate-disk-utility $VERSION
                =============================

        prefix:                     ${prefix}
        libdir:                     ${libdir}
        libexecdir:                 ${libexecdir}
        bindir:                     ${bindir}
        sbindir:                    ${sbindir}
        datadir:                    ${datadir}
        sysconfdir:                 ${sysconfdir}
        localstatedir:              ${localstatedir}

        compiler:                   ${CC}
        cflags:                     ${CFLAGS}
        cppflags:                   ${CPPFLAGS}

        Maintainer mode:            ${USE_MAINTAINER_MODE}
        Building api docs:          ${enable_gtk_doc}
        Caja extension:             ${have_caja}

        Use libunique:              ${enable_libunique}

        Keyring support:            ${selected_keyring}
        Remote Access:              ${have_remote_access}
"
