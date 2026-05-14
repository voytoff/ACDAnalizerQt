#include "channelitem.h"

ChannelItem::ChannelItem(ChannelBlock* channelBlock, ChannelItem *parentItem)
  : channelBlock(channelBlock) , parent(parentItem) {}

ChannelItem::ChannelItem(QChar symbol) : symbol(symbol) {}

void ChannelItem::appendChild(ChannelItem &child) {
  childItems.append(child);
}
/*
ChannelItem *ChannelItem::child(int row) {
  return row >= 0 && row < childCount() ? &childItems[row] : nullptr;
}
*/
int ChannelItem::childCount() const {
  return childItems.length();
}

int ChannelItem::columnCount() const {
  return channelBlock ? 3 : 1;
}

QVariant ChannelItem::data(int column) const {
  if (channelBlock) {
    switch (column) {
      case 0: return symbol;
      case 1: return channelBlock->name;
      case 2: return channelBlock->name;
      default: return QVariant();
      }
  } else return symbol;
}

int ChannelItem::row() const {
  if (parent == nullptr)
    return 0;
  const auto it = std::find_if(
    parent->childItems.begin(), parent->childItems.end(),
    [this](ChannelItem &treeItem) {
      return treeItem.channelBlock->channelID == this->channelBlock->channelID;
    });

  if (it != parent->childItems.cend())
    return std::distance(parent->childItems.begin(), it);
  Q_ASSERT(false);
  return -1;
}

ChannelItem *ChannelItem::parentItem() {
  return parent;
}
