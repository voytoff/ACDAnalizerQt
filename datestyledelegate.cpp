#include "datestyledelegate.h"

QString DateStyleDelegate::displayText(const QVariant &value, const QLocale &locale) const {
  if (value.typeId() == QMetaType::QDateTime)
    return value.toDateTime().toString("dd.MM.yyyy HH:mm:ss.zz");
  return QStyledItemDelegate::displayText(value, locale);
}