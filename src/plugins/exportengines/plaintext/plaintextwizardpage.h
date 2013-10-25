#ifndef PLAINTEXTWIZARDPAGE_H
#define PLAINTEXTWIZARDPAGE_H

#include "ui_plaintextwizardpage.h"

class PlainTextWizardPage : public QWizardPage, private Ui::PlainTextWizardPage
{
Q_OBJECT
public:
  PlainTextWizardPage();

  void initializePage();
  void reload();
  void updatePreview();

private:
};

#endif // PLAINTEXTWIZARDPAGE_H
