#ifndef _ITEP_POWER_MANAGER_H
#define _ITEP_POWER_MANAGER_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib-object.h>

G_BEGIN_DECLS

#define ITEP_POWER_MANAGER_TYPE		\
	(itep_power_manager_get_type())
#define ITEP_POWER_MANAGER(o)		\
	(G_TYPE_CHECK_INSTANCE_CAST((o), ITEP_POWER_MANAGER_TYPE, \
				    ItepPowerManager))
#define IS_ITEP_POWER_MANAGER(o)	\
	(G_TYPE_CHECK_INSTANCE_TYPE((o), ITEP_POWER_MANAGER_TYPE))

typedef struct ItepPowerManagerPrivate ItepPowerManagerPrivate;

typedef struct {
	GObject			parent;
	ItepPowerManagerPrivate *priv;
} ItepPowerManager;

typedef struct {
	GObjectClass		parent_class;
} ItepPowerManagerClass;

GType itep_power_manager_get_type(void);

ItepPowerManager *itep_power_manager_new(void);
void itep_power_manager_start(ItepPowerManager *ipm);
void itep_power_manager_stop(ItepPowerManager *ipm);

G_END_DECLS

#endif
