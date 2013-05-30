NULL =

AM_CPPFLAGS =						\
	-DDATADIR=\"$(datadir)\"			\
	-DMATELOCALEDIR=\""$(datadir)/locale"\"	\
	-DLIBEXECDIR=\"$(libexecdir)\"			\
	-I$(top_srcdir)/src				\
	-I$(top_builddir)/src				\
	$(WARN_CFLAGS)					\
	$(AM_CFLAGS)					\
	-DMDU_API_IS_SUBJECT_TO_CHANGE			\
	-DMDU_GTK_API_IS_SUBJECT_TO_CHANGE		\
	$(NULL)

CORE_CFLAGS = 						\
	$(GLIB2_CFLAGS)					\
	$(GOBJECT2_CFLAGS)				\
	$(GIO2_CFLAGS)					\
	$(GIO_UNIX2_CFLAGS)				\
	$(GTHREAD2_CFLAGS)				\
	$(GTK2_CFLAGS)					\
	$(NULL)

CORE_LIBADD = 						\
	$(GLIB2_LIBS)					\
	$(GOBJECT2_LIBS)				\
	$(GIO2_LIBS)					\
	$(GIO_UNIX2_LIBS)				\
	$(GTHREAD2_LIBS)				\
	$(GTK2_LIBS)					\
	$(INTLLIBS)					\
	$(top_builddir)/src/mdu/libmdu.la		\
	$(top_builddir)/src/mdu-gtk/libmdu-gtk.la	\
	$(NULL)


caja_extensiondir = $(libdir)/caja/extensions-2.0
caja_extension_LTLIBRARIES = libcaja-mdu.la

libcaja_mdu_la_SOURCES =			\
	caja-module.c			\
	caja-mdu.c				\
	caja-mdu.h				\
	$(NULL)
libcaja_mdu_la_CPPFLAGS = $(CORE_CFLAGS) $(CAJA_CFLAGS) -DG_LOG_DOMAIN=\"Caja-MDU\"
libcaja_mdu_la_LDFLAGS = $(AM_LDFLAGS) -export_dynamic -module -avoid-version -no-undefined
libcaja_mdu_la_LIBADD  = $(CORE_LIBADD) $(CAJA_LIBS)
