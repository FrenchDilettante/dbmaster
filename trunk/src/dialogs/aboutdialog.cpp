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


#include "aboutdialog.h"
#include "../iconmanager.h"

#include <QApplication>

AboutDialog::AboutDialog(QWidget *parent)
  : QDialog(parent)
{
  setupUi(this);

  infoLabel->setText(QString(tr("Compiled with Qt %1")).arg(QT_VERSION_STR));
  versionLabel->setText(QString(tr("DbMaster %1"))
                        .arg(QApplication::applicationVersion()));

  // loading icons from theme
  setWindowIcon(IconManager::get("help-about"));
}
