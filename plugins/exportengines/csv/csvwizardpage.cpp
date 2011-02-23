#include "csvwizardpage.h"

CsvWizardPage::CsvWizardPage(QWidget *parent)
  : QWizardPage(parent)
{
  setupUi(this);

  registerField("csvdelimiter", delimiterLineEdit);
  registerField("csvheader"   , headerCheckBox);
  registerField("csvseparator", separatorLineEdit);
}
