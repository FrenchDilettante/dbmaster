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


#include "../config.h"
#include "../dbmanager.h"
#include "../iconmanager.h"
#include "../mainwindow.h"
#include "../tools/logger.h"
#include "../widgets/resultview.h"

#include "queryeditorwidget.h"

#include <QSqlQuery>

QueryEditorWidget::QueryEditorWidget(QWidget *parent)
  : AbstractTabWidget(parent) {
  setupUi(this);
  setupWidgets();
  setupConnections();

  setAutoDelete(false);

  model       = new QSqlQueryModel(this);
  shortModel  = new QStandardItemModel(this);
  // watcher     = new QFileSystemWatcher(this);
}

AbstractTabWidget::Actions QueryEditorWidget::availableActions() {
  Actions ret = baseActions;
  if (!isSaved()) {
    ret |= Save;
  }

  if (editor->document()->isUndoAvailable()) {
    ret |= Undo;
  }

  if (editor->document()->isRedoAvailable()) {
    ret |= Redo;
  }

  return ret;
}

void QueryEditorWidget::checkDbOpen() {
  DbManager::lastIndex = dbChooser->currentIndex();

  QSqlDatabase *db = DbManager::getDatabase(dbChooser->currentIndex());
  if (db == NULL) {
    runButton->setEnabled(false);
    return;
  }

  runButton->setEnabled(db->isOpen());

  if (!db->isOpen() || db->driverName().startsWith("QOCI")) {
    return;
  }

  QMultiMap<QString, QString> fields;
  QSqlRecord r;
  QStringList tables = db->tables();
  foreach (QString t, tables) {
    r = db->record(t);
    for (int i=0; i<r.count(); i++) {
      fields.insert(t, r.fieldName(i));
    }
  }

  editor->reloadContext(tables, fields);
}

void QueryEditorWidget::closeEvent(QCloseEvent *event) {
  if (isSaved()) {
    event->accept();
  } else {
    if (confirmClose()) {
      event->accept();
    } else {
      event->ignore();
    }
  }
}

bool QueryEditorWidget::confirmClose() {
  // check if it's saved or not
  if (!isSaved()) {
    int ret = QMessageBox::warning(
        this,
        tr( "DbMaster" ),
        tr( "Warning ! All your modifications will be lost.\nDo you want to save ?" ),
        QMessageBox::Cancel | QMessageBox::Save | QMessageBox::Discard,
        QMessageBox::Cancel);

    switch (ret) {
    case QMessageBox::Cancel:
      return false;

    case QMessageBox::Save:
      return save();
    }
  }

  return true;
}

int QueryEditorWidget::confirmCloseAll() {
  if (isSaved()) {
    return QMessageBox::Yes;
  }

  QString msg(tr("Warning ! All your modifications will be lost.\nDo you want to save ?"));
  if (filePath.isNull()) {
    msg = QString(tr("Warning ! %1 hasn't been saved\nDo you want to save ?"))
                  .arg(filePath);
  }

  int ret = QMessageBox::warning(this, tr("DbMaster"), msg,
                                 QMessageBox::Save    | QMessageBox::SaveAll  |
                                 QMessageBox::No      | QMessageBox::NoAll    |
                                 QMessageBox::Cancel,
                                 QMessageBox::Cancel);

  return ret;
}

void QueryEditorWidget::copy() {
  editor->copy();
}

void QueryEditorWidget::cut() {
  editor->cut();
}

QString QueryEditorWidget::file() {
  return filePath;
}

void QueryEditorWidget::onFileChanged(QString path) {
  if (QFile::exists(path)) {
    // Le fichier a été modifié
  } else {
    // Le fichier a été supprimé
  }
}

QIcon QueryEditorWidget::icon() {
  return IconManager::get("accessories-text-editor");
}

QString QueryEditorWidget::id() {
  QString ret = "q";
  if (!filePath.isEmpty()) {
    ret.append(" %1").arg( filePath );
  }
  return ret;
}

