#include "settings.h"
#include <QSettings>

Settings::Settings() : QSettings(Company, AppName) {}

int Settings::frequency(QVariant value) {
  auto frequency = value.toInt();
  if (frequency > 0) {
    this->setValue("geometry", frequency);
    /*emit*/ frequencyChanged(frequency);
    qDebug() << "frequencyChanged" << frequency;
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
