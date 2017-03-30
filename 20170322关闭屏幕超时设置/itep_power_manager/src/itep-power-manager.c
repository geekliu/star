#include "itep-power-manager.h"

#include "ipm-display.h"
#include "ipm-idle-shutdown.h"
#include "ipm-timed-shutdown.h"

#define ITEP_POWER_MANAGER_GET_PRIVATE(o)	\
	(G_TYPE_INSTANCE_GET_PRIVATE((o), ITEP_POWER_MANAGER_TYPE, \
				     ItepPowerManagerPrivate))

struct ItepPowerManagerPrivate {
	IpmDisplay *display;
	IpmIdleShutdown *idle_shutdown;
	IpmTimedShutdown *timed_shutdown;
	guint g_source_tag;
};

G_DEFINE_TYPE(ItepPowerManager, itep_power_manager, G_TYPE_OBJECT);

static void itep_power_manager_dispose(GObject *o) {
	ItepPowerManager *pm = ITEP_POWER_MANAGER(o);
	ItepPowerManagerPrivate *priv = ITEP_POWER_MANAGER_GET_PRIVATE(pm);

	if (priv) {
		if (priv->g_source_tag) {
			g_source_remove(priv->g_source_tag);
			priv->g_source_tag = 0;
		}

		if (priv->display) {
			g_object_unref(priv->display);
			priv->display = NULL;
		}

		if (priv->idle_shutdown) {
			g_object_unref(priv->idle_shutdown);
			priv->idle_shutdown = NULL;
		}

		if (priv->timed_shutdown) {
			g_object_unref(priv->timed_shutdown);
			priv->timed_shutdown = NULL;
		}
	}

	G_OBJECT_CLASS(itep_power_manager_parent_class)->dispose(o);
}

static void itep_power_manager_class_init(ItepPowerManagerClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);
	g_type_class_add_private(klass, sizeof(ItepPowerManagerPrivate));
	object_class->dispose = itep_power_manager_dispose;
}

static void itep_power_manager_init(ItepPowerManager *o)
{
	o->priv = ITEP_POWER_MANAGER_GET_PRIVATE(o);
	o->priv->display = ipm_display_new();
	o->priv->idle_shutdown = ipm_idle_shutdown_new();
	o->priv->timed_shutdown = ipm_timed_shutdown_new();
	o->priv->g_source_tag = 0;
}

ItepPowerManager *itep_power_manager_new(void)
{
	return ITEP_POWER_MANAGER(g_object_new(ITEP_POWER_MANAGER_TYPE, NULL));
}

static gboolean ipm_routine(gpointer data)
{
	ItepPowerManager *ipm = (ItepPowerManager *)data;
	time_t now = 0;
	struct tm *now_tm = NULL;

	time(&now);
	now_tm = localtime(&now);

	if (ipm->priv == NULL) return FALSE;

	// ipm_idle_shutdown_notify_time(ipm->priv->idle_shutdown, now_tm);
	ipm_timed_shutdown_notify_time(ipm->priv->timed_shutdown, now_tm);
	return TRUE;
}
gint itep_power_manager_display_get_timeout(ItepPowerManager *ipm)
{
	return ipm_display_get_timeout(ipm->priv->display);
}

void itep_power_manager_display_set_timeout(ItepPowerManager *ipm, gint settime)
{
	ipm_display_set_timeout(ipm->priv->display, settime);
}
void itep_power_manager_start(ItepPowerManager *ipm)
{
	if (ipm->priv->g_source_tag != 0) return;
	ipm_display_set_timeout(ipm->priv->display, 60);
	ipm_idle_shutdown_set_timeout(ipm->priv->idle_shutdown, 120);
	ipm_idle_shutdown_set_wait(ipm->priv->idle_shutdown, 10);
	struct tm time;
	time.tm_hour = 18;
	time.tm_min = 0;
	time.tm_sec = 0;
	ipm_timed_shutdown_set_oneshot(ipm->priv->timed_shutdown,
			TRUE, ITEP_TIMED_REBOOT, &time);
	time.tm_hour = 17;
	time.tm_min = 00;
	ipm_timed_shutdown_set_daily(ipm->priv->timed_shutdown,
			TRUE, ITEP_TIMED_SHUTDOWN, &time);
	ipm_timed_shutdown_set_advance(ipm->priv->timed_shutdown, 3600);
	ipm->priv->g_source_tag = g_timeout_add(1000, ipm_routine, ipm);
}

void itep_power_manager_stop(ItepPowerManager *ipm)
{
	g_source_remove(ipm->priv->g_source_tag);
	ipm->priv->g_source_tag = 0;
}
