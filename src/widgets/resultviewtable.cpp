#include "resultviewtable.h"

#include <QContextMenuEvent>
#include <QDebug>

ResultViewTable::ResultViewTable(QWidget *parent)
  : QTableView(parent) {
  currentEditedRow = -1;
  setModel(0);

  blobDialog = new BlobDialog(this);

  setupMenus();
  setupConnections();
}

void ResultViewTable::contextMenuEvent(QContextMenuEvent *event) {
  if (event->reason() != QContextMenuEvent::Mouse
      || model() == 0) {
    return;
  }

  if (model()->columnCount() == 0 || model()->rowCount() == 0) {
    return;
  }

  actionDetails->setEnabled(selectedIndexes().size() == 1);

  contextMenu->move(event->globalPos());
  contextMenu->exec();
}

void ResultViewTable::selectionChanged(const QItemSelection &selected,
                                       const QItemSelection &deselected) {
  QTableView::selectionChanged(selected, deselected);

  QSet<int> rows;
  foreach (QModelIndex idx, selectedIndexes()) {
    rows << idx.row();
  }

  if (currentEditedRow == -1
      || rows.size() != 1
      || !rows.contains(currentEditedRow)) {
    currentEditedRow = rows.size() > 0 ? rows.toList()[0] : -1;
    emit rowLeaved(currentEditedRow);
  }
}

void ResultViewTable::setAlternatingRowColors(bool enable) {
   actionAlternateColor->setEnabled(enable);

   QTableView::setAlternatingRowColors(enable);
}

void ResultViewTable::setupConnections() {
  connect(actionAlternateColor, SIGNAL(toggled(bool)),
          this, SIGNAL(alternateRowsRequested()));
  connect(actionDetails, SIGNAL(triggered()), this, SLOT(showBlob()));
  connect(actionExport, SIGNAL(triggered()), this, SIGNAL(exportRequested()));
}

void ResultViewTable::setupMenus() {
  contextMenu = new QMenu(this);

  actionDetails = new QAction(tr("Details"), this);
  actionDetails->setEnabled(false);
  contextMenu->addAction(actionDetails);

  actionAlternateColor = new QAction(tr("Alternate row colors"), this);
  actionAlternateColor->setCheckable(true);
  actionAlternateColor->setChecked(false);
  contextMenu->addAction(actionAlternateColor);

  actionExport = new QAction(tr("Export"), this);
  actionExport->setIcon(IconManager::get("document-save-as"));
  actionExport->setShortcut(QKeySequence("Ctrl+E"));
  contextMenu->addAction(actionExport);
}

void ResultViewTable::showBlob() {
  if (selectedIndexes().size() != 1) {
    return;
  }

  QVariant blob = selectedIndexes().at(0).data();
  blobDialog->setBlob(blob);
  blobDialog->show();
}
