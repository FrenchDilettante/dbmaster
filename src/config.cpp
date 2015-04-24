#include "config.h"

#include "widgets/dbcombobox.h"

int             Config::compCharCount   = 3;
QString         Config::defaultDriver   = "";
bool            Config::editorAutoSave  = false;
QString         Config::editorEncoding  = "utf8";
QFont           Config::editorFont;
QString Config::editorIndentation = "\t";
bool            Config::editorSemantic  = true;
QMap<QString,QColor> Config::shColor;
QMap<QString,QTextCharFormat> Config::shFormat;
QStringList     Config::shGroupList;
int             Config::editorTabSize   = 4;
bool Config::editorTabUseSpaces = false;

void Config::init() {
  shGroupList << "sql_basics" << "sql_functions" << "sql_types" << "strings"
      << "numbers" << "comments" << "ctxt_table" << "ctxt_field";

  reload();
}

void Config::loadDefaults() {
  Config::compCharCount = 3;
  Config::editorAutoSave = false;
  Config::editorEncoding = "utf8";
  Config::editorFont = QFont("Monospace", 10);
  Config::editorSemantic = true;

  Config::shColor.clear();
  Config::shColor["sql_basics"] = Qt::black;
  Config::shColor["sql_functions"] = QColor("#644a9b");
  Config::shColor["sql_types"] = QColor("#0057ae");
  Config::shColor["strings"] = QColor("#bf0303");
  Config::shColor["numbers"] = QColor("#b07e00");
  Config::shColor["comments"] = QColor("#888786");
  Config::shColor["ctxt_table"] = QColor("#006e26");
  Config::shColor["ctxt_field"] = Qt::black;

  Config::shFormat.clear();
  Config::shFormat["sql_basics"] = QTextCharFormat();
  Config::shFormat["sql_basics"].setFontWeight(QFont::Bold);
  Config::shFormat["sql_functions"] = QTextCharFormat();
  Config::shFormat["sql_functions"].setFontItalic(true);
  Config::shFormat["sql_types"] = QTextCharFormat();
  Config::shFormat["strings"] = QTextCharFormat();
  Config::shFormat["numbers"] = QTextCharFormat();
  Config::shFormat["comments"] = QTextCharFormat();
  Config::shFormat["ctxt_table"] = QTextCharFormat();
  Config::shFormat["ctxt_field"] = QTextCharFormat();
}

void Config::reload() {
  QSettings s;
  if (!s.contains("default_database")) {
    loadDefaults();
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
  editorTabUseSpaces = s.value("tabusespaces").toBool();
  editorEncoding    = s.value("encoding").toString();
  editorFont.setFamily(s.value("font_name").toString());
  if (s.value("font_size").toInt() > 0) {
    editorFont.setPointSize(s.value("font_size").toInt());
  }
  updateIndentation(false);
  editorSemantic    = s.value("semantic").toBool();
  s.endGroup();

  /*
   * Syntax highlighting properties
   */
  int max = s.beginReadArray("highlighting");
  for (int i=0; i<max; i++) {
    s.setArrayIndex(i);
    QString group = s.value("groupname").toString();
    QColor c;
    c.setNamedColor(s.value("color").toString());
    shColor[group] = c;
    QTextCharFormat f;
    if (s.value("bold").toBool()) {
      f.setFontWeight(QFont::Bold);
    } else {
      f.setFontWeight(QFont::Normal);
    }
    f.setFontItalic(s.value("italic").toBool());
    shFormat[group] = f;
  }

  s.endArray();
}

void Config::save() {
  QSettings s;

  s.setValue("default_database", defaultDriver);

  s.beginGroup("editor");
  s.setValue("font_name", editorFont.family());
  s.setValue("font_size", editorFont.pointSize());
  s.setValue("autosave", editorAutoSave);
  s.setValue("comp_count", compCharCount);
  s.setValue("tabsize", editorTabSize);
  s.setValue("tabusespaces", editorTabUseSpaces);
  s.setValue("encoding", editorEncoding);
  s.setValue("semantic", editorSemantic);
  s.endGroup();

  // Write syntax highlighting preferences
  s.beginWriteArray("highlighting", shGroupList.size());

  QStringListIterator it( shGroupList );
  QString name;
  for (int i=0; it.hasNext(); i++) {
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

QString Config::updateIndentation(bool saveSettings) {
  if (editorTabUseSpaces) {
    switch (editorTabSize) {
    case 2:
      editorIndentation = "  ";
      break;
    case 4:
      editorIndentation = "    ";
      break;
    case 8:
      editorIndentation = "        ";
      break;
    }
  } else {
    editorIndentation = "\t";
  }

  if (saveSettings) {
    save();
  }
  return editorIndentation;
}
