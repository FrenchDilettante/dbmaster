#include "htmlwizardpage.h"

HtmlWizardPage::HtmlWizardPage(QAbstractItemModel *model, QWidget *parent)
  : QWizardPage(parent) {
  setupUi(this);
  this->model = model;

  registerField("htmlexportquery",  queryGroupBox,
                "checked", SIGNAL(toggled(bool)));
  registerField("htmlsyntax",       shCheckBox);
  registerField("htmlwholeresult",  wholeResultRadioButton);
  registerField("htmllimit",        limitSpinBox);
  registerField("htmllimitto",      limitComboBox);
}

void HtmlWizardPage::initializePage() {
  limitResultRadioButton->setChecked(model->rowCount() >= 100);
}
