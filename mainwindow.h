#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "acdobject.h"
#include "parametertable.h"
#include "settings.h"
#include "treemodel.h"
#include <QMainWindow>
#include <QTreeView>
#include <QPromise>
#include <QSplitter>
#include <QProgressBar>
#include <QSplashScreen>

class MainWindow : public QMainWindow
{
  Q_OBJECT
public:
  explicit MainWindow(QWidget *parent = nullptr);
  const QString fillLight = "#1f1f1f";
  const QString fillDark = "#e3e3e3";

protected:
  void closeEvent(QCloseEvent *event) override;

  ACDObject *acdObject;
  Settings* settings;

private:
  QString changeFillSvg(QString svg, QString fillColorHexText);
  QIcon getIcon(const QString &path);
  QIcon iconFromSvgString(const QString &svgString, int width = 24, int height = 24);
  void createControlBar();
  void createDashboard();
  void restoreLayout();
  void saveLayout();
  void createTree();
  void selectChannel();
  void openACD(QPromise<ACDObject *> &promise, ACDObject* obj);
  void frequencyChanged(int index);
  ParameterTable* getTable();
  void about();
  int addTab(QWidget *widget, const QString &name);
  void applayColorScheme(ColorScheme scheme);
  void setColorScheme(ColorScheme scheme);
  void setIcons();
  void setDarkTitleBar(bool dark);

  QTreeView *view;
  TreeModel *model;
  QSplitter *splitter;
  QTabWidget *tabWidget;
  QWidget *empty;
  QProgressBar *progressBar;
  QSplashScreen *splash;

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
