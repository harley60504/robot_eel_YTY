import 'dart:convert';
import 'package:http/http.dart' as http;
import '../config.dart';

class EspApi {

  static Future<Map<String, dynamic>> getStatus() async {
    final res = await http.get(
      Uri.parse("${ApiConfig.espBaseUrl}/status")
    );
    return jsonDecode(res.body);
  }

  static Future<String> setMode(int mode) async {
    final res = await http.get(
      Uri.parse("${ApiConfig.espBaseUrl}/setMode?m=$mode")
    );
    return res.body;
  }
}
