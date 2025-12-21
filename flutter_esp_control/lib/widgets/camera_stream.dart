import 'dart:typed_data';
import 'dart:html';
import 'package:flutter/material.dart';

class CameraStreamWS extends StatefulWidget {
  final String wsUrl;
  const CameraStreamWS({super.key, required this.wsUrl});

  @override
  State<CameraStreamWS> createState() => _CameraStreamWSState();
}

class _CameraStreamWSState extends State<CameraStreamWS> {
  WebSocket? socket;
  Uint8List? frame;

  @override
  void initState() {
    super.initState();

    socket = WebSocket(widget.wsUrl);
    socket!.binaryType = 'arraybuffer';

    socket!.onMessage.listen((event) {
      final data = event.data as ByteBuffer;
      setState(() => frame = Uint8List.view(data));
    });
  }

  @override
  void dispose() {
    socket?.close();
    super.dispose();
  }

  @override
  @override
  Widget build(BuildContext context) {
    return Card(
      elevation: 4,
      clipBehavior: Clip.hardEdge,       // 確保畫面不溢出
      child: AspectRatio(
        aspectRatio: 4 / 3,              // 640x480 = 4:3
        child: frame == null
            ? const Center(child: Text("Waiting…"))
            : Image.memory(
                frame!,
                fit: BoxFit.cover,       // 剛好填滿卡片
                gaplessPlayback: true,
              ),
      ),
    );
  }
}