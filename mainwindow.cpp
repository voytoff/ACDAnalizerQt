#include "mainwindow.h"
#include <QMenu>
#include <QMenuBar>
#include <QFileDialog>
#include <QFlag>
#include <QString>

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow{parent}
{
  createMenuBar();
}

void MainWindow::createMenuBar()
{
  QAction *openAction = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::FolderOpen), tr("открыть..."), this);

  openAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_O));

  connect(openAction, &QAction::triggered, this, &MainWindow::open);

  QMenu *fileMenu = menuBar()->addMenu(tr("Файл"));
  fileMenu->addAction(openAction);

  menuBar()->setNativeMenuBar(false);
}

void MainWindow::open() {
  const QFileDialog::Options options;// = QFlag::
  QString selectedFilter;
  QStringList files = QFileDialog::getOpenFileNames(
    this, tr("Выбор файлов"),
    "",
    tr("ACD2 файлы (*.acd);;Все файлы (*)"),
    &selectedFilter,
    options);
  if (!files.isEmpty()) {
    auto openFilesPath = files.constFirst();
    //openFileNamesLabel->setText(u'[' + files.join(", "_L1) + u']');
  }

}

