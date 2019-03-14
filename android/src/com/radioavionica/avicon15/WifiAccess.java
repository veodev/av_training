package com.radioavionica.avicon15;

import android.content.Context;
import android.net.wifi.ScanResult;
import android.net.wifi.WifiManager;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiInfo;
import android.net.DhcpInfo;
import java.util.ArrayList;
import java.util.List;
import java.util.Formatter;

public class WifiAccess
{
    public static boolean enableWifi(Context context)
    {
        WifiManager wifiManager = (WifiManager) context.getSystemService(context.WIFI_SERVICE);
        return wifiManager.setWifiEnabled(true);
    }

    public static boolean disableWifi(Context context)
    {
        WifiManager wifiManager = (WifiManager) context.getSystemService(context.WIFI_SERVICE);
        return wifiManager.setWifiEnabled(false);
    }

    public static boolean isWifiEnabled(Context context)
    {
        WifiManager wifiManager = (WifiManager) context.getSystemService(context.WIFI_SERVICE);
        return wifiManager.isWifiEnabled();
    }

    private static String scanWifi(Context context)
    {
        WifiManager wifiManager = (WifiManager) context.getSystemService(context.WIFI_SERVICE);
        wifiManager.startScan();
        ArrayList<String> networkSsids = new ArrayList<String>();
        String result = new String();
        for (ScanResult scanResult : wifiManager.getScanResults()) {
            networkSsids.add(scanResult.SSID);

        }
        for (String item : networkSsids) {
            result = result.concat(item).concat("///");
        }
        return result;
    }

    private static void connectToSsid(Context context, String ssid)
    {
        WifiConfiguration wifiConfig = new WifiConfiguration();
        wifiConfig.SSID = String.format("\"%s\"", ssid);
        wifiConfig.allowedKeyManagement.set(WifiConfiguration.KeyMgmt.NONE);
        WifiManager wifiManager = (WifiManager) context.getSystemService(context.WIFI_SERVICE);
        int netId = wifiManager.addNetwork(wifiConfig);
        List<WifiConfiguration> listConfigs = wifiManager.getConfiguredNetworks();

        for (WifiConfiguration conf: listConfigs) {
            if (conf.SSID != null && conf.SSID.equals("\"" + ssid + "\"")) {
                wifiManager.disconnect();
                wifiManager.enableNetwork(netId, true);
                wifiManager.reconnect();
                break;
            }
        }
    }

    private static String getSsid(Context context)
    {
        WifiManager wifiManager = (WifiManager) context.getSystemService(context.WIFI_SERVICE);
        WifiInfo wifiInfo = wifiManager.getConnectionInfo();
        return  wifiInfo.getSSID();
    }

    private static String getIpAddress(Context context)
    {
        WifiManager wifiManager = (WifiManager) context.getSystemService(context.WIFI_SERVICE);
        int ip = wifiManager.getConnectionInfo().getIpAddress();
        return String.format("%d.%d.%d.%d", (ip & 0xff), (ip >> 8 & 0xff), (ip >> 16 & 0xff), (ip >> 24 & 0xff));
    }

    private static String getDefaultGateway(Context context)
    {
        WifiManager wifiManager = (WifiManager) context.getSystemService(context.WIFI_SERVICE);
        DhcpInfo dhcpInfo = wifiManager.getDhcpInfo();
        int ip = wifiManager.getDhcpInfo().gateway;
        return String.format("%d.%d.%d.%d", (ip & 0xff), (ip >> 8 & 0xff), (ip >> 16 & 0xff), (ip >> 24 & 0xff));
    }
}
