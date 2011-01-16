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
   * Nom du format, par ex. CSV, HTML, PDF, etc.
   */
  virtual QString formatName() =0;

  virtual void setModel(QAbstractItemModel *m) =0;
  virtual void setWizard(QWizard *w) =0;
  virtual QWizardPage *wizardPage() =0;

protected:
  /**
   * Traitement de l'export : fonction threadée.
   */
  virtual void process(QFile &f) =0;

  QAbstractItemModel *model;
  QWizard *wizard;
  QWizardPage *m_wizardPage;
};

Q_DECLARE_INTERFACE(ExportEngine, "dbmaster.ExportEngine/0.8")

#endif // EXPORTENGINE_H
