#include "resultviewtable.h"

#include <QDebug>

ResultViewTable::ResultViewTable(QWidget *parent)
  : QTableView(parent) {
  currentEditedRow = -1;
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
