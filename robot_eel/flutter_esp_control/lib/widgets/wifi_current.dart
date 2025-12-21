import 'package:flutter/material.dart';

class WiFiCurrentCard extends StatelessWidget {
  const WiFiCurrentCard({super.key});

  @override
  Widget build(BuildContext context) {
    return Card(
      elevation: 3,
      child: Padding(
        padding: const EdgeInsets.all(16),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: const [
            Text("目前連線 WiFi",
              style: TextStyle(fontSize: 20)),
            SizedBox(height: 12),
            Text("SSID：-"),
            Text("訊號：-"),
            Text("IP：-"),
          ],
        ),
      ),
    );
  }
}
