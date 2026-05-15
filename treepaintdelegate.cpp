#include "treepaintdelegate.h"

void TreePaintDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
  if (index.column() == 0) {
    //QModelIndex active = index.sibling(index.row(), 0 /*checked*/);
    auto item = get(index);
    QVariant bold = item->checkCount() > 0;
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

TreeItem *TreePaintDelegate::get(const QModelIndex &index) const {
  return static_cast<TreeItem*>(index.internalPointer());
}
