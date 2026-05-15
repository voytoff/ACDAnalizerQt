#include "mainwindow.h"
#include "parametertable.h"
#include "tablemodel.h"
#include "tableview.h"
#include "treepaintdelegate.h"

#include <QFileDialog>
#include <QFlag>
#include <QFile>
#include <QMenu>
#include <QMenuBar>
#include <QString>
#include <QApplication>
#include <QStyleHints>
#include <QSettings>
#include <QGridLayout>
#include <QSplitter>
#include <QGroupBox>
#include <QStatusBar>
#include <QtConcurrentTask>
#include <QHeaderView>
#include <QScreen>
#include <QToolBar>

MainWindow::MainWindow(QWidget *parent) : QMainWindow{parent}
{
  QIcon::setThemeName("Material Symbols Outlined");
  QGuiApplication::styleHints()->setColorScheme(Qt::ColorScheme::Dark);
  this->setWindowIcon(QIcon::fromTheme(QIcon::ThemeIcon::NetworkWired));
  createControlBar();
  createTree();
  //adjustHeader();
  createDashboard();
  statusBar()->setSizeGripEnabled(true);
  restoreLayout();
/*
  QFile file(":/default.txt");
  file.open(QIODevice::ReadOnly | QIODevice::Text);
  model = new TreeModel(QString::fromUtf8(file.readAll()));
  file.close();
  view->setModel(model);
*/
}

void MainWindow::createControlBar()
{
  QAction *openAction = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::FolderOpen), tr("Открыть..."), this);
  QAction *closeAction = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::WindowClose), tr("Закрыть..."), this);
  QAction *quitAction = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::SystemLogOut), tr("Выход"), this);
  QAction *tableAction = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentProperties), tr("Таблица"), this);

  openAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_O));
  quitAction->setShortcuts(QKeySequence::Quit);

  connect(openAction, &QAction::triggered, this, &MainWindow::openExp);
  connect(closeAction, &QAction::triggered, this, &MainWindow::closeExp);
  connect(quitAction, &QAction::triggered, this, &MainWindow::close);
  connect(tableAction, &QAction::triggered, this, &MainWindow::showTable);

  QMenu *fileMenu = menuBar()->addMenu(tr("Файл"));
  fileMenu->addAction(openAction);
  fileMenu->addSeparator();
  fileMenu->addAction(closeAction);
  fileMenu->addSeparator();
  fileMenu->addAction(quitAction);

  menuBar()->setNativeMenuBar(false);

  auto toolbar = addToolBar("Главный");
  toolbar->addAction(openAction);
  toolbar->addAction(tableAction);

}

void MainWindow::createDashboard() {
  QGridLayout *layout = new QGridLayout;
  QGroupBox *box = new QGroupBox("text");
  splitter = new QSplitter(this);
  splitter->addWidget(view);
  splitter->addWidget(box);
  layout->addWidget(splitter, 0, 0);
  //layout->addWidget(view, 0, 0);
  //layout->addWidget(controlBox, 0, 1);

  QWidget *widget = new QWidget;
  widget->setLayout(layout);
  setCentralWidget(widget);
}

void MainWindow::openExp() {
  const QFileDialog::Options options = QFileDialog::DontUseNativeDialog;
  QString selectedFilter;
  QStringList files = QFileDialog::getOpenFileNames(
    this,
    tr("Выбор файлов"),
    "",
    tr("ACD2 файлы (*.acd);;Все файлы (*)"),
    &selectedFilter,
    options);
  if (!files.isEmpty() && files.length() > 0) {
    auto future = QtConcurrent::task(
      &MainWindow::openACD)
      .withArguments(this, files)
      .withPriority(5)
      .spawn();
    future.waitForFinished();
    acdObject = future.result();
    model = new TreeModel(acdObject);
    view->setModel(model);
  }
}

void MainWindow::openACD(QPromise<ACDObject*> &promise, QStringList files) {
  ACDObject* acdo = new ACDObject(files);
  connect(acdo, &ACDObject::fileLoaded, this, [=](int index, QString fileName) { qDebug() << index << fileName; });
  acdo->load();
  promise.addResult(acdo);
}

void MainWindow::closeExp() {
  if (acdObject)
    acdObject->close();
}

void MainWindow::restoreLayout() {
  QSettings settings(Company, AppName);
  restoreGeometry(settings.value("geometry").toByteArray());
  restoreState(settings.value("windowState").toByteArray());
}

void MainWindow::saveLayout() {
  QSettings settings(Company, AppName);
  settings.setValue("geometry", saveGeometry());
  settings.setValue("windowState", saveState());
}

void MainWindow::createTree() {
  view = new QTreeView();
  view->header()->hide();
  connect(view, &QTreeView::doubleClicked, this, &MainWindow::selectChannel);
  QLocale locale = view->locale();
  locale.setNumberOptions(QLocale::OmitGroupSeparator);
  view->setLocale(locale);
  view->setItemDelegate(new TreePaintDelegate());
}

void MainWindow::selectChannel() {
  QModelIndex index = view->currentIndex();
  if (index.isValid()) {
    auto item = model->get(index);
    item->toggle();
    model->layoutChanged();
  }
}

void MainWindow::closeEvent(QCloseEvent *event) {
  saveLayout();
  QMainWindow::closeEvent(event);
}

void MainWindow::showTable() {
  auto channels = model->channels();
  ParameterTable* table = new ParameterTable();
  foreach (ChannelBlock* channelBlock, model->channels()) {
    auto data = channelBlock->data();
    table->append(channelBlock->name, data);
  }
  auto model = new TableModel(table);
  auto view = new TableView();
  view->setModel(model);
  splitter->replaceWidget(1, view);
}
