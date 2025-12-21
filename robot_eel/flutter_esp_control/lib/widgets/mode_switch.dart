import 'package:flutter/material.dart';

class ModeSwitch extends StatelessWidget {
  const ModeSwitch({super.key});

  @override
  Widget build(BuildContext context) {
    return Card(
      elevation: 3,
      child: Padding(
        padding: const EdgeInsets.all(16),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            const Text("模式切換", style: TextStyle(fontSize: 20)),
            const SizedBox(height: 12),
            Wrap(
              spacing: 12,
              children: [
                ElevatedButton(onPressed: (){}, child: const Text("Sin")),
                ElevatedButton(onPressed: (){}, child: const Text("CPG")),
                ElevatedButton(onPressed: (){}, child: const Text("Offset")),
              ],
            ),
            const SizedBox(height: 12),
            const Text("目前模式： -"),
          ],
        ),
      ),
    );
  }
}
