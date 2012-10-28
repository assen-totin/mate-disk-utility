
NULL =

libexec_PROGRAMS = mdu-notification-daemon

mdu_notification_daemon_SOURCES = 							\
						notification-main.c			\
	mdu-slow-unmount-dialog.h		mdu-slow-unmount-dialog.c		\
	$(NULL)

mdu_notification_daemon_CPPFLAGS = 			\
	-I$(top_srcdir)/src/				\
	-I$(top_builddir)/src/				\
	-DG_LOG_DOMAIN=\"MduNotification\"		\
	-DMATELOCALEDIR=\""$(datadir)/locale"\"	\
	$(DISABLE_DEPRECATED)				\
	-DMDU_API_IS_SUBJECT_TO_CHANGE			\
	-DMDU_GTK_API_IS_SUBJECT_TO_CHANGE		\
	$(AM_CPPFLAGS)

mdu_notification_daemon_CFLAGS = 			\
	$(GLIB2_CFLAGS)					\
	$(GOBJECT2_CFLAGS)				\
	$(GIO2_CFLAGS)					\
	$(GIO_UNIX2_CFLAGS)				\
	$(GTK2_CFLAGS)					\
	$(WARN_CFLAGS)					\
	$(LIBMATENOTIFY_CFLAGS)				\
	$(AM_CFLAGS)

mdu_notification_daemon_LDFLAGS = 			\
	$(AM_LDFLAGS)

mdu_notification_daemon_LDADD = 			\
	$(GLIB2_LIBS)					\
	$(GOBJECT2_LIBS)				\
	$(GIO2_LIBS)					\
	$(GTK2_LIBS)					\
	$(LIBMATENOTIFY_LIBS)				\
	$(top_builddir)/src/mdu/libmdu.la		\
	$(top_builddir)/src/mdu-gtk/libmdu-gtk.la

clean-local :
	rm -f *~
