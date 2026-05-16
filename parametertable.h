#ifndef PARAMETERTABLE_H
#define PARAMETERTABLE_H

#include "parameter.h"
#include "parameterrow.h"
#include <QStringList>

class ParameterTable
{
public:
  ParameterTable();
  QStringList headers;
  QList<ParameterRow> table;
  void createIndex(QVector<Parameter*> data);
  void appendColumn(QString name, QVector<Parameter*> data);
  void appendRow(Parameter* parameter);
};

#endif // PARAMETERTABLE_H
