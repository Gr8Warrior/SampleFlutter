package com.example.batterylevel;

import android.content.Context;
import android.content.ContextWrapper;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.wifi.WifiManager;
import android.net.wifi.WifiNetworkSuggestion;
import android.os.BatteryManager;
import android.os.Build;
import android.util.Log;
import android.widget.Toast;

import androidx.annotation.NonNull;

import java.util.ArrayList;
import java.util.List;

import io.flutter.embedding.android.FlutterActivity;
import io.flutter.embedding.engine.FlutterEngine;
import io.flutter.plugin.common.MethodChannel;

public class MainActivity extends FlutterActivity {
    private static final String CHANNEL = "samples.flutter.dev/battery";
    private static final String TAG = "Shailu";

    @Override
    public void configureFlutterEngine(@NonNull FlutterEngine flutterEngine) {
        super.configureFlutterEngine(flutterEngine);
        new MethodChannel(flutterEngine.getDartExecutor().getBinaryMessenger(), CHANNEL)
                .setMethodCallHandler((call, result) -> {
                    // This method is invoked in main thread
                    if (call.method.equals("getBatteryLevel")){
                        int batteryLevel = getBatteryLevel();
                        String text = call.argument("text");
                        Log.i("Shailu", "configureFlutterEngine: "+text);

                        if (batteryLevel != -1){
                            result.success(batteryLevel);
                        }else {
                            result.error("Unavailable", "Battery is not available",null);
                        }
                    }else if (call.method.equals("configurePSK")){

                        String ssid = call.argument("ssid");
                        String psk = call.argument("psk");

                        Log.i("Shailu", "ssid: "+ssid);
                        Log.i("Shailu", "psk: "+psk);


                        int batteryLevel = configurePSK(ssid,psk);

                        if (batteryLevel != -1){
                            result.success(batteryLevel);
                        }else {
                            result.error("Unavailable", "Battery is not available",null);
                        }
                    }else {
                        result.notImplemented();
                    }

                });
    }

    private int configurePSK(String ssid,String psk) {
        final WifiNetworkSuggestion suggestion1;
        int status = -1;
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.Q) {
            suggestion1 = new WifiNetworkSuggestion.Builder()
                    .setSsid(ssid)
                    .setWpa2Passphrase(psk)
                    .build();

            final List<WifiNetworkSuggestion> suggestionsList =
                    new ArrayList<WifiNetworkSuggestion>();
            suggestionsList.add(suggestion1);

            WifiManager wifiManager =
                    (WifiManager) getApplicationContext().getSystemService(Context.WIFI_SERVICE);
            status = wifiManager.addNetworkSuggestions(suggestionsList);
            if (status == 0) {
                Toast.makeText(this, "PSK network added", Toast.LENGTH_LONG).show();
                Log.i(TAG, "PSK network added: " + status);

            } else {
                Toast.makeText(this, "PSK network not added", Toast.LENGTH_LONG).show();
                Log.i(TAG, "PSK network not added: " + status);
            }
        }
        return  status;
    }

    private int getBatteryLevel() {
        int batteryLevel = -1;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            BatteryManager batteryManager = (BatteryManager) getSystemService(BATTERY_SERVICE);
            batteryLevel = batteryManager.getIntProperty(BatteryManager.BATTERY_PROPERTY_CAPACITY);
        } else {
            Intent intent = new ContextWrapper(getApplicationContext()).
                    registerReceiver(null, new IntentFilter(Intent.ACTION_BATTERY_CHANGED));
            batteryLevel = (intent.getIntExtra(BatteryManager.EXTRA_LEVEL, -1) * 100) /
                    intent.getIntExtra(BatteryManager.EXTRA_SCALE, -1);
        }

        return batteryLevel;
    }

}
