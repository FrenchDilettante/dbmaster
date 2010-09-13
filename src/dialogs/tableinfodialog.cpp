#include "tableinfodialog.h"

#include "dbmanager.h"

TableInfoDialog::TableInfoDialog( int db, QString table, QWidget *parent) :
    QDialog(parent)
{
    setupUi( this );

    this->db = DbManager::getDatabase( db );
    this->table = table;

    reload();
}

void TableInfoDialog::refresh()
{
  nameLabel->setText( table );
  int count = db->tables().size();
  int index = db->tables().indexOf( table );

  prevTableButton->setEnabled( index > 0 && count > 0 );
  nextTableButton->setEnabled( index != count - 1 );
}

void TableInfoDialog::reject()
{
  emit rejected();
  close();
}

void TableInfoDialog::reload()
{
  if( !db->isOpen() )
  {
    QMessageBox::critical( this,
                           tr( "Closed database" ),
                           tr( "The database is closed" ) );
    reject();
  }

  if( !db->tables().contains( table ) )
  {
    QMessageBox::critical( this,
                           tr( "Unknown table" ),
                           tr( "The table %1 cannot be found." )
                           .arg( table ) );
    reject();
  }

  model = new QSqlTableModel( this, *db );

  refresh();
}
