import 'dart:io';

import 'package:flutter/material.dart';
import 'package:flutter/services.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  // This widget is the root of your application.
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Flutter Demo',
      theme: ThemeData(
        // This is the theme of your application.
        //
        // Try running your application with "flutter run". You'll see the
        // application has a blue toolbar. Then, without quitting the app, try
        // changing the primarySwatch below to Colors.green and then invoke
        // "hot reload" (press "r" in the console where you ran "flutter run",
        // or simply save your changes to "hot reload" in a Flutter IDE).
        // Notice that the counter didn't reset back to zero; the application
        // is not restarted.
        primarySwatch: Colors.blue,
      ),
      home: const MyHomePage(title: 'Flutter Demo Home Page'),
    );
  }
}

class MyHomePage extends StatefulWidget {
  const MyHomePage({super.key, required this.title});

  // This widget is the home page of your application. It is stateful, meaning
  // that it has a State object (defined below) that contains fields that affect
  // how it looks.

  // This class is the configuration for the state. It holds the values (in this
  // case the title) provided by the parent (in this case the App widget) and
  // used by the build method of the State. Fields in a Widget subclass are
  // always marked "final".

  final String title;

  @override
  State<MyHomePage> createState() => _MyHomePageState();
}

class _MyHomePageState extends State<MyHomePage> {
  static const platform = MethodChannel('samples.flutter.dev/battery');

//Get battery level
  String _batteryLevel = 'Unknown battery level';

  Future<void> _getBatteryLevel() async {
    String batteryLevel;
    String text = "whatever";

    try {
      final int result =
          await platform.invokeMethod('getBatteryLevel', {"text": text});
      batteryLevel = 'Battery level at $result % .';
    } on PlatformException catch (e) {
      batteryLevel = "failed to get battery level: '${e.message}'.";
    }

    setState(() {
      _batteryLevel = batteryLevel;
    });
  }

  Future<void> _checkCmd() async {
    String batteryLevel;
    String text = "whatever";

    try {
      //certutil.exe -f -user -p 123456 -importpfx C:\openssl-certs\domain.pfx
      var result = await Process.run(
          'C:\\Windows\\System32\\netsh.exe', ['wlan', 'show', 'profiles']);

//Adding User Certificate
      // var result = await Process.run('C:\\Windows\\System32\\certutil.exe', [
      //   '-f',
      //   '-user',
      //   '-p',
      //   '123456',
      //   '-importpfx',
      //   'C:\\openssl-certs\\domain.pfx'
      // ]);

//Adding root certificate
//Getting Error for not using Administrator permissions
      // var result = await Process.run('C:\\Windows\\System32\\certutil.exe',
      //     ['-addstore', '-f', 'Root', 'C:\\openssl-certs\\domain.crt']);
      batteryLevel = (result.stdout);

//Connecting to ssid
      // var result = await Process.run('C:\\Windows\\System32\\netsh.exe',
      //     ['wlan', 'connect', 'name=TheWalnut', 'ssid=TheWalnut']);
      // batteryLevel = (result.stdout);
      //batteryLevel = 'Battery level at $result % .';
    } on PlatformException catch (e) {
      batteryLevel = "failed to get battery level: '${e.message}'.";
    }

    setState(() {
      _batteryLevel = batteryLevel;
    });
  }

  Future<void> _configurePSK() async {
    String batteryLevel;
    String ssid = "TYPE_YOUR_SSID";
    String psk = "TYPE_PSK";

    try {
      final int result = await platform
          .invokeMethod('configurePSK', {"ssid": ssid, "psk": psk});
      batteryLevel = 'SSID profile configured = $result % .';
    } on PlatformException catch (e) {
      batteryLevel = "failed to get battery level: '${e.message}'.";
    }

    setState(() {
      _batteryLevel = batteryLevel;
    });
  }

  @override
  @override
  Widget build(BuildContext context) {
    return Material(
      child: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.spaceEvenly,
          children: [
            ElevatedButton(
              onPressed: _getBatteryLevel,
              child: const Text('Configure PSK'),
            ),
            ElevatedButton(
              onPressed: _checkCmd,
              child: const Text('Configure Test'),
            ),
            Text(_batteryLevel),
          ],
        ),
      ),
    );
  }
}
