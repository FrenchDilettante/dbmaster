#ifndef NEWDBWIZARD_H
#define NEWDBWIZARD_H

#include "ui_newdbwizard.h"
#include "ui_ndw_firstpage.h"
#include "ui_ndw_secondpage.h"

class NewDbWizard : public QWizard, Ui::NewDbWizard {
Q_OBJECT
public:
  enum Pages {
    FirstPage,
    SecondPage,
    OptionsPage
  };
  NewDbWizard(QWidget *parent = 0);

public slots:
  void accept();

protected:
  void setupPages();
};

class NdwFirstPage : public QWizardPage, Ui::NdwFirstPage {
Q_OBJECT
public:
  NdwFirstPage(QWizard *parent=0);
  void initializePage();
  bool isComplete() const;

private:
  bool odbcAvailable;

private slots:
  void on_dbTypeComboBox_currentIndexChanged(int index);
};

class NdwSecondPage : public QWizardPage, Ui::NdwSecondPage {
Q_OBJECT
public:
  NdwSecondPage(QWizard *parent=0);
  void initializePage();

public slots:
  void browse();

protected slots:
  void test();
  void updateAlias();
};

#endif // NEWDBWIZARD_H
