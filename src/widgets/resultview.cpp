#include "resultview.h"

#include "../iconmanager.h"

#include <QDebug>
#include <QMessageBox>
#include <QModelIndex>
#include <QScrollBar>
#include <QSqlError>
#include <math.h>

bool ResultView::alternateRows = false;
QList<ResultView*> ResultView::instances = QList<ResultView*>();

ResultView::ResultView(QWidget *parent)
  : QWidget(parent) {
  setupUi(this);

  exportWizard = new ExportWizard(this);

  currentAction = Browse;
  currentSorting = QPair<int, Qt::SortOrder>(-1, Qt::AscendingOrder);
  instances << this;
  offset = 0;
  shortModel = new QStandardItemModel(this);
  table->setModel(shortModel);
  table->setAlternatingRowColors(alternateRows);

  setupConnections();

  firstPageButton->setIcon(IconManager::get("go-first"));
  lastPageButton->setIcon(IconManager::get("go-last"));
  nextPageButton->setIcon(IconManager::get("go-next"));
  prevPageButton->setIcon(IconManager::get("go-previous"));
  reloadButton->setIcon(IconManager::get("view-refresh"));
  insertButton->setIcon(IconManager::get("list-add"));
  deleteButton->setIcon(IconManager::get("list-remove"));

  insertButton->setVisible(false);
  deleteButton->setVisible(false);
}

ResultView::~ResultView() {
  instances.removeAll(this);
}

void ResultView::apply() {
  if (modifiedRecords.size() == 0) {
    return;
  }

  insertButton->setIcon(IconManager::get("list-add"));
  deleteButton->setIcon(IconManager::get("list-remove"));

  QSqlTableModel *tmodel = (QSqlTableModel*) dataProvider->model();

  foreach (int row, modifiedRecords.keys()) {
    tmodel->setRecord(row, modifiedRecords[row]);
  }

  modifiedRecords.clear();

  if (!tmodel->submitAll()) {
    QMessageBox::critical(this, "Error", tmodel->lastError().text());
  }

  currentAction = Browse;
  tmodel->select();
  updateView();
}

void ResultView::exportContent() {
  if (!dataProvider) {
    return;
  }

  QSqlQueryModel* model = dataProvider->model();

  if (model->columnCount() == 0 || model->rowCount() == 0) {
    return;
  }

  exportWizard->setModel(model);
  exportWizard->exec();
}

/**
 * Clic sur le bouton supprimer : peut annuler l'opération en cours ou supprimer
 * la ligne en cours.
 */
void ResultView::on_deleteButton_clicked()
{
  if (dataProvider->isReadOnly()) {
    return;
  }

  QSqlTableModel *tmodel = (QSqlTableModel*) dataProvider->model();

  if (currentAction != Browse) {
    // Annulation de l'opération en cours
    insertButton->setIcon(IconManager::get("list-add"));
    deleteButton->setIcon(IconManager::get("list-remove"));

    currentAction = Browse;
    tmodel->revertAll();
  } else {
    // suppression des lignes sélectionnées
    QSet<int> rows;
    foreach (QModelIndex i, table->selectionModel()->selectedIndexes()) {
      rows << (i.row() + offset);
    }

    foreach (int r, rows) {
      tmodel->removeRow(r);
    }

    tmodel->submitAll();
  }

  updateView();
}

/**
 * Clic sur le bouton d'insertion : peut passer en mode insertion ou valider un
 * ajout/modif.
 */
void ResultView::on_insertButton_clicked() {
  if (dataProvider->isReadOnly()) {
    return;
  }

  if (currentAction == Insert || currentAction == Update) {
    apply();
  } else {
    insertButton->setIcon(QIcon());
    insertButton->setText(tr("Apply"));
    deleteButton->setIcon(QIcon());
    deleteButton->setText(tr("Cancel"));

    dataProvider->model()->insertRow(dataProvider->model()->rowCount());
    currentAction = Insert;
    updateView();
    table->scrollToBottom();
//    table->hold();
  }
}

void ResultView::on_reloadButton_clicked() {
  dataProvider->start();
}

void ResultView::reload() {
  dataProvider->start();
}

void ResultView::resizeColumnsToContents() {
  table->resizeColumnsToContents();
}

void ResultView::resizeRowsToContents() {
  table->resizeRowsToContents();
}

void ResultView::scrollBegin() {
  offset = 0;
  updateView();
}

void ResultView::scrollDown() {
  offset += resultSpinBox->value();
  updateView();
}

void ResultView::scrollEnd() {
  double last;
  modf(dataProvider->model()->rowCount() / resultSpinBox->value(), &last);

  offset = last * resultSpinBox->value();
  updateView();
}

void ResultView::scrollUp() {
  offset -= resultSpinBox->value();
  updateView();
}

void ResultView::setAlternatingRowColors(bool enable, bool loop) {
  alternateRows = enable;
  table->setAlternatingRowColors(enable);

  if (loop) {
    foreach (ResultView *v, instances) {
      if (dynamic_cast<ResultView*>(v) && v != this) {
        v->setAlternatingRowColors(enable, false);
      }
    }
  }
}

void ResultView::setDataProvider(DataProvider* dataProvider) {
  this->dataProvider = dataProvider;

  offset = 0;

  reloadButton->setEnabled(true);
  resultSpinBox->setEnabled(true);

  bool readOnly = dataProvider->isReadOnly();
  insertButton->setVisible(!readOnly);
  deleteButton->setVisible(!readOnly);
  table->setSortingEnabled(!readOnly);

  updateView();

  connect(dataProvider, SIGNAL(complete()), this, SLOT(updateView()));
}

