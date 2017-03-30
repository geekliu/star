#include <gtk/gtk.h>
#include "generated.h"

typedef struct	ItepPowerManagerSettingPrivate
{	
	GtkWidget	*dialog	;
	GtkWidget	*button	;
	GtkWidget	*hscale	;
	GtkAdjustment 	*adj	;
	ItepPowerManagerDisplay *display;
}ItepPowerManagerSettingPrivate;

void on_confirm_clicked(GtkWidget *widget, ItepPowerManagerSettingPrivate *ipmsp)
{
	itep_power_manager_display_call_set_timeout_sync(
					ipmsp->display,
					(gint)gtk_adjustment_get_value(ipmsp->adj),
					NULL,
					NULL);
}

gint main(int argc, char *argv[])
{
	GtkBuilder	*builder= NULL;
	GError		*error	= NULL;
	struct ItepPowerManagerSettingPrivate ipmsp;
	gint 	 gettime = 0;

	ipmsp.display = itep_power_manager_display_proxy_new_for_bus_sync(
				G_BUS_TYPE_SESSION,
				G_DBUS_PROXY_FLAGS_NONE,
				"cn.com.itep.powermanager",
				"/cn/com/itep/powermanager/display",
				NULL,
				&error);

	if (ipmsp.display == NULL)
	{
		g_printf("Error getting proxy: %s\n",error->message);
		g_error_free(error);
		return 0;
	}
	gtk_init(&argc,&argv);

	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder,GLADE_FILE,NULL);
	ipmsp.button = GTK_WIDGET(gtk_builder_get_object(builder,"button1"));
	ipmsp.dialog = GTK_WIDGET(gtk_builder_get_object(builder,"dialog1"));
	ipmsp.hscale = GTK_WIDGET(gtk_builder_get_object(builder,"hscale1"));
	ipmsp.adj = GTK_ADJUSTMENT(gtk_builder_get_object(builder,"adj"));
	g_object_unref(G_OBJECT(builder));

	itep_power_manager_display_call_get_timeout_sync(ipmsp.display, &gettime, NULL, NULL);
	gtk_adjustment_set_value(ipmsp.adj,(gdouble)gettime);

	g_signal_connect(G_OBJECT(ipmsp.button), "clicked",G_CALLBACK(on_confirm_clicked), &ipmsp);
	g_signal_connect(G_OBJECT(ipmsp.dialog), "destroy",G_CALLBACK(gtk_main_quit), NULL);
	
	gtk_widget_show_all(ipmsp.dialog);
	gtk_main();
	return 0;
}
