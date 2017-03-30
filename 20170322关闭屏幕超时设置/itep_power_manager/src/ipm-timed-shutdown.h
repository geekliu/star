#ifndef _IPM_TIMED_SHUTDOWN_H
#define _IPM_TIMED_SHUTDOWN_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib-object.h>

G_BEGIN_DECLS

#define IPM_TIMED_SHUTDOWN_TYPE		\
	(ipm_timed_shutdown_get_type())
#define IPM_TIMED_SHUTDOWN(o)		\
	(G_TYPE_CHECK_INSTANCE_CAST((o), IPM_TIMED_SHUTDOWN_TYPE, \
			IpmTimedShutdown))
#define IS_IPM_TIMED_SHUTDOWN(o)	\
	(G_TYPE_CHECK_INSTANCE_TYPE((o), IPM_TIMED_SHUTDOWN_TYPE))

typedef struct IpmTimedShutdownPrivate IpmTimedShutdownPrivate;

typedef struct {
	GObject			parent;
	IpmTimedShutdownPrivate	*priv;
} IpmTimedShutdown;

typedef struct {
	GObjectClass		parent_class;
} IpmTimedShutdownClass;

typedef enum {
	ITEP_TIMED_REBOOT,
	ITEP_TIMED_SHUTDOWN,
} IpmTimedAction;

GType ipm_timed_shutdown_get_type(void);

IpmTimedShutdown *ipm_timed_shutdown_new(void);

gboolean ipm_timed_shutdown_get_oneshot(IpmTimedShutdown *ts,
		IpmTimedAction *action, struct tm *time);
void ipm_timed_shutdown_set_oneshot(IpmTimedShutdown *ts,
		gboolean enable, IpmTimedAction action, struct tm *time);
gboolean ipm_timed_shutdown_get_daily(IpmTimedShutdown *ts,
		IpmTimedAction *action, struct tm *time);
void ipm_timed_shutdown_set_daily(IpmTimedShutdown *ts,
		gboolean enable, IpmTimedAction action, struct tm *time);
gint ipm_timed_shutdown_get_advance(IpmTimedShutdown *ts);
void ipm_timed_shutdown_set_advance(IpmTimedShutdown *ts, gint advance);
void ipm_timed_shutdown_notify_time(IpmTimedShutdown *ts, struct tm *now);

G_END_DECLS

#endif
