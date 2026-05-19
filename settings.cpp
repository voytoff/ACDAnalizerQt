#include "settings.h"
#include <QSettings>

Settings::Settings() : QSettings(Company, AppName) {}

int Settings::frequency(QVariant value) {
  if (value.isValid() && value.toInt() > 0) {
    this->setValue("frequency", value);
    emit propertyChanged("frequency", this->value("frequency"));
  }
  auto result = this->value("frequency").toInt();
  return result == 0 ? 10 : result;
}
AxisXType Settings::axisXType(QVariant value) {
  if (value.isValid() && value.toInt() > 0) {
    this->setValue("axisXType", value);
    emit propertyChanged("axisXType", this->value("axisXType"));
  }
  auto result = this->value("axisXType").toInt();
  return result == 0 ? AxisXType::Index : (AxisXType)this->value("axisXType").toInt();
}

QByteArray Settings::geometry(QByteArray value) {
  if (!value.isEmpty())
    this->setValue("geometry", value);
  return this->value("geometry").toByteArray();
}

QByteArray Settings::windowState(QByteArray value) {
  if (!value.isEmpty())
    this->setValue("windowState", value);
  return this->value("windowState").toByteArray();
}

QByteArray Settings::splitter(QByteArray value) {
  if (!value.isEmpty())
    this->setValue("splitter", value);
  return this->value("splitter").toByteArray();
}
