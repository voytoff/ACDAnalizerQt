#include "ACD2File_global.h"
#include "mainwindow.h"
#include "modeldatawidget.h"
#include "parametertable.h"
#include "tablemodel.h"
#include "tableview.h"
#include "treepaintdelegate.h"
#include "mchannelblock.h"
#include "settingsdlg.h"
#include "schemehelper.h"

#include <QRegularExpression>
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
#include <QPainter>
#include <QPixmap>
#include <QtSvg/QSvgRenderer>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>

#include "odsutils.h"
#include <ods/ods>
#include <float.h>

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow{parent}
  , acdObject(nullptr)
  , empty(new QWidget())
  , settings(new Settings())
  , progressBar(new QProgressBar())
  , tabWidget(new QTabWidget)
  , schemeHelper(new SchemeHelper(this, ":/images/calc.svg"))
  , splash(new QSplashScreen(QPixmap(":/images/wait.swg"), Qt::WindowStaysOnTopHint)) {
  QIcon::setThemeName("Material Symbols Outlined");
  createActions();
  createControlBar();
  createDashboard();
  restoreLayout();
}

void MainWindow::createActions() {
  openAction = schemeHelper->create(tr("Открыть..."), ":/images/open.svg", QKeySequence(Qt::CTRL | Qt::Key_O));
  closeAction = schemeHelper->create(tr("Закрыть..."), ":/images/close.svg");
  quitAction = schemeHelper->create(tr("Выход"), ":/images/quit.svg", QKeySequence::Quit);
  tableAction = schemeHelper->create(tr("Таблица"), ":/images/table.svg", QKeySequence(Qt::CTRL | Qt::Key_T));
  chartAction = schemeHelper->create(tr("График"), ":/images/chart.svg", QKeySequence(Qt::CTRL | Qt::Key_U));
  settingsAction = schemeHelper->create(tr("Установки..."), ":/images/settings.svg");
  aboutAction = schemeHelper->create(tr("&О программе..."), ":/images/about.svg");
  lightAction = schemeHelper->createLightAction(tr("Дневной режим"), ":/images/light.svg");
  darkAction = schemeHelper->createDarkAction(tr("Ночной режим"), ":/images/dark.svg");
  exportAction = schemeHelper->create(tr("Экспорт..."), ":/images/export.svg", QKeySequence(Qt::CTRL | Qt::Key_E));

  connect(openAction, &QAction::triggered, this, &MainWindow::openExp);
  connect(exportAction, &QAction::triggered, this, &MainWindow::exportExp);
  connect(closeAction, &QAction::triggered, this, &MainWindow::closeExp);
  connect(quitAction, &QAction::triggered, this, &MainWindow::close);
  connect(tableAction, &QAction::triggered, this, &MainWindow::showTable);
  connect(settingsAction, &QAction::triggered, this, &MainWindow::doSettings);
  connect(chartAction, &QAction::triggered, this, &MainWindow::showChart);
  connect(aboutAction, &QAction::triggered, this, &MainWindow::about);

  schemeHelper->applayColorScheme(settings->colorScheme(), true);
}

void MainWindow::createControlBar() {
  QMenu *fileMenu = menuBar()->addMenu(tr("Файл"));
  fileMenu->addAction(openAction);
  fileMenu->addAction(exportAction);
  fileMenu->addSeparator();
  fileMenu->addAction(closeAction);
  fileMenu->addSeparator();
  fileMenu->addAction(quitAction);

  QMenu *viewMenu = menuBar()->addMenu(tr("Вид"));
  viewMenu->addAction(lightAction);
  viewMenu->addAction(darkAction);

  QMenu *toolMenu = menuBar()->addMenu(tr("Инструменты"));
  toolMenu->addAction(tableAction);
  toolMenu->addAction(chartAction);
  toolMenu->addSeparator();
  toolMenu->addAction(settingsAction);

  QMenu *helpMenu = menuBar()->addMenu(tr("&?"));
  helpMenu->addAction(aboutAction);


  auto toolbar = addToolBar("Главный");
  toolbar->setObjectName("General");
  toolbar->addAction(openAction);
  toolbar->addAction(exportAction);
  toolbar->addSeparator();
  toolbar->addAction(tableAction);
  toolbar->addAction(chartAction);
  toolbar->addSeparator();
  toolbar->addAction(closeAction);

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

  QComboBox* axisXcombo = new QComboBox(this);
  axisXcombo->addItem("Индекс", AxisXType::Index);
  axisXcombo->addItem("Время", AxisXType::Time);
  axisXcombo->setCurrentIndex(settings->axisXType()-1);
  connect(axisXcombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this, axisXcombo]() {
    settings->axisXType(axisXcombo->currentData());
  });
  connect(settings, &Settings::propertyChanged, this, [axisXcombo](QString name, QVariant value) {
    if (name == "axisXType" && value.isValid())
      axisXcombo->setCurrentIndex(value.toInt()-1);
  });
  toolbar->addSeparator();
  toolbar->addWidget(axisXcombo);

  progressBar->setRange(0, 0);
  progressBar->setValue(0);
  progressBar->setTextVisible(true);
  statusBar()->addPermanentWidget(progressBar);
  progressBar->setMaximumWidth(120);
  progressBar->setMaximumHeight(16);
  progressBar->hide();
}

