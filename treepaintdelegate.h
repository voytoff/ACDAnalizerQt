#ifndef TREEPAINTDELEGATE_H
#define TREEPAINTDELEGATE_H

#include "treeitem.h"
#include <QStyledItemDelegate>

class TreePaintDelegate : public QStyledItemDelegate
{
public:
  void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
  TreeItem* get(const QModelIndex &index) const;
};

#endif // TREEPAINTDELEGATE_H
