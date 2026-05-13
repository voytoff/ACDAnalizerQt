#ifndef TREEPAINTDELEGATE_H
#define TREEPAINTDELEGATE_H

#include <QStyledItemDelegate>

class TreePaintDelegate : public QStyledItemDelegate
{
public:
  void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // TREEPAINTDELEGATE_H
