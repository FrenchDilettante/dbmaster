#include "searchdialog.h"

#include "../iconmanager.h"

QStringList SearchDialog::replaceHistory;
QStringList SearchDialog::searchHistory;

SearchDialog::SearchDialog(QWidget *parent)
  : QDialog(parent) {

  setupUi(this);

  setupWidgets();
}

/**
 * Interception événement fermeture dialogue
 */
void SearchDialog::closeEvent(QCloseEvent *event) {
  // on désactive le bouton remplacer pour qu'il le soit au prochain usage
  replaceButton->setEnabled(false);

  event->accept();
}

/**
 * Remplace le texte sélectionné puis passe à la suite
 */
void SearchDialog::replace() {
  QTextCursor tc = m_textEdit->textCursor();

  if (tc.selectedText().size() > 0) {
    tc.removeSelectedText();
    tc.insertText(replaceComboBox->currentText());
  }

  search();
}

/**
 * Recherche sur le document. Applique les différentes options sélectionnées
 * (expressions régulières, sens de recherche, etc.)
 */
QTextCursor SearchDialog::search() {

  // contruction des options avec des flags
  QTextDocument::FindFlags flags = 0x0;
  if(backwardCheckBox->isChecked())
    flags |= QTextDocument::FindBackward;
  if(caseSensitiveCheckBox->isChecked())
    flags |= QTextDocument::FindCaseSensitively;
  if(wholeWordsCheckBox->isChecked())
    flags |= QTextDocument::FindWholeWords;

  QTextCursor tc = m_textEdit->textCursor();
  QString text = searchComboBox->currentText();

  if(!regExpCheckBox->isChecked())
    tc = m_textEdit->document()->find(text, tc, flags);
  else
    tc = m_textEdit->document()->find(QRegExp(text), tc, flags);

  // ajout à l'historique des recherches
  searchHistory << text;

  if(tc.isNull())
    infosLabel->setText(tr("Search has reached the end of the document."));
  else
    infosLabel->clear();

  // on active le bouton remplacer que si la selection aboutit
  replaceButton->setEnabled(!tc.isNull());

  m_textEdit->setTextCursor(tc);

  return tc;
}

void SearchDialog::setupWidgets()
{
  setWindowIcon(IconManager::get("edit-find"));

  // Les boutons sont stockés dans une QButtonBox pour des raisons pratiques.
  searchButton = buttonBox->addButton(tr("Search"),
                                      QDialogButtonBox::ActionRole);
  replaceButton = buttonBox->addButton(tr("Replace"),
                                       QDialogButtonBox::ActionRole);
  replaceButton->setEnabled(false);
  buttonBox->addButton(QDialogButtonBox::Close);

  connect(replaceButton, SIGNAL(clicked()), this, SLOT(replace()));
  connect(searchButton, SIGNAL(clicked()), this, SLOT(search()));
}
