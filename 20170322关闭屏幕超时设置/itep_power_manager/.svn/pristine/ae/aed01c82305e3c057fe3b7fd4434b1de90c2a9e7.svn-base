#include <gtk/gtk.h>
#include "itep-power-manager.h"
#include "generated.h"
#define	DEBUG_LIUHW	0

static gboolean display_get_timeout(ItepPowerManagerDisplay *display,
					GDBusMethodInvocation *invocation,
					ItepPowerManager *ipm)
{
	itep_power_manager_display_complete_get_timeout(
					display,
					invocation,
					itep_power_manager_display_get_timeout(ipm));
	return TRUE;
}

static gboolean display_set_timeout(ItepPowerManagerDisplay *display,
					GDBusMethodInvocation *invocation,
					gint timeout, 
					ItepPowerManager *ipm)
{
	itep_power_manager_display_set_timeout(ipm, timeout);
	itep_power_manager_display_complete_set_timeout(display,invocation);
	return TRUE;
}

static void on_bus_acquired(GDBusConnection *connection,
				const gchar *name,
				ItepPowerManager *ipm)
{
	if (DEBUG_LIUHW)
	{
		g_printf("on_bus_acquired\n");
	}
	ItepPowerManagerDisplay *display = NULL;
	display = itep_power_manager_display_skeleton_new();
	g_signal_connect(display,"handle-get-timeout",G_CALLBACK(display_get_timeout),ipm);
	g_signal_connect(display,"handle-set-timeout",G_CALLBACK(display_set_timeout),ipm);
	g_dbus_interface_skeleton_export(G_DBUS_INTERFACE_SKELETON(display),connection,
						"/cn/com/itep/powermanager/display",NULL);
}

static void on_name_acquired(GDBusConnection *connect,
				const gchar *name,
				ItepPowerManager *ipm)
{
	if (DEBUG_LIUHW)
	{
		g_printf("Acquired the name %s\n",name);
	}
}

static void on_name_lost(GDBusConnection *connection,
				const gchar *name,
				ItepPowerManager *ipm)
{
	if (DEBUG_LIUHW)
	{
		g_printf("Lost the name %s\n",name);
	}
}

int main(int argc, char *argv[])
{
	ItepPowerManager *ipm;

	gtk_init(&argc, &argv);
	ipm = itep_power_manager_new();
	itep_power_manager_start(ipm);
	g_bus_own_name(G_BUS_TYPE_SESSION,
			"cn.com.itep.powermanager",
			G_BUS_NAME_OWNER_FLAGS_ALLOW_REPLACEMENT | G_BUS_NAME_OWNER_FLAGS_REPLACE,
			on_bus_acquired,
			on_name_acquired,
			on_name_lost,
			ipm,
			NULL);
	gtk_main();
	return 0;
}
