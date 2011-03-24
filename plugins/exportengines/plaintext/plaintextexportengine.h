#ifndef PLAINTEXTEXPORTENGINE_H
#define PLAINTEXTEXPORTENGINE_H

#include "exportengine.h"

#include <QApplication>
#include <QObject>

class PlainTextExportEngine : public QObject, public ExportEngine {
Q_OBJECT
Q_INTERFACES(ExportEngine)
public:
  PlainTextExportEngine();

  // Fonctions de Plugin
  QString plid() { return "DBM.PLAINTEXT.EXPORTENGINE"; };
  QString title() { return tr("Plain text export engine"); };
  QString vendor() { return "DbMaster"; };
  QString version() { return QApplication::applicationVersion(); };

  // Fonctions de ExportEngine
  QString displayIconCode() { return "text"; };
  QString displayName() { return tr("Plain text"); };
  QString extension() { return "txt"; };
  void process(QFile *f);

  void setModel(QAbstractItemModel *m) { model = m; };
  void setWizard(QWizard *w) { wizard = w ;};
  QWizardPage *wizardPage() { return m_wizardPage; };

signals:
  void progress(int);

public slots:

};

#endif // PLAINTEXTEXPORTENGINE_H
