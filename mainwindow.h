#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "acdobject.h"

class MainWindow : public QMainWindow
{
  Q_OBJECT
public:
  explicit MainWindow(QWidget *parent = nullptr);

protected:
  ACDObject *acdObject;

private:
  void createMenuBar();

public slots:
  void open();

signals:
};

#endif // MAINWINDOW_H
