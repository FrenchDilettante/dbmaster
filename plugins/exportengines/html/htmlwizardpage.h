#ifndef HTMLWIZARDPAGE_H
#define HTMLWIZARDPAGE_H

#include "ui_htmlwizardpage.h"

#include <QWizardPage>

class HtmlWizardPage : public QWizardPage, Ui::HtmlWizardPage {
Q_OBJECT
public:
  HtmlWizardPage(QAbstractItemModel *model, QWidget *parent = 0);

  void initializePage();

  void setModel(QAbstractItemModel *m) { model = m; };

private:
  QAbstractItemModel *model;
};

#endif // HTMLWIZARDPAGE_H
