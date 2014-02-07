#ifndef RESULTVIEW_H
#define RESULTVIEW_H

#include "../dialogs/blobdialog.h"
#include "../resultview/dataprovider.h"
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
  enum Action {
    Browse, Update, Insert
  };

  ResultView(QWidget *parent=0);
  ~ResultView();

  void setDataProvider(DataProvider* dataProvider);

signals:
  void reloadRequested();
  void structureChanged();

public slots:
  void reload();
  void resizeColumnsToContents();
  void resizeRowsToContents();
  void setAlternatingRowColors(bool enable, bool loop =true);
  void scrollBegin();
  void scrollDown();
  void scrollEnd();
  void scrollUp();
  void sort(int col);

private:
  void checkReadOnly();
  int endIndex(int start, QSqlQueryModel* model);
  void setupButtons();
  void setupConnections();
  int startIndex(QSqlQueryModel* model);
  void updatePageCount(int start, int end, QSqlQueryModel *model);
  void updateVerticalLabels(int start, int end);
  void updateViewHeader(QSqlQueryModel *model);
  QStandardItem* viewItem(QVariant value);
  QList<QStandardItem*> viewRow(int rowIdx, QSqlQueryModel *model);

  Action                currentAction;
  QPair<int, Qt::SortOrder> currentSorting;
  DataProvider* dataProvider =0;
  ExportWizard         *exportWizard;
  QMap<int, QSqlRecord> modifiedRecords;
  int                   lastEditedRow;
  QStandardItemModel   *shortModel;

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
