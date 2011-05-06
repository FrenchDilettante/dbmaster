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


#include "config.h"

#include "widgets/dbcombobox.h"
#include "wizards/firstlaunchwizard.h"

int             Config::compCharCount   = 3;
QString         Config::defaultDriver   = "";
bool            Config::editorAutoSave  = false;
QString         Config::editorEncoding  = "utf8";
QFont           Config::editorFont;
bool            Config::editorSemantic  = true;
QMap<QString,QColor> Config::shColor;
QMap<QString,QTextCharFormat> Config::shFormat;
QStringList     Config::shGroupList;
int             Config::editorTabSize   = 4;

void Config::init()
{
  shGroupList << "sql_basics" << "sql_functions" << "sql_types" << "strings"
      << "numbers" << "comments" << "ctxt_table" << "ctxt_field";

  reload();
}

void Config::reload()
{
  QSettings s;
  if(!s.contains("default_database"))
  {
    // Test version < 0.8
    QSettings s_0_7("dbmaster.sourceforge.net", "dbmaster");
    if (s_0_7.contains("default_database")) {
      import_0_7();
    } else {
      QWizard *wizard = new FirstLaunchWizard;
      wizard->exec();
    }
    save();
  }

  defaultDriver = s.value("default_database").toString();

  /*
   * Editor's properties
   */
  s.beginGroup("editor");
  editorAutoSave    = s.value("autosave").toBool();
  compCharCount     = s.value("comp_count").toInt();
  editorTabSize     = s.value("tabsize").toInt();
  editorEncoding    = s.value("encoding").toString();
  editorFont.setFamily(s.value("font_name").toString());
  if(s.value("font_size").toInt() > 0)
    editorFont.setPointSize(s.value("font_size").toInt());
  editorSemantic    = s.value("semantic").toBool();
  s.endGroup();

  /*
   * Syntax highlighting properties
   */
  int max = s.beginReadArray("highlighting");
  for(int i=0; i<max; i++)
  {
    s.setArrayIndex(i);
    QString group = s.value("groupname").toString();
    QColor c;
    c.setNamedColor(s.value("color").toString());
    shColor[group] = c;
    QTextCharFormat f;
    if(s.value("bold").toBool())
      f.setFontWeight(QFont::Bold);
    else
      f.setFontWeight(QFont::Normal);
    f.setFontItalic(s.value("italic").toBool());
    shFormat[group] = f;
  }

  s.endArray();
}

void Config::save()
{
  QSettings s;

  s.setValue("default_database", defaultDriver);

  s.beginGroup("editor");
  s.setValue("font_name", editorFont.family());
  s.setValue("font_size", editorFont.pointSize());
  s.setValue("autosave", editorAutoSave);
  s.setValue("comp_count", compCharCount);
  s.setValue("tabsize", editorTabSize);
  s.setValue("encoding", editorEncoding);
  s.setValue("semantic", editorSemantic);
  s.endGroup();

  // Write syntax highlighting preferences
  s.beginWriteArray("highlighting", shGroupList.size());

  QStringListIterator it( shGroupList );
  QString name;
  for( int i=0; it.hasNext(); i++ )
  {
    name = it.next();

    s.setArrayIndex(i);
    s.setValue("groupname", name);
    s.setValue("color", shColor[name].name());
    QFont f = shFormat[name].font();
    s.setValue("bold", f.weight() == QFont::Bold);
    s.setValue("italic", f.italic());
  }
  s.endArray();

  s.sync();
}

void Config::import_0_7() {
  QSettings sold("dbmaster.sourceforge.net", "dbmaster");
  defaultDriver = sold.value("default_database").toString();

  /*
   * Editor's properties
   */
  sold.beginGroup("editor");
  editorAutoSave = sold.value("autosave").toBool();
  compCharCount = sold.value("comp_count").toInt();
  editorEncoding = sold.value("encoding").toString();
  editorFont.setFamily(sold.value("font_name").toString());
  if(sold.value("font_size").toInt() > 0)
    editorFont.setPointSize(sold.value("font_size").toInt());
  editorSemantic = sold.value("semantic").toBool();
  sold.endGroup();

  /*
   * Syntax highlighting properties
   */
  int max = sold.beginReadArray("highlighting");
  for(int i=0; i<max; i++)
  {
    sold.setArrayIndex(i);
    QString group = sold.value("groupname").toString();
    QColor c;
    c.setNamedColor(sold.value("color").toString());
    shColor[group] = c;
    QTextCharFormat f;
    if(sold.value("bold").toBool())
      f.setFontWeight(QFont::Bold);
    else
      f.setFontWeight(QFont::Normal);
    f.setFontItalic(sold.value("italic").toBool());
    shFormat[group] = f;
  }

  sold.endArray();
}
