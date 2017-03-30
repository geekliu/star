#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "ipm-display.h"

#include <gdk/gdkx.h>
#include <X11/Xproto.h>
#include <X11/extensions/dpms.h>

#define IPM_DISPLAY_GET_PRIVATE(o) \
	(G_TYPE_INSTANCE_GET_PRIVATE((o), IPM_DISPLAY_TYPE, IpmDisplayPrivate))

struct IpmDisplayPrivate {
	gint		timeout;
	gboolean	lock_session;
};

G_DEFINE_TYPE(IpmDisplay, ipm_display, G_TYPE_OBJECT);

static void ipm_display_class_init(IpmDisplayClass *klass)
{
	g_type_class_add_private(klass, sizeof(IpmDisplayPrivate));
}

static void ipm_display_init(IpmDisplay *display)
{
	display->priv = IPM_DISPLAY_GET_PRIVATE(display);
	display->priv->timeout = 300;
	display->priv->lock_session = TRUE;
}

IpmDisplay *ipm_display_new(void)
{
	return IPM_DISPLAY(g_object_new(IPM_DISPLAY_TYPE, NULL));
}

gint ipm_display_get_timeout(IpmDisplay *display)
{
	return display->priv->timeout;
}

void ipm_display_set_timeout(IpmDisplay *display, int timeout)
{
	display->priv->timeout = timeout;
	DPMSEnable(gdk_x11_get_default_xdisplay());
	DPMSSetTimeouts(gdk_x11_get_default_xdisplay(),
			timeout, 0, 0);
}

gboolean ipm_display_get_lock_session(IpmDisplay *display)
{
	return display->priv->lock_session;
}

void ipm_display_set_lock_session(IpmDisplay *display, gboolean lock_session)
{
}