void MainWindow::createDashboard() {
  createTree();

  QGridLayout *layout = new QGridLayout();
  layout->setContentsMargins(2, 1, 2, 0);

  splitter = new QSplitter(this);
  splitter->addWidget(view);
  splitter->addWidget(tabWidget);
  layout->addWidget(splitter, 0, 0);

  tabWidget->setTabsClosable(true);
  connect(tabWidget, &QTabWidget::tabCloseRequested, this, [this](int index) {
    tabWidget->widget(index)->close();
    tabWidget->removeTab(index);
  });

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
    QString("%1;;%2;;%3").arg(filter_acd, filter_mmp, filter_all),
    &selectedFilter,
    options);
  if (!files.isEmpty() && files.length() > 0) {
    view->setModel(NULL);
    progressBar->show();

    if (selectedFilter == filter_acd) {
      QFutureWatcher<ACDObject*> *watcher = new QFutureWatcher<ACDObject*>(this);
      connect(watcher, &QFutureWatcher<ACDObject*>::finished, this, [this, watcher]() {
        statusBar()->showMessage("Готово");
        acdObject = watcher->result();
        model = new TreeModel(acdObject);
        view->setModel(model);
        progressBar->hide();
      });

      ACDObject* obj = new ACDObject(files);
      //connect(obj, &ACDObject::fileLoaded, this, [this](int index, QString fileName) {});
      //connect(obj, &ACDObject::channelBlockRead, this, [this, obj](QString fileName, int channelID, QString name) {
      //  statusBar()->showMessage(QString("%1 : %2 : %3").arg(fileName).arg(channelID).arg(name));
      //});
      connect(obj, &ACDObject::dataBlockRead, this, [this, obj](QString fileName, int channelID, int blockID, int size) {
        statusBar()->showMessage(QString("%1 : %2").arg(fileName, obj->channels->value(channelID)->name));
      });
      watcher->setFuture(
        QtConcurrent::task(
          &MainWindow::openACD)
          .withArguments(this, obj)
          .withPriority(5)
          .spawn());

    } else if (selectedFilter == filter_mmp) {
      QFutureWatcher<MMPObject*> *watcher = new QFutureWatcher<MMPObject*>(this);
      connect(watcher, &QFutureWatcher<MMPObject*>::finished, this, [this, watcher]() {
        statusBar()->showMessage("Готово");
        mmpObject = watcher->result();
        model = new TreeModel(mmpObject);
        view->setModel(model);
        progressBar->hide();
      });

      MMPObject* obj = new MMPObject(files);
      //connect(obj, &ACDObject::fileLoaded, this, [this](int index, QString fileName) {});
      //connect(obj, &ACDObject::channelBlockRead, this, [this, obj](QString fileName, int channelID, QString name) {
      //  statusBar()->showMessage(QString("%1 : %2 : %3").arg(fileName).arg(channelID).arg(name));
      //});
      connect(obj, &MMPObject::dataBlockRead, this, [this, obj](QString fileName, int channelID, int blockID, int size) {
        statusBar()->showMessage(QString("%1 : %2").arg(fileName, obj->channels->value(channelID)->name));
      });
      watcher->setFuture(
        QtConcurrent::task(
          &MainWindow::openMMP)
          .withArguments(this, obj)
          .withPriority(5)
          .spawn());

    } else {

    }
  }
}

void MainWindow::openACD(QPromise<ACDObject*> &promise, ACDObject* obj) {
  obj->load();
  promise.addResult(obj);
}
void MainWindow::openMMP(QPromise<MMPObject*> &promise, MMPObject* obj) {
  obj->load();
  promise.addResult(obj);
}

void MainWindow::closeExp() {
  if (acdObject) {
    acdObject->close();
    acdObject = nullptr;
  } else if (mmpObject) {
    mmpObject->close();
    mmpObject = nullptr;
  }
}

void MainWindow::restoreLayout() {
  restoreGeometry(settings->geometry());
  restoreState(settings->windowState());
  splitter->restoreState(settings->splitter());
}

