#include "dbtestwidget.h"

#include <QtGui>

DbTestWidget::DbTestWidget( QWidget *parent )
		: QWidget( parent )
{
  testButton = new QPushButton( tr( "Test connection" ), this );
  resultLabel = new QLabel( tr( "<clic to test>" ), this );

  QHBoxLayout *layout = new QHBoxLayout;
  setLayout( layout );

  layout->addWidget( testButton );
  layout->addWidget( resultLabel );
}

bool DbTestWidget::test( QSqlDatabase *db )
{
	bool ret = db->open();
	if( ret )
	{
		testButton->setIcon( QIcon( ":/img/button_ok.png" ) );
		resultLabel->setText( tr( "Connection established" ) );
	}
	else
	{
		testButton->setIcon( QIcon( ":/img/button_cancel.png" ) );
		resultLabel->setText( db->lastError().text() );
	}
	db->close();

	return ret;
}
