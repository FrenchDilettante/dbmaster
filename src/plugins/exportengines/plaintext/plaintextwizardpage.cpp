#include "plaintextwizardpage.h"

#include "wizards/exportwizard.h"

PlainTextWizardPage::PlainTextWizardPage()
  : QWizardPage() {
  setupUi(this);
}

void PlainTextWizardPage::initializePage() {
  reload();
}

void PlainTextWizardPage::reload() {
  columnsTree->clear();

  QAbstractItemModel* model = ((ExportWizard*) wizard())->model;

  for (int i=0; i<model->columnCount(); i++) {
    QStringList item;
    item << model->headerData(i, Qt::Horizontal).toString()
         << QString::number(10)
         << QString::number(10 * i);
    columnsTree->addTopLevelItem(new QTreeWidgetItem(item));
  }

  updatePreview();
}

void PlainTextWizardPage::updatePreview() {
  /*
  int nb = model->rowCount() < 5 ? model->rowCount() : 5;
  for (int i=0; i<nb; i++) {
    //
  }
  */
}