void MainWindow::saveLayout() {
  settings->geometry(saveGeometry());
  settings->windowState(saveState());
  settings->splitter(splitter->saveState());
}

void MainWindow::createTree() {
  view = new QTreeView();
  view->header()->hide();
  connect(view, &QTreeView::doubleClicked, this, &MainWindow::selectChannel);
  QLocale locale = view->locale();
  locale.setNumberOptions(QLocale::OmitGroupSeparator);
  view->setLocale(locale);
  view->setItemDelegate(new TreePaintDelegate());
  view->setSelectionMode(QAbstractItemView::NoSelection);
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

int MainWindow::addTab(QWidget *widget, const QString &name) {
  int index = tabWidget->addTab(widget, name);
  tabWidget->setCurrentIndex(index);
  return index;
}

ParameterTable *MainWindow::getTable() {
  if (!(acdObject || mmpObject)) {
    QApplication::beep();
    return nullptr;
  }
  ParameterTable* table = nullptr;
  if (acdObject) {
    auto channels = model->channels();
    if (channels.count() == 0) {
      QApplication::beep();
      return nullptr;
    }
    ParameterTable* table = new ParameterTable(channels, settings->frequency());
  } else if (mmpObject) {
    auto channels = model->mchannels();
    if (channels.count() == 0) {
      QApplication::beep();
      return nullptr;
    }
    ParameterTable* table = new ParameterTable(channels, settings->frequency());
  }
  return table;
}

void MainWindow::showTable() {
  ParameterTable* table = getTable();
  if (!table) return;
  TableModel* model = new TableModel(table);
  TableView* view = new TableView();
  view->setModel(model);
  view->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
  addTab(view, table->headers.at(2));
}

void MainWindow::showChart() {
  ParameterTable* table = getTable();
  if (!table) return;
  TableModel* model = new TableModel(table);

  ModelDataWidget* view = new ModelDataWidget(model, settings->axisXType());
  ///splitter->replaceWidget(1, view);
  addTab(view, table->headers.at(2));
}

void MainWindow::doSettings() {
  SettingsDlg *dialog = new SettingsDlg(settings, this);
  int accepted = dialog->exec();
  if (accepted == QDialog::Accepted) {
    auto colorScheme = settings->colorScheme();
    if ((Qt::ColorScheme)colorScheme != QGuiApplication::styleHints()->colorScheme())
      schemeHelper->applayColorScheme(colorScheme);
  }
}

void MainWindow::about() {
  QMessageBox::about(
    this,
    QString("О %1").arg(AppName),
    QString("<p><b>%1</b> программа обработки данных результатов экспериментов "
            "с датчиков регистрации аналоговой и цифровой информации.</p>").arg(AppName)
    );
}

void MainWindow::exportExp() {
  const QFileDialog::Options options = QFileDialog::DontUseNativeDialog;
  QString selectedFilter;
  QString filePath = QFileDialog::getSaveFileName(
    this,
    tr("Имя файла"),
    "file.ods",
    tr("Open Document файлы (*.ods);;Все файлы (*)"),
    &selectedFilter,
    options);
  if (filePath.isEmpty()) return;

  ParameterTable* table = getTable();
  if (!table) return;

  auto *book = ods::Book::New();
  ods::AutoDelete<ods::Book*> ad(book);
  auto *spreadsheet = book->spreadsheet();
  auto *sheet = spreadsheet->NewSheet("Лист1");
  // 1. Headers
  auto *row = sheet->NewRowAt(0);
  for (int n = 0; n < table->headers.length(); n++) {
    auto *cell = row->NewCellAt(n);
    cell->SetValue(table->headers.at(n));
  }
  // 2. Values
  auto *style = odsutils::getDateStyle(book);
  for(int r = 1; r < table->table.count(); r++) {
    auto *row = sheet->NewRowAt(r);
    auto tableRow = table->row(r - 1);
    for (int n = 0; n < tableRow->count(); n++) {
      auto *cell = row->NewCellAt(n);
      odsutils::setValue(cell, tableRow->value(n));
      if (n == 1) cell->SetStyle(style);
    }
  }

  //auto col = sheet->GetColumn(1);
  //auto *style = odsutils::getDateStyle(book);
  //col->SetStyle(style);

  if (QFile::exists(filePath)) QFile::remove(filePath);
  odsutils::Save(book, filePath);
  auto response = QMessageBox::information(
    this, AppName,
    QString("Файл '%1' сохранен в формате Open Document. Открыть этот файл?").arg(filePath),
    QMessageBox::StandardButton::Yes,
    QMessageBox::StandardButton::No);
  if (response == QMessageBox::Yes) {
    QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
  }
}

