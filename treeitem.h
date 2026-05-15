#ifndef TREEITEM_H
#define TREEITEM_H

#include "channelblock.h"
#include <QVariant>
#include <QList>

class TreeItem
{
public:
  explicit TreeItem(QVariantList data, TreeItem *parentItem = nullptr, ChannelBlock* channelBlock = nullptr);

  void appendChild(std::unique_ptr<TreeItem> &&child);

  TreeItem *child(int row);
  int childCount() const;
  int checkCount() const;
  QVector<ChannelBlock*> channels() const;
  int columnCount() const;
  QVariant data(int column) const;
  int row() const;
  TreeItem *parentItem();
  void toggle();

  bool root = false;
  bool checked = false;
  std::unique_ptr<ChannelBlock> channelBlock;
  QVector<TreeItem *> childs() const;

private:
  std::vector<std::unique_ptr<TreeItem>> childItems;
  QVariantList itemData;
  TreeItem *parent;
};

#endif // TREEITEM_H
