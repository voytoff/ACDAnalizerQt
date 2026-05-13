#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QObject>
#include <QUuid>
#include <QAbstractTableModel>

struct Channel
{
  QUuid oid;
  QString name;
};

class TreeModel : public QAbstractTableModel
{
  Q_OBJECT
public:
  explicit TreeModel(QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent) const override;
  int columnCount(const QModelIndex &parent) const override;
  QVariant data(const QModelIndex &index, int role) const override;

private:
  QList<Channel> channels;

signals:
};

#endif // TREEMODEL_H
