#include "psqlconfig.h"
#include "psqlwrapper.h"

PsqlConfig::PsqlConfig(Plugin *wrapper)
  : QDialog(NULL) {
  setupUi(this);

  this->wrapper = wrapper;

  infoschemaCheckbox->setChecked(
        ((PsqlWrapper*) wrapper)->informationSchemaHidden);
  pgcatalogCheckbox->setChecked(
        ((PsqlWrapper*) wrapper)->pgCatalogHidden);
}

void PsqlConfig::accept() {
  ((PsqlWrapper*) wrapper)->informationSchemaHidden =
      infoschemaCheckbox->isChecked();

  ((PsqlWrapper*) wrapper)->pgCatalogHidden =
      pgcatalogCheckbox->isChecked();

  ((PsqlWrapper*) wrapper)->save();

  QDialog::accept();
}
