#include "contentwidget.h"

#include <QChart>
#include <QChartView>
#include <QFont>
#include <QLabel>
#include <QVBoxLayout>

ContentWidget::ContentWidget(QWidget *parent)
  : QWidget(parent)
{
}

void ContentWidget::load()
{
  if (loaded || layout())
    return;

  if (!doLoad()) {
    auto errorLabel = new QLabel(this);
    auto errorLayout = new QVBoxLayout(this);
    errorLabel->setText(tr("Error loading the example:\n%1").arg(m_loadError));
    QFont font = errorLabel->font();
    font.setPointSize(20);
    errorLabel->setFont(font);
    errorLabel->setAlignment(Qt::AlignCenter);
    errorLayout->addWidget(errorLabel);
    setLayout(errorLayout);
  }

  loaded = true;
}

bool ContentWidget::doLoad()
{
  // Most examples do their initialization in constructor.
  // Only those that can fail and show error message need to reimplement this method.
  return true;
}

void ContentWidget::resizeEvent(QResizeEvent *)
{
  if (_defaultChartView)
    _defaultChartView->resize(size());
}

// Most examples are simple and need only basic chart view widget, so provide it in this base class
// to avoid duplicating code
void ContentWidget::createDefaultChartView(QChart *chart)
{
  _defaultChartView = new QChartView(chart, this);
  _defaultChartView->setRenderHint(QPainter::Antialiasing);
}

void ContentWidget::setDefaultChartView(QChartView *view)
{
  _defaultChartView = view;
  _defaultChartView->setRenderHint(QPainter::Antialiasing);
}
