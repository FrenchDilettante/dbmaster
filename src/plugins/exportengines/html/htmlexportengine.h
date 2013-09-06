#ifndef HTMLEXPORTENGINE_H
#define HTMLEXPORTENGINE_H

#include "../../exportengine.h"

#include <QApplication>
#include <QObject>

class HtmlExportEngine : public QObject, public ExportEngine {
Q_OBJECT
Q_INTERFACES(ExportEngine)
public:
  HtmlExportEngine();

  // Fonctions de Plugin
  QString plid() { return "DBM.HTML.EXPORTENGINE"; };
  QString title() { return tr("HTML export engine"); };
  QString vendor() { return "DbMaster"; };
  QString version() { return QApplication::applicationVersion(); };

  // Fonctions de ExportEngine
  QString displayIconCode() { return "html"; };
  QString displayName() { return tr("HTML"); };
  QString extension() { return "html"; };
  void process(QFile *f);

  void setModel(QAbstractItemModel *m);
  void setWizard(QWizard *w) { wizard = w; };
  QWizardPage *wizardPage() { return m_wizardPage; };

signals:
  void progress(int);

protected:
};

#endif // HTMLEXPORTENGINE_H
