import 'package:flutter/material.dart';
import '../widgets/wifi_current.dart';
import '../widgets/wifi_scan.dart';
import '../widgets/wifi_saved.dart';

class WiFiPage extends StatelessWidget {
  const WiFiPage({super.key});

  @override
  Widget build(BuildContext context) {
    final width = MediaQuery.of(context).size.width;

    return Center(
      child: ConstrainedBox(
        constraints: const BoxConstraints(maxWidth: 1200),
        child: GridView.count(
          padding: const EdgeInsets.all(16),
          crossAxisSpacing: 10,
          mainAxisSpacing: 10,
          crossAxisCount: width > 900 ? 2 : 1,
          childAspectRatio: 1.3,
          children: const [
            WiFiCurrentCard(),
            WiFiScanCard(),
            WiFiSavedCard(),
          ],
        ),
      ),
    );
  }
}
