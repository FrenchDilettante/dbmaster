#ifndef RESULTVIEWTABLE_H
#define RESULTVIEWTABLE_H

#include <QTableView>

class ResultViewTable : public QTableView {
Q_OBJECT
public:
  ResultViewTable(QWidget *parent = 0);

signals:
  void rowLeaved(int);

protected:
  void selectionChanged(const QItemSelection &selected,
                        const QItemSelection &deselected);

private:
  int currentEditedRow;
};

#endif // RESULTVIEWTABLE_H
