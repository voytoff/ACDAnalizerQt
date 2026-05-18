#ifndef MODELDATAWIDGET_H
#define MODELDATAWIDGET_H

#include "contentwidget.h"
#include "tablemodel.h"

class ModelDataWidget : public ContentWidget
{
  Q_OBJECT

public:
  ModelDataWidget(TableModel *model, QWidget *parent = nullptr);
};

#endif
