
NULL =

bin_PROGRAMS = matepalimpsest

matepalimpsest_SOURCES = 									\
						mdu-main.c				\
	mdu-shell.h				mdu-shell.c				\
	mdu-section.h				mdu-section.c				\
	mdu-section-linux-md-drive.h		mdu-section-linux-md-drive.c		\
	mdu-section-linux-lvm2-volume-group.h	mdu-section-linux-lvm2-volume-group.c	\
	mdu-section-drive.h			mdu-section-drive.c			\
	mdu-section-volumes.h			mdu-section-volumes.c			\
	mdu-section-hub.h			mdu-section-hub.c			\
	$(NULL)

matepalimpsest_CPPFLAGS = 					\
	-I$(top_srcdir)/src/				\
	-I$(top_builddir)/src/				\
	-DG_LOG_DOMAIN=\"Palimpsest\"			\
	-DMATELOCALEDIR=\""$(datadir)/locale"\"	\
	$(DISABLE_DEPRECATED)				\
	-DMDU_API_IS_SUBJECT_TO_CHANGE			\
	-DMDU_GTK_API_IS_SUBJECT_TO_CHANGE		\
	$(AM_CPPFLAGS)

matepalimpsest_CFLAGS = 					\
	$(GLIB2_CFLAGS)					\
	$(GOBJECT2_CFLAGS)				\
	$(GIO2_CFLAGS)					\
	$(GIO_UNIX2_CFLAGS)				\
	$(DBUS_GLIB_CFLAGS)				\
	$(MATE_KEYRING_CFLAGS)			\
	$(LIBSECRET_CFLAGS)				\
	$(GTK2_CFLAGS)					\
	$(UNIQUE_CFLAGS)				\
	$(X11_CFLAGS)					\
	$(WARN_CFLAGS)					\
	$(AM_CFLAGS)

matepalimpsest_LDFLAGS = 					\
	$(AM_LDFLAGS)

matepalimpsest_LDADD = 					\
	$(GLIB2_LIBS)					\
	$(GIO2_LIBS)					\
	$(GIO_UNIX2_LIBS)				\
	$(DBUS_GLIB_LIBS)				\
	$(MATE_KEYRING_LIBS)			\
	$(LIBSECRET_LIBS)				\
	$(GTK2_LIBS)					\
	$(UNIQUE_LIBS)					\
	$(X11_LIBS)					\
	$(INTLLIBS)					\
	$(top_builddir)/src/mdu/libmdu.la		\
	$(top_builddir)/src/mdu-gtk/libmdu-gtk.la

clean-local :
	rm -f *~
