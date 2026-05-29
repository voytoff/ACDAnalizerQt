#include "parametertable.h"
#include "datablockarray.h"
#include <qstatusbar.h>

ParameterTable::ParameterTable(QVector<ChannelBlock *> channels, int frequency)
  : headers{"Индекс","Время"}
  , units{"сек","время"} {
  foreach (ChannelBlock* channelBlock, channels) {
    auto array = channelBlock->array(frequency);
    if (array) appendColumn(*array);
  }
}

ParameterTable::ParameterTable(QVector<MChannelBlock *> channels, int frequency)
  : headers{"Индекс","Время"}
  , units{"сек","время"} {
  foreach (MChannelBlock* channelBlock, channels) {
    auto array = channelBlock->array(frequency);
    if (array) appendColumn(*array);
  }
}

ParameterRow* ParameterTable::row(int index) {
  return index < table.length() ? const_cast<ParameterRow*>(&table.at(index)) : nullptr;
}

void ParameterTable::createIndex(QVector<Parameter *> data) {
  foreach (Parameter* parameter, data)
    table.append(ParameterRow(parameter->index, parameter->time, {}));
}

void ParameterTable::appendColumn(QString name, QVector<Parameter*> data, QString unit) {
  headers.append(name);
  units.append(unit);
  foreach (Parameter* parameter, data)
    appendRow(parameter);
}

void ParameterTable::appendColumn(DataBlockArray array) {
  headers.append(array.name);
  units.append(array.unit);
  int index = 0;
  foreach (Parameter parameter, array) {
    if (index >= table.length())
      table.append(ParameterRow(parameter.index, parameter.time, {}));
    table[index++].values.append(parameter.value);
  }
}

void ParameterTable::clear() {
  headers.clear();
  table.clear();
}

QString ParameterTable::tittle() {
  return headers.mid(2).join(",");
}

void ParameterTable::appendRow(Parameter *parameter) {
  auto it = std::find_if(table.begin(), table.end(), [parameter](const ParameterRow &item) {
    return item.index == parameter->index;// && item.time == parameter->time;
  });
  QVariant value = it != table.end() ? parameter->value : QVariant();
  (*it).values.append(value);
}
