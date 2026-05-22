#ifndef PARAMETERARRAY_H
#define PARAMETERARRAY_H

#include <QDateTime>

class ParameterRow
{
public:
  ParameterRow(double index, QDateTime time, QVariantList values);

  bool operator<(const ParameterRow &other) const {
    return time < other.time;
  }

  QDateTime time;
  double index;
  QVariantList values;
  QVariant value(int index);
  int count() const;
};

#endif // PARAMETERARRAY_H
