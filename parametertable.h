#ifndef PARAMETERTABLE_H
#define PARAMETERTABLE_H

#include "channelblock.h"
#include "mchannelblock.h"
#include "datablockarray.h"
#include "parameter.h"
#include "parameterrow.h"
#include <QStringList>

class ParameterTable
{
public:
  ParameterTable(QVector<ChannelBlock*> channels, int frequency);
  ParameterTable(QVector<MChannelBlock*> channels, int frequency);
  QStringList headers;
  QList<ParameterRow> table;
  ParameterRow *row(int index);
  void createIndex(QVector<Parameter*> data);
  void appendRow(Parameter* parameter);
  void appendColumn(QString name, QVector<Parameter*> data);
  void appendColumn(DataBlockArray array);
};

#endif // PARAMETERTABLE_H
