/*
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 */


#include "exportwizard.h"

#include "../iconmanager.h"
#include "../plugins/exportengine.h"
#include "../plugins/pluginmanager.h"

ExportWizard::ExportWizard(QueryToken *token, QWidget *parent)
  : QWizard(parent)
{
  setWindowTitle(tr("Export"));

  this->token = token;

  setWindowIcon(IconManager::get("filesaveas"));

  addPage(new EwFirstPage(this));
}

/**
 * First page
 */
QString EwFirstPage::lastPath;

EwFirstPage::EwFirstPage(QWizard *parent)
    : QWizardPage(parent)
{
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

void EwFirstPage::browse()
{
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

void EwFirstPage::initializePage() {
  // Nettoyage liste formats
  for (int i=formatLayout->count() - 1; i>=0; i--) {
    QWidget *w = formatLayout->itemAt(i)->widget();
    if (w) {
      w->disconnect();
      formatLayout->removeWidget(w);
      delete w;
    }
  }

  QList<ExportEngine*> engines = PluginManager::exportEngines();
  bool left = false;
  int x = -1, y = 0;
  foreach (ExportEngine *e, engines) {
    QRadioButton *btn = new QRadioButton(e->displayName());
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
  }
}

int EwFirstPage::nextId() const {
  return 0;
}


/**
 * Export page
 */
EwExportPage::EwExportPage(QueryToken *token, QWizard *parent)
  : QWizardPage(parent)
{
  setupUi(this);

  this->token = token;
}

void EwExportPage::checkProgress()
{
  if(!finished)
    dial->show();
}

/**
 * This is a common function to all formats. The conversion must obey to some
 * rules, see the documentation for more information.
 */
void EwExportPage::initializePage()
{
  finished = false;
  dial = new QProgressDialog(this);
  dial->setWindowTitle(tr("Export running..."));
  dial->setMaximum(token->model()->rowCount()-1);
  connect(this, SIGNAL(progress(int)), dial, SLOT(setValue(int)));
  QTimer::singleShot(1000, this, SLOT(checkProgress()));
  run();
}

void EwExportPage::run()
{
  QFile f(field("path").toString());
  if( !f.open( QFile::WriteOnly ) )
  {
    QMessageBox::critical( this,
                           tr( "Openning error" ),
                           tr( "Unable to open the file %1." )
                             .arg( field( "path" ).toString() ),
                           QMessageBox::Ok );
    return;
  }

  // Ici, mettre l'appel vers le moteur d'export.

  f.close();
  finished = true;
  emit completeChanged();
}
