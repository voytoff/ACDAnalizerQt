// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include "acdobject.h"

class ChannelArray;
class TreeItem;

class TreeModel : public QAbstractItemModel {
  Q_OBJECT

public:
  Q_DISABLE_COPY_MOVE(TreeModel)

  explicit TreeModel(const QString &data, QObject *parent = nullptr);
  explicit TreeModel(const ACDObject *acdObject, QObject *parent = nullptr);
  ~TreeModel() override;

  QVariant data(const QModelIndex &index, int role) const override;
  Qt::ItemFlags flags(const QModelIndex &index) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
  QModelIndex index(int row, int column, const QModelIndex &parent = {}) const override;
  QModelIndex parent(const QModelIndex &index) const override;
  int rowCount(const QModelIndex &parent = {}) const override;
  int columnCount(const QModelIndex &parent = {}) const override;
  bool setData(const QModelIndex &index, const QVariant &value, int role) override;
  TreeItem* get(const QModelIndex &index) const;

private:
  static void setupModelData(const QList<QStringView> &lines, TreeItem *parent);
  static void setupModelData(const ACDObject *acdObject, TreeItem *parent);

  std::unique_ptr<TreeItem> rootItem;
  const ACDObject *acdObject;

public slots:
  QVector<ChannelBlock*> channels() const;
};

#endif // TREEMODEL_H
