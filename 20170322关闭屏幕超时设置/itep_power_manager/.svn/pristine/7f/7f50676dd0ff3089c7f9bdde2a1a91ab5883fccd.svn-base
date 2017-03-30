#ifndef _IPM_IDLE_SHUTDOWN_H
#define _IPM_IDLE_SHUTDOWN_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib-object.h>
#include <time.h>

G_BEGIN_DECLS

#define IPM_IDLE_SHUTDOWN_TYPE	\
	(ipm_idle_shutdown_get_type())
#define IPM_IDLE_SHUTDOWN(o)	\
	(G_TYPE_CHECK_INSTANCE_CAST((o), IPM_IDLE_SHUTDOWN_TYPE, \
				    IpmIdleShutdown))
#define IS_IPM_IDLE_SHUTDOWN(o)	\
	(G_TYPE_CHECK_INSTANCE_TYPE(o), IPM_IDLE_SHUTDOWN_TYPE)

typedef struct IpmIdleShutdownPrivate IpmIdleShutdownPrivate;

typedef struct {
	GObject			parent;
	IpmIdleShutdownPrivate	*priv;
} IpmIdleShutdown;

typedef struct {
	GObjectClass		parent_class;
} IpmIdleShutdownClass;

GType ipm_idle_shutdown_get_type(void);

IpmIdleShutdown *ipm_idle_shutdown_new(void);
gint ipm_idle_shutdown_get_timeout(IpmIdleShutdown *is);
void ipm_idle_shutdown_set_timeout(IpmIdleShutdown *is, gint timeout);
gboolean ipm_idle_shutdown_get_inhibited_period(IpmIdleShutdown *is,
		struct tm *begin, struct tm *end);
void ipm_idle_shutdown_set_inhibited_period(IpmIdleShutdown *is,
		gboolean enable, struct tm *begin, struct tm *end);
gboolean ipm_idle_shutdown_get_connected_shutdown(IpmIdleShutdown *is);
void ipm_idle_shutdown_set_connected_shutdown(IpmIdleShutdown *is,
		gboolean shutdown);
gint ipm_idle_shutdown_get_wait(IpmIdleShutdown *is);
void ipm_idle_shutdown_set_wait(IpmIdleShutdown *is, gint length);
void ipm_idle_shutdown_notify_time(IpmIdleShutdown *is, struct tm *now);

G_END_DECLS

#endif
