#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QStandardItemModel>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  app.setStyle("fusion");

  MainWindow window;
  window.show();
  return app.exec();
}
