#include "plaintextexportengine.h"

#include "plaintextwizardpage.h"

PlainTextExportEngine::PlainTextExportEngine() {
  m_wizardPage = new PlainTextWizardPage();
}

void PlainTextExportEngine::process(QFile *f) {
  emit progress(1);
}

