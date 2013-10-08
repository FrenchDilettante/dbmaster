#ifndef EXPORTWIZARD_H
#define EXPORTWIZARD_H

#include "../plugins/exportengine.h"

#include "ui_ew_firstpage.h"
#include "ui_ew_exportpage.h"

#include <QtGui>
#include <QSqlQueryModel>

class ExportWizard : public QWizard {
Q_OBJECT
public:
  ExportWizard(QAbstractItemModel *model, QWidget *parent =0);

  QAbstractItemModel *model;

  ExportEngine *engine() { return m_engine; };
  void setEngine(ExportEngine *e);

private:
  ExportEngine *m_engine;
};


class EwFirstPage : public QWizardPage, Ui::EwFirstPage {
Q_OBJECT
public:
  EwFirstPage(QWizard *parent=0);

  void initializePage();
  int nextId() const;
  bool validatePage();

public slots:
  void browse();

private:
  static QString lastPath;
  QMap<QRadioButton*, ExportEngine*> formatMap;
  QGridLayout *formatLayout;

private slots:
  void changeEngine();
};


class EwExportPage : public QWizardPage, Ui::EwExportPage, QRunnable
{
Q_OBJECT
public:
  EwExportPage(QAbstractItemModel *model, QWizard *parent =0);
  //bool isComplete();
  void initializePage();

signals:
  void progress(int);

protected:
  void run();

private slots:
  void checkProgress();

private:
  QProgressDialog    *dial;
  bool                finished;
  QAbstractItemModel *model;
};

#endif // EXPORTWIZARD_H
