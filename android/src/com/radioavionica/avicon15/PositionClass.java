package com.radioavionica.avicon15;

import android.content.Context;
import android.content.Intent;
import android.location.LocationManager;
import android.provider.Settings;
import android.net.Uri;
import android.os.Build.VERSION;
import android.os.Build.VERSION_CODES;
import android.util.Log;

public class PositionClass
{               
    public static void enableGps(Context context)
    {
        Log.d("SYSTEM", "ANDROID SDK: " + android.os.Build.VERSION.SDK_INT);
        if (android.os.Build.VERSION.SDK_INT == 19) {
            context.startActivity(new Intent(Settings.ACTION_LOCATION_SOURCE_SETTINGS));
            return;
        }
        Intent intent = new Intent("android.location.GPS_ENABLED_CHANGE");
        intent.putExtra("enabled", true);
        context.sendBroadcast(intent);
        String provider = Settings.Secure.getString(context.getContentResolver(), Settings.Secure.LOCATION_PROVIDERS_ALLOWED);
        if (!provider.contains("gps")) {
             final Intent poke = new Intent();
             poke.setClassName("com.android.settings", "com.android.settings.widget.SettingsAppWidgetProvider");
             poke.addCategory(Intent.CATEGORY_ALTERNATIVE);
             poke.setData(Uri.parse("3"));
             context.sendBroadcast(poke);
            }
    }

    public static void disableGps(Context context)
    {
        Log.d("SYSTEM", "ANDROID SDK: " + android.os.Build.VERSION.SDK_INT);
        if (android.os.Build.VERSION.SDK_INT == 19) {
            context.startActivity(new Intent(Settings.ACTION_LOCATION_SOURCE_SETTINGS));
            return;
        }
        Intent intent = new Intent("android.location.GPS_ENABLED_CHANGE");
        intent.putExtra("enabled", false);
        context.sendBroadcast(intent);
        String provider = Settings.Secure.getString(context.getContentResolver(), Settings.Secure.LOCATION_PROVIDERS_ALLOWED);
            if (provider.contains("gps")) {
                final Intent poke = new Intent();
                poke.setClassName("com.android.settings", "com.android.settings.widget.SettingsAppWidgetProvider");
                poke.addCategory(Intent.CATEGORY_ALTERNATIVE);
                poke.setData(Uri.parse("3"));
                context.sendBroadcast(poke);
            }
    }

    public static boolean isGpsEnabled(Context context)
    {
        LocationManager locationManager = (LocationManager) context.getSystemService(context.LOCATION_SERVICE);                                
        boolean status = locationManager.isProviderEnabled(LocationManager.GPS_PROVIDER);
        return status;
    }    
}
