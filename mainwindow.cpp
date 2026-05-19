#include "ACD2File_global.h"
#include "mainwindow.h"
#include "modeldatawidget.h"
#include "parametertable.h"
#include "tablemodel.h"
#include "tableview.h"
#include "treepaintdelegate.h"
#include "channelblock.h"
#include "settingsdlg.h"

#include <regex>
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

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow{parent}
  , acdObject(nullptr)
  , empty(new QWidget())
  , settings(new Settings())
  , progressBar(new QProgressBar())
  , tabWidget(new QTabWidget)
  , splash(new QSplashScreen(QPixmap(":/images/wait.swg"), Qt::WindowStaysOnTopHint)) {
  //QIcon::setThemeName("Material Symbols Outlined");
  QGuiApplication::styleHints()->setColorScheme(Qt::ColorScheme::Light);
  this->setWindowIcon(getIcon(":/images/calc.svg"));
  createControlBar();
  createDashboard();
  restoreLayout();
}

void MainWindow::createControlBar() {
  QAction *openAction = new QAction(getIcon(":/images/open.svg"), tr("Открыть..."), this);
  QAction *closeAction = new QAction(getIcon(":/images/close.svg"), tr("Закрыть..."), this);
  QAction *quitAction = new QAction(getIcon(":/images/quit.svg"), tr("Выход"), this);
  QAction *tableAction = new QAction(getIcon(":/images/table.svg"), tr("Таблица"), this);
  QAction *chartAction = new QAction(getIcon(":/images/chart.svg"), tr("График"), this);
  QAction *settingsAction = new QAction(getIcon(":/images/settings.svg"), tr("Установки..."), this);
  QAction *aboutAction = new QAction(getIcon(":/images/about.svg"), tr("&О программе..."), this);

  openAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_O));
  quitAction->setShortcuts(QKeySequence::Quit);

  connect(openAction, &QAction::triggered, this, &MainWindow::openExp);
  connect(closeAction, &QAction::triggered, this, &MainWindow::closeExp);
  connect(quitAction, &QAction::triggered, this, &MainWindow::close);
  connect(tableAction, &QAction::triggered, this, &MainWindow::showTable);
  connect(settingsAction, &QAction::triggered, this, &MainWindow::doSettings);
  connect(chartAction, &QAction::triggered, this, &MainWindow::showChart);
  connect(aboutAction, &QAction::triggered, this, &MainWindow::about);

  QMenu *fileMenu = menuBar()->addMenu(tr("Файл"));
  fileMenu->addAction(openAction);
  fileMenu->addSeparator();
  fileMenu->addAction(closeAction);
  fileMenu->addSeparator();
  fileMenu->addAction(quitAction);

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

  //progressBar->setRange(0, 100);
  progressBar->setMaximum(0);
  progressBar->setMinimum(0);
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
  Settings settings;
  restoreGeometry(settings.geometry());
  restoreState(settings.windowState());
  splitter->restoreState(settings.splitter());
}

void MainWindow::saveLayout() {
  Settings settings;
  settings.geometry(saveGeometry());
  settings.windowState(saveState());
  settings.splitter(splitter->saveState());
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

QString MainWindow::changeFillSvg(QString svg, QString fillColorHexText) {
  std::regex fillRegex("fill=\"[^\"]*\"");
  std::string newFill = fillColorHexText.toStdString();
  std::string updatedSvg = std::regex_replace(svg.toStdString(), fillRegex, newFill);
  return QString::fromStdString(updatedSvg);
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

void MainWindow::showTable() {
  ParameterTable* table = getTable();
  if (!table) return;
  TableModel* model = new TableModel(table);
  TableView* view = new TableView();
  view->setModel(model);
  ///splitter->replaceWidget(1, view);
  int index = tabWidget->addTab(view, table->headers.at(2));
  tabWidget->setCurrentIndex(index);
}

void MainWindow::showChart() {
  ParameterTable* table = getTable();
  if (!table) return;
  TableModel* model = new TableModel(table);

  ModelDataWidget* view = new ModelDataWidget(model, settings->axisXType());
  ///splitter->replaceWidget(1, view);
  int index = tabWidget->addTab(view, table->headers.at(2));
  tabWidget->setCurrentIndex(index);
}

void MainWindow::doSettings() {
  SettingsDlg *dialog = new SettingsDlg(settings, this);
  int accepted = dialog->exec();
  if (accepted == QDialog::Accepted) {
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

