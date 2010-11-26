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

#include <QSqlDatabase>
#include "../config.h"
#include "../dbmanager.h"
#include "../iconmanager.h"

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
                           field("name").toString());
  }
  else
  {
    DbManager::addDatabase(field("driver").toString(),
                           field("host").toString(),
                           field("user").toString(),
                           field("name").toString());
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
  : QWizardPage(parent)
{
  setupUi(this);

  registerField("host", hostLineEdit);
  registerField("driver", dbTypeComboBox, "currentDriver");

  QCompleter *c = new QCompleter(QStringList("localhost"), this);
  c->setCompletionMode(QCompleter::InlineCompletion);
  hostLineEdit->setCompleter(c);
}

void NdwFirstPage::initializePage()
{
  dbTypeComboBox->setCurrentDriver(Config::defaultDriver);
}

/*
 * Second page
 */
NdwSecondPage::NdwSecondPage(QWizard *parent)
  : QWizardPage(parent)
{
  setupUi(this);

  registerField( "name*", dbLineEdit );
  registerField( "user", userLineEdit );
  registerField( "savepswd", pswdCheckBox );
  registerField( "pswd", pswdLineEdit );

  connect( dbBrowseButton, SIGNAL(clicked()), this, SLOT(browse() ) );
  connect( testButton, SIGNAL(clicked()), this, SLOT(test()) );
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
}

void NdwSecondPage::test()
{
  QSqlDatabase db = QSqlDatabase::addDatabase( field( "driver" ).toString(),
                                               "testdb" );
  db.setHostName( field( "host" ).toString() );
  db.setUserName( field( "user" ).toString() );
  db.setPassword( field( "pswd" ).toString() );
  db.setDatabaseName( field( "name" ).toString() );

  if( db.open() )
    resultLabel->setText( tr( "Connection succeded" ) );
  else
  {
    resultLabel->setText( tr( "Connection failed" ) );
    QMessageBox::critical( this,
                           tr( "Connection failed" ),
                           db.lastError().text() );
  }

  QSqlDatabase::removeDatabase( "testdb" );
}
