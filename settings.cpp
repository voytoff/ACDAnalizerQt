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
ColorScheme Settings::colorScheme(QVariant value) {
  if (value.isValid() && value.toInt() > 0) {
    this->setValue("colorScheme", value);
    emit propertyChanged("colorScheme", this->value("colorScheme"));
  }
  auto result = this->value("colorScheme").toInt();
  return result == 0 ? ColorScheme::Light : (ColorScheme)this->value("colorScheme").toInt();
}
bool Settings::nativeDlg(QVariant value) {
  if (value.isValid()) {
    this->setValue("nativeDlg", value);
    emit propertyChanged("nativeDlg", this->value("nativeDlg"));
  }
  return this->value("nativeDlg").toBool();
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
