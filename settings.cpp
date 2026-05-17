#include "settings.h"
#include <QSettings>

Settings::Settings() : QSettings(Company, AppName) {}

int Settings::frequency(int value) {
  if (value > 0)
    this->setValue("geometry", value);
  return this->value("frequency").toInt();
}
