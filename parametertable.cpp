#include "parametertable.h"

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

void ParameterTable::appendRow(Parameter *parameter) {
  auto it = std::find_if(table.begin(), table.end(), [parameter](const ParameterRow &item) {
    return item.index == parameter->index;// && item.time == parameter->time;
  });
  QVariant value = it != table.end() ? parameter->value : QVariant();
  (*it).values.append(value);
}
