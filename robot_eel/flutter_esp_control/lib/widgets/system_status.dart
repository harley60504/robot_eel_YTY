import 'package:flutter/material.dart';

class SystemStatus extends StatelessWidget {
  const SystemStatus({super.key});

  @override
  Widget build(BuildContext context) {
    return Card(
      elevation: 3,
      child: Padding(
        padding: const EdgeInsets.all(16),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: const [
            Text("系統狀態", style: TextStyle(fontSize: 20)),
            SizedBox(height: 10),
            Text("頻率：- Hz"),
            Text("振幅：- °"),
            Text("λ：-"),
            Text("L：-"),
            Text("回授權重：-"),
            Text("運作時間：-"),
          ],
        ),
      ),
    );
  }
}
