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


#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QtGui>

#include "../sqlhighlighter.h"

#include "ui_configdialog.h"

class ConfigDialog: public QDialog, Ui::ConfigDialog {
Q_OBJECT
public:
  ConfigDialog(QWidget* =0);

  /**
   * Reload preferences
   */
  void reload();

  /**
   * Save preferences
   * @todo migrate it to Config
   */
  void save();

private:
  void accept();
  SqlHighlighter *syntaxHighlighter;
  void setupConnections();

  static QFont editorFont;
  static QMap<QString,QColor> shColor;
  static QMap<QString,QTextCharFormat> shFormat;

private slots:
  void handleClick(QAbstractButton *button);
  void refreshSyntaxFont(int index =0);
  void reloadKeywords(int index);

  /**
   * Update the editor's font settings
   */
  void updateEditorFont();

  /**
   * Set the current font with the user's parameters
   */
  void updateSyntaxFont();
};

#endif // CONFIGDIALOG_H
