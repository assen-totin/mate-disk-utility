
NULL =

mdu-gtk-enumtypes.h: mdu-gtk-enums.h mdu-gtk-enumtypes.h.template
	( top_builddir=`cd $(top_builddir) && pwd`; \
	 cd $(srcdir) && glib-mkenums --template mdu-gtk-enumtypes.h.template mdu-gtk-enums.h ) > \
	   mdu-gtk-enumtypes.h.tmp && mv mdu-gtk-enumtypes.h.tmp mdu-gtk-enumtypes.h

mdu-gtk-enumtypes.c: mdu-gtk-enums.h mdu-gtk-enumtypes.c.template
	( top_builddir=`cd $(top_builddir) && pwd`; \
	 cd $(srcdir) && glib-mkenums --template mdu-gtk-enumtypes.c.template mdu-gtk-enums.h ) > \
	   mdu-gtk-enumtypes.c.tmp && mv mdu-gtk-enumtypes.c.tmp mdu-gtk-enumtypes.c

BUILT_SOURCES =            	\
	mdu-gtk-enumtypes.c     \
	mdu-gtk-enumtypes.h     \
	$(NULL)

EXTRA_DIST =                         	\
	mdu-gtk-enumtypes.h.template    \
	mdu-gtk-enumtypes.c.template    \
	$(NULL)

lib_LTLIBRARIES=libmdu-gtk.la

libmdu_gtkincludedir=$(includedir)/mate-disk-utility/mdu-gtk

libmdu_gtkinclude_HEADERS =              				\
	mdu-gtk.h							\
	mdu-gtk-types.h							\
	mdu-gtk-enums.h							\
	mdu-gtk-enumtypes.h						\
	mdu-time-label.h						\
	mdu-pool-tree-view.h						\
	mdu-pool-tree-model.h						\
	mdu-size-widget.h						\
	mdu-create-linux-md-dialog.h					\
	mdu-ata-smart-dialog.h						\
	mdu-spinner.h							\
	mdu-volume-grid.h						\
	mdu-details-table.h						\
	mdu-details-element.h						\
	mdu-error-dialog.h						\
	mdu-confirmation-dialog.h					\
	mdu-button-element.h						\
	mdu-button-table.h						\
	mdu-dialog.h							\
	mdu-edit-partition-dialog.h					\
	mdu-format-dialog.h						\
	mdu-partition-dialog.h						\
	mdu-create-partition-dialog.h					\
	mdu-create-linux-lvm2-volume-dialog.h				\
	mdu-edit-name-dialog.h						\
	mdu-disk-selection-widget.h					\
	mdu-add-component-linux-md-dialog.h				\
	mdu-add-pv-linux-lvm2-dialog.h				\
	mdu-edit-linux-md-dialog.h					\
	mdu-edit-linux-lvm2-dialog.h					\
	mdu-drive-benchmark-dialog.h					\
	mdu-connect-to-server-dialog.h					\
	$(NULL)

