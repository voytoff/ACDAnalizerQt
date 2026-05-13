#include "treepaintdelegate.h"

void TreePaintDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  if (index.column() == 1 /*Name*/) {
    QModelIndex active = index.sibling(index.row(), 2 /*Active*/);
    QVariant bold = active.data(Qt::DisplayRole);
    if (bold.isValid() && bold.toBool()) {
      QStyleOptionViewItem opt = option;
      opt.font.setBold(true);
      initStyleOption(&opt, index);
      QStyledItemDelegate::paint(painter, opt, index);
      return;
    }
  }
  QStyledItemDelegate::paint(painter, option, index);
}
