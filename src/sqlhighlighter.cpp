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


#include "sqlhighlighter.h"

#include "config.h"

QVector<SqlHighlighter::HighlightRule>
                        SqlHighlighter::highlightingRules;
QStringList             SqlHighlighter::basicSqlKeywords;
QRegExp                 SqlHighlighter::beginBlockMarkers[2];
QTextCharFormat         SqlHighlighter::blockFormats[2];
QRegExp                 SqlHighlighter::endBlockMarkers[2];
QStringList             SqlHighlighter::sqlFunctions;
QStringList             SqlHighlighter::sqlTypes;

/*
 * Blockstates :
 *   0 : nothing special
 *   1 : string
 *   2 : comment
 */

SqlHighlighter::SqlHighlighter( QTextEdit *parent )
		: QSyntaxHighlighter( parent )
{
  QMap<QString,QColor> colors = Config::shColor;
  QMap<QString,QTextCharFormat> formats = Config::shFormat;
  HighlightRule rule;

  // basic SQL syntax
  foreach( const QString &pattern, basicSqlKeywords )
  {
    rule.format = formats["sql_basics"];
    rule.format.setForeground( colors["sql_basics"] );
    rule.pattern = QRegExp( "\\b" + pattern.toLower() + "\\b" );
    highlightingRules.append( rule );
    rule.pattern = QRegExp( "\\b" + pattern.toUpper() + "\\b" );
    highlightingRules.append( rule );
  }

  // SQL functions
  foreach( const QString &pattern, sqlFunctions )
  {
    rule.format = formats["sql_functions"];
    rule.format.setForeground( colors["sql_functions"] );
    rule.pattern = QRegExp( "\\b" + pattern.toLower() + "\\b" );
    highlightingRules.append( rule );
    rule.pattern = QRegExp( "\\b" + pattern.toUpper() + "\\b" );
    highlightingRules.append( rule );
  }

  // SQL types
  foreach( const QString &pattern, sqlTypes )
  {
    rule.format = formats["sql_types"];
    rule.format.setForeground( colors["sql_types"] );
    rule.pattern = QRegExp( "\\b" + pattern.toLower() + "\\b" );
    highlightingRules.append( rule );
    rule.pattern = QRegExp( "\\b" + pattern.toUpper() + "\\b" );
    highlightingRules.append( rule );
  }

  // numbers
  rule.format = formats["numbers"];
  rule.format.setForeground( colors["numbers"] );
  rule.pattern = QRegExp( "\\b\\d+\\.?\\d*\\b" );
  highlightingRules << rule;

  /*
   * Blocks
   */
  blockFormats[0] = formats["strings"];
  blockFormats[0].setForeground( colors["strings"] );

  blockFormats[1] = formats["comments"];
  blockFormats[1].setForeground( colors["comments"] );

  beginBlockMarkers[0] = QRegExp( "(\"|'|`)" );
  beginBlockMarkers[1] = QRegExp( "--" );

  endBlockMarkers[0] = QRegExp( "(\"|'|`)" );
  endBlockMarkers[1] = QRegExp( "\n" );
}

void SqlHighlighter::highlightBlock( const QString &block )
{
  if( block.isEmpty() || block.isNull() )
    return;

  // keywords
  foreach( const HighlightRule &rule, highlightingRules )
  {
    QRegExp expression( rule.pattern );
    int index = expression.indexIn( block );
    while( index >= 0 )
    {
      int length = expression.matchedLength();
      setFormat( index, length, rule.format );
      index = expression.indexIn( block, index + length );
    }
  }

  // context
  foreach( const HighlightRule &rule, contextRules )
  {
    QRegExp expression( rule.pattern );
    int index = expression.indexIn( block );
    while( index >= 0 )
    {
      int length = expression.matchedLength();
      setFormat( index, length, rule.format );
      index = expression.indexIn( block, index + length );
    }
  }

  /*
   * Handling blocks (comments, strings, etc.)
   */
  setCurrentBlockState(0);

  int startIndex = 0;

  /*
   * Each block category (comments, strings, etc.) will be analysed.
   */
  for( int s = 0; s < 2; s++ )
  {
    if( previousBlockState() != s + 1 )
      startIndex = block.indexOf( beginBlockMarkers[s] );

    // if startIndex < 0, it means all blocks have been proceeded
    while( startIndex >= 0 )
    {
      // strings have a special case :)
      if( s == 0 )
        endBlockMarkers[0] = QRegExp( block.at( startIndex ) );

      int endIndex = block.indexOf( endBlockMarkers[s], startIndex + 1);
      int length;

      // does the block end anywhere ?
      if( endIndex == -1 )
      {
        setCurrentBlockState( s + 1 );
        length = block.length() - startIndex;
      }
      else
      {
        length = endIndex - startIndex + beginBlockMarkers[s].matchedLength();
      }

      // applying syntax format
      setFormat( startIndex, length, blockFormats[s] );

      startIndex = block.indexOf( beginBlockMarkers[s], startIndex + length );
    }
  }
}

void SqlHighlighter::reloadColors()
{
}

void SqlHighlighter::reloadContext(QStringList tables,
                                   QMultiMap<QString, QString>fields)
{
  contextRules.clear();
  HighlightRule r;
  foreach( QString t, tables )
  {
    r.pattern = QRegExp( "\\b" + t + "\\b" );
    r.format = Config::shFormat["ctxt_table"];
    r.format.setForeground( Config::shColor["ctxt_table"] );
    r.format.setUnderlineStyle(QTextCharFormat::SingleUnderline);
    contextRules << r;
  }

  QStringList l = fields.values();
#if QT_VERSION >= 0x040500
  l.removeDuplicates();
#endif

  foreach( QString f, l )
  {
    r.pattern = QRegExp( "\\b" + f + "\\b" );
    r.format = Config::shFormat["ctxt_field"];
    r.format.setForeground( Config::shColor["ctxt_field"] );
    contextRules << r;
  }

  rehighlight();
}

void SqlHighlighter::reloadKeywords()
{
  basicSqlKeywords.clear();
  sqlFunctions.clear();
  sqlTypes.clear();

  QStringList files;
  QString prefix;
#if defined( Q_WS_X11 )
  prefix = QString( PREFIX ).append( "/share/dbmaster/sqlsyntax/" );
#endif
#if defined(Q_WS_WIN)
  prefix = QString(PREFIX).append("\\share\\sqlsyntax\\");
#endif
  files << "sql_basics"
        << "sql_functions"
        << "sql_types";

  for( int i=0; i<files.size(); i++ )
  {
    QFile file( files[i].prepend( prefix ) );
    if( !file.open( QIODevice::ReadOnly | QIODevice::Text ) )
    {
      qDebug() << "Cannot open " << file.fileName();
      return;
    }

    while( !file.atEnd() )
    {
      QString line = file.readLine();
      if( line.isEmpty() )
        continue;

      line.remove( "\n" );

      switch( i )
      {
      case 0:
        basicSqlKeywords << line;
        break;
      case 1:
        sqlFunctions << line;
        break;
      case 2:
        sqlTypes << line;
      }
    }

    file.close();
  }

  reloadColors();
}

QStringList SqlHighlighter::sqlFunctionList()
{
  return sqlFunctions;
}

QStringList SqlHighlighter::sqlKeywordList()
{
  return basicSqlKeywords;
}

QStringList SqlHighlighter::sqlTypeList()
{
  return sqlTypes;
}
