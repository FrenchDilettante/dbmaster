#ifndef BLOBDIALOG_H
#define BLOBDIALOG_H

#include "ui_blobdialog.h"

class BlobDialog : public QDialog, private Ui::BlobDialog {
Q_OBJECT
public:
  BlobDialog(QWidget *parent = 0);

  void setBlob(QVariant blob);
};

#endif // BLOBDIALOG_H
