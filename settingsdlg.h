#ifndef SETTINGSDLG_H
#define SETTINGSDLG_H

#include "settings.h"
#include <QDialog>

namespace Ui {
class SettingsDlg;
}

class SettingsDlg : public QDialog
{
  Q_OBJECT

public:
  explicit SettingsDlg(QWidget *parent = nullptr);
  ~SettingsDlg();

private:
  void accept(const int result);
  Ui::SettingsDlg *ui;
  Settings settings;

};

#endif // SETTINGSDLG_H
