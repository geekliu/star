AM_CPPFLAGS = -DLOCALEDIR=\"$(localedir)\"
desktopdir = $(datadir)/applications
gladedir = $(pkgdatadir)

introspection = cn.com.itep.powermanager.xml

bin_PROGRAMS = itep-power-manager
glade_DATA = itep-power-manager.glade
desktop_DATA = itep-power-manager.desktop
itep_power_manager_SOURCES = generated.c generated.h main.c \
			     itep-power-manager.c \
			     itep-power-manager.h \
			     ipm-display.c \
			     ipm-dispaly.h \
			     egg-idletime.c \
			     egg-idletime.h \
			     ipm-idle-shutdown.c \
			     ipm-idle-shutdown.h \
			     ipm-timed-shutdown.c \
			     ipm-timed-shutdown.h \
			     ipm-systemd.c \
			     ipm-systemd.h
itep_power_manager_CFLAGS = $(GTK_CFLAGS)
itep_power_manager_CFLAGS += -DGLADE_FILE=\"$(gladedir)\/itep-power-manager.glade\"
itep_power_manager_LDADD = $(GTK_LIBS) -lXext -lX11

generated.c generated.h: $(introspection)
	gdbus-codegen --interface-prefix cn.com.itep. --generate-c-code generated --c-namespace Itep $^

EXTRA_DIST = $(introspection) $(glade_DATA) $(desktop_DATA)

clean-local:
	-rm -rf generated.c generated.h
