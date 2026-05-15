#include "parametertable.h"

ParameterTable::ParameterTable() {
  headers.append({"Индекс","Время"});
}

void ParameterTable::append(QString name, QVector<Parameter*> data) {
  headers.append(name);
  foreach (Parameter* p, data) {
    table.append(ParameterArray(p->index, p->time, {p->value}));
  }
}
