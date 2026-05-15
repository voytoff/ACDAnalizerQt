#ifndef TABLEVIEW_H
#define TABLEVIEW_H

#include <QTreeView>

class TableView : public QTreeView
{
  Q_OBJECT
public:
  explicit TableView(QWidget *parent = nullptr);

signals:

};

#endif // TABLEVIEW_H
