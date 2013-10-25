#include "exportwizard.h"

#include "../iconmanager.h"
#include "../plugins/exportengine.h"
#include "../plugins/pluginmanager.h"

#include <QCompleter>
#include <QDirModel>
#include <QFileDialog>
#include <QMessageBox>

ExportWizard::ExportWizard(QAbstractItemModel *model, QWidget *parent)
  : QWizard(parent) {
  setWindowTitle(tr("Export"));

  this->model = model;

  setWindowIcon(IconManager::get("filesaveas"));

  setPage(0, new EwFirstPage(this));
  setPage(2, new EwExportPage(this->model, this));
}

void ExportWizard::setEngine(ExportEngine *e) {
  m_engine = e;
  if (e->wizardPage()) {
    if (page(1)) {
      removePage(1);
    }
    e->setWizard(this);
    e->setModel(model);
    setPage(1, e->wizardPage());
  }
}

/**
 * First page
 */
QString EwFirstPage::lastPath;

EwFirstPage::EwFirstPage(QWizard *parent)
    : QWizardPage(parent) {
  setupUi(this);

  formatLayout = new QGridLayout(formatGroupBox);
  formatGroupBox->setLayout(formatLayout);

  /* Objectif : buter la formatMap et en générer une dynamiquement dans
   * initialisePage. */

  registerField("path*", pathLineEdit);
  connect(browseButton, SIGNAL(clicked()), this, SLOT(browse()));

  pathLineEdit->setCompleter(new QCompleter(
      new QDirModel(QStringList("*"),
                    QDir::AllDirs | QDir::NoDotAndDotDot,
                    QDir::Type, this),
      this));

  pathLineEdit->setText(lastPath);
}

void EwFirstPage::browse() {
  lastPath = QFileDialog::getSaveFileName(this,
                                          tr("Output file"),
                                          QDir::homePath()
                                          );

  QString extension = QFileInfo(lastPath).suffix().toLower();
  // Si le nom de fichier contient une extension, on met à jour l'option choisie

    // L'extension n'a pas été reconnue : on va l'ajouter
    if (lastPath.endsWith(".")) {
      lastPath = lastPath.left(lastPath.length()-1);
    }


  pathLineEdit->setText(lastPath);
}

void EwFirstPage::changeEngine() {
  QRadioButton *btn = dynamic_cast<QRadioButton*>(sender());
  if (btn == NULL) {
    return;
  }

  ExportEngine *engine = formatMap[btn];

  QString path = pathLineEdit->text();
  foreach (ExportEngine *e, formatMap.values()) {
    if (path.endsWith(e->extension())) {
      int right = e->extension().length();
      path = path.left(path.length() - right);
      path.append(engine->extension());

      pathLineEdit->setText(path);
    }
  }
}

void EwFirstPage::initializePage() {
  // Nettoyage liste formats
  foreach (QRadioButton *r, formatMap.keys()) {
    r->disconnect();
    formatLayout->removeWidget(r);
    delete r;
  }
  formatMap.clear();

  QList<ExportEngine*> engines = PluginManager::exportEngines();
  bool left = false;
  int x = -1, y = 0;
  foreach (ExportEngine *e, engines) {
    e->setWizard(wizard());
    QRadioButton *btn = new QRadioButton(e->displayName());
    formatMap[btn] = e;
    if (x == -1) {
      btn->setChecked(true);
    }
    if (e->displayIconCode().length() > 0) {
      btn->setIcon(IconManager::get(e->displayIconCode()));
    }
    if (left) {
      y = 1;
    } else {
      x++;
      y = 0;
    }
    left = !left;
    formatLayout->addWidget(btn, x, y);

    connect(btn, SIGNAL(clicked()), this, SLOT(changeEngine()));
  }
}

int EwFirstPage::nextId() const {
  foreach (QRadioButton *r, formatMap.keys()) {
    if (r->isChecked()) {
      if (formatMap[r]->wizardPage()) {
        return 1;
      } else {
        return 2;
      }
    }
  }
  return 2;
}

bool EwFirstPage::validatePage() {
  foreach (QRadioButton *r, formatMap.keys()) {
    if (r->isChecked()) {
      ((ExportWizard*) wizard())->setEngine(formatMap[r]);
      break;
    }
  }
  return true;
}


/**
 * Export page
 */
EwExportPage::EwExportPage(QAbstractItemModel *model, QWizard *parent)
  : QWizardPage(parent) {
  setupUi(this);

  dial = new QProgressDialog(this);
  dial->setWindowTitle(tr("Export running..."));

  this->model = model;
}

void EwExportPage::checkProgress() {
  if (!finished) {
    dial->show();
  }
}

void EwExportPage::initializePage() {
  finished = false;
  dial->setMaximum(model->rowCount()-1);
  connect(this, SIGNAL(progress(int)), dial, SLOT(setValue(int)));
  QTimer::singleShot(1000, this, SLOT(checkProgress()));
  run();
}

void EwExportPage::run() {
  QFile f(field("path").toString());
  if (!f.open(QFile::WriteOnly)) {
    QMessageBox::critical(this,
                          tr("Openning error"),
                          tr("Unable to open the file %1.")
                            .arg(field("path").toString()),
                          QMessageBox::Ok);
    return;
  }

  ExportEngine *engine = ((ExportWizard*) wizard())->engine();
  connect(PluginManager::pluginObject(engine->plid()), SIGNAL(progress(int)),
          dial, SLOT(setValue(int)));
  engine->process(&f);

  f.close();
  finished = true;
  emit completeChanged();
}
