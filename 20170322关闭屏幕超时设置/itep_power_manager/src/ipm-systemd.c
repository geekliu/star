#include "ipm-systemd.h"

#include <gio/gio.h>

#define SYSTEMD_DBUS_NAME		"org.freedesktop.login1"
#define SYSTEMD_DBUS_PATH		"/org/freedesktop/login1"
#define SYSTEMD_DBUS_INTERFACE		"org.freedesktop.login1.Manager"
#define SYSTEMD_REBOOT_ACTION		"Reboot"
#define SYSTEMD_POWEROFF_ACTION		"PowerOff"

static void ipm_systemd_try_method(const char *method, GError **error)
{
	GDBusConnection *bus;
	GError *local_error = NULL;

	bus = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, error);
	if (G_LIKELY(bus != NULL)) {
		g_dbus_connection_call_sync(bus,
					    SYSTEMD_DBUS_NAME,
					    SYSTEMD_DBUS_PATH,
					    SYSTEMD_DBUS_INTERFACE,
					    method,
					    g_variant_new("(b)", TRUE),
					    NULL, 0, G_MAXINT, NULL,
					    &local_error);
		g_object_unref(G_OBJECT(bus));

		if (local_error != NULL) {
			g_propagate_error(error, local_error);
		}
	}
}

void ipm_systemd_shutdown(GError **error)
{
	ipm_systemd_try_method(SYSTEMD_POWEROFF_ACTION, error);
}

void ipm_systemd_reboot(GError **error)
{
	ipm_systemd_try_method(SYSTEMD_REBOOT_ACTION, error);
}
