#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QStandardItemModel>
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  app.setStyle("fusion");

  QTranslator qtTranslator;
  QString lang = QLocale::system().name();
  // Загружаем системный перевод (ищет файлы вида qt_ru.qm)
  if (qtTranslator.load("qt_" + lang, QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
    app.installTranslator(&qtTranslator);
  }
  // Здесь загружается перевод для вашего собственного приложения
  QTranslator myTranslator;
  if (myTranslator.load("acdanalizer_" + lang, ":/translations")) {
    app.installTranslator(&myTranslator);
  }

  MainWindow window;
  window.show();
  return app.exec();
}
