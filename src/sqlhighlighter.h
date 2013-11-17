#ifndef SQLHIGHLIGHTER_H
#define SQLHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QtWidgets/QTextEdit>

/**
 * Syntax highlighting
 */
class SqlHighlighter : public QSyntaxHighlighter {
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
  struct HighlightRule {
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
