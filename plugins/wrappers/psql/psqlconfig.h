#ifndef PSQLCONFIG_H
#define PSQLCONFIG_H

#include "ui_psqlconfig.h"
#include "plugin.h"

class PsqlConfig : public QDialog, private Ui::PsqlConfig {
Q_OBJECT

public:
  PsqlConfig(Plugin *wrapper);

public slots:
  void accept();

private:
  Plugin *wrapper;
};

#endif // PSQLCONFIG_H
