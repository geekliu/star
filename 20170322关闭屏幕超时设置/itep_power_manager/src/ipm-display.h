#ifndef _IPM_DISPLAY_H
#define _IPM_DISPLAY_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib-object.h>

G_BEGIN_DECLS

#define IPM_DISPLAY_TYPE 	\
	(ipm_display_get_type())
#define IPM_DISPLAY(o)		\
	(G_TYPE_CHECK_INSTANCE_CAST((o), IPM_DISPLAY_TYPE, IpmDisplay))
#define IS_IPM_DISPLAY		\
	(G_TYPE_CHECK_INSTANCE_TYPE((o), IPM_DISPLAY_TYPE))

typedef struct IpmDisplayPrivate IpmDisplayPrivate;

typedef struct {
	GObject			parent;
	IpmDisplayPrivate 	*priv;
} IpmDisplay;

typedef struct {
	GObjectClass		parent_class;
} IpmDisplayClass;

GType ipm_display_get_type(void);

IpmDisplay *ipm_display_new(void);
gint ipm_display_get_timeout(IpmDisplay *display);
void ipm_display_set_timeout(IpmDisplay *display, int timeout);
gboolean ipm_display_get_lock_session(IpmDisplay *display);
void ipm_display_set_lock_session(IpmDisplay *display, gboolean lock_session);

G_END_DECLS

#endif
