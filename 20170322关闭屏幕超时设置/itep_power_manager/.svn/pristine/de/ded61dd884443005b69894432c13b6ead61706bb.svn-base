#include "ipm-timed-shutdown.h"

#include <memory.h>
#include <time.h>
#include <gtk/gtk.h>

#define IPM_TIMED_SHUTDOWN_GET_PRIVATE(o) \
	(G_TYPE_INSTANCE_GET_PRIVATE((o), IPM_TIMED_SHUTDOWN_TYPE, \
			IpmTimedShutdownPrivate))

typedef struct {
	gboolean enable;
	IpmTimedAction action;
	struct tm at;
	gboolean canceled;
	gboolean repeat;
	struct tm postponed;
	gint arrival;
} TimeSet;

struct IpmTimedShutdownPrivate {
	TimeSet oneshot;
	TimeSet daily;
	gint advance;
	TimeSet *waiting;
	gint wait;
	GtkWindow *window;
	GtkLabel *label;
	GtkComboBoxText *combo;
	GtkProgressBar *pbar;
};

G_DEFINE_TYPE(IpmTimedShutdown, ipm_timed_shutdown, G_TYPE_OBJECT);

#define intraday_time(time) \
	(time->tm_hour * 3600 + time->tm_min * 60 + time->tm_sec)

static gint time_diff(struct tm *t1, struct tm *t2)
{
	return intraday_time(t1) - intraday_time(t2);
}

static gint time_interval(struct tm *t1, struct tm *t2)
{
	gint diff = time_diff(t1, t2);
	return diff >= 0 ? diff : 86400 + diff;
}

static gint next_arrival(TimeSet *set, struct tm *now)
{
	if (set->enable) {
		if (!set->canceled) {
			return time_interval(&set->postponed, now);
		} else if (set->repeat) {
			return time_interval(&set->postponed, now) + 86400;
		}
	} else {
		return INT_MAX;
	}
}

static void ipm_timed_shutdown_class_init(IpmTimedShutdownClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);
	g_type_class_add_private(klass, sizeof(IpmTimedShutdownPrivate));
}

static gboolean hide_dialog(GtkWidget *widget, GdkEvent *event,
		gpointer user_data)
{
	gtk_widget_hide_all(widget);
	return TRUE;
}

static void reboot_or_shutdown(IpmTimedAction action)
{
	switch (action) {
	case ITEP_TIMED_REBOOT:
		ipm_systemd_reboot(NULL);
		break;
	case ITEP_TIMED_SHUTDOWN:
		ipm_systemd_shutdown(NULL);
		break;
	default:
		break;
	}
}

static void timed_shutdown(GtkButton *btn, IpmTimedShutdownPrivate *priv)
{
	int opt = gtk_combo_box_get_active(GTK_COMBO_BOX(priv->combo));

	switch (opt) {
	case 0:
		reboot_or_shutdown(priv->waiting->action);
		break;
	case 1:
		priv->waiting->postponed.tm_min += 10;
		priv->waiting = NULL;
		break;
	case 2:
		priv->waiting->postponed.tm_min += 30;
		priv->waiting = NULL;
		break;
	case 3:
		priv->waiting->postponed.tm_hour += 1;
		priv->waiting = NULL;
		break;
	case 4:
		priv->waiting->postponed.tm_hour += 2;
		priv->waiting = NULL;
		break;
	default:
		break;
	}
	gtk_widget_hide_all(GTK_WIDGET(priv->window));
}

static void cancel_shutdown(GtkButton *btn, IpmTimedShutdownPrivate *priv)
{
	priv->waiting->canceled = TRUE;
	priv->waiting->enable = priv->waiting->repeat;
	priv->waiting = NULL;
	gtk_widget_hide_all(GTK_WIDGET(priv->window));
}

