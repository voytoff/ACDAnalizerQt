#ifndef CHANNELITEM_H
#define CHANNELITEM_H

#include <QVariant>
#include "channelblock.h"

class ChannelItem {
public:
  explicit ChannelItem(ChannelBlock* channelBlock, ChannelItem *parentItem = nullptr);
  explicit ChannelItem(QChar symbol);

  void appendChild(ChannelItem &child);
  ChannelItem *child(int row);
  int childCount() const;
  int columnCount() const;
  QVariant data(int column) const;
  int row() const;
  ChannelItem *parentItem();

  ChannelBlock* channelBlock;
  QChar symbol;
  bool checked;

  bool operator<(const ChannelItem &other) const {
    return channelBlock->name < other.channelBlock->name;
  }

private:
  QVector<ChannelItem> childItems;
  ChannelItem *parent = nullptr;
};

#endif // CHANNELITEM_H
