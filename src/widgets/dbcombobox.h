#ifndef DBCOMBOBOX_H
#define DBCOMBOBOX_H

#include <QtWidgets/QComboBox>

class DbComboBox : public QComboBox {
Q_OBJECT
public:
  DbComboBox(QWidget* =0);

  QString currentDriverName();
  void setCurrentDriver(QString);


  Q_PROPERTY(QString currentDriver    READ currentDriverName
                                      WRITE setCurrentDriver)

private:
};

#endif // DBCOMBOBOX_H
