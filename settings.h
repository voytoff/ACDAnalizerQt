#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
const QString Company = "NIIHM";
const QString AppName = "ACDAnalizer";

class Settings : public QSettings
{
  Q_OBJECT
public:

public:
  explicit Settings();

public slots:
  int frequency(int value = 0);

signals:
};

#endif // SETTINGS_H
