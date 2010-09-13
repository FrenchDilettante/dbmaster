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


#include "searchdialog.h"

#include "../iconmanager.h"

SearchDialog::SearchDialog(QWidget *parent)
  : QDialog(parent)
{
  setupUi(this);

  setupWidgets();
}

void SearchDialog::search()
{
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
    tc = m_textEdit->document()->find(text, tc);
  else
    tc = m_textEdit->document()->find(QRegExp(text), tc);

  if(tc.isNull())
    infosLabel->setText(tr("Search has reached the end of the document."));
  else
    infosLabel->clear();

  m_textEdit->setTextCursor(tc);
}

void SearchDialog::setupWidgets()
{
  setWindowIcon(IconManager::get("edit-search"));

  searchButton = buttonBox->addButton(tr("Search"),
                                      QDialogButtonBox::ActionRole);
  replaceButton = buttonBox->addButton(tr("Replace"),
                                       QDialogButtonBox::ActionRole);
  replaceButton->setEnabled(false);
  buttonBox->addButton(QDialogButtonBox::Close);

  connect(searchButton, SIGNAL(clicked()), this, SLOT(search()));
}
