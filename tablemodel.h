#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include "parametertable.h"
#include <QAbstractTableModel>

class TableModel : public QAbstractTableModel {
  Q_OBJECT
public:
  explicit TableModel(const ParameterTable *table = nullptr, QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
  void addMapping(const QString &color, const QRect &area);
  QString unit(int section);

private:
  const ParameterTable *table;
  QMultiHash<QString, QRect> mapping;
};

#endif // TABLEMODEL_H
