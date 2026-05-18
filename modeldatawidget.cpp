#include "modeldatawidget.h"

#include <QChart>
#include <QChartView>
#include <QGraphicsLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QLineSeries>
#include <QTableView>
#include <QVXYModelMapper>
#include <QDateTimeAxis>

QT_USE_NAMESPACE

ModelDataWidget::ModelDataWidget(TableModel* model, AxisXType axisXType, QWidget *parent)
  : ContentWidget(parent) {
  auto chart = new QChart;
  chart->setAnimationOptions(QChart::AllAnimations);
  //auto axisY = createAxisY();
  //chart->addAxis(axisY, Qt::AlignLeft);
  auto axisX = createAxisX(axisXType);
  chart->addAxis(axisX, Qt::AlignBottom);

  for (int n = 2; n < model->columnCount(); n++) {
    auto series = new QLineSeries;
    series->setName(model->headerData(n, Qt::Horizontal, Qt::DisplayRole).toString());
    QVXYModelMapper* mapper = new QVXYModelMapper(this);
    mapper->setXColumn(axisXType == AxisXType::Time ? 1 : 0);
    mapper->setYColumn(n);
    mapper->setSeries(series);
    mapper->setModel(model);
    chart->addSeries(series);
    // for storing color hex from the series
    QString seriesColorHex = "#000000";
    // get the color of the series and use it for showing the mapped area
    seriesColorHex = "#" + QString::number(series->pen().color().rgb(), 16).right(6).toUpper();
    model->addMapping(seriesColorHex, QRect((n-2)*2, 0, 2, model->rowCount()));

    auto axisY = createAxisY();
    chart->addAxis(axisY, Qt::AlignLeft);
    //auto axisX = createAxisX(axisXType);
    //chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    series->attachAxis(axisY);
  }

  //chart->createDefaultAxes();
  chart->layout()->setContentsMargins(0, 0, 0, 0);
  auto chartView = new QChartView(chart, this);
  chartView->setRenderHint(QPainter::Antialiasing);

  // create main layout
  auto mainLayout = new QGridLayout;
  mainLayout->addWidget(chartView, 0, 0);
  setLayout(mainLayout);
}

QAbstractAxis *ModelDataWidget::createAxisX(AxisXType axisXType, QString *title) {
  QAbstractAxis* result;
  if (axisXType == AxisXType::Time) {
    auto axisX = new QDateTimeAxis;
    axisX->setTickCount(5);
    axisX->setFormat("HH:mm:ss.zz");
    result = axisX;
  } else { // if (axisXType == AxisXType::Index)
    auto axisX = new QValueAxis;
    axisX->setLabelFormat("%i");
    result = axisX;
  }
  if (result && title)
    result->setTitleText(*title);
  return result;
}

QAbstractAxis* ModelDataWidget::createAxisY(QString* title) {
  auto axisY = new QValueAxis;
  axisY->setLabelFormat("%.4f");
  if (title) axisY->setTitleText(*title);
  return axisY;
}
