import 'package:flutter/material.dart';

class CameraView extends StatelessWidget {
  const CameraView({super.key});

  @override
  Widget build(BuildContext context) {
    return Container(
      clipBehavior: Clip.hardEdge,
      decoration: BoxDecoration(
        borderRadius: BorderRadius.circular(12),
        color: Colors.black,
      ),
      child: Image.network(
        "http://192.168.4.1/stream",
        fit: BoxFit.cover,
        errorBuilder: (_, __, ___) => const Center(
          child: Text("Camera stream unavailable"),
        ),
      ),
    );
  }
}
