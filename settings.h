#ifndef SETTINGS_H
#define SETTINGS_H

#include "ACD2File_global.h"
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
  AxisXType axisXType(QVariant value = 0);
  QByteArray geometry(QByteArray value = QByteArray());
  QByteArray windowState(QByteArray value = QByteArray());
  QByteArray splitter(QByteArray value = 0);
  ColorScheme colorScheme(QVariant value = 0);

signals:
  void propertyChanged(QString name, QVariant value);

};

#endif // SETTINGS_H
