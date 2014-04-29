#include "paginationwidget.h"

#include "iconmanager.h"

PaginationWidget::PaginationWidget(QWidget *parent)
  : QWidget(parent) {
  setupWidgets();
  setupConnections();
}

QToolButton* PaginationWidget::genButton(QString tooltip, QString iconName) {
  QToolButton* btn = new QToolButton(this);
  btn->setAutoRaise(true);
  btn->setEnabled(false);
  btn->setIcon(IconManager::get(iconName));
  btn->setText("");
  btn->setToolTip(tooltip);

  return btn;
}

QLabel* PaginationWidget::genLabel(QString text) {
  QLabel *label = new QLabel(this);
  label->setText(text);
  return label;
}

QString PaginationWidget::genPageCount(int page, int maxpage) {
  return tr("Page %1/%2").arg(page+1).arg(maxpage+1);
}

void PaginationWidget::setPage(int page, int pageCount) {
  this->page = page;
  this->pageCount = pageCount;

  firstPageButton->setEnabled(page != 0);
  prevPageButton->setEnabled(page != 0);
  nextPageButton->setEnabled(page < pageCount);
  lastPageButton->setEnabled(page < pageCount);

  pageLabel->setText(genPageCount(page, pageCount));
}

void PaginationWidget::setReloadEnabled(bool enable) {
  reloadButton->setEnabled(enable);
}

void PaginationWidget::setRowsPerPage(int rows) {
  resultSpinBox->setValue(rows);
}

void PaginationWidget::setupConnections() {
  connect(firstPageButton, SIGNAL(clicked()), this, SIGNAL(first()));
  connect(prevPageButton, SIGNAL(clicked()), this, SIGNAL(previous()));
  connect(nextPageButton, SIGNAL(clicked()), this, SIGNAL(next()));
  connect(lastPageButton, SIGNAL(clicked()), this, SIGNAL(last()));
  connect(reloadButton, SIGNAL(clicked()), this, SIGNAL(reload()));
}

void PaginationWidget::setupWidgets() {

  pageLabel = genLabel(genPageCount(-1, -1));

  resultSpinBox = new QSpinBox(this);
  resultSpinBox->setEnabled(false);
  resultSpinBox->setMaximumHeight(20);
  resultSpinBox->setMinimum(10);
  resultSpinBox->setMaximum(200);
  resultSpinBox->setSingleStep(10);
  resultSpinBox->setValue(30);

  layout = new QHBoxLayout(this);
  QMargins margins = layout->contentsMargins();
  margins.setLeft(0);
  layout->setContentsMargins(margins);
  layout->setSpacing(2);
  setLayout(layout);
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  layout->addWidget(pageLabel);

  layout->addSpacing(10);

  firstPageButton = genButton(tr("First page"), "go-first");
  layout->addWidget(firstPageButton);
  prevPageButton = genButton(tr("Previous page"), "go-previous");
  layout->addWidget(prevPageButton);
  nextPageButton = genButton(tr("Next page"), "go-next");
  layout->addWidget(nextPageButton);
  lastPageButton = genButton(tr("Last page"), "go-last");
  layout->addWidget(lastPageButton);

  layout->addSpacing(10);

  reloadButton = genButton(tr("Reload"), "view-refresh");
  layout->addWidget(reloadButton);

  layout->addSpacing(10);

  layout->addWidget(genLabel(tr("Show")));
  layout->addWidget(resultSpinBox);
  layout->addWidget(genLabel(tr("results")));
}
