#ifndef CSVWIZARDPAGE_H
#define CSVWIZARDPAGE_H

#include "ui_csvwizardpage.h"

#include <QWizardPage>

class CsvWizardPage : public QWizardPage, Ui::CsvWizardPage {
Q_OBJECT
public:
  CsvWizardPage(QWidget *parent = 0);
};

#endif // CSVWIZARDPAGE_H
