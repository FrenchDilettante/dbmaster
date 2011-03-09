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

#include "resultview.h"

#include "../iconmanager.h"

#include <QModelIndex>

bool ResultView::alternateRows = false;
QList<ResultView*> ResultView::instances = QList<ResultView*>();

ResultView::ResultView(QWidget *parent)
  : QWidget(parent) {
  setupUi(this);
  // Sincèrement, un jour, il faudra se poser la question de l'utilité de la
  // ligne suivante :
  table->setModel(0);

  // Par défaut, on visualise
  currentAction = Browse;
  // Aucun tri
  currentSorting = QPair<int, Qt::SortOrder>(-1, Qt::AscendingOrder);
  // Nécessaire pour la synchronisation sur l'option d'alternance des lignes
  instances << this;
  // On commence à la première ligne
  offset = 0;
  // Purement arbitraire, j'avoue
  m_mode = QueryMode;
  shortModel = new QStandardItemModel(this);
  table->setModel(shortModel);
  table->setAlternatingRowColors(alternateRows);

  blobDialog = new BlobDialog(this);

  setupMenus();
  setupConnections();

  // loading icons from theme
  firstPageButton->setIcon(IconManager::get("go-first"));
  lastPageButton->setIcon(IconManager::get("go-last"));
  nextPageButton->setIcon(IconManager::get("go-next"));
  prevPageButton->setIcon(IconManager::get("go-previous"));
  reloadButton->setIcon(IconManager::get("view-refresh"));
  insertButton->setIcon(IconManager::get("list-add"));
  deleteButton->setIcon(IconManager::get("list-remove"));
}

ResultView::~ResultView() {
  instances.removeAll(this);
}

void ResultView::contextMenuEvent(QContextMenuEvent *e)
{
  if(e->reason() != QContextMenuEvent::Mouse
     || !table->geometry().contains(e->pos())
     || model == 0)
    return;

  if( model->columnCount() == 0 || model->rowCount() == 0 )
    return;

  actionDetails->setEnabled(
        table->selectionModel()->selectedIndexes().size() > 0
        && table->selectionModel()->selectedIndexes().size() < 2);

  contextMenu->move(e->globalPos());
  contextMenu->exec();
}

void ResultView::exportContent()
{
  if (model == 0)
    return;

  if (model->columnCount() == 0 || model->rowCount() == 0)
    return;

//  exportWizard = new ExportWizard(model, this);
  if (m_mode == TableMode) {
    exportWizard = new ExportWizard(model, this);
  } else {
    exportWizard = new ExportWizard(m_token, this);
  }

  exportWizard->exec();
}

/**
 * Clic sur le bouton supprimer : peut annuler l'opération en cours ou supprimer
 * la ligne en cours.
 */
void ResultView::on_deleteButton_clicked()
{
  if(m_mode != TableMode)
    return;

  QSqlTableModel *tmodel = (QSqlTableModel*) model;

  if (currentAction != Browse) {
    // Annulation de l'opération en cours
    insertButton->setIcon(IconManager::get("list-add"));
    deleteButton->setIcon(IconManager::get("list-remove"));

    currentAction = Browse;
    tmodel->revertAll();
  } else {
    // suppression des lignes sélectionnées
    QSet<int> rows;
    foreach(QModelIndex i, table->selectionModel()->selectedIndexes())
      rows << (i.row() + offset);

    foreach(int r, rows)
      model->removeRow(r);

    tmodel->submitAll();
  }

  updateView();
}

/**
 * Clic sur le bouton d'insertion : peut passer en mode insertion ou valider un
 * ajout/modif.
 */
void ResultView::on_insertButton_clicked()
{
  if(m_mode != TableMode)
    return;

  if (currentAction == Insert || currentAction == Update) {
    insertButton->setIcon(IconManager::get("list-add"));
    deleteButton->setIcon(IconManager::get("list-remove"));

    QSqlTableModel *tmodel = (QSqlTableModel*) model;

    foreach (int row, modifiedRecords.keys()) {
      tmodel->setRecord(row, modifiedRecords[row]);
    }

    if (!tmodel->submitAll()) {
      QMessageBox::critical(this, "Error", tmodel->lastError().text());
    }

    currentAction = Browse;
    tmodel->select();
    updateView();
  } else {
    insertButton->setIcon(QIcon());
    insertButton->setText(tr("Apply"));
    deleteButton->setIcon(QIcon());
    deleteButton->setText(tr("Cancel"));

    model->insertRow(model->rowCount());
    currentAction = Insert;
    updateView();
    table->scrollToBottom();
  }
}

void ResultView::on_reloadButton_clicked()
{
  switch (m_mode) {
  case QueryMode:
    emit reloadRequested();
    break;

  case TableMode:
    ((QSqlTableModel*) model)->select();
    updateView();
    break;
  }
}

void ResultView::resizeColumnsToContents()
{
  table->resizeColumnsToContents();
}

void ResultView::resizeRowsToContents()
{
  table->resizeRowsToContents();
}

