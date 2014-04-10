#ifndef RESULTVIEWTABLE_H
#define RESULTVIEWTABLE_H

#include "../iconmanager.h"
#include "../dialogs/blobdialog.h"
#include "resultview/dataprovider.h"
#include "resultview/paginationwidget.h"

#include <QMenu>
#include <QSqlRecord>
#include <QStandardItemModel>
#include <QTableView>

class ResultViewTable : public QTableView {
Q_OBJECT
public:
  ResultViewTable(QWidget *parent = 0);

  void setAlternatingRowColors(bool enable);
  void setDataProvider(DataProvider* dataProvider);
  void setPagination(PaginationWidget* pagination);

signals:
  void alternateRowsRequested(bool);
  void editRequested(bool);
  void exportRequested();
  void rowLeaved(int);

public slots:
  void commit();
  void copy();
  void deleteRow();
  void insertRow();
  void resetColumnSizes();
  void resizeColumnsToContents();
  void rollback();

  void firstPage();
  void lastPage();
  void nextPage();
  void previousPage();

protected:
  void contextMenuEvent(QContextMenuEvent *event);
  void selectionChanged(const QItemSelection &selected,
                        const QItemSelection &deselected);

private:
  int endIndex(int start);
  void populateShortModel();
  void setupConnections();
  void setupMenus();
  int startIndex();
  void updatePagination();
  void updateVerticalLabels(int start, int end);
  void updateViewHeader();
  QStandardItem* viewItem(QVariant value);
  QList<QStandardItem*> viewRow(int rowIdx);

  QAction* actionAlternateColor;
  QAction* actionCopy;
  QAction* actionDetails;
  QAction* actionExport;

  BlobDialog* blobDialog;
  QList<int> columnSizes;
  QMenu* contextMenu;
  int currentEditedRow;
  DataProvider* dataProvider =0;
  QMap<int, QSqlRecord> modifiedRecords;
  QStandardItemModel *shortModel;
  bool showInsertRow = false;

  PaginationWidget* pagination;
  int page = 0;
  int rowsPerPage = 20;

private slots:
  void showBlob();
  void updateItem(QStandardItem *item);
  void updateView();
};

#endif // RESULTVIEWTABLE_H
