import 'package:flutter/material.dart';
import '../api/esp_api.dart';   // 你說這個是 WebSocket 版，所以保留它

class CameraControlPanel extends StatefulWidget {
  const CameraControlPanel({super.key});

  @override
  State<CameraControlPanel> createState() => _CameraControlPanelState();
}

class _CameraControlPanelState extends State<CameraControlPanel> {
  
  String resolution = "SVGA";
  double quality = 10;

  /// ESP32 cam 的 frame size 對照表
  final Map<String, int> frameSizeMap = {
    "UXGA": 11,
    "SXGA": 10,
    "SVGA": 7,
    "VGA": 6,
  };

  /// 套用解析度
  void applyResolution(String value){
    setState(() => resolution = value);

    WsControlApi.setCameraParam({
      "framesize": frameSizeMap[value]!,
    });
  }

  /// 套用畫質
  void applyQuality(double v){
    setState(() => quality = v);

    WsControlApi.setCameraParam({
      "quality": v.toInt(),
    });
  }

  @override
  Widget build(BuildContext context) {
    return Card(
      elevation: 3,
      child: Padding(
        padding: const EdgeInsets.all(16),

        child: Column(
          mainAxisSize: MainAxisSize.min,
          crossAxisAlignment: CrossAxisAlignment.start,

          children: [

            const Text("相機控制", style: TextStyle(fontSize: 20)),
            const SizedBox(height: 18),

            const Text("解析度 Resolution"),
            DropdownButton<String>(
              value: resolution,
              items: frameSizeMap.keys
                  .map((v) => DropdownMenuItem(value: v, child: Text(v)))
                  .toList(),
              onChanged: (v) => applyResolution(v!),
            ),

            const SizedBox(height: 20),

            const Text("JPEG Quality"),
            Slider(
              value: quality,
              min: 5,
              max: 60,
              divisions: 55,
              label: quality.toInt().toString(),
              onChanged: applyQuality,
            ),
          ],
        ),
      ),
    );
  }
}
