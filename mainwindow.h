#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "acdobject.h"
#include "settings.h"
#include "treemodel.h"
#include <QMainWindow>
#include <QTreeView>
#include <QPromise>
#include <QSplitter>

class MainWindow : public QMainWindow
{
  Q_OBJECT
public:
  explicit MainWindow(QWidget *parent = nullptr);

protected:
  void closeEvent(QCloseEvent *event) override;

  ACDObject *acdObject;
  Settings* settings;

private:
  void createControlBar();
  void createDashboard();
  void restoreLayout();
  void saveLayout();
  void createTree();
  void selectChannel();
  void openACD(QPromise<ACDObject *> &promise, ACDObject* obj);
  void frequencyChanged(int index);

  QTreeView *view;
  TreeModel *model;
  QSplitter *splitter;
  QWidget *empty;

public slots:
  void openExp();
  void closeExp();
  void showTable();
  void doSettings();

signals:
  void updateStatusBar(const QString &message);

};

#endif // MAINWINDOW_H
