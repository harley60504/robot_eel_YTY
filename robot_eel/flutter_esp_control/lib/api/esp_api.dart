import 'dart:convert';
import 'package:web_socket_channel/web_socket_channel.dart';
import '../config.dart';

class WsControlApi {
  
  static WebSocketChannel? _ws;

  static ensureConnect(){
    _ws ??= WebSocketChannel.connect(Uri.parse(ApiConfig.wsControlUrl));
  }

  static void send(Map<String, dynamic> body){
    ensureConnect();
    final text = jsonEncode(body);
    _ws!.sink.add(text);
  }

  static setMode(int mode){
    send({"cmd":"set_mode","value":mode});
  }

  static setServo(int angle){
    send({"cmd":"servo_angle","value":angle});
  }

  static setCameraParam(Map<String, dynamic> param){
    send({"cmd":"camera_param", ...param});
  }
}
