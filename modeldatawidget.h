#ifndef MODELDATAWIDGET_H
#define MODELDATAWIDGET_H

#include "ACD2File_global.h"
#include "contentwidget.h"
#include "tablemodel.h"
#include <QValueAxis>

class ModelDataWidget : public ContentWidget
{
  Q_OBJECT

public:
  ModelDataWidget(TableModel *model, AxisXType axisXType, QWidget *parent = nullptr);
private:
  QAbstractAxis* createAxisX(AxisXType axisXType, QString *title = nullptr);
  QAbstractAxis *createAxisY(QString *title = nullptr);
};

#endif
