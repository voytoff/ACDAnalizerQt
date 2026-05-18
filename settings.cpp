#include "settings.h"
#include <QSettings>

Settings::Settings() : QSettings(Company, AppName) {}

int Settings::frequency(QVariant value) {
  if (value.isValid() && value.toInt() > 0) {
    this->setValue("frequency", value);
    emit propertyChanged("frequency", this->value("frequency"));
  }
  return this->value("frequency").toInt();
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