bool QueryEditorWidget::isSaved() {
  return !editor->document()->isModified();
}

void QueryEditorWidget::keyPressEvent(QKeyEvent *event) {
  if (event->key() == Qt::Key_Escape) {
    // Appui touche échap = masque le panel de résultat
    tabView->hide();
    resultButton->setChecked(false);
  } else {
    QWidget::keyPressEvent(event);
  }
}

void QueryEditorWidget::lowerCase() {
  QTextCursor tc = editor->textCursor();
  if (tc.selectedText().size() > 0) {
    QString txt = tc.selectedText();
    tc.removeSelectedText();
    tc.insertText(txt.toLower());
    tc.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor, txt.size());
    editor->setTextCursor(tc);
  }
}

void QueryEditorWidget::open(QString path) {
  if (!confirmClose())
    return;

  setFilePath(path);
  reloadFile();
}

void QueryEditorWidget::paste() {
  editor->paste();
}

void QueryEditorWidget::print() {
  QPainter painter;
  painter.begin(&m_printer);
  editor->document()->drawContents(&painter);
  painter.end();
}

QPrinter *QueryEditorWidget::printer() {
  return &m_printer;
}

void QueryEditorWidget::redo() {
  editor->redo();
}

void QueryEditorWidget::refresh() {
  checkDbOpen();
}

void QueryEditorWidget::reload() {
  run();
}

/**
 * Called after open(QString)
 */
void QueryEditorWidget::reloadFile() {
  if (!confirmClose()) {
    return;
  }

  editor->clear();

  // loading the file
  QFile file(filePath);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    // OMG ! an error occured
    QMessageBox::critical(this, tr("DbMaster"),
                          tr("Unable to open the file !"),
                          QMessageBox::Ok );
    return;
  }

  if (Config::editorEncoding == "latin1") {
    editor->append(QString::fromLatin1(file.readAll().data()));
  }

  if (Config::editorEncoding == "utf8") {
    editor->append(QString::fromUtf8(file.readAll().data()));
  }

  file.close();

  editor->document()->setModified(false);

  emit fileChanged(filePath);
}

void QueryEditorWidget::run() {
  QString qtext = editor->textCursor().selectedText();
  if (qtext.isEmpty()) {
    qtext = editor->toPlainText();
  }

  QSqlDatabase* db = DbManager::getDatabase(dbChooser->currentIndex());
  query = QSqlQuery(*db);
  query.exec(qtext);
  model->setQuery(query);

  emit queryFinished();
}

/**
 * @returns false in case of error
 */
bool QueryEditorWidget::save() {
  if (isSaved()) {
    return true;
  }

  if (filePath.isNull()) {
    setFilePath(QFileDialog::getSaveFileName(
        this, tr("DbMaster"), lastDir.path(),
        tr("Query file (*.sql);;All files (*.*)")));

    if(filePath.isNull())
      return false;
  }

  QFile file(filePath);
  lastDir = filePath;

  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QMessageBox::critical(this, tr("DbMaster"), tr("Unable to save the file !"),
                          QMessageBox::Ok);
    return false;
  }

  editor->setEnabled(false);
  setCursor(Qt::BusyCursor);

  QTextStream out( &file );
  QStringList list( editor->toPlainText().split( '\n' ) );

  int count = list.count();
  for (int i = 0; i < count; i++) {
    out << list[i];
    if( i != count )
    out << '\n';
  }

  file.close();

  editor->document()->setModified(false);

  editor->setEnabled(true);
  setCursor(Qt::ArrowCursor);

  emit modificationChanged(editor->document()->isModified());
  return true;
}

void QueryEditorWidget::saveAs(QString name) {
  filePath = name;
  save();
}

void QueryEditorWidget::selectAll() {
  editor->selectAll();
}

