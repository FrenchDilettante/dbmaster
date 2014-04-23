#include "searchdialog.h"

#include "../iconmanager.h"

QStringList SearchDialog::replaceHistory;
QStringList SearchDialog::searchHistory;

SearchDialog::SearchDialog(QWidget *parent)
  : QDialog(parent) {

  setupUi(this);

  setupWidgets();
}

void SearchDialog::closeEvent(QCloseEvent *event) {
  replaceButton->setEnabled(false);
  event->accept();
}

void SearchDialog::replace() {
  QTextCursor tc = m_textEdit->textCursor();

  if (tc.selectedText().size() > 0) {
    tc.removeSelectedText();
    tc.insertText(replaceComboBox->currentText());
  }

  search();
}

QTextCursor SearchDialog::search() {

  QTextDocument::FindFlags searchFlags = 0x0;
  if (backwardCheckBox->isChecked()) {
    searchFlags |= QTextDocument::FindBackward;
  }
  if (caseSensitiveCheckBox->isChecked()) {
    searchFlags |= QTextDocument::FindCaseSensitively;
  }
  if (wholeWordsCheckBox->isChecked()) {
    searchFlags |= QTextDocument::FindWholeWords;
  }

  QTextCursor tc = m_textEdit->textCursor();
  QString text = searchComboBox->currentText();

  if (!regExpCheckBox->isChecked()) {
    tc = m_textEdit->document()->find(text, tc, searchFlags);
  } else {
    tc = m_textEdit->document()->find(QRegExp(text), tc, searchFlags);
  }

  searchHistory << text;

  if (tc.isNull()) {
    infosLabel->setText(tr("Search has reached the end of the document."));
  } else {
    infosLabel->clear();
  }

  replaceButton->setEnabled(!tc.isNull());
  m_textEdit->setTextCursor(tc);

  return tc;
}

void SearchDialog::setupWidgets() {
  setWindowIcon(IconManager::get("edit-find"));

  searchButton = buttonBox->addButton(tr("Search"),
                                      QDialogButtonBox::ActionRole);
  searchButton->setDefault(true);
  replaceButton = buttonBox->addButton(tr("Replace"),
                                       QDialogButtonBox::ActionRole);
  replaceButton->setEnabled(false);
  buttonBox->addButton(QDialogButtonBox::Close);

  connect(replaceButton, SIGNAL(clicked()), this, SLOT(replace()));
  connect(searchButton, SIGNAL(clicked()), this, SLOT(search()));
}
