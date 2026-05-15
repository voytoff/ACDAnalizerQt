#ifndef PARAMETERTABLE_H
#define PARAMETERTABLE_H

#include "parameter.h"
#include "parameterarray.h"
#include <QStringList>

class ParameterTable
{
public:
  ParameterTable();
  QStringList headers;
  QList<ParameterArray> table;
  void append(QString name, QVector<Parameter*> data);
};

#endif // PARAMETERTABLE_H
