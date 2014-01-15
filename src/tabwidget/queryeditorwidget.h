#ifndef QUERYEDITORWIDGET_H
#define QUERYEDITORWIDGET_H

#include <QtGui>
#include <QRunnable>
#include <QSqlError>
#include <QSqlResult>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QStatusBar>

#include "abstracttabwidget.h"

#include "ui_queryeditorwidget.h"

class QueryEditorWidget: public AbstractTabWidget, QRunnable, Ui::QueryEditorWidget {
Q_OBJECT
public:
  QueryEditorWidget(QWidget* = 0);

  Actions       availableActions();
  int           confirmCloseAll();
  QString       file();
  QIcon         icon();
  QString       id();
  bool          isSaved();
  void          print();
  QPrinter     *printer();
  void          saveAs(QString = QString::null);
  QTextEdit    *textEdit();
  QString       title();

public slots:
  void copy();
  void cut();
  void lowerCase();
  void open( QString );
  void paste();
  void redo();
  void refresh();
  void reload();
  bool save();
  void selectAll();
  void undo();
  void upperCase();

signals:
  void fileChanged(QString);
  void queryFinished();

private:
  void closeEvent(QCloseEvent *event);
  QSqlDatabase* currentDb();
  bool confirmClose();
  void keyPressEvent(QKeyEvent *event);
  void reloadFile();
  void run();
  void setFilePath(QString);
  void setupConnections();
  void setupWidgets();
  void showEvent(QShowEvent *event);

  Actions               baseActions;
  QString               filePath;
  QSqlQueryModel       *model;
  int                   oldCount;
  int                   page;
  QToolButton*          resultButton;
  QSqlQuery             query;
  QStandardItemModel   *shortModel;
  QStatusBar           *statusBar;
  QFileSystemWatcher   *watcher;

private slots:
  void checkDbOpen();
  void commit();
  void onFileChanged(QString path);
  void rollback();
  void start();
  void startTransaction();
  void validateQuery();
};

#endif // QUERYEDITORWIDGET_H
