import 'package:flutter/material.dart';
import '../widgets/mode_switch.dart';
import '../widgets/motion_param.dart';
import '../widgets/system_status.dart';
import '../widgets/servo_table.dart';

class DashboardPage extends StatelessWidget {
  const DashboardPage({super.key});

  @override
  Widget build(BuildContext context) {
    final width = MediaQuery.of(context).size.width;

    return Center(
      child: ConstrainedBox(
        constraints: const BoxConstraints(maxWidth: 1200), // 限制整頁寬度
        child: GridView.count(
          padding: const EdgeInsets.all(10),
          crossAxisSpacing: 10,
          mainAxisSpacing: 10,
          crossAxisCount: width > 1000 ? 2 : 1, // 大螢幕 2 列，小螢幕自適應
          childAspectRatio: 1.35, // 調整比例讓卡片寬高合適
          children: const [
            ModeSwitch(),
            MotionParam(),
            SystemStatus(),
            ServoTable(),
          ],
        ),
      ),
    );
  }
}
