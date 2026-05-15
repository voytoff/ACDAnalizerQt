#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "acdobject.h"
#include "treemodel.h"
#include <QMainWindow>
#include <QTreeView>
#include <QPromise>
#include <QSplitter>

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
  void createControlBar();
  void createDashboard();
  void restoreLayout();
  void saveLayout();
  void createTree();
  void selectChannel();
  void openACD(QPromise<ACDObject *> &promise, QStringList files);

  QTreeView *view;
  TreeModel *model;
  QSplitter *splitter;

public slots:
  void openExp();
  void closeExp();
  void showTable();

signals:
};

#endif // MAINWINDOW_H