static void ipm_timed_shutdown_init(IpmTimedShutdown *o)
{
	GtkButton *btn;

	o->priv = IPM_TIMED_SHUTDOWN_GET_PRIVATE(o);
	memset(o->priv, 0, sizeof(o->priv));

	GtkBuilder *builder = gtk_builder_new();
	if (gtk_builder_add_from_file(builder, GLADE_FILE, NULL) == 0) {
		g_print("failed to load glade file\n");
		g_object_unref(builder);
		return;
	}

	o->priv->window = GTK_WINDOW(gtk_builder_get_object(builder,
				"timed_shutdown_dialog"));
	g_signal_connect(o->priv->window, "delete-event",
			G_CALLBACK(hide_dialog), NULL);
	gtk_window_set_keep_above(o->priv->window, TRUE);

	o->priv->label = GTK_LABEL(gtk_builder_get_object(builder,
				"timed_shutdown_msg"));
	o->priv->pbar = GTK_PROGRESS_BAR(gtk_builder_get_object(builder,
				"timed_shutdown_pbar"));
	o->priv->combo = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(builder,
				"timed_shutdown_option"));

	btn = GTK_BUTTON(gtk_builder_get_object(builder, "timed_shutdown"));
	g_signal_connect(btn, "clicked", G_CALLBACK(timed_shutdown), o->priv);

	btn = GTK_BUTTON(gtk_builder_get_object(builder,
				"cancel_timed_shutdown"));
	g_signal_connect(btn, "clicked", G_CALLBACK(cancel_shutdown), o->priv);
	g_object_unref(builder);
}

IpmTimedShutdown *ipm_timed_shutdown_new(void)
{
	return IPM_TIMED_SHUTDOWN(g_object_new(IPM_TIMED_SHUTDOWN_TYPE, NULL));
}

gboolean ipm_timed_shutdown_get_oneshot(IpmTimedShutdown *ts,
		IpmTimedAction *action, struct tm *time)
{
	*action = ts->priv->oneshot.action;
	*time = ts->priv->oneshot.at;
	return ts->priv->oneshot.enable;
}

void ipm_timed_shutdown_set_oneshot(IpmTimedShutdown *ts,
		gboolean enable, IpmTimedAction action, struct tm *time)
{
	ts->priv->oneshot.enable = enable;
	ts->priv->oneshot.action = action;
	ts->priv->oneshot.at = *time;
	ts->priv->oneshot.postponed = *time;
	ts->priv->oneshot.canceled = FALSE;
}

gboolean ipm_timed_shutdown_get_daily(IpmTimedShutdown *ts,
		IpmTimedAction *action, struct tm *time)
{
	*action = ts->priv->daily.action;
	*time = ts->priv->daily.at;
	return ts->priv->daily.enable;
}

void ipm_timed_shutdown_set_daily(IpmTimedShutdown *ts,
		gboolean enable, IpmTimedAction action, struct tm *time)
{
	ts->priv->daily.enable = enable;
	ts->priv->daily.action = action;
	ts->priv->daily.at = *time;
	ts->priv->daily.postponed = *time;
	ts->priv->daily.canceled = FALSE;
}

gint ipm_timed_shutdown_get_advance(IpmTimedShutdown *ts)
{
	return ts->priv->advance;
}

void ipm_timed_shutdown_set_advance(IpmTimedShutdown *ts, gint advance)
{
	ts->priv->advance = advance;
}

static void update_window(IpmTimedShutdownPrivate *priv, TimeSet *set,
	       gint countdown)
{
	if (priv->waiting == NULL) {
		if (countdown <= priv->advance) {
			gtk_combo_box_set_active(GTK_COMBO_BOX(priv->combo), 0);
			gtk_widget_show_all(GTK_WIDGET(priv->window));
			priv->wait = countdown;
			priv->waiting = set;
		} else {
			return;
		}
	}

	gchar text[64] = "";
	g_snprintf(text, sizeof(text), "%02d:%02d", countdown / 60,
			countdown % 60);
	gtk_progress_bar_set_fraction(priv->pbar, (double)countdown /
			priv->wait);
	gtk_progress_bar_set_text(priv->pbar, text);

	if (countdown == 0) {
		reboot_or_shutdown(set->action);
	}
}

static gint time_set_notify_time(TimeSet *set, struct tm *now)
{
	if (!set->enable || set->canceled) return INT_MAX;

	gint interval = next_arrival(set, now);
	if (interval == 0) {
		if (set->canceled) {
			set->canceled = FALSE;
			set->enable = set->repeat;
			set->postponed = set->at;
		}
	}
	return interval;
}

void ipm_timed_shutdown_notify_time(IpmTimedShutdown *ts, struct tm *now)
{
	IpmTimedShutdownPrivate *priv = ts->priv;
	gint oneshot_t = 0, daily_t = 0;

	if (!priv->oneshot.enable && !priv->daily.enable) return;

	oneshot_t = time_set_notify_time(&priv->oneshot, now);
	daily_t = time_set_notify_time(&priv->daily, now);

	if (oneshot_t <= daily_t) {
		update_window(priv, &priv->oneshot, oneshot_t);
	} else {
		update_window(priv, &priv->daily, daily_t);
	}
}
