import 'package:flutter/material.dart';

class ServoTable extends StatelessWidget {
  const ServoTable({super.key});

  @override
  Widget build(BuildContext context) {
    return Card(
      elevation: 3,
      child: SizedBox(
        width: 600,
        child: DataTable(
          columns: const [
            DataColumn(label: Text("ID")),
            DataColumn(label: Text("Target")),
            DataColumn(label: Text("Actual")),
            DataColumn(label: Text("Error")),
          ],
          rows: const [
            DataRow(cells: [
              DataCell(Text("1")),
              DataCell(Text("20")),
              DataCell(Text("18")),
              DataCell(Text("2")),
            ]),
          ],
        ),
      ),
    );
  }
}
