#include "mainwindow.h"
#include "modeldatawidget.h"
#include "parametertable.h"
#include "tablemodel.h"
#include "tableview.h"
#include "treepaintdelegate.h"
#include "channelblock.h"
#include "settingsdlg.h"

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
#include <QtConcurrent>
#include <QtConcurrentTask>
#include <QHeaderView>
#include <QScreen>
#include <QToolBar>
#include <QComboBox>

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow{parent}
  , acdObject(nullptr)
  , empty(new QWidget())
  , settings(new Settings()) {
  QIcon::setThemeName("Material Symbols Outlined");
  QGuiApplication::styleHints()->setColorScheme(Qt::ColorScheme::Light);
  this->setWindowIcon(QIcon::fromTheme(QIcon::ThemeIcon::NetworkWired));
  createControlBar();
  createDashboard();
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
  QAction *tableAction = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::EditSelectAll), tr("Таблица"), this);
  QAction *settingsAction = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::ViewRestore), tr("Установки..."), this);
  QAction *chartAction = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::ViewFullscreen), tr("График"), this);

  openAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_O));
  quitAction->setShortcuts(QKeySequence::Quit);

  connect(openAction, &QAction::triggered, this, &MainWindow::openExp);
  connect(closeAction, &QAction::triggered, this, &MainWindow::closeExp);
  connect(quitAction, &QAction::triggered, this, &MainWindow::close);
  connect(tableAction, &QAction::triggered, this, &MainWindow::showTable);
  connect(settingsAction, &QAction::triggered, this, &MainWindow::doSettings);
  connect(chartAction, &QAction::triggered, this, &MainWindow::showChart);

  QMenu *fileMenu = menuBar()->addMenu(tr("Файл"));
  fileMenu->addAction(openAction);
  fileMenu->addSeparator();
  fileMenu->addAction(closeAction);
  fileMenu->addSeparator();
  fileMenu->addAction(quitAction);

  QMenu *toolMenu = menuBar()->addMenu(tr("Инструменты"));
  toolMenu->addAction(settingsAction);


  auto toolbar = addToolBar("Главный");
  toolbar->addAction(openAction);
  toolbar->addAction(tableAction);
  toolbar->addAction(chartAction);

  QComboBox* frequency = new QComboBox(this);
  frequency->addItem("1", 1);
  frequency->addItem("10", 10);
  frequency->addItem("100", 100);
  frequency->setCurrentText(QString::number(settings->frequency()));
  connect(frequency, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this, frequency]() {
    settings->frequency(frequency->currentData());
  });
  connect(settings, &Settings::propertyChanged, this, [frequency](QString name, QVariant value) {
    if (name == "frequency" && value.isValid())
      frequency->setCurrentText(QString::number(value.toInt()));
  });

  toolbar->addSeparator();
  toolbar->addWidget(frequency);
}

void MainWindow::createDashboard() {
  createTree();

  QGridLayout *layout = new QGridLayout();
  layout->setContentsMargins(2, 0, 2, 0);

  splitter = new QSplitter(this);
  splitter->addWidget(view);
  splitter->addWidget(empty);
  layout->addWidget(splitter, 0, 0);

  QWidget *widget = new QWidget;
  widget->setLayout(layout);
  setCentralWidget(widget);

  statusBar()->setSizeGripEnabled(true);
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
    view->setModel(NULL);
    splitter->replaceWidget(1, empty);
    QFutureWatcher<ACDObject*> *watcher = new QFutureWatcher<ACDObject*>(this);
    connect(watcher, &QFutureWatcher<ACDObject*>::finished, this, [this, watcher]() {
      statusBar()->showMessage("Готово");
      acdObject = watcher->result();
      model = new TreeModel(acdObject);
      view->setModel(model);
    });
    ACDObject* obj = new ACDObject(files);
    connect(obj, &ACDObject::fileLoaded, this, [this](int index, QString fileName) {
      statusBar()->showMessage(QString("%1: %2").arg(index).arg(fileName));
    });
    watcher->setFuture(
      QtConcurrent::task(
        &MainWindow::openACD)
          .withArguments(this, obj)
          .withPriority(5)
          .spawn());
  }
}

void MainWindow::openACD(QPromise<ACDObject*> &promise, ACDObject* obj) {
  obj->load();
  promise.addResult(obj);
}

void MainWindow::closeExp() {
  if (acdObject) {
    acdObject->close();
    acdObject = nullptr;
  }
}

void MainWindow::restoreLayout() {
  Settings settings;
  restoreGeometry(settings.geometry());
  restoreState(settings.windowState());
}

void MainWindow::saveLayout() {
  Settings settings;
  settings.geometry(saveGeometry());
  settings.windowState(saveState());
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

ParameterTable *MainWindow::getTable()
{
  if (!acdObject) {
    QApplication::beep();
    return nullptr;
  }
  splitter->replaceWidget(1, empty);
  auto channels = model->channels();
  if (channels.count() == 0) {
    QApplication::beep();
    return nullptr;
  }
  ParameterTable* table = new ParameterTable();
  foreach (ChannelBlock* channelBlock, channels) {
    auto array = channelBlock->array(settings->frequency());
    table->appendColumn(*array);
  }
  return table;
}

void MainWindow::showTable() {
  /*
  // 1 находим параметр с максимальной частотой дискретизации И максимальным набором элементов
  auto it = std::max_element(channels.begin(), channels.end(), [](ChannelBlock *a, ChannelBlock *b) {
    return a->frequency() < b->frequency() && a->dataBlockArray->count() < b->dataBlockArray->count();
  });
  table->createIndex((*it)->data());
  foreach (ChannelBlock* channelBlock, channels) {
    auto data = channelBlock->data();
    table->appendColumn(channelBlock->name, data);
  }
*/
  ParameterTable* table = getTable();
  if (!table) return;
  TableModel* model = new TableModel(table);
  TableView* view = new TableView();
  view->setModel(model);
  splitter->replaceWidget(1, view);
}

void MainWindow::showChart()
{
  ParameterTable* table = getTable();
  if (!table) return;
  TableModel* model = new TableModel(table);

  ModelDataWidget* view = new ModelDataWidget(model);
  splitter->replaceWidget(1, view);
}

void MainWindow::doSettings()
{
  SettingsDlg *dialog = new SettingsDlg(settings, this);
  int accepted = dialog->exec();
  if (accepted == QDialog::Accepted) {
    //model->replace(index.row(), sensor);
  }
}
