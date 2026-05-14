#include "channelitem.h"

Channel::Channel(QVariantList data, Channel *parentItem)
  : itemData(data)
  , _parentItem(parentItem)
{}

void Channel::appendChild(Channel &child) {
  childItems.append(child);
}
/*
Channel *Channel::child(int row) {
  return row >= 0 && row < childCount() ? &childItems[row] : nullptr;
}
*/
int Channel::childCount() const {
  return childItems.length();
}

int Channel::columnCount() const {
  return itemData.count();
}

QVariant Channel::data(int column) const {
  return itemData.value(column);
}

int Channel::row() const {
  if (_parentItem == nullptr)
    return 0;
  const auto it = std::find_if(
    _parentItem->childItems.begin(), _parentItem->childItems.end(),
    [this](Channel &treeItem) {
      return &treeItem == this;
    });

  if (it != _parentItem->childItems.cend())
    return std::distance(_parentItem->childItems.begin(), it);
  Q_ASSERT(false);
  return -1;
}

Channel *Channel::parentItem() {
  return _parentItem;
}
