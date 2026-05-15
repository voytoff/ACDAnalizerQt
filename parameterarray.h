#ifndef PARAMETERARRAY_H
#define PARAMETERARRAY_H

#include <QDateTime>

class ParameterArray
{
public:
  //ParameterArray();
  ParameterArray(double index, QDateTime time, QVariantList values);

  bool operator<(const ParameterArray &other) const {
    return time < other.time;
  }

  QDateTime time;
  double index;
  QVariantList values;
};

#endif // PARAMETERARRAY_H
