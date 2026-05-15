#include "tableview.h"
#include <QHeaderView>

TableView::TableView(QWidget *parent) : QTreeView{parent} {
  ///this->verticalHeader()->setDefaultSectionSize(22);
  // Optional: Prevent users from manually resizing rows
  //this->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
  //this->setRowHeight(0, 10);
  ///this->setShowGrid(false);
  this->setAlternatingRowColors(true);
  this->setAllColumnsShowFocus(true);
  this->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
  this->setHorizontalScrollMode(QAbstractItemView::ScrollMode::ScrollPerPixel);
  this->setSortingEnabled(true);
  this->setRootIsDecorated(false);

  QLocale locale = this->locale();
  locale.setNumberOptions(QLocale::OmitGroupSeparator);
  this->setLocale(locale);
}
