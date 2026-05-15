#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include "parametertable.h"
#include <QAbstractTableModel>

class TableModel : public QAbstractTableModel {
  Q_OBJECT
public:
  TableModel(const ParameterTable *table, QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent) const override;
  int columnCount(const QModelIndex &parent) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
  const ParameterTable *table;
};

#endif // TABLEMODEL_H
