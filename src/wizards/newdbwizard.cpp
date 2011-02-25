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


#include "newdbwizard.h"

#include "../config.h"
#include "../dbmanager.h"
#include "../iconmanager.h"
#include "../plugins/pluginmanager.h"
#include "../plugins/sqlwrapper.h"

#include <QSqlDatabase>

/*
 * Wizard
 */
NewDbWizard::NewDbWizard(QWidget *parent) :
	QWizard(parent)
{
  setupUi(this);
  setWindowIcon(IconManager::get("bookmark-new"));

  setupPages();
}

void NewDbWizard::accept()
{
  if(field("savepswd").toBool())
  {
    DbManager::addDatabase(field("driver").toString(),
                           field("host").toString(),
                           field("user").toString(),
                           field("pswd").toString(),
                           field("name").toString(),
                           field("alias").toString());
  }
  else
  {
    DbManager::addDatabase(field("driver").toString(),
                           field("host").toString(),
                           field("user").toString(),
                           field("name").toString(),
                           field("alias").toString());
  }

  emit(accepted());
  close();
}

void NewDbWizard::setupPages()
{
  setPage(FirstPage, new NdwFirstPage(this));
  setPage(SecondPage, new NdwSecondPage(this));
}


/*
 * First page
 */
NdwFirstPage::NdwFirstPage(QWizard *parent)
  : QWizardPage(parent) {
  setupUi(this);

  registerField("host", hostLineEdit);
  registerField("driver", dbTypeComboBox, "currentDriver");

  QCompleter *c = new QCompleter(QStringList("localhost"), this);
  c->setCompletionMode(QCompleter::InlineCompletion);
  hostLineEdit->setCompleter(c);
}

void NdwFirstPage::initializePage() {
  dbTypeComboBox->setCurrentDriver(Config::defaultDriver);
  on_dbTypeComboBox_currentIndexChanged(0);
}

void NdwFirstPage::on_dbTypeComboBox_currentIndexChanged(int index) {
  SqlWrapper *wrapper =
      PluginManager::availableWrapper(dbTypeComboBox->currentDriverName());

  if (wrapper) {
    odbcCheckBox->setEnabled(wrapper->features() & SqlWrapper::ODBC);
  } else {
    odbcCheckBox->setEnabled(true);
  }
}

/*
 * Second page
 */
NdwSecondPage::NdwSecondPage(QWizard *parent)
  : QWizardPage(parent) {
  setupUi(this);

  registerField("name*", dbLineEdit);
  registerField("user", userLineEdit);
  registerField("savepswd", pswdCheckBox);
  registerField("pswd", pswdLineEdit);
  registerField("alias", aliasLineEdit);

  connect(dbBrowseButton, SIGNAL(clicked()), this, SLOT(browse()));
  connect(testButton, SIGNAL(clicked()), this, SLOT(test()));

  connect(dbLineEdit, SIGNAL(textChanged(QString)),
          this, SLOT(updateAlias()));
}

void NdwSecondPage::browse()
{
  dbLineEdit->setText( QFileDialog::getOpenFileName(this,
                                                    tr("Open Database"),
                                                    QDir::homePath(),
                                                    tr("Database file (*.*)")
                                                    ) );
}

void NdwSecondPage::initializePage() {
  dbBrowseButton->setVisible(field("driver").toString()
                             .startsWith("QSQLITE"));
  resultLabel->setText("");
  updateAlias();
}

void NdwSecondPage::test() {
  QSqlDatabase db =
      QSqlDatabase::addDatabase(field("driver").toString(), "testdb");
  db.setHostName(field("host").toString());
  db.setUserName(field("user").toString());
  db.setPassword(field("pswd").toString());
  db.setDatabaseName(field("name").toString());

  if(db.open()) {
    resultLabel->setText(tr("Connection succeded"));
  } else {
    resultLabel->setText(tr("Connection failed"));
    QMessageBox::critical(this,
                          tr("Connection failed"),
                          db.lastError().text());
  }

  QSqlDatabase::removeDatabase("testdb");
}

void NdwSecondPage::updateAlias() {
  if (field("host").toString().isEmpty()) {
    aliasLineEdit->setText(tr("%1 (local)").arg(dbLineEdit->text()));
  } else {
    aliasLineEdit->setText(tr("%1 on %2")
                           .arg(dbLineEdit->text())
                           .arg(field("host").toString()));
  }
}
