#include "parametertable.h"
#include "datablockarray.h"

ParameterTable::ParameterTable(): headers{"Индекс","Время"} {}

void ParameterTable::createIndex(QVector<Parameter *> data) {
  foreach (Parameter* parameter, data)
    table.append(ParameterRow(parameter->index, parameter->time, {}));
}

void ParameterTable::appendColumn(QString name, QVector<Parameter*> data) {
  headers.append(name);
  foreach (Parameter* parameter, data)
    appendRow(parameter);
}

void ParameterTable::appendColumn(DataBlockArray array) {
  headers.append(array.name);
  int index = 0;
  foreach (Parameter parameter, array) {
    if (index >= table.length())
      table.append(ParameterRow(parameter.index, parameter.time, {}));
    //auto row = table[index].values;
    //if (row.length() == 0)
    //  row.append({parameter.index, parameter.time});
    table[index++].values.append(parameter.value);
  }
}

void ParameterTable::appendRow(Parameter *parameter) {
  auto it = std::find_if(table.begin(), table.end(), [parameter](const ParameterRow &item) {
    return item.index == parameter->index;// && item.time == parameter->time;
  });
  QVariant value = it != table.end() ? parameter->value : QVariant();
  (*it).values.append(value);
}
