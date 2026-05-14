#include "treemodel.h"
#include <QMap>

TreeModel::TreeModel(QObject *parent) : QAbstractItemModel(parent) {}
TreeModel::~TreeModel() = default;

int TreeModel::rowCount(const QModelIndex &parent) const {
  //return 7;
  return parent.isValid() ? 0 : channels.length();
  if (parent.column() > 0)
    return 0;

  const Channel *parentItem = parent.isValid()
                                 ? static_cast<const Channel*>(parent.internalPointer())
                                 : rootItem;
  return parentItem->childCount();
}

int TreeModel::columnCount(const QModelIndex &parent) const {
  return parent.isValid() ? 0 : 3;
  if (parent.isValid())
    return static_cast<Channel*>(parent.internalPointer())->columnCount();
  return rootItem ? rootItem->columnCount() : 2;
}

bool TreeModel::visible(int col) const {
  switch (col) {
  case 0:
    return true;
  case 1:
    return true;
  case 2:
    return false;
  default:
    break;
  }
  return false;
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (role != Qt::DisplayRole)
    return QVariant();

  if (orientation == Qt::Horizontal) {
    switch (section) {
    case 0:
      return tr("#");
    case 1:
      return tr("Название");
    case 2:
      return tr("ID");
    default:
      break;
    }
  }
  return QVariant();
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const {
  //return index.isValid() ? QAbstractItemModel::flags(index) : Qt::ItemFlags(Qt::NoItemFlags);

  if (!index.isValid()) return Qt::NoItemFlags;

  Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
  // Only make column 0 checkable
  if (index.column() == 0) {
    flags |= Qt::ItemIsUserCheckable;
  }
  return flags;

}

QVariant TreeModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || role != Qt::DisplayRole)
    return {};

  const auto &channel = channels.at(index.row());
  return QVariant("text");

  const auto *item = static_cast<const Channel*>(index.internalPointer());
  return item->data(index.column());
  /*
  if (!index.isValid())
    return QVariant();

  if (index.row() >= channels.size() || index.row() < 0)
    return QVariant();

  if (role == Qt::DisplayRole) {
    const auto &sensor = channels.at(index.row());

    switch (index.column()) {
    //case 0:
    //  return sensor.checked;
    case 1:
      return sensor->name;
    case 2:
      return sensor->channelID;
    default:
      break;
    }
  } else if (role == Qt::CheckStateRole) {
    if (index.column() == 0)
      return channels.at(index.row())->checked;
  }
  return QVariant();
*/
}

bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role) {
  if (role == Qt::CheckStateRole && index.column() == 0) {
    auto sensor = channels.at(index.row());
    sensor->checked = !sensor->checked;// (value.toInt() == Qt::Checked);
    //dataChanged(index, index, {Qt::CheckStateRole});
    layoutChanged();
    return true;
  }
  return false;
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const {
  /*
  if (!hasIndex(row, column, parent))
    return {};

  Channel *parentItem = parent.isValid()
                           ? static_cast<Channel*>(parent.internalPointer())
                           : rootItem;

  if (auto *childItem = parentItem->child(row))
    return createIndex(row, column, childItem);
  return {};*/
  return createIndex(row, column);
}

QModelIndex TreeModel::parent(const QModelIndex &index) const {
  //if (!index.isValid())
    return {};

  auto *childItem = static_cast<Channel*>(index.internalPointer());
  Channel *parentItem = childItem->parentItem();

  return parentItem != rootItem
           ? createIndex(parentItem->row(), 0, parentItem) : QModelIndex{};
}

//void TreeModel::setupModelData(const QList<QStringView> &lines, Channel *parent) { }

void TreeModel::init(ACDObject *source) {
  QMap<QChar, QList<ChannelBlock*>> grouped;
  foreach (ChannelBlock* channelBlock, *source->channels) {
    grouped[channelBlock->name[0].toUpper()].append(channelBlock);
  }

  channels.clear();
  for (QChar group : grouped.keys()) {
    QList<ChannelBlock*> list = grouped[group];
    std::sort(list.begin(), list.end(), [](ChannelBlock* a, ChannelBlock* b) { return a->name < b->name; });
    auto root = new Channel(QVariantList{group});
    foreach (ChannelBlock* item, list) {
      auto child = Channel(QVariantList{group, item->channelID, item->name, false}, root);
      root->appendChild(child);
    }
    channels.append(root);
  }
  std::sort(channels.begin(), channels.end());
  layoutChanged();
}