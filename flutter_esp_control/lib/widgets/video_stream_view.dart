import 'dart:html' as html;
import 'package:flutter/material.dart';

class VideoStreamView extends StatefulWidget {
  final String url;

  const VideoStreamView({super.key, required this.url});

  @override
  State<VideoStreamView> createState() => _VideoStreamViewState();
}

class _VideoStreamViewState extends State<VideoStreamView> {
  late html.ImageElement _img;

  @override
  void initState() {
    super.initState();

    print("Starting stream: ${widget.url}");

    _img = html.ImageElement()
      ..src = widget.url
      ..style.border = "0"
      ..style.width = "100%"
      ..style.height = "100%"
      ..onError.listen((event) {
        print("Stream error!");
      });
  }

  @override
  Widget build(BuildContext context) {

    // bridge img html element -> Flutter Web widget
    return HtmlElementView(
      viewType: widget.url,
    );
  }
}
