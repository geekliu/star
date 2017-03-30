#include "ipm-idle-shutdown.h"

#include <memory.h>
#include <gtk/gtk.h>

#include <gdk/gdkx.h>
#include <X11/Xproto.h>
#include <X11/extensions/dpms.h>

#include "egg-idletime.h"
#include "ipm-systemd.h"

#define IPM_IDLE_SHUTDOWN_GET_PRIVATE(o) \
	(G_TYPE_INSTANCE_GET_PRIVATE((o), IPM_IDLE_SHUTDOWN_TYPE, \
				     IpmIdleShutdownPrivate))

struct IpmIdleShutdownPrivate {
	/* timeout before shutdown */
	gint timeout;
	gboolean period_inhibition;
	/* inhibited period begin time */
	struct tm begin;
	/* inhibited period end time */
	struct tm end;
	/* idle shutdown while connected to remote */
	gboolean connected_shutdown;
	/* wait time after timeout and before shutdown */
	gint wait;
	/* inhibit idle shutdown */
	gboolean inhibited;
	/* idle timer */
	EggIdletime *idletime;
	/* idle timer running */
	gboolean running;
	gboolean waiting;
	gint countdown;
	GtkWindow *window;
	GtkProgressBar *pbar;
	GtkLabel *label;
};

static gboolean day_time_before(struct tm *t1, struct tm *t2)
{
	if (t1->tm_hour < t2->tm_hour) return TRUE;
	if (t1->tm_hour > t2->tm_hour) return FALSE;
	if (t1->tm_min <= t2->tm_min) return TRUE;
	return FALSE;
}

static gboolean inhibited_time(IpmIdleShutdown *is, struct tm *t)
{
	if (!is->priv->period_inhibition) return FALSE;

	struct tm *begin = &is->priv->begin;
	struct tm *end = &is->priv->end;

	if (day_time_before(begin, end)) {
		if (day_time_before(begin, t) && day_time_before(t, end))
			return TRUE;
	} else {
		if (day_time_before(end, t) && day_time_before(t, begin))
			return TRUE;
	}

	return FALSE;
}

static void update_pbar(IpmIdleShutdownPrivate *priv)
{
	gchar text[64] = "";
	g_snprintf(text, sizeof(text), "%02d:%02d", priv->countdown / 60,
			priv->countdown % 60);
	gtk_progress_bar_set_fraction(priv->pbar, (double)priv->countdown /
			priv->wait);
	gtk_progress_bar_set_text(priv->pbar, text);
}

static void idle_timeout(EggIdletime *idle, guint id, IpmIdleShutdown *is)
{
	IpmIdleShutdownPrivate *priv = is->priv;
	gchar msg[128] = "";

	priv->countdown = priv->wait;
	priv->waiting = TRUE;
	int n = priv->timeout / 60;
	g_snprintf(msg, sizeof(msg),
			"You haven't done anything for %d %s.\n"
			"Do you want to shutdown?", n,
			n > 1 ? "minutes" : "minute");
	gtk_label_set_text(priv->label, msg);
	update_pbar(priv);

	DPMSDisable(gdk_x11_get_default_xdisplay());
	gtk_widget_show_all(GTK_WIDGET(priv->window));
}

G_DEFINE_TYPE(IpmIdleShutdown, ipm_idle_shutdown, G_TYPE_OBJECT);


static void ipm_idle_shutdown_class_init(IpmIdleShutdownClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);
	g_type_class_add_private(klass, sizeof(IpmIdleShutdownPrivate));
}

static gboolean hide_dialog(GtkWidget *widget, GdkEvent *event,
		gpointer user_data)
{
	gtk_widget_hide_all(widget);
	return TRUE;
}

static void shutdown_now(GtkButton *button, IpmIdleShutdownPrivate *priv)
{
	priv->waiting = FALSE;
	ipm_systemd_shutdown(NULL);
}

static void cancel_shutdown(GtkButton *button, IpmIdleShutdownPrivate *priv)
{
	priv->waiting = FALSE;
	gtk_widget_hide_all(GTK_WIDGET(priv->window));
}