void ResultView::scrollBegin()
{
  offset = 0;
  updateView();
}

void ResultView::scrollDown() {
  offset += resultSpinBox->value();
  updateView();
}

void ResultView::scrollEnd() {
  double last;
  modf(model->rowCount() / resultSpinBox->value(), &last);

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
  actionAlternateColor->setChecked(enable);

  if (loop) {
    foreach (ResultView *v, instances) {
      if (dynamic_cast<ResultView*>(v) && v != this) {
        v->setAlternatingRowColors(enable, false);
      }
    }
  }
}

void ResultView::setMode(Mode m) {
  m_mode = m;

  switch(m_mode)
  {
  case QueryMode:
    insertButton->setVisible(false);
    deleteButton->setVisible(false);
    table->setSortingEnabled(false);
    break;

  case TableMode:
    insertButton->setVisible(true);
    deleteButton->setVisible(true);
    table->setSortingEnabled(true);
    break;
  }
}

void ResultView::setModel(QSqlQueryModel *model) {
  this->model = model;
  updateView();
}

bool ResultView::setTable(QString table, QSqlDatabase *db) {
  setMode(TableMode);
  QSqlTableModel *m = new QSqlTableModel(this, *db);
  m->setTable(table);
  m->setEditStrategy(QSqlTableModel::OnManualSubmit);
  m->select();
  if(m->lastError().type() == QSqlError::NoError)   {
    setModel(m);
    return true;
  } else {
    QMessageBox::critical(this,
                          tr("Error"),
                          tr("Unable to open the table. Returned error :\n%1")
                          .arg(m->lastError().text()),
                          QMessageBox::Ok);
    return false;
  }
}

void ResultView::setToken(QueryToken *token) {
  setMode(QueryMode);
  m_token = token;
  offset = 0;

  setModel(m_token->model());
}

void ResultView::setupConnections() {
  connect(actionAlternateColor, SIGNAL(toggled(bool)),
          this, SLOT(setAlternatingRowColors(bool)));
  connect(actionDetails, SIGNAL(triggered()), this, SLOT(showBlob()));
  connect(actionExport, SIGNAL(triggered()), this, SLOT(exportContent()));

  connect(firstPageButton, SIGNAL(clicked()), this, SLOT(scrollBegin()));
  connect(prevPageButton, SIGNAL(clicked()), this, SLOT(scrollUp()));
  connect(nextPageButton, SIGNAL(clicked()), this, SLOT(scrollDown()));
  connect(lastPageButton, SIGNAL(clicked()), this, SLOT(scrollEnd()));

  connect(reloadButton, SIGNAL(clicked()), this, SLOT(on_reloadButton_clicked()));
  connect(resultSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateView()));

  connect(deleteButton, SIGNAL(clicked()), this, SLOT(on_deleteButton_clicked()));

  connect(shortModel, SIGNAL(itemChanged(QStandardItem*)),
          this, SLOT(updateItem(QStandardItem*)));

  connect(table->horizontalHeader(), SIGNAL(sectionClicked(int)),
          this, SLOT(sort(int)));
}

void ResultView::setupMenus() {
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

void ResultView::showBlob() {
  if (table->selectionModel()->selectedIndexes().size() != 1) {
    return;
  }

  QVariant blob = table->selectionModel()->selectedIndexes().at(0).data();
  blobDialog->setBlob(blob);
  blobDialog->show();
}

void ResultView::sort(int col) {
  if (m_mode == QueryMode) {
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
  model->sort(currentSorting.first, currentSorting.second);
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
    record = model->record(row);
  }
  record.setValue(item->column(), item->data(Qt::DisplayRole));
  modifiedRecords[row] = record;
}

/**
 * Mise à jour pagination
 */
void ResultView::updateView() {
  shortModel->clear();

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

  for (int i=0; i<model->columnCount(); i++) {
    shortModel->setHorizontalHeaderItem(i, new QStandardItem(
        model->headerData(i, Qt::Horizontal).toString()));
  }

  QStandardItem *item;
  QStringList vlabels;
  QSqlRecord r;
  QList<QStandardItem*> row;
  for(int i=startIndex; i<endIndex; i++) {
    row.clear();
    r = model->record(i);
    for(int j=0; j<model->columnCount(); j++) {
      item = new QStandardItem();
      if (r.value(j).canConvert(QVariant::String)
          && r.value(j).toString().length() > 50) {
        QString val = r.value(j).toString();
        // BLOB
        item->setData(val.left(47).append("..."), Qt::DisplayRole);
        item->setData(val, Qt::ToolTipRole);
        item->setEditable(false);
        item->setData(true, Qt::UserRole);
      } else {
        item->setData(r.value(j), Qt::DisplayRole);
        item->setData(false, Qt::UserRole);
      }
      item->setEditable(m_mode == TableMode);
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

  if (m_mode == TableMode) {
    table->horizontalHeader()->setSortIndicatorShown(true);
    table->horizontalHeader()->setSortIndicator(currentSorting.first,
                                                currentSorting.second);
  }
}
