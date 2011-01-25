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


#include "configdialog.h"

#include "../config.h"
#include "../iconmanager.h"

QFont                           ConfigDialog::editorFont;
QMap<QString,QColor>            ConfigDialog::shColor;
QMap<QString,QTextCharFormat>   ConfigDialog::shFormat;

ConfigDialog::ConfigDialog(QWidget *parent)
  : QDialog(parent)
{
  setupUi(this);

  setupConnections();

  reload();

  // loading icons from theme
  setWindowIcon(                IconManager::get("preferences"));
  listWidget->item(0)->setIcon( IconManager::get("preferences"));
  listWidget->item(1)->setIcon( IconManager::get("accessories-text-editor"));
}

void ConfigDialog::accept()
{
  save();
  close();
}

/**
 * Proceeds a click from the buttonGroup.
 */
void ConfigDialog::handleClick(QAbstractButton *button)
{
  switch(buttonBox->buttonRole(button))
  {
  case QDialogButtonBox::AcceptRole:
    accept();
    break;

  case QDialogButtonBox::ApplyRole:
    save();
    break;

  case QDialogButtonBox::RejectRole:
    reject();
    break;

  default:
    break;
  }
}

/**
 * Set the parameters with the current font
 */
void ConfigDialog::refreshSyntaxFont(int index)
{
  if(index == -1 && index >= Config::shGroupList.size())
    return;
	
  QListWidgetItem *item = shListWidget->item(index);
  QFont font = item->font();

  QColor color = item->foreground().color();
  shColorButton->setColor(color);

  shBoldButton->setChecked(font.weight() == QFont::Bold);
  shItalicButton->setChecked(font.italic());
}

void ConfigDialog::reload()
{
  /*
   * Editor's properties
   */

  // Query editor font
  int index = editorFontCombo->findText( Config::editorFont.family() );
  if( index != -1 )
  {
    editorFontCombo->setCurrentIndex( index );
    editorFont.setFamily( editorFontCombo->currentText() );
  }

  // Query editor font size
  if( Config::editorFont.pointSize() > 0 )
    editorFontSizeSpin->setValue( Config::editorFont.pointSize() );

  edAutoSaveCheckBox->setChecked( Config::editorAutoSave );
  editorSemanticCheckBox->setChecked( Config::editorSemantic );
  compGroubBox->setChecked( Config::compCharCount > 0 );
  encodingComboBox->setCurrentIndex( encodingComboBox->findText(
      Config::editorEncoding ) );
  if( Config::compCharCount > 0 )
    acSpinBox->setValue( Config::compCharCount );

  tabsizeSpin->setValue(Config::editorTabSize);


  /*
   * Syntax highlighting properties
   */
  QMapIterator<QString, QColor> it( Config::shColor );
  while( it.hasNext() )
  {
    it.next();
    QString key = it.key();
    int index = Config::shGroupList.indexOf( key );
    if( index == -1 )
      continue;

    QListWidgetItem *item = shListWidget->item( index );

    // color
    QColor color = it.value();
    if(color.isValid())
      item->setForeground(color);
    else
    qDebug() << tr("Invalid color") << color;

    // font
    QFont font;
    QTextCharFormat format = Config::shFormat[it.key()];
    font.setBold( format.fontWeight() == QFont::Bold );
    font.setItalic( format.fontItalic() );
    font.setUnderline( format.fontUnderline() );
    item->setFont( font );
  }

  /*
   * Reload syntax highlighting preferences
   */
  refreshSyntaxFont();
  reloadKeywords( 0 );
}

/**
 * Reloads the keywords list for the selected item
 */
void ConfigDialog::reloadKeywords(int index)
{
  shKeywordsListWidget->clear();
  switch(index)
  {
  case 0:
    // SQL keywords
    shKeywordsListWidget->addItems(SqlHighlighter::sqlKeywordList());
    break;
  case 1:
    // SQL functions
    shKeywordsListWidget->addItems(SqlHighlighter::sqlFunctionList());
    break;
  case 2:
    shKeywordsListWidget->addItems(SqlHighlighter::sqlTypeList());
    break;
  }
}

void ConfigDialog::save()
{
  Config::defaultDriver   = defaultDbCombo->currentDriverName();
  Config::editorAutoSave  = edAutoSaveCheckBox->isChecked();
  Config::editorEncoding  = encodingComboBox->currentText().toLower();
  Config::editorFont      = editorFontCombo->currentFont();
  Config::editorSemantic  = editorSemanticCheckBox->isChecked();
  Config::editorTabSize   = tabsizeSpin->value();
  if( compGroubBox->isEnabled() )
    Config::compCharCount = acSpinBox->value();
  else
    Config::compCharCount = -1;

	// Write syntax highlighting preferences
  QStringListIterator it( Config::shGroupList );
  QString name;
  QListWidgetItem *item;
  for( int i=0; it.hasNext(); i++ )
  {
    name = it.next();
    item = shListWidget->item( i );

    Config::shFormat[name].setFontWeight( item->font().weight() );
    Config::shFormat[name].setFontItalic( item->font().italic() );

    Config::shColor[name]   = item->foreground().color();
  }

  Config::save();
}

void ConfigDialog::setupConnections() {
  connect(buttonBox, SIGNAL(clicked(QAbstractButton*)),
          this, SLOT(handleClick(QAbstractButton*)));

  connect(editorFontCombo, SIGNAL(currentIndexChanged(int)),
          this, SLOT(updateEditorFont()));
  connect(editorFontSizeSpin, SIGNAL(valueChanged(int)),
          this, SLOT(updateEditorFont()));

  connect(shBoldButton, SIGNAL(clicked(bool)),
          this, SLOT(updateSyntaxFont()));
  connect(shColorButton, SIGNAL(clicked()),
          this, SLOT(updateSyntaxFont()));
  connect(shItalicButton, SIGNAL(clicked(bool)),
          this, SLOT(updateSyntaxFont()));

  connect(shListWidget, SIGNAL(currentRowChanged(int)),
          this, SLOT(refreshSyntaxFont(int)));
  connect(shListWidget, SIGNAL(currentRowChanged(int)),
          this, SLOT(reloadKeywords(int)));
}

void ConfigDialog::updateEditorFont() {
  editorTestArea->selectAll();
  editorFont.setFamily(editorFontCombo->currentText());
  editorFont.setPointSize(editorFontSizeSpin->value());
  editorTestArea->setFont(editorFont);
}

void ConfigDialog::updateSyntaxFont() {
  QListWidgetItem *item = shListWidget->currentItem();
  QFont font;
  if(shBoldButton->isChecked())
    font.setWeight(QFont::Bold);
  if(shItalicButton->isChecked())
    font.setItalic(true);
  item->setFont(font);

  item->setForeground(shColorButton->color());
}
