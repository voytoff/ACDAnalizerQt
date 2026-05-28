#include "treeitem.h"

TreeItem::TreeItem(QVariantList data, TreeItem *parent, ChannelBlock* channelBlock)
  : itemData(std::move(data))
  , parent(parent)
  , channelBlock(channelBlock) {
  root = channelBlock == nullptr;
}
TreeItem::TreeItem(QVariantList data, TreeItem *parent, MChannelBlock* channelBlock)
  : itemData(std::move(data))
  , parent(parent)
  , mchannelBlock(channelBlock) {
  root = channelBlock == nullptr;
}

void TreeItem::appendChild(std::unique_ptr<TreeItem> &&child) {
  childItems.push_back(std::move(child));
}

TreeItem *TreeItem::child(int row) {
  return row >= 0 && row < childCount() ? childItems.at(row).get() : nullptr;
}

int TreeItem::childCount() const {
  return int(childItems.size());
}

int TreeItem::checkCount() const {
  int count = 0;
  std::for_each(childItems.cbegin(), childItems.cend(), [&count](const std::unique_ptr<TreeItem> &treeItem) {
    if (treeItem->checked) ++count;
  });
  return count;
}

QVector<ChannelBlock*> TreeItem::channels() const {
  if (!root) return {};
  QVector<ChannelBlock*> items;
  std::for_each(childItems.cbegin(), childItems.cend(), [&items](const std::unique_ptr<TreeItem> &treeItem) {
    if (treeItem->checked)
      items.append(treeItem.get()->channelBlock.get());
  });
  return items;
}
QVector<MChannelBlock*> TreeItem::mchannels() const {
  if (!root) return {};
  QVector<MChannelBlock*> items;
  std::for_each(childItems.cbegin(), childItems.cend(), [&items](const std::unique_ptr<TreeItem> &treeItem) {
    if (treeItem->checked)
      items.append(treeItem.get()->mchannelBlock.get());
  });
  return items;
}


int TreeItem::columnCount() const {
  return 1;
}

QVariant TreeItem::data(int column) const {
  return itemData.value(column);
}

TreeItem *TreeItem::parentItem() {
  return parent;
}

void TreeItem::toggle() {
  if (channelBlock.get()) {
    checked = !checked;
  } else if (mchannelBlock.get()) {
    checked = !checked;
  }
}

QVector<TreeItem*> TreeItem::childs() const {
  if (!root) return {};
  QVector<TreeItem*> dest;
  for (const auto& ptr : childItems) {
    if (ptr) dest.append(ptr.get());
  }
  return dest;
}

int TreeItem::row() const {
  if (parent == nullptr)
    return 0;
  const auto it = std::find_if(
    parent->childItems.cbegin(), parent->childItems.cend(),
     [this](const std::unique_ptr<TreeItem> &treeItem) {
       return treeItem.get() == this;
    });

  if (it != parent->childItems.cend())
    return std::distance(parent->childItems.cbegin(), it);
  Q_ASSERT(false); // should not happen
  return -1;
}
