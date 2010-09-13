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


#ifndef DBCOMBOBOX_H
#define DBCOMBOBOX_H

#include <QtGui>

class DbComboBox : public QComboBox
{
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
