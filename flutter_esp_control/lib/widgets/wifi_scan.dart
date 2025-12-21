import 'package:flutter/material.dart';

class WiFiScanCard extends StatelessWidget {
  const WiFiScanCard({super.key});

  @override
  Widget build(BuildContext context) {
    return Card(
      elevation: 3,
      child: Padding(
        padding: const EdgeInsets.all(16),

        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            const Text(
              "附近 WiFi 掃描",
              style: TextStyle(fontSize: 20)),
            const SizedBox(height: 10),

            ElevatedButton(onPressed: (){}, child: const Text("掃描 WiFi")),

            const SizedBox(height: 10),

            Expanded(
              child: ListView(
                children: const [
                  ListTile(
                    title: Text("⏳ 掃描結果..."),
                  ),
                ],
              ),
            ),
          ],
        ),
      ),
    );
  }
}
