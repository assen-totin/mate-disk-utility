NULL =

AM_CPPFLAGS =						\
	-DDATADIR=\"$(datadir)\"			\
	-DMATELOCALEDIR=\""$(datadir)/locale"\"	\
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
	$(AM_CPPFLAGS)					\
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

libexec_PROGRAMS = mdu-format-tool

mdu_format_tool_SOURCES =						\
					mdu-format-tool.c		\
	mdu-format-progress-dialog.h	mdu-format-progress-dialog.c	\
	$(NULL)

mdu_format_tool_CPPFLAGS = $(CORE_CFLAGS) -DG_LOG_DOMAIN=\"MDU-Format-Tool\"
mdu_format_tool_LDFLAGS = $(AM_LDFLAGS)
mdu_format_tool_LDADD = $(CORE_LIBADD)

EXTRA_DIST = TODO
