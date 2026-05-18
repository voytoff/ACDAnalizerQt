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

//QT_USE_NAMESPACE

ModelDataWidget::ModelDataWidget(TableModel* model, QWidget *parent)
  : ContentWidget(parent)
{
  auto chart = new QChart;
  chart->setAnimationOptions(QChart::AllAnimations);

  for (int n = 2; n < model->columnCount(); n++) {
    // series 1
    auto series = new QLineSeries;
    auto axisX = new QDateTimeAxis;
    axisX->setTickCount(10);
    axisX->setFormat("dd.MM.yyyy dd.HH.mm");
    axisX->setTitleText("Дата");
    chart->addAxis(axisX, Qt::AlignTop);

    series->setName(model->headerData(n, Qt::Horizontal, Qt::DisplayRole).toString());
    QVXYModelMapper* mapper = new QVXYModelMapper(this);
    mapper->setXColumn(1);
    mapper->setYColumn(n);
    mapper->setSeries(series);
    mapper->setModel(model);
    chart->addSeries(series);
    // for storing color hex from the series
    QString seriesColorHex = "#000000";
    // get the color of the series and use it for showing the mapped area
    seriesColorHex = "#" + QString::number(series->pen().color().rgb(), 16).right(6).toUpper();
    model->addMapping(seriesColorHex, QRect((n-2)*2, 0, 2, model->rowCount()));

    series->attachAxis(axisX);
  }

  chart->createDefaultAxes();
  chart->layout()->setContentsMargins(0, 0, 0, 0);
  auto chartView = new QChartView(chart, this);
  chartView->setRenderHint(QPainter::Antialiasing);

  // create main layout
  auto mainLayout = new QGridLayout;
  mainLayout->addWidget(chartView, 0, 0);
  setLayout(mainLayout);
}
