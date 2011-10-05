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


#ifndef SQLHIGHLIGHTER_H
#define SQLHIGHLIGHTER_H

#include <QtGui>

/**
 * Syntax highlighting
 */
class SqlHighlighter : public QSyntaxHighlighter
{
Q_OBJECT
public:
  SqlHighlighter(QTextEdit*);

  void reloadContext(QStringList tables, QMultiMap<QString, QString> fields);

  static void reloadColors();
  static bool reloadKeywords();
  static QStringList sqlKeywordList();
  static QStringList sqlFunctionList();
  static QStringList sqlTypeList();

private:
  /**
    * This struct 
    */
  struct HighlightRule
  {
    QRegExp pattern;
    QTextCharFormat format;
  };
  void highlightBlock(const QString&);

  QVector<HighlightRule>        contextRules;

  static QStringList            basicSqlKeywords;
  static QRegExp                beginBlockMarkers[2];
  static QTextCharFormat        blockFormats[2];
  static QRegExp                endBlockMarkers[2];
  static QStringList            sqlFunctions;
  static QStringList            sqlTypes;
  static QVector<HighlightRule> highlightingRules;
};

#endif // SQLHIGHLIGHTER_H
