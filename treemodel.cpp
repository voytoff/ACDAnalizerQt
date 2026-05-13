#include "treemodel.h"

TreeModel::TreeModel(QObject *parent) : QAbstractTableModel(parent) {

}

int TreeModel::rowCount(const QModelIndex &parent) const {
  return parent.isValid() ? 0 : channels.size();
}

int TreeModel::columnCount(const QModelIndex &parent) const {
  return parent.isValid() ? 0 : 2;
}

QVariant TreeModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return QVariant();

  if (index.row() >= channels.size() || index.row() < 0)
    return QVariant();

  if (role == Qt::DisplayRole) {
    const auto &sensor = channels.at(index.row());

    switch (index.column()) {
    case 0:
      return sensor.oid;
    case 1:
      return sensor.name;
    default:
      break;
    }
  }
  return QVariant();
}
