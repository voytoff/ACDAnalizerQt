#ifndef TABLEVIEW_H
#define TABLEVIEW_H

#include "parametertable.h"
#include <QTableView>

class TableView : public QTableView
{
  Q_OBJECT
public:
  explicit TableView(QWidget *parent = nullptr);
  const ParameterTable *table;

signals:

};

#endif // TABLEVIEW_H
