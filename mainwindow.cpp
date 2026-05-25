#include "ACD2File_global.h"
#include "mainwindow.h"
#include "modeldatawidget.h"
#include "parametertable.h"
#include "tablemodel.h"
#include "tableview.h"
#include "treepaintdelegate.h"
#include "channelblock.h"
#include "settingsdlg.h"

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

#include <windows.h>
#include <dwmapi.h>

#pragma comment(lib, "dwmapi.lib") // MSVC Only

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow{parent}
  , acdObject(nullptr)
  , empty(new QWidget())
  , settings(new Settings())
  , progressBar(new QProgressBar())
  , tabWidget(new QTabWidget)
  , splash(new QSplashScreen(QPixmap(":/images/wait.swg"), Qt::WindowStaysOnTopHint)) {
  QIcon::setThemeName("Material Symbols Outlined");
  createControlBar();
  createDashboard();
  restoreLayout();
}

void MainWindow::setIcons() {
  this->setWindowIcon(getIcon(":/images/calc.svg"));

  openAction->setIcon(getIcon(":/images/open.svg"));
  closeAction->setIcon(getIcon(":/images/close.svg"));
  quitAction->setIcon(getIcon(":/images/quit.svg"));
  tableAction->setIcon(getIcon(":/images/table.svg"));
  chartAction->setIcon(getIcon(":/images/chart.svg"));
  settingsAction->setIcon(getIcon(":/images/settings.svg"));
  aboutAction->setIcon(getIcon(":/images/about.svg"));
  lightAction->setIcon(getIcon(":/images/light.svg"));
  darkAction->setIcon(getIcon(":/images/dark.svg"));
  exportAction->setIcon(getIcon(":/images/export.svg"));
}

void MainWindow::createControlBar() {
  openAction = new QAction(tr("Открыть..."), this);
  closeAction = new QAction(tr("Закрыть..."), this);
  quitAction = new QAction(tr("Выход"), this);
  tableAction = new QAction(tr("Таблица"), this);
  chartAction = new QAction(tr("График"), this);
  settingsAction = new QAction(tr("Установки..."), this);
  aboutAction = new QAction(tr("&О программе..."), this);
  lightAction = new QAction(tr("Дневной режим"), this);
  darkAction = new QAction(tr("Ночной режим"), this);
  exportAction = new QAction(tr("Экспорт..."), this);

  openAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_O));
  tableAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_T));
  chartAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_U));
  exportAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_E));
  quitAction->setShortcuts(QKeySequence::Quit);

  connect(openAction, &QAction::triggered, this, &MainWindow::openExp);
  connect(exportAction, &QAction::triggered, this, &MainWindow::exportExp);
  connect(closeAction, &QAction::triggered, this, &MainWindow::closeExp);
  connect(quitAction, &QAction::triggered, this, &MainWindow::close);
  connect(tableAction, &QAction::triggered, this, &MainWindow::showTable);
  connect(settingsAction, &QAction::triggered, this, &MainWindow::doSettings);
  connect(chartAction, &QAction::triggered, this, &MainWindow::showChart);
  connect(aboutAction, &QAction::triggered, this, &MainWindow::about);
  connect(lightAction, &QAction::triggered, this, [this]() { applayColorScheme(ColorScheme::Light); });
  connect(darkAction, &QAction::triggered, this, [this]() { applayColorScheme(ColorScheme::Dark); });

  applayColorScheme(settings->colorScheme());

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
    tr("ACD2 файлы (*.acd);;Все файлы (*)"),
    &selectedFilter,
    options);
  if (!files.isEmpty() && files.length() > 0) {
    view->setModel(NULL);
    ///splitter->replaceWidget(1, empty);
    progressBar->show();
    ///splash->show();
    ///splash->showMessage("Loading modules...", Qt::AlignBottom | Qt::AlignRight, Qt::white);
    QFutureWatcher<ACDObject*> *watcher = new QFutureWatcher<ACDObject*>(this);
    connect(watcher, &QFutureWatcher<ACDObject*>::finished, this, [this, watcher]() {
      statusBar()->showMessage("Готово");
      acdObject = watcher->result();
      model = new TreeModel(acdObject);
      view->setModel(model);
      progressBar->hide();
      ///splash->finish(this);
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
  //Settings settings;
  restoreGeometry(settings->geometry());
  restoreState(settings->windowState());
  splitter->restoreState(settings->splitter());
}

void MainWindow::saveLayout() {
  //Settings settings;
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

QString MainWindow::changeFillSvg(QString svg, QString fillColorHexText) {
  auto updatedSvg = svg.replace(QRegularExpression("fill=\"[^\"]*\""), fillColorHexText);
  return updatedSvg;
}

QIcon MainWindow::getIcon(const QString &path) {
  QFile file(path);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    return QIcon(path);

  QTextStream in(&file);
  QString content = in.readAll();
  file.close();
  auto scheme = QGuiApplication::styleHints()->colorScheme();
  auto color = scheme == Qt::ColorScheme::Dark ? fillDark : fillLight;
  content = changeFillSvg(content, QString("fill=\"%1\"").arg(color));
  return iconFromSvgString(content);
}

QIcon MainWindow::iconFromSvgString(const QString &svgString, int width, int height) {
  // 1. Prepare SVG data
  QByteArray byteArray = svgString.toUtf8();
  QSvgRenderer renderer(byteArray);
  // 2. Prepare a transparent Pixmap
  QPixmap pixmap(width, height);
  pixmap.fill(Qt::transparent);
  // 3. Paint the SVG onto the Pixmap
  QPainter painter(&pixmap);
  renderer.render(&painter);
  // 4. Return as QIcon
  return QIcon(pixmap);
}

ParameterTable *MainWindow::getTable() {
  if (!acdObject) {
    QApplication::beep();
    return nullptr;
  }
  ///splitter->replaceWidget(1, empty);
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

int MainWindow::addTab(QWidget *widget, const QString &name) {
  int index = tabWidget->addTab(widget, name);
  tabWidget->setCurrentIndex(index);
  return index;
}

void MainWindow::setColorScheme(ColorScheme scheme) {
  settings->colorScheme(scheme);
  applayColorScheme(scheme);
}

void MainWindow::applayColorScheme(ColorScheme scheme) {
  QGuiApplication::styleHints()->setColorScheme((Qt::ColorScheme)scheme);
  setIcons();
  bool dark = scheme == ColorScheme::Dark;
  lightAction->setVisible(dark);
  darkAction->setVisible(!dark);
  setDarkTitleBar(dark);
}

void MainWindow::setDarkTitleBar(bool dark) {
  BOOL value = dark ? TRUE : FALSE;
  DwmSetWindowAttribute(reinterpret_cast<HWND>(this->winId()), 20, &value, sizeof(value));
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
      applayColorScheme(colorScheme);
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

