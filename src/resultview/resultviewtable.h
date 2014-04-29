#ifndef RESULTVIEWTABLE_H
#define RESULTVIEWTABLE_H

#include "../iconmanager.h"
#include "../dialogs/blobdialog.h"
#include "wizards/exportwizard.h"
#include "resultview/dataprovider.h"
#include "resultview/paginationwidget.h"
#include "resultview/sqlitemdelegate.h"

#include <QMenu>
#include <QSqlRecord>
#include <QStandardItemModel>
#include <QTableView>

class ResultViewTable : public QTableView {
Q_OBJECT
public:
  ResultViewTable(QWidget *parent = 0);

  void setDataProvider(DataProvider* dataProvider);
  void setPagination(PaginationWidget* pagination);

signals:
  void editRequested(bool);
  void rowLeaved(int);

public slots:
  void commit();
  void copy();
  void deleteRow();
  void exportContent();
  void insertRow();
  void resetColumnSizes();
  void resizeColumnsToContents();
  void rollback();
  void setRowsPerPage(int rpp);

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

  QAction* actionCopy;
  QAction* actionDetails;
  QAction* actionExport;

  BlobDialog* blobDialog;
  QList<int> columnSizes;
  QMenu* contextMenu;
  int currentEditedRow;
  DataProvider* dataProvider =0;
  ExportWizard* exportWizard;
  SqlItemDelegate* sqlItemDelegate;
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
