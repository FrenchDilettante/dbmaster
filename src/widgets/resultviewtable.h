#ifndef RESULTVIEWTABLE_H
#define RESULTVIEWTABLE_H

#include "../iconmanager.h"
#include "../dialogs/blobdialog.h"

#include <QMenu>
#include <QTableView>

class ResultViewTable : public QTableView {
Q_OBJECT
public:
  ResultViewTable(QWidget *parent = 0);

  void setAlternatingRowColors(bool enable);

signals:
  void alternateRowsRequested(bool);
  void exportRequested();
  void rowLeaved(int);

public slots:

protected:
  void contextMenuEvent(QContextMenuEvent *event);
  void selectionChanged(const QItemSelection &selected,
                        const QItemSelection &deselected);

private:
  void setupConnections();
  void setupMenus();

  QAction              *actionAlternateColor;
  QAction              *actionDetails;
  QAction              *actionExport;
  BlobDialog           *blobDialog;
  QMenu                *contextMenu;
  int                   currentEditedRow;

private slots:
  void showBlob();
};

#endif // RESULTVIEWTABLE_H
