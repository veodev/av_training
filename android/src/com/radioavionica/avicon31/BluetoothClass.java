package com.radioavionica.avicon31;

import android.content.Context;
import android.content.BroadcastReceiver;
import android.content.Intent;
import android.content.IntentFilter;
import android.media.AudioManager;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothProfile.ServiceListener;
import android.bluetooth.BluetoothA2dp;
import android.bluetooth.BluetoothProfile;
import android.util.Log;
import java.util.Set;
import java.lang.reflect.Method;

public class BluetoothClass {
    Context mainContext;
    BluetoothA2dp bluetoothA2dp = null;
    BluetoothDevice bluetoothDevice = null;
    static BluetoothAdapter bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();

    final BluetoothProfile.ServiceListener btServiceListener = new BluetoothProfile.ServiceListener() {
        public void onServiceConnected(int profile, BluetoothProfile proxy) {
            if (profile == BluetoothProfile.A2DP) {
                Log.d("", "=== A2DP SERVICE CONNECTED ===");
                bluetoothA2dp = (BluetoothA2dp) proxy;
                AudioManager audioManager = (AudioManager) mainContext.getSystemService(mainContext.AUDIO_SERVICE);
                resetAudioManager(audioManager);
            }
        }

        public void onServiceDisconnected(int profile) {
            if (profile == BluetoothProfile.A2DP) {
                Log.d("", "=== A2DP SERVICE DISCONNECTED ===");
                bluetoothA2dp = null;
            }
        }
    };

    public static boolean enableBluetooth()
    {        
        Log.d("", "=== BLUETOOTH ENABLED ===");
        return bluetoothAdapter.enable();
    }

    public static boolean disableBluetooth()
    {       
        Log.d("", "=== BLUETOOTH ENABLED ===");
        return bluetoothAdapter.disable();
    }

    private static void resetAudioManager(AudioManager audioManager)
    {
        if (audioManager != null) {
            audioManager.setMode(AudioManager.MODE_NORMAL);
            audioManager.stopBluetoothSco();
            audioManager.setBluetoothScoOn(false);
            audioManager.setSpeakerphoneOn(false);
            audioManager.setWiredHeadsetOn(false);
        }
    }

    public void registerBroadcastReceiver(Context context)
    {
        mainContext = context;
        BroadcastReceiver broadcastReceiver = new BroadcastReceiver() {
            public void onReceive(Context context, Intent intent) {
                String action = intent.getAction();
                if (action.equals(BluetoothDevice.ACTION_BOND_STATE_CHANGED)) {
                    bluetoothDevice = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                    bluetoothAdapter.getProfileProxy(context, btServiceListener, BluetoothProfile.A2DP);
                    Log.d("", "=== PAIRED: " + bluetoothDevice.getAddress() + " ===");
                    Method connect;
                    try {
                        connect = BluetoothA2dp.class.getDeclaredMethod("connect", BluetoothDevice.class);
                        connect.invoke(bluetoothA2dp, bluetoothDevice);
                    }
                    catch(Exception e) {
                    }
                }
            }
        };

        IntentFilter intentFilter = new IntentFilter(BluetoothDevice.ACTION_BOND_STATE_CHANGED);
        context.registerReceiver(broadcastReceiver, intentFilter);
    }

    public static String getPairedDevices()
    {
        String pairedDevices = new String();
        Set<BluetoothDevice> setPairedDevices = bluetoothAdapter.getBondedDevices();
        for (BluetoothDevice bluetoothDevice: setPairedDevices) {
            pairedDevices = pairedDevices.concat(bluetoothDevice.getAddress()).concat("|");
        }
        return pairedDevices;
    }

    public static boolean isBluetoothConnected()
    {
        return bluetoothAdapter.isEnabled() && (bluetoothAdapter.getProfileConnectionState(BluetoothProfile.A2DP) == BluetoothProfile.STATE_CONNECTED);
    }

    public static void unpairDevice(String macAddr)
    {
        BluetoothDevice device = bluetoothAdapter.getRemoteDevice(macAddr);
        try {
            Method method = device.getClass().getMethod("removeBond", (Class[]) null);
            method.invoke(device, (Object[]) null);
        }
        catch (Exception e) {
        }
        Log.d("", "=== UNPAIRED: " + device.getAddress() + " ===");
    }
}
