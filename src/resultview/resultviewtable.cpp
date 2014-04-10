#include "resultviewtable.h"

#include <QClipboard>
#include <QContextMenuEvent>
#include <QDebug>
#include <QMimeData>
#include <QScrollBar>
#include <QSqlRecord>

ResultViewTable::ResultViewTable(QWidget *parent)
  : QTableView(parent) {
  currentEditedRow = -1;
  setModel(0);

  blobDialog = new BlobDialog(this);
  shortModel = new QStandardItemModel(this);
  setModel(shortModel);

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

  actionCopy->setEnabled(selectedIndexes().size() > 0);
  actionDetails->setEnabled(selectedIndexes().size() == 1);

  contextMenu->move(event->globalPos());
  contextMenu->exec();
}

void ResultViewTable::copy() {

  if (selectedIndexes().size() == 0) {
    return;
  }

  if (selectedIndexes().size() == 1) {
    QApplication::clipboard()->setText(selectedIndexes()[0].data().toString());
    return;
  }

  QString text = "<table>";

  QMap<int, QString> data;

  foreach (QModelIndex idx, selectedIndexes()) {
    if (!data.contains(idx.row())) {
      data[idx.row()] += "<tr>";
    }

    data[idx.row()] += "<td>" + idx.data().toString() + "</td>";
  }

  foreach (int line, data.keys()) {
    data[line] += "</tr>";
    text += data[line];
  }

  text += "</table>";

  QMimeData *mime = new QMimeData();
  mime->setHtml(text);
  QApplication::clipboard()->setMimeData(mime);
}

int ResultViewTable::endIndex(int start) {
  int end = start + rowsPerPage;
  if (end > dataProvider->model()->rowCount()) {
    end = dataProvider->model()->rowCount();
  }
  return end;
}

void ResultViewTable::firstPage() {
  page = 0;
  updateView();
}

void ResultViewTable::lastPage() {
  page = (int) dataProvider->model()->rowCount() / rowsPerPage;
  updateView();
}

void ResultViewTable::nextPage() {
  if ((page + 1) * rowsPerPage < dataProvider->model()->rowCount()) {
    page++;
    updateView();
  } else {
    lastPage();
  }
}

void ResultViewTable::previousPage() {
  if (page > 0) {
    page--;
    updateView();
  } else {
    firstPage();
  }
}

void ResultViewTable::populateShortModel() {
  int start = startIndex();
  int end = endIndex(start);

  for (int i=start; i<end; i++) {
    shortModel->appendRow(viewRow(i));
  }
  updateVerticalLabels(start, end);
}

void ResultViewTable::resetColumnSizes() {
  columnSizes = QList<int>();
}

void ResultViewTable::resizeColumnsToContents() {
  QTableView::resizeColumnsToContents();

  if (columnSizes.size() != model()->columnCount()) {
    // si le nombre de colonnes ne correspond pas : on remplit
    columnSizes.clear();
    for (int i=0; i<model()->columnCount(); i++) {
      columnSizes.append(columnWidth(i));
    }
  } else {
    // Pour chaque colonne, on change la taille ssi la nouvelle est plus petite
    // que l'ancienne. Sinon, on stocke.
    for (int i=0; i<model()->columnCount(); i++) {
      if (columnSizes[i] > columnWidth(i) && i < columnSizes.size()) {
        setColumnWidth(i, columnSizes[i]);
      } else {
        columnSizes[i] = columnWidth(i);
      }
    }
  }
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
   actionAlternateColor->setChecked(enable);

   QTableView::setAlternatingRowColors(enable);
}

void ResultViewTable::setDataProvider(DataProvider *dataProvider) {
  this->dataProvider = dataProvider;

  this->page = 0;

  updateView();
  connect(dataProvider, SIGNAL(complete()), this, SLOT(updateView()));
}

void ResultViewTable::setPagination(PaginationWidget *pagination) {
  this->pagination = pagination;

  connect(this->pagination, SIGNAL(first()), this, SLOT(firstPage()));
  connect(this->pagination, SIGNAL(previous()), this, SLOT(previousPage()));
  connect(this->pagination, SIGNAL(next()), this, SLOT(nextPage()));
  connect(this->pagination, SIGNAL(last()), this, SLOT(lastPage()));
}

void ResultViewTable::setupConnections() {
  connect(actionAlternateColor, SIGNAL(toggled(bool)),
          this, SIGNAL(alternateRowsRequested(bool)));
  connect(actionCopy, SIGNAL(triggered()), this, SLOT(copy()));
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

  contextMenu->addSeparator();

  actionCopy = new QAction(tr("Copy"), this);
  actionCopy->setIcon(IconManager::get("edit-copy"));
  contextMenu->addAction(actionCopy);

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

int ResultViewTable::startIndex() {
  int start = this->page * this->rowsPerPage;
  if (start > dataProvider->model()->rowCount()) {
    start = dataProvider->model()->rowCount() - rowsPerPage;
  }
  if (start < 0) {
    start = 0;
  }
  return start;
}

void ResultViewTable::updatePagination() {
  pagination->setPage(page, (int) dataProvider->model()->rowCount() / rowsPerPage);
  pagination->setRowsPerPage(rowsPerPage);
}

void ResultViewTable::updateVerticalLabels(int start, int end) {
  QStringList vlabels;
  for (int i=start; i<end; i++) {
    vlabels << QString::number(i+1);
  }
  /*
  if (currentAction == Insert) {
    vlabels.removeLast();
    vlabels << "*";
  }
  */
  shortModel->setVerticalHeaderLabels(vlabels);
}

void ResultViewTable::updateView() {
  updatePagination();
  resetColumnSizes();

  int hpos = horizontalScrollBar()->value();
  int vpos = verticalScrollBar()->value();

  shortModel->clear();

  if (!dataProvider) {
    return;
  }

  updateViewHeader();

  if (dataProvider->model()->rowCount() == 0) {
    return;
  }

  populateShortModel();

  resizeColumnsToContents();
  resizeRowsToContents();

  horizontalScrollBar()->setValue(hpos);
  verticalScrollBar()->setValue(vpos);
}

void ResultViewTable::updateViewHeader() {
  for (int i=0; i<dataProvider->model()->columnCount(); i++) {
    shortModel->setHorizontalHeaderItem(i, new QStandardItem(
        dataProvider->model()->headerData(i, Qt::Horizontal).toString()));
  }
}

QStandardItem* ResultViewTable::viewItem(QVariant value) {
  QStandardItem* item = new QStandardItem();
  item->setData(value, Qt::DisplayRole);
  item->setData(false, Qt::UserRole);
  item->setEditable(!dataProvider->isReadOnly());
  return item;
}

QList<QStandardItem*> ResultViewTable::viewRow(int rowIdx) {
  QList<QStandardItem*> row;
  QSqlRecord r = dataProvider->model()->record(rowIdx);
  for (int j=0; j<dataProvider->model()->columnCount(); j++) {
    row << viewItem(r.value(j));
  }
  return row;
}
