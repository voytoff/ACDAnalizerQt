#include "TableModel.h"
#include <QDataStream>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>
#include <QList>
#include <QMessageBox>

//TableModel::TableModel(QObject *parent): QAbstractTableModel(parent) {}

TableModel::TableModel(const ParameterTable *table, QObject *parent) : QAbstractTableModel(parent) {
  this->table = table;
}

int TableModel::rowCount(const QModelIndex &parent) const {
  return parent.isValid() ? 0 : table->table.size();
}

int TableModel::columnCount(const QModelIndex &parent) const {
  return parent.isValid() ? 0 : table->headers.size();
}

QVariant TableModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid()) return QVariant();
  if (index.row() >= table->table.size() || index.row() < 0) return QVariant();
  if (role != Qt::DisplayRole) return QVariant();
  if (index.column() == 0) return table->table.at(index.row()).index;
  else if (index.column() == 1) return table->table.at(index.row()).time;
  else return table->table.at(index.row()).values.at(index.column()-2);
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (role != Qt::DisplayRole) return QVariant();
  if (orientation != Qt::Horizontal) return QVariant();
  if (section >= table->headers.size()) return {};
  return table->headers.at(section);
}