void QueryEditorWidget::setFilePath(QString path) {
  // watcher->removePath(this->filePath);
  this->filePath = path;

  if(path.size() > 30)
    path = QFileInfo(path).fileName();

  pathLabel->setText(path);
  if (path.length() > 0) {
    // watcher->addPath(path);
  }
}

void QueryEditorWidget::setupConnections() {
  connect(dbChooser, SIGNAL(currentIndexChanged(int)),
          this, SLOT(checkDbOpen()));
  connect(tabView, SIGNAL(reloadRequested()), this, SLOT(reload()));

  connect(runButton, SIGNAL(clicked()), this, SLOT(start()));

  connect(editor->document(), SIGNAL(modificationChanged(bool)),
          this, SIGNAL(modificationChanged(bool)));

  connect(this, SIGNAL(queryFinished()),
          this, SLOT(validateQuery()));

  // connect(watcher, SIGNAL(fileChanged(QString)),
  //         this, SLOT(onFileChanged(QString)));
}

void QueryEditorWidget::setupWidgets() {
  editor->setFont(Config::editorFont);

  tabView->hide();

  statusBar = new QStatusBar(this);
  statusBar->setSizeGripEnabled(false);
  gridLayout->addWidget(statusBar, gridLayout->rowCount(), 0, 1, -1);

  resultButton = new QToolButton(this);
  resultButton->setText(tr("Display result"));
  resultButton->setCheckable(true);
  statusBar->addPermanentWidget(resultButton);
  connect(resultButton, SIGNAL(clicked(bool)),
          tabView, SLOT(setVisible(bool)));

  optionsMenu = new QMenu(this);
  optionsMenu->addAction(actionEnqueue);
  optionsMenu->addAction(actionLogQuery);
  optionsMenu->addAction(actionClearOnSuccess);
  toolButton->setMenu(optionsMenu);

  baseActions = CaseLower | CaseUpper | Copy | Cut | Paste | Print | SaveAs
              | Search | SelectAll;

  dbChooser->setModel(DbManager::model());
  dbChooser->setCurrentIndex(DbManager::lastIndex);

  runButton->setIcon(IconManager::get("player_play"));

  refresh();
}

void QueryEditorWidget::showEvent(QShowEvent *event) {
  editor->setFocus();
}

void QueryEditorWidget::start() {
  resultButton->setChecked(false);
  tabView->setVisible(false);
  runButton->setEnabled(false);

  statusBar->showMessage(tr("Running..."));

  QThreadPool::globalInstance()->start(this);
}

QString QueryEditorWidget::title() {
  QString t;
  if(!filePath.isEmpty())
    t = QFileInfo(filePath).fileName();
  else
    t = tr("Query editor");

  return t;
}

QTextEdit* QueryEditorWidget::textEdit() {
  return editor;
}

void QueryEditorWidget::undo() {
  editor->undo();
}

void QueryEditorWidget::upperCase() {
  QTextCursor tc = editor->textCursor();
  if (tc.selectedText().size() > 0) {
    QString txt = tc.selectedText();
    tc.removeSelectedText();
    tc.insertText(txt.toUpper());
    tc.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor, txt.size());
    editor->setTextCursor(tc);
  }
}

void QueryEditorWidget::validateQuery() {
  // tabWidget->setTabEnabled(1, false);

  QString logMsg;

  switch(query.lastError().type()) {
  case QSqlError::NoError:
    tabView->setQuery(model);
    tabView->setVisible(true);
    // tabWidget->setCurrentIndex(1);
    // tabWidget->setTabEnabled(1, true);
    logMsg = tr("Query executed with success (%1 lines returned)")
        .arg(model->rowCount());
        // .arg(token->duration());

    statusBar->showMessage(logMsg);
    break;

  default:
    logMsg = tr("Unable to run query");
    statusBar->showMessage(logMsg);
    break;
  }

  logMsg.append(
        QString("<br /><span style=\"color: blue\">%1</span>")
          .arg(query.lastQuery().replace("\n", " ")));
  Logger::instance->log(logMsg);

  runButton->setEnabled(true);
}
