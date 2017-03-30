AM_CPPFLAGS = -DLOCALEDIR=\"$(localedir)\"
gladedir = $(pkgdatadir)

introspection = cn.com.itep.powermanager.xml

bin_PROGRAMS = itep-power-manager-settings
glade_DATA = power-manager-settings.glade
itep_power_manager_settings_SOURCES = generated.c generated.h itep-power-manager-settings.c

itep_power_manager_settings_CFLAGS = $(GTK_CFLAGS)
itep_power_manager_settings_CFLAGS += -DGLADE_FILE=\"$(gladedir)\/power-manager-settings.glade\"
itep_power_manager_settings_LDADD = $(GTK_LIBS)

generated.c generated.h: $(introspection)
	gdbus-codegen --interface-prefix cn.com.itep. --generate-c-code generated --c-namespace Itep $^

EXTRA_DIST = $(introspection) $(glade_DATA)

clean-local:
	-rm -rf generated.c generated.h
