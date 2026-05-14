#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "acdobject.h"
#include "treemodel.h"
#include <QMainWindow>
#include <QTreeView>
#include <QPromise>

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
  void createMenuBar();
  void createDashboard();
  void restoreLayout();
  void saveLayout();
  void createTree();
  void adjustHeader();
  void selectChannel();
  void openACD(QPromise<ACDObject *> &promise, QStringList files);

  QTreeView *view;
  TreeModel *model;

public slots:
  void openExp();
  void closeExp();

signals:
};

#endif // MAINWINDOW_H
