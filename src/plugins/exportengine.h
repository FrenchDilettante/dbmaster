#ifndef EXPORTENGINE_H
#define EXPORTENGINE_H

#include "plugin.h"

#include <QAbstractItemModel>
#include <QApplication>
#include <QFile>
#include <QWizard>

class ExportEngine : public Plugin {
public:
  /**
   * Nom du format à afficher, par ex. CSV, HTML, Document PDF, etc.
   */
  virtual QString displayName() =0;
  /**
   * L'extention du fichier qui sera généré (csv, html, etc.)
   */
  virtual QString extension() =0;
  virtual QString displayIconCode() { return ""; };
  /**
   * Traitement de l'export : fonction threadée.
   */
  virtual void process(QFile *f) =0;

  virtual void setModel(QAbstractItemModel *m) =0;
  virtual void setWizard(QWizard *w) =0;
  virtual QWizardPage *wizardPage() =0;

protected:

  QAbstractItemModel *model;
  QWizard *wizard;
  QWizardPage *m_wizardPage;
};

Q_DECLARE_INTERFACE(ExportEngine, "dbmaster.ExportEngine")

#endif // EXPORTENGINE_H
