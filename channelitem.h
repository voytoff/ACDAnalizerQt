#ifndef CHANNELITEM_H
#define CHANNELITEM_H

#include <QVariant>

class Channel {
public:
  explicit Channel(QVariantList data, Channel *parentItem = nullptr);

  void appendChild(Channel &child);
  //Channel *child(int row);
  int childCount() const;
  int columnCount() const;
  QVariant data(int column) const;
  int row() const;
  Channel *parentItem();

  QChar letter;
  int channelID;
  QString name;
  bool checked;

  bool operator<(const Channel &other) const {
    return name < other.name;
  }

private:
  QVector<Channel> childItems;
  QVariantList itemData; // letter, channelID, name, checked
  Channel *_parentItem = nullptr;
};

#endif // CHANNELITEM_H
