#ifndef TREEMODEL_H
#define TREEMODEL_H

#include "acdobject.h"
#include "channelitem.h"
#include <QObject>
#include <QAbstractItemModel>

class TreeModel : public QAbstractItemModel
{
  Q_OBJECT
public:
  explicit TreeModel(QObject *parent = nullptr);
  explicit TreeModel(const QString &data, QObject *parent = nullptr);
  ~TreeModel() override;

  int rowCount(const QModelIndex &parent) const override;
  int columnCount(const QModelIndex &parent) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
  Qt::ItemFlags flags(const QModelIndex &index) const override;
  bool visible(int col) const;
  QVariant data(const QModelIndex &index, int role) const override;
  bool setData(const QModelIndex &index, const QVariant &value, int role) override;

  QModelIndex index(int row, int column, const QModelIndex &parent = {}) const override;
  QModelIndex parent(const QModelIndex &index) const override;

private:
  QList<Channel*> channels;
  Channel* rootItem = nullptr;

public slots:
  void init(ACDObject* source);

signals:
};

#endif // TREEMODEL_H
