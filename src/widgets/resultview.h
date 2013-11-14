#ifndef RESULTVIEW_H
#define RESULTVIEW_H

#include "../dialogs/blobdialog.h"
#include "../wizards/exportwizard.h"
#include "ui_resultview.h"

#include <QAction>
#include <QContextMenuEvent>
#include <QSqlRecord>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QWidget>

class ResultView : public QWidget, Ui::ResultView {
Q_OBJECT
public:
  enum Mode {
    QueryMode, TableMode
  };

  enum Action {
    Browse, Update, Insert
  };

  ResultView(QWidget *parent=0);
  ~ResultView();

  void setQuery(QSqlQueryModel *queryModel);
  void setTable(QString table, QSqlDatabase *db);

  Mode mode()           { return m_mode;    };
  void setMode(Mode m);

  Q_PROPERTY(Mode mode READ mode WRITE setMode)

signals:
  void reloadRequested();
  void structureChanged();

public slots:
  void resizeColumnsToContents();
  void resizeRowsToContents();
  void setAlternatingRowColors(bool enable, bool loop =true);
  void scrollBegin();
  void scrollDown();
  void scrollEnd();
  void scrollUp();
  void sort(int col);

private:
  void setModel(QSqlQueryModel *model);
  void setupConnections();

  Action                currentAction;
  QPair<int, Qt::SortOrder> currentSorting;
  ExportWizard         *exportWizard;
  QMap<int, QSqlRecord> modifiedRecords;
  int                   lastEditedRow;
  QStandardItemModel   *shortModel;
  Mode                  m_mode;
  QSqlQueryModel       *model;

  int                   offset;

  static bool           alternateRows;
  static QList<ResultView*> instances;

private slots:
  void apply();
  void on_deleteButton_clicked();
  void exportContent();
  void on_reloadButton_clicked();
  void on_insertButton_clicked();
  void updateItem(QStandardItem *item);
  void updateView();
};

#endif // RESULTVIEW_H
