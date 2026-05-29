#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "acdobject.h"
#include "mmpobject.h"
#include "parametertable.h"
#include "schemehelper.h"
#include "settings.h"
#include "treemodel.h"
#include <QMainWindow>
#include <QTreeView>
#include <QPromise>
#include <QSplitter>
#include <QProgressBar>
#include <QSplashScreen>

const QString filter_acd = "ACD2 файлы (*.acd)";
const QString filter_mmp = "MMP файлы (*.mmp *.prm)";
const QString filter_all = "Все файлы (*)";

class MainWindow : public QMainWindow
{
  Q_OBJECT
public:
  explicit MainWindow(QWidget *parent = nullptr);

protected:
  void closeEvent(QCloseEvent *event) override;

  ACDObject *acdObject;
  MMPObject *mmpObject;
  Settings* settings;

private:
  void createActions();
  void createControlBar();
  void createDashboard();
  void restoreLayout();
  void saveLayout();
  void createTree();
  void selectChannel();
  void openACD(QPromise<ACDObject *> &promise, ACDObject* obj);
  void openMMP(QPromise<MMPObject*> &promise, MMPObject* obj);
  void frequencyChanged(int index);
  void getTable(const std::function<void(ParameterTable*)>& callback);
  void about();
  int addTab(QWidget *widget, const QString &name);

  QTreeView *treeView;
  TreeModel *model;
  QSplitter *splitter;
  QTabWidget *tabWidget;
  QWidget *empty;
  QProgressBar *progressBar;
  QSplashScreen *splash;
  ParameterTable* currentTable;

  SchemeHelper *schemeHelper;

  QAction *openAction;
  QAction *closeAction;
  QAction *quitAction;
  QAction *tableAction;
  QAction *chartAction;
  QAction *settingsAction;
  QAction *aboutAction;
  QAction *lightAction;
  QAction *darkAction;
  QAction *exportAction;

public slots:
  void openExp();
  void exportExp();
  void closeExp();
  void showTable();
  void showChart();
  void doSettings();

signals:
  void updateStatusBar(const QString &message);

};

#endif // MAINWINDOW_H
