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


#ifndef EXPORTWIZARD_H
#define EXPORTWIZARD_H

#include <QtGui>

#include "../query/querytoken.h"
#include "../plugins/exportengine.h"

#include "ui_ew_firstpage.h"
#include "ui_ew_exportpage.h"

class ExportWizard : public QWizard {
Q_OBJECT
public:
  ExportWizard(QueryToken *token, QWidget *parent =0);

  QueryToken *token;

  ExportEngine *engine() { return m_engine; };
  QAbstractItemModel *model() { return token->model(); };
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
};


class EwExportPage : public QWizardPage, Ui::EwExportPage, QRunnable
{
Q_OBJECT
public:
  EwExportPage(QueryToken *token, QWizard *parent =0);
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
  QueryToken         *token;
};

#endif // EXPORTWIZARD_H
