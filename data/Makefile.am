NULL =

SUBDIRS = icons

desktopdir = $(datadir)/applications
desktop_in_files = mate-disk.desktop.in
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)

autostartdir = $(sysconfdir)/xdg/autostart
autostart_in_files = mdu-notification-daemon.desktop.in
autostart_DATA = $(autostart_in_files:.desktop.in=.desktop)

mdu-notification-daemon.desktop.in: mdu-notification-daemon.desktop.in.in
	@sed -e "s|\@LIBEXECDIR\@|$(libexecdir)|" $< > $@

@INTLTOOL_DESKTOP_RULE@

distuninstallcheck_listfiles = find . -type f -print | grep -v scrollkeeper

EXTRA_DIST = 			\
	$(desktop_in_files)	\
	$(autostart_in_files)	\
	$(NULL)

CLEANFILES = \
	$(desktop_DATA)				\
	$(autostart_DATA)			\
	mdu-notification-daemon.desktop.in	\
	mdu-notification-daemon.desktop.in.in	\
	$(NULL)

clean-local :
	rm -f *~
