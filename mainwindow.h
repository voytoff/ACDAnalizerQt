#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "acdobject.h"
#include "treemodel.h"
#include <QMainWindow>
#include <QTreeView>

class MainWindow : public QMainWindow
{
  Q_OBJECT
public:
  const QString Company = "NIIHM";
  const QString AppName = "ACDAnalizer";

  explicit MainWindow(QWidget *parent = nullptr);

protected:
  void closeEvent(QCloseEvent *event) override;

  ACDObject *acdObject;

private:
  void openExp();
  void closeExp();
  void createMenuBar();
  void createDashboard();
  void restoreLayout();
  void saveLayout();
  void createTree();
  void selectChannel();

  QTreeView *view;
  TreeModel *model;

public slots:

signals:
};

#endif // MAINWINDOW_H