libmdu_gtk_la_SOURCES =                 	               				\
	mdu-gtk.h				mdu-gtk.c				\
	mdu-gtk-enums.h									\
	mdu-gtk-enumtypes.h			mdu-gtk-enumtypes.c			\
	mdu-gtk-types.h									\
	mdu-time-label.h			mdu-time-label.c			\
	mdu-pool-tree-view.h			mdu-pool-tree-view.c			\
	mdu-pool-tree-model.h			mdu-pool-tree-model.c			\
	mdu-size-widget.h			mdu-size-widget.c			\
	mdu-create-linux-md-dialog.h		mdu-create-linux-md-dialog.c		\
	mdu-ata-smart-dialog.h			mdu-ata-smart-dialog.c			\
	mdu-spinner.h				mdu-spinner.c				\
	mdu-volume-grid.h			mdu-volume-grid.c			\
	mdu-details-table.h			mdu-details-table.c			\
	mdu-details-element.h			mdu-details-element.c			\
	mdu-error-dialog.h			mdu-error-dialog.c			\
	mdu-confirmation-dialog.h		mdu-confirmation-dialog.c		\
	mdu-button-element.h			mdu-button-element.c			\
	mdu-button-table.h			mdu-button-table.c			\
	mdu-dialog.h				mdu-dialog.c				\
	mdu-edit-partition-dialog.h		mdu-edit-partition-dialog.c		\
	mdu-format-dialog.h			mdu-format-dialog.c			\
	mdu-partition-dialog.h			mdu-partition-dialog.c			\
	mdu-create-partition-dialog.h		mdu-create-partition-dialog.c		\
	mdu-create-linux-lvm2-volume-dialog.h	mdu-create-linux-lvm2-volume-dialog.c	\
	mdu-edit-name-dialog.h			mdu-edit-name-dialog.c			\
	mdu-disk-selection-widget.h		mdu-disk-selection-widget.c		\
	mdu-add-component-linux-md-dialog.h	mdu-add-component-linux-md-dialog.c	\
	mdu-add-pv-linux-lvm2-dialog.h		mdu-add-pv-linux-lvm2-dialog.c		\
	mdu-edit-linux-md-dialog.h		mdu-edit-linux-md-dialog.c		\
	mdu-edit-linux-lvm2-dialog.h		mdu-edit-linux-lvm2-dialog.c		\
	mdu-drive-benchmark-dialog.h		mdu-drive-benchmark-dialog.c		\
	mdu-connect-to-server-dialog.h		mdu-connect-to-server-dialog.c		\
	$(NULL)

libmdu_gtk_la_CPPFLAGS = 				\
	-I$(top_srcdir)/src				\
	-I$(top_builddir)/src				\
	-DG_LOG_DOMAIN=\"libmdu-gtk\"			\
	-DMATELOCALEDIR=\""$(datadir)/locale"\"	\
	$(DISABLE_DEPRECATED)				\
	$(AM_CPPFLAGS)					\
	-DMDU_API_IS_SUBJECT_TO_CHANGE			\
	-DMDU_GTK_API_IS_SUBJECT_TO_CHANGE		\
	-DMDU_GTK_COMPILATION

libmdu_gtk_la_CFLAGS = 					\
	$(GLIB2_CFLAGS)					\
	$(GOBJECT2_CFLAGS)				\
	$(GIO2_CFLAGS)					\
	$(GIO_UNIX2_CFLAGS)				\
	$(DBUS_GLIB_CFLAGS)				\
	$(MATE_KEYRING_CFLAGS)				\
	$(LIBSECRET_CFLAGS)				\
	$(GTK2_CFLAGS)					\
	$(WARN_CFLAGS)					\
	$(AM_CFLAGS)					\
	$(LIBATASMART_CFLAGS)				\
	$(AVAHI_UI_CFLAGS)				\
	$(NULL)

libmdu_gtk_la_LIBADD = 					\
	$(GLIB2_LIBS)					\
	$(GIO2_LIBS)					\
	$(GIO_UNIX2_LIBS)				\
	$(DBUS_GLIB_LIBS)				\
	$(MATE_KEYRING_LIBS)				\
	$(LIBSECRET_LIBS)				\
	$(GTK2_LIBS)					\
	$(INTLLIBS)					\
	$(LIBATASMART_LIBS)				\
	$(AVAHI_UI_LIBS)				\
	$(NULL)

libmdu_gtk_la_LDFLAGS = -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) \
		        -export-dynamic -no-undefined -export-symbols-regex '(^mdu_.*)'

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = mdu-gtk.pc

CLEANFILES = $(BUILT_SOURCES) $(pkgconfig_DATA)

EXTRADIST =				\
	mdu-gtk-enumtypes.h.template	\
	mdu-gtk-enumtypes.c.template	\
	$(NULL)

clean-local :
	rm -f *~ $(BUILT_SOURCES)

