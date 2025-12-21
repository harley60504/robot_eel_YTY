import 'package:flutter/material.dart';

class MotionParam extends StatelessWidget {
  const MotionParam({super.key});

  @override
  Widget build(BuildContext context) {
    return Card(
      elevation: 3,
      child: Padding(
        padding: const EdgeInsets.all(16),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            const Text("參數設定", style: TextStyle(fontSize: 20)),
            paramRow("頻率 (Hz)"),
            paramRow("振幅 (°)"),
            paramRow("λ"),
            paramRow("L"),
          ],
        ),
      ),
    );
  }

  Widget paramRow(String label) {
    return Padding(
      padding: const EdgeInsets.only(top: 8),
      child: Row(
        children: [
          SizedBox(width: 100, child: Text(label)),
          const SizedBox(width: 6),
          const Expanded(
            child: TextField(decoration: InputDecoration(border: OutlineInputBorder())),
          ),
          const SizedBox(width: 6),
          ElevatedButton(onPressed: (){}, child: const Text("設定")),
        ],
      ),
    );
  }
}
