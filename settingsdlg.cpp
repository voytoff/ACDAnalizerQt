#include "settingsdlg.h"
#include "ui_settingsdlg.h"
#include <QVariant>
#include <QMetaEnum>

SettingsDlg::SettingsDlg(Settings* settings, QWidget *parent)
  : QDialog(parent)
  , ui(new Ui::SettingsDlg)
  , settings(settings) {

  ui->setupUi(this);

  ui->frequency->addItem("1", 1);
  ui->frequency->addItem("10", 10);
  ui->frequency->addItem("100", 100);
  ui->frequency->setCurrentText(QString::number(settings->frequency()));

  ui->axisXType->addItem("Индекс", AxisXType::Index);
  ui->axisXType->addItem("Время", AxisXType::Time);
  ui->axisXType->setCurrentIndex(settings->axisXType()-1);

  ui->colorScheme->addItem("Light", ColorScheme::Light);
  ui->colorScheme->addItem("Dark", ColorScheme::Dark);
  ui->colorScheme->setCurrentIndex(settings->colorScheme()-1);

  ui->nativeDlg->setChecked(settings->nativeDlg());

  connect(this, &QDialog::finished, this, [this](int result) { accept(result); });

  setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
}

SettingsDlg::~SettingsDlg() {
  delete ui;
}

void SettingsDlg::accept(const int result) {
  if (result == QDialog::Accepted) {
    settings->frequency(ui->frequency->currentData());
    settings->axisXType(ui->axisXType->currentData());
    settings->colorScheme(ui->colorScheme->currentData());
    settings->nativeDlg(ui->nativeDlg->isChecked());
  }
}
