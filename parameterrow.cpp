#include "parameterrow.h"

ParameterRow::ParameterRow(double index, QDateTime time, QVariantList values) : index(index), time(time), values(values) {}

QVariant ParameterRow::value(int index) {
  if (index < 0 || index >= count()) return QVariant();
  if (index == 0) return this->index;
  else if (index == 1) return this->time;
  else return values.at(index - 2);
}

int ParameterRow::count() const {
  return values.length() + 2;
}


