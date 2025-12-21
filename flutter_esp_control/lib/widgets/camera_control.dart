import 'package:flutter/material.dart';

class CameraControlPanel extends StatelessWidget {
  const CameraControlPanel({super.key});

  @override
  Widget build(BuildContext context) {
    return Card(
      elevation: 3,
      child: Padding(
        padding: const EdgeInsets.all(16),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            const Text("相機控制", style: TextStyle(fontSize: 20)),
            const SizedBox(height: 12),

            DropdownButton(
              value: "SVGA",
              items: const [
                DropdownMenuItem(value: "UXGA", child: Text("UXGA")),
                DropdownMenuItem(value: "SXGA", child: Text("SXGA")),
                DropdownMenuItem(value: "SVGA", child: Text("SVGA")),
                DropdownMenuItem(value: "VGA", child: Text("VGA")),
              ],
              onChanged: (v){},
            ),

            const SizedBox(height: 12),
            Slider(
              value: 10,
              min: 5,
              max: 60,
              onChanged: (x){},
            ),
          ],
        ),
      ),
    );
  }
}
