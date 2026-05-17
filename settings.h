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
  int frequency(QVariant value = 0);
  QByteArray geometry(QByteArray value = QByteArray());
  QByteArray windowState(QByteArray value = QByteArray());

signals:
  void frequencyChanged(int value);

};

#endif // SETTINGS_H
