#include "treemodel.h"
#include "treeitem.h"
#include "channelblock.h"

#include <QMap>
#include <QList>
#include <QStringList>

using namespace Qt::StringLiterals;

TreeModel::TreeModel(const QString &data, QObject *parent)
  : QAbstractItemModel(parent)
  , rootItem(std::make_unique<TreeItem>(QVariantList{tr("Имя канала"), tr("Summary")})) {
  setupModelData(QStringView{data}.split(u'\n'), rootItem.get());
}
TreeModel::TreeModel(const ACDObject *acdObject, QObject *parent)
  : QAbstractItemModel(parent)
  , rootItem(std::make_unique<TreeItem>(QVariantList{tr("Имя канала"), tr("Summary")})) {
  this->acdObject = acdObject;
  setupModelData(acdObject, rootItem.get());
}
TreeModel::~TreeModel() = default;

int TreeModel::columnCount(const QModelIndex &parent) const {
  if (parent.isValid())
    return get(parent)->columnCount();
  return rootItem->columnCount();
}

QVariant TreeModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return {};

  if (role == Qt::DisplayRole) {
    const auto *item = get(index);
    return item->data(index.column());
  } else if (role == Qt::CheckStateRole) {
    const auto *item = get(index);
    if (item->childCount() == 0) return item->checked;
  }
  return {};
}

bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role) {
  if (role == Qt::CheckStateRole && index.column() == 0) {
    auto *item = get(index);
    item->toggle();
    layoutChanged();
    return true;
  }
  return false;
}

TreeItem* TreeModel::get(const QModelIndex &index) const {
  return static_cast<TreeItem*>(index.internalPointer());
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const {
  if (!index.isValid()) return Qt::NoItemFlags;

  Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
  const auto *item = get(index);
  auto p = item->data(index.column());

  if (item->childCount() == 0)
    flags |= Qt::ItemIsUserCheckable;
  return flags;
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation, int role) const {
  return orientation == Qt::Horizontal && role == Qt::DisplayRole
           ? rootItem->data(section) : QVariant{};
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const {
  if (!hasIndex(row, column, parent)) return {};

  TreeItem *parentItem = parent.isValid()
   ? static_cast<TreeItem*>(parent.internalPointer())
   : rootItem.get();

  if (auto *childItem = parentItem->child(row))
    return createIndex(row, column, childItem);
  return {};
}

QModelIndex TreeModel::parent(const QModelIndex &index) const {
  if (!index.isValid())
    return {};

  auto *childItem = get(index);
  TreeItem *parentItem = childItem->parentItem();

  return parentItem != rootItem.get()
           ? createIndex(parentItem->row(), 0, parentItem) : QModelIndex{};
}

int TreeModel::rowCount(const QModelIndex &parent) const {
  if (parent.column() > 0)
    return 0;

  const TreeItem *parentItem = parent.isValid()
   ? get(parent)
   : rootItem.get();

  return parentItem->childCount();
}

void TreeModel::setupModelData(const QList<QStringView> &lines, TreeItem *parent) {
  struct ParentIndentation
  {
    TreeItem *parent;
    qsizetype indentation;
  };

  QList<ParentIndentation> state{{parent, 0}};

  for (const auto &line : lines) {
    qsizetype position = 0;
    for ( ; position < line.length() && line.at(position).isSpace(); ++position) {
    }

    const QStringView lineData = line.sliced(position).trimmed();
    if (!lineData.isEmpty()) {
      // Read the column data from the rest of the line.
      const auto columnStrings = lineData.split(u'\t', Qt::SkipEmptyParts);
      QVariantList columnData;
      columnData.reserve(columnStrings.count());
      for (const auto &columnString : columnStrings)
        columnData << columnString.toString();

      if (position > state.constLast().indentation) {
        // Последний потомок текущего родителя теперь является новым родителем
        // если только у текущего родителя нет потомков.
        auto *lastParent = state.constLast().parent;
        if (lastParent->childCount() > 0)
          state.append({lastParent->child(lastParent->childCount() - 1), position});
      } else {
        while (position < state.constLast().indentation && !state.isEmpty())
          state.removeLast();
      }

      // Добавляем новый элемент в список дочерних элементов текущего родительского элемента.
      auto *lastParent = state.constLast().parent;
      lastParent->appendChild(std::make_unique<TreeItem>(columnData, lastParent));
    }
  }
}

void TreeModel::setupModelData(const ACDObject *acdObject, TreeItem *parent) {
  QMap<QChar, QList<ChannelBlock*>> groups;
  foreach (ChannelBlock *chanelBlock, *acdObject->channels) {
    groups[chanelBlock->name.at(0).toUpper()].append(chanelBlock);
  }
  auto keys =  groups.keys();
  std::sort(keys.begin(), keys.end());
  foreach (QChar key, keys) {
    auto symPtr = std::make_unique<TreeItem>(QVariantList{key}, parent);
    TreeItem* symItem = symPtr.get();
    auto list = groups[key];
    std::sort(list.begin(), list.end(), [](ChannelBlock* a, ChannelBlock* b) {return a->name < b->name;});
    foreach (ChannelBlock* item, list)
      symItem->appendChild(std::make_unique<TreeItem>(QVariantList{item->name}, symItem, item));
    parent->appendChild(std::move(symPtr));
  }
}

QVector<ChannelBlock*> TreeModel::channels() const {
  QVector<ChannelBlock*> dest;
  foreach (auto root, rootItem->childs()) {
    dest.append(root->channels());
  }
  return dest;
}