/*


void ResultView::setQuery(QSqlQueryModel *queryModel) {
  setMode(QueryMode);
  offset = 0;

  setModel(queryModel);
}

void ResultView::setTable(QString table, QSqlDatabase *db) {
}*/

void ResultView::setupConnections() {
  connect(firstPageButton, SIGNAL(clicked()), this, SLOT(scrollBegin()));
  connect(prevPageButton, SIGNAL(clicked()), this, SLOT(scrollUp()));
  connect(nextPageButton, SIGNAL(clicked()), this, SLOT(scrollDown()));
  connect(lastPageButton, SIGNAL(clicked()), this, SLOT(scrollEnd()));

  connect(resultSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateView()));

  connect(shortModel, SIGNAL(itemChanged(QStandardItem*)),
          this, SLOT(updateItem(QStandardItem*)));

  connect(table, SIGNAL(rowLeaved(int)), this, SLOT(apply()));
  connect(table->horizontalHeader(), SIGNAL(sectionClicked(int)),
          this, SLOT(sort(int)));
  connect(table, SIGNAL(alternateRowsRequested(bool)),
          this, SLOT(setAlternatingRowColors(bool)));
  connect(table, SIGNAL(exportRequested()), this, SLOT(exportContent()));
}

void ResultView::sort(int col) {
  if (dataProvider->isReadOnly()) {
    return;
  }

  if (currentSorting.first == col) {
    if (currentSorting.second == Qt::AscendingOrder) {
      currentSorting.second = Qt::DescendingOrder;
    } else {
      currentSorting.second = Qt::AscendingOrder;
    }
  } else {
    currentSorting = QPair<int, Qt::SortOrder>(col, Qt::AscendingOrder);
  }
  dataProvider->model()->sort(currentSorting.first, currentSorting.second);
  updateView();
}

/**
 * Called by the shortmodel's signal dataChanged in order to forward it to the
 * real one.
 */
void ResultView::updateItem(QStandardItem *item) {
  if (currentAction == Browse) {
    currentAction = Update;
    insertButton->setIcon(QIcon());
    insertButton->setText(tr("Apply"));
    deleteButton->setIcon(QIcon());
    deleteButton->setText(tr("Cancel"));
  }

  QSqlRecord record;
  int row = item->row() + offset;
  if (modifiedRecords.contains(row)) {
    record = modifiedRecords[row];
  } else {
    record = dataProvider->model()->record(row);
  }
  record.setValue(item->column(), item->data(Qt::DisplayRole));
  modifiedRecords[row] = record;
}

/**
 * Mise à jour pagination
 */
void ResultView::updateView() {
  table->resetColumnSizes();

  int hpos = table->horizontalScrollBar()->value();
  int vpos = table->verticalScrollBar()->value();

  shortModel->clear();

  if (!dataProvider) {
    return;
  }

  QSqlQueryModel* model = dataProvider->model();

  for(int i=0; i<model->columnCount(); i++)
    shortModel->setHorizontalHeaderItem(i, new QStandardItem(
        model->headerData(i, Qt::Horizontal).toString()));

  if(model->rowCount() == 0)
    return;

  // 1ère ligne à afficher
  int startIndex;
  startIndex = offset;
  if (startIndex > model->rowCount())
    startIndex = model->rowCount() - resultSpinBox->value();
  if (startIndex < 0)
    startIndex = 0;

  // Page en cours, nb de page
  double page, maxpage;
  modf(startIndex / resultSpinBox->value(), &page);
  modf(model->rowCount() / resultSpinBox->value(), &maxpage);
  pageLabel->setText(tr("Page %1/%2")
                     .arg(page+1)
                     .arg(maxpage+1));

  // dernière ligne à afficher
  int endIndex;
  endIndex = startIndex + resultSpinBox->value();
  if (endIndex > model->rowCount())
    endIndex = model->rowCount();

  firstPageButton->setEnabled(startIndex > 0);
  prevPageButton->setEnabled(startIndex > 0);
  nextPageButton->setEnabled(endIndex < model->rowCount());
  lastPageButton->setEnabled(endIndex < model->rowCount());

  QStandardItem *item;
  QStringList vlabels;
  QSqlRecord r;
  QList<QStandardItem*> row;
  for(int i=startIndex; i<endIndex; i++) {
    row.clear();
    r = model->record(i);
    for(int j=0; j<model->columnCount(); j++) {
      item = new QStandardItem();
//      if (r.value(j).canConvert(QVariant::String)
//          && r.value(j).toString().length() > 50) {
//        QString val = r.value(j).toString();
//        // BLOB
//        item->setData(val.left(47).append("..."), Qt::DisplayRole);
//        item->setData(val, Qt::ToolTipRole);
//        item->setEditable(false);
//        item->setData(true, Qt::UserRole);
//      } else {
        item->setData(r.value(j), Qt::DisplayRole);
        item->setData(false, Qt::UserRole);
//      }
      item->setEditable(!dataProvider->isReadOnly());
      row << item;
    }
    shortModel->appendRow(row);
    vlabels << QString::number(i+1);
  }
  if(currentAction == Insert) {
    vlabels.removeLast();
    vlabels << "*";
  }
  shortModel->setVerticalHeaderLabels(vlabels);

  resizeColumnsToContents();
  resizeRowsToContents();

  if (!dataProvider->isReadOnly()) {
    table->horizontalHeader()->setSortIndicatorShown(true);
    table->horizontalHeader()->setSortIndicator(currentSorting.first,
                                                currentSorting.second);
  }

  table->horizontalScrollBar()->setValue(hpos);
  table->verticalScrollBar()->setValue(vpos);
}
