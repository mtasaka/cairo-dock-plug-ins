#include <string.h>

#include "powermanager-struct.h"
#include "powermanager-config.h"

AppletConfig myConfig;
AppletData myData;


CD_APPLET_CONFIG_BEGIN
	reset_config ();
	
	myConfig.quickInfoType = CD_CONFIG_GET_INTEGER_WITH_DEFAULT ("Configuration", "quick_info_type", MY_APPLET_TIME);
	
CD_APPLET_CONFIG_END


void reset_config (void)
{
	g_free (myConfig.defaultTitle);
	myConfig.defaultTitle = NULL;
	
	memset (&myConfig, 0, sizeof (AppletConfig));
}

void reset_data (void)
{
	reset_surfaces ();
	
	gboolean dbus_enable = myData.dbus_enable;
	memset (&myData, 0, sizeof (AppletData));
	myData.dbus_enable = dbus_enable;
}


void reset_surfaces (void)
{
	if (myData.pSurfaceBattery44 != NULL)
	{
		cairo_surface_destroy (myData.pSurfaceBattery44);
		myData.pSurfaceBattery44 = NULL;
	}
	if (myData.pSurfaceBattery34 != NULL)
	{
		cairo_surface_destroy (myData.pSurfaceBattery34);
		myData.pSurfaceBattery34 = NULL;
	}
	if (myData.pSurfaceBattery24 != NULL)
	{
		cairo_surface_destroy (myData.pSurfaceBattery24);
		myData.pSurfaceBattery24 = NULL;
	}
	if (myData.pSurfaceBattery14 != NULL)
	{
		cairo_surface_destroy (myData.pSurfaceBattery14);
		myData.pSurfaceBattery14 = NULL;
	}
	if (myData.pSurfaceBattery04 != NULL)
	{
		cairo_surface_destroy (myData.pSurfaceBattery04);
		myData.pSurfaceBattery04 = NULL;
	}
	if (myData.pSurfaceCharge44 != NULL)
	{
		cairo_surface_destroy (myData.pSurfaceCharge44);
		myData.pSurfaceCharge44 = NULL;
	}
	if (myData.pSurfaceCharge34 != NULL)
	{
		cairo_surface_destroy (myData.pSurfaceCharge34);
		myData.pSurfaceCharge34 = NULL;
	}
	if (myData.pSurfaceCharge24 != NULL)
	{
		cairo_surface_destroy (myData.pSurfaceCharge24);
		myData.pSurfaceCharge24 = NULL;
	}
	if (myData.pSurfaceCharge14 != NULL)
	{
		cairo_surface_destroy (myData.pSurfaceCharge14);
		myData.pSurfaceCharge14 = NULL;
	}
	if (myData.pSurfaceCharge04 != NULL)
	{
		cairo_surface_destroy (myData.pSurfaceCharge04);
		myData.pSurfaceCharge04 = NULL;
	}
	if (myData.pSurfaceSector!= NULL)
	{
		cairo_surface_destroy (myData.pSurfaceSector);
		myData.pSurfaceSector = NULL;
	}
	if (myData.pSurfaceBroken!= NULL)
	{
		cairo_surface_destroy (myData.pSurfaceBroken);
		myData.pSurfaceBroken = NULL;
	}
}
