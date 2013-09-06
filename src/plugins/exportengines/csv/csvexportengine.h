#ifndef CSVEXPORTENGINE_H
#define CSVEXPORTENGINE_H

#include "../../exportengine.h"

#include <QApplication>
#include <QObject>

class CsvExportEngine : public QObject, public ExportEngine {
Q_OBJECT
Q_INTERFACES(ExportEngine)
public:
  CsvExportEngine();

  // Fonctions de Plugin
  QString plid() { return "DBM.CSV.EXPORTENGINE"; };
  QString title() { return tr("CSV export engine"); };
  QString vendor() { return "DbMaster"; };
  QString version() { return QApplication::applicationVersion(); };

  // Fonctions de ExportEngine
  QString displayIconCode() { return "spreadsheet"; };
  QString displayName() { return tr("CSV"); };
  QString extension() { return "csv"; };
  void process(QFile *f);

  void setModel(QAbstractItemModel *m) { model = m; };
  void setWizard(QWizard *w) { wizard = w; };
  QWizardPage *wizardPage() { return m_wizardPage; };

signals:
  void progress(int);

protected:
};

#endif // CSVEXPORTENGINE_H
