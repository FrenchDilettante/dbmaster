#include "blobdialog.h"

BlobDialog::BlobDialog(QWidget *parent)
  : QDialog(parent) {
  setupUi(this);
}

void BlobDialog::setBlob(QVariant blob) {
  textViewer->setPlainText(blob.toString());
  htmlViewer->setHtml(blob.toString());
}
