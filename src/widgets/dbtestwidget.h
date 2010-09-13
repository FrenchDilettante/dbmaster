#ifndef DBTESTWIDGET_H
#define DBTESTWIDGET_H

#include <QtGui>
#include <QtSql>

class DbTestWidget : public QWidget
{
public:
	DbTestWidget(QWidget *parent=0);

public slots:
	bool test(QSqlDatabase *db);

protected:
	QPushButton *testButton;
	QLabel *resultLabel;
};

#endif // DBTESTWIDGET_H
