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
