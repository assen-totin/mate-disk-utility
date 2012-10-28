NULL =

BUILT_SOURCES =                                         	\
        udisks-daemon-glue.h                      	\
        udisks-device-glue.h				\
        udisks-adapter-glue.h				\
        udisks-expander-glue.h				\
        udisks-port-glue.h				\
	mdu-marshal.h			mdu-marshal.c

mdu-marshal.h: mdu-marshal.list
	glib-genmarshal $< --prefix=mdu_marshal --header > $@

mdu-marshal.c: mdu-marshal.list
	echo "#include \"mdu-marshal.h\"" > $@ && glib-genmarshal $< --prefix=mdu_marshal --body >> $@

udisks-daemon-glue.h: /usr/share/dbus-1/interfaces/org.freedesktop.UDisks.xml Makefile.am
	dbus-binding-tool --prefix=devkit_disks_daemon --mode=glib-client --output=udisks-daemon-glue.h /usr/share/dbus-1/interfaces/org.freedesktop.UDisks.xml

udisks-device-glue.h: /usr/share/dbus-1/interfaces/org.freedesktop.UDisks.Device.xml Makefile.am
	dbus-binding-tool --prefix=devkit_disks_daemon --mode=glib-client --output=udisks-device-glue.h /usr/share/dbus-1/interfaces/org.freedesktop.UDisks.Device.xml

udisks-adapter-glue.h: /usr/share/dbus-1/interfaces/org.freedesktop.UDisks.Adapter.xml Makefile.am
	dbus-binding-tool --prefix=devkit_disks_daemon --mode=glib-client --output=udisks-adapter-glue.h /usr/share/dbus-1/interfaces/org.freedesktop.UDisks.Adapter.xml

udisks-expander-glue.h: /usr/share/dbus-1/interfaces/org.freedesktop.UDisks.Expander.xml Makefile.am
	dbus-binding-tool --prefix=devkit_disks_daemon --mode=glib-client --output=udisks-expander-glue.h /usr/share/dbus-1/interfaces/org.freedesktop.UDisks.Expander.xml

udisks-port-glue.h: /usr/share/dbus-1/interfaces/org.freedesktop.UDisks.Port.xml Makefile.am
	dbus-binding-tool --prefix=devkit_disks_daemon --mode=glib-client --output=udisks-port-glue.h /usr/share/dbus-1/interfaces/org.freedesktop.UDisks.Port.xml

lib_LTLIBRARIES=libmdu.la

libmduincludedir=$(includedir)/mate-disk-utility/mdu

libmduinclude_HEADERS =              			\
	mdu.h						\
	mdu-types.h					\
	mdu-callbacks.h					\
	mdu-device.h					\
	mdu-adapter.h					\
	mdu-expander.h					\
	mdu-port.h					\
	mdu-drive.h					\
	mdu-linux-md-drive.h				\
	mdu-linux-lvm2-volume-group.h			\
	mdu-linux-lvm2-volume-hole.h			\
	mdu-linux-lvm2-volume.h				\
	mdu-error.h					\
	mdu-known-filesystem.h				\
	mdu-pool.h					\
	mdu-presentable.h				\
	mdu-process.h					\
	mdu-util.h					\
	mdu-volume.h					\
	mdu-volume-hole.h				\
	mdu-hub.h					\
	mdu-machine.h					\
	$(NULL)

libmdu_la_SOURCES =                                					\
						mdu.h					\
						mdu-types.h				\
						mdu-callbacks.h				\
	mdu-util.h				mdu-util.c				\
	mdu-pool.c				mdu-pool.h				\
	mdu-device.c				mdu-device.h				\
	mdu-adapter.c				mdu-adapter.h				\
	mdu-expander.c				mdu-expander.h				\
	mdu-port.c				mdu-port.h				\
	mdu-drive.c				mdu-drive.h				\
	mdu-linux-md-drive.c			mdu-linux-md-drive.h			\
	mdu-linux-lvm2-volume-group.c		mdu-linux-lvm2-volume-group.h		\
	mdu-linux-lvm2-volume.c			mdu-linux-lvm2-volume.h			\
	mdu-linux-lvm2-volume-hole.c		mdu-linux-lvm2-volume-hole.h		\
	mdu-volume.c				mdu-volume.h				\
	mdu-presentable.c			mdu-presentable.h			\
	mdu-volume-hole.c			mdu-volume-hole.h			\
	mdu-known-filesystem.c			mdu-known-filesystem.h			\
	mdu-error.c				mdu-error.h				\
	mdu-process.c				mdu-process.h				\
	mdu-hub.c				mdu-hub.h				\
	mdu-machine.c				mdu-machine.h				\
						mdu-private.h				\
	mdu-ssh-bridge.c			mdu-ssh-bridge.h			\
	$(BUILT_SOURCES)								\
	$(NULL)

libmdu_la_CPPFLAGS = 					\
	-I$(top_srcdir)/src				\
	-I$(top_builddir)/src				\
	-DG_LOG_DOMAIN=\"libmdu\"			\
	-DMATELOCALEDIR=\""$(datadir)/locale"\"	\
	$(DISABLE_DEPRECATED)				\
	$(AM_CPPFLAGS)					\
	-DMDU_API_IS_SUBJECT_TO_CHANGE			\
	-DMDU_COMPILATION

libmdu_la_CFLAGS = 					\
	$(GLIB2_CFLAGS)					\
	$(GOBJECT2_CFLAGS)				\
	$(GIO2_CFLAGS)					\
	$(GIO_UNIX2_CFLAGS)				\
	$(DBUS_GLIB_CFLAGS)				\
	$(MATE_KEYRING_CFLAGS)				\
	$(WARN_CFLAGS)					\
	$(AM_CFLAGS)

libmdu_la_LIBADD = 					\
	$(GLIB2_LIBS)					\
	$(GIO2_LIBS)					\
	$(GIO_UNIX2_LIBS)				\
	$(DBUS_GLIB_LIBS)				\
	$(MATE_KEYRING_LIBS)				\
	$(INTLLIBS)

libmdu_la_LDFLAGS = -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) \
		    -export-dynamic -no-undefined -export-symbols-regex '(^mdu_.*)'

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = mdu.pc

CLEANFILES = $(BUILT_SOURCES) $(pkgconfig_DATA)

EXTRA_DIST = mdu-marshal.list

clean-local :
	rm -f *~ $(BUILT_SOURCES)

