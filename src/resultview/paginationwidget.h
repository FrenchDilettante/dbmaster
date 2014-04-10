#ifndef PAGINATIONWIDGET_H
#define PAGINATIONWIDGET_H

#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QToolButton>
#include <QWidget>

class PaginationWidget : public QWidget {
Q_OBJECT
public:
  explicit PaginationWidget(QWidget *parent = 0);

  void setPage(int page, int pageCount);
  void setRowsPerPage(int rows);

signals:
  void first();
  void last();
  void next();
  void previous();

public slots:

private:
  QToolButton* genButton(QString tooltip, QString iconName);
  QLabel* genLabel(QString text);
  QString genPageCount(int page, int maxpage);
  void setupConnections();
  void setupWidgets();

  int page = 0;
  int pageCount = 0;

  QHBoxLayout* layout;

  QLabel* pageLabel;

  QToolButton* firstPageButton;
  QToolButton* prevPageButton;
  QToolButton* nextPageButton;
  QToolButton* lastPageButton;

  QSpinBox* resultSpinBox;

};

#endif // PAGINATIONWIDGET_H
