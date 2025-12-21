import 'package:flutter/material.dart';

class WiFiSavedCard extends StatelessWidget {
  const WiFiSavedCard({super.key});

  @override
  Widget build(BuildContext context) {
    return Card(
      elevation: 3,

      child: Padding(
        padding: const EdgeInsets.all(16),

        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            const Text("已儲存 WiFi",
              style: TextStyle(fontSize: 20)),
            const SizedBox(height: 10),

            Expanded(
              child: ListView(
                children: const [
                  ListTile(
                    title: Text("尚無儲存 WiFi"),
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
