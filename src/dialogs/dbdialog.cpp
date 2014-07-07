#include "../dbmanager.h"
#include "../iconmanager.h"
#include "../wizards/newdbwizard.h"

#include "dbdialog.h"

#include <QCompleter>
#include <QFileInfo>
#include <QMessageBox>

DbDialog::DbDialog(QWidget *parent)
  : QDialog(parent) {
  setupUi(this);
  setupWidgets();
  setupConnections();

  connection = NULL;
  reload();
}

void DbDialog::accept() {
  apply();

  QDialog::accept();
  close();
}

void DbDialog::apply() {
  QSqlDatabase* db = connection->db();
  db->setHostName(hostEdit->text());
  db->setUserName(userEdit->text());
  db->setPassword(passEdit->text());
  db->setDatabaseName(dbEdit->text());
  connection->setAlias(aliasEdit->text());

  DbManager::instance->update(connection, aliasEdit->text());
}

void DbDialog::refresh(QModelIndex index) {
  if(dbListView->selectionModel()->selectedRows().contains(index)) {
    reload();
  }
}

/**
 * Re-fills all fields and enables/disables widgets
 */
void DbDialog::reload() {
  if (connection == NULL) {
    return;
  }

  QSqlDatabase* db = connection->db();
  bool emptyList = DbManager::instance->connections().isEmpty();
  bool selected = !emptyList
                  && dbListView->selectionModel()->selection().size() > 0;
  bool open = selected && db->isOpen();

  aliasOnCurrent = selected &&
      (connection->alias() != DbManager::instance->dbTitle(db));

  groupBox->setEnabled(selected && !open);
  toggleButton->setEnabled(selected);
  remButton->setEnabled(selected);

  if (open) {
    toggleButton->setIcon(IconManager::get("connect_no"));
    toggleButton->setText(tr("Disconnect"));
  } else {
    toggleButton->setIcon(IconManager::get("connect_creating"));
    toggleButton->setText(tr("Connect"));
  }

  if (!DbManager::instance->connections().isEmpty() && connection) {
    hostEdit->setText(db->hostName());
    userEdit->setText(db->userName());
    saveCheckBox->setChecked(!open && !db->password().isEmpty());
    passEdit->setText(db->password());
    dbTypeComboBox->setCurrentDriver(db->driverName());
    dbEdit->setText(db->databaseName());
    aliasEdit->setText(connection->alias());
  }

  dbBrowseButton->setVisible(false);
  resultLabel->setText(tr("<- Click to test"));
}

void DbDialog::removeCurrent() {
  int index = dbListView->currentIndex().row();
  if (DbManager::instance->connections()[index]->db()->isOpen()) {
    QMessageBox::critical(this,
                          tr("Cannot remove"),
                          tr("You must close the connexion before remove it.")
                          );
    return;
  }
  DbManager::instance->removeDatabase(index);
  reload();
}

void DbDialog::setConnection(QModelIndex index) {
  QItemSelectionModel *m = dbListView->selectionModel();
  m->setCurrentIndex(index, QItemSelectionModel::ClearAndSelect);
  connection = DbManager::instance->connections()[index.row()];

  reload();
}

void DbDialog::setupConnections()
{
  // buttons
  connect(addButton,    SIGNAL(clicked()), addWzd,  SLOT(exec()));
  connect(remButton,    SIGNAL(clicked()), this,    SLOT(removeCurrent()));
  connect(testButton,   SIGNAL(clicked()), this,    SLOT(testConnection()));
  connect(toggleButton, SIGNAL(clicked()), this,    SLOT(toggleConnection()));
  connect(buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()),
          this, SLOT(apply()));
  connect(buttonBox->button(QDialogButtonBox::Reset), SIGNAL(clicked()),
          this, SLOT(reload()));

  connect(hostEdit, SIGNAL(textChanged(QString)), this, SLOT(updateAlias()));
  connect(dbEdit, SIGNAL(textChanged(QString)), this, SLOT(updateAlias()));

  // list view
  connect(dbListView, SIGNAL(clicked(QModelIndex)),
          this, SLOT(setConnection(QModelIndex)));

  connect(DbManager::instance, SIGNAL(statusChanged(QModelIndex)),
          this, SLOT(refresh(QModelIndex)));
}

void DbDialog::setupWidgets()
{
  addWzd = new NewDbWizard(this);
  dbBrowseButton->setVisible(false);
  dbListView->setModel(DbManager::instance->model());

  QCompleter *c = new QCompleter(QStringList("localhost"), this);
  c->setCompletionMode(QCompleter::InlineCompletion);
  hostEdit->setCompleter(c);

  // loading icons from theme
  addButton->setIcon(IconManager::get("list-add"));
  remButton->setIcon(IconManager::get("list-remove"));
//  hostLabel->setPixmap(IconManager::get("network-server")
//                       .pixmap(QSize(32, 32)));
//  userLabel->setPixmap(IconManager::get("user-info")
//                         .pixmap(QSize(32, 32)));
//  pswdLabel->setPixmap(IconManager::get("dialog-password")
//                         .pixmap(QSize(32, 32)));
  toggleButton->setIcon(IconManager::get("connect_creating"));
}

void DbDialog::testConnection() {
  QSqlDatabase db = QSqlDatabase::addDatabase(
      dbTypeComboBox->currentDriverName(), "testdb");
  db.setHostName(hostEdit->text());
  db.setUserName(userEdit->text());
  db.setPassword(passEdit->text());
  db.setDatabaseName(dbEdit->text());

  if(db.open())
  {
    resultLabel->setText(tr("Connection succeded"));
  }
  else
  {
    resultLabel->setText(tr("Connection failed"));
    QMessageBox::critical(this,
                          tr("Connection failed"),
                          db.lastError().text());
  }

  QSqlDatabase::removeDatabase("testdb");
}

void DbDialog::toggleConnection() {
  int index = dbListView->selectionModel()->selectedIndexes()[0].row();
  DbManager::instance->connections()[index]->toggle();
}

void DbDialog::updateAlias() {
  if (!aliasOnCurrent && connection != NULL) {
    QString title;
    QString simplifiedName = QFileInfo(dbEdit->text()).fileName();
    if (simplifiedName.length() == 0) {
      simplifiedName = dbEdit->text();
    }
    if(hostEdit->text().isEmpty())
      title = tr("%1 (local)").arg(simplifiedName);
    else
      title = tr("%1 on %2").arg(simplifiedName).arg(hostEdit->text());

    aliasEdit->setText(title);
  }
}
