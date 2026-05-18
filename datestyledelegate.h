#ifndef DATESTYLEDELEGATE_H
#define DATESTYLEDELEGATE_H

#include <QStyledItemDelegate>
#include <qdatetime.h>

class DateStyleDelegate : public QStyledItemDelegate {
public:
  using QStyledItemDelegate::QStyledItemDelegate;
  QString displayText(const QVariant &value, const QLocale &locale) const override;
};

#endif // DATESTYLEDELEGATE_H