static void ipm_idle_shutdown_init(IpmIdleShutdown *is)
{
	GtkButton *btn = NULL;

	is->priv = IPM_IDLE_SHUTDOWN_GET_PRIVATE(is);
	memset(is->priv, 0, sizeof(is->priv));
	is->priv->idletime = egg_idletime_new();
	g_signal_connect(is->priv->idletime, "alarm-expired",
			G_CALLBACK(idle_timeout), is);
	is->priv->running = FALSE;
	is->priv->waiting = FALSE;
	is->priv->countdown = 0;

	GtkBuilder *builder = gtk_builder_new();
	if (gtk_builder_add_from_file(builder, GLADE_FILE, NULL) == 0) {
		g_print("failed to load glade file\n");
		g_object_unref(builder);
		return;
	}

	is->priv->window = GTK_WINDOW(gtk_builder_get_object(builder,
				"idle_shutdown_dialog"));
	g_signal_connect(is->priv->window, "delete-event",
			G_CALLBACK(hide_dialog), NULL);
	gtk_window_set_keep_above(is->priv->window, TRUE);

	is->priv->label = GTK_LABEL(gtk_builder_get_object(builder,
				"idle_shutdown_msg"));

	is->priv->pbar = GTK_PROGRESS_BAR(gtk_builder_get_object(builder,
				"idle_shutdown_pbar"));

	btn = GTK_BUTTON(gtk_builder_get_object(builder, "idle_shutdown"));
	g_signal_connect(btn, "clicked", G_CALLBACK(shutdown_now), is->priv);

	btn = GTK_BUTTON(gtk_builder_get_object(builder,
				"cancel_idle_shutdown"));
	g_signal_connect(btn, "clicked", G_CALLBACK(cancel_shutdown), is->priv);

	g_object_unref(builder);
}

IpmIdleShutdown *ipm_idle_shutdown_new(void)
{
	return IPM_IDLE_SHUTDOWN(g_object_new(IPM_IDLE_SHUTDOWN_TYPE, NULL));
}

gint ipm_idle_shutdown_get_timeout(IpmIdleShutdown *is)
{
	return is->priv->timeout;
}

void ipm_idle_shutdown_set_timeout(IpmIdleShutdown *is, gint timeout)
{
	is->priv->timeout = timeout;
}

gint ipm_idle_shutdown_get_wait(IpmIdleShutdown *is)
{
	return is->priv->wait;
}

void ipm_idle_shutdown_set_wait(IpmIdleShutdown *is, gint length)
{
	is->priv->wait = length;
}

static void idletime_start(IpmIdleShutdown *is) {
	if (is->priv->running) return;

	if (is->priv->timeout > 0) {
		egg_idletime_alarm_set(is->priv->idletime,
				TIMEOUT_INACTIVITY_ON_AC,
				is->priv->timeout * 1000);
		is->priv->running = TRUE;
	}
}

static void idletime_stop(IpmIdleShutdown *is) {
	if (!is->priv->running) return;

	egg_idletime_alarm_remove(is->priv->idletime, TIMEOUT_INACTIVITY_ON_AC);
	is->priv->running = FALSE;
}

void ipm_idle_shutdown_notify_time(IpmIdleShutdown *is, struct tm *now)
{
	IpmIdleShutdownPrivate *priv = is->priv;
	gboolean inhibited = priv->inhibited;
	gboolean running = priv->running;

	if (inhibited) {
		if (running) idletime_stop(is);
	} else if (inhibited_time(is, now)) {
		if (running) idletime_stop(is);
	} else {
		if (running) {
			if (priv->waiting) {
				--priv->countdown;
				/* update dialop */
				update_pbar(priv);
				if (priv->countdown == 0) {
					/* shutdown */
					priv->waiting = FALSE;
					ipm_systemd_shutdown(NULL);
				}
			}
		} else {
			idletime_start(is); 
		}
	}
}
