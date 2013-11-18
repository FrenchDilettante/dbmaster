#include "tabledataprovider.h"


TableDataProvider::TableDataProvider(QString table, QSqlDatabase *db, QObject *parent) {
  this->table = table;
  this->db = db;

  m_model = new QSqlTableModel(this, *db);
  m_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
  m_model->setTable(table);

  setParent(parent);
}

void TableDataProvider::run() {
  if (m_model->select()) {
    emit complete();
  } else {
    emit error(m_model->lastError());
  }
}

/*
  setMode(TableMode);
  this->table->resetColumnSizes();

  QSqlTableModel *m = new QSqlTableModel(this, *db);
  m->setTable(table);
  m->setEditStrategy(QSqlTableModel::OnManualSubmit);
  m->select();
  if (m->lastError().type() == QSqlError::NoError) {
    setModel(m);
    emit structureChanged();
  } else {
    QMessageBox::critical(this,
                          tr("Error"),
                          tr("Unable to open the table. Returned error :\n%1")
                          .arg(m->lastError().text()),
                          QMessageBox::Ok);
  }
  */
