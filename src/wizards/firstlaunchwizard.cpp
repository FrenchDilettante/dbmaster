#include "../config.h"
#include "firstlaunchwizard.h"

#include "../config.h"

FirstLaunchWizard::FirstLaunchWizard(QWidget *parent)
  : QWizard(parent) {
  setupUi(this);

  Config::compCharCount = 3;
  Config::editorAutoSave = false;
  Config::editorEncoding = "utf8";
  Config::editorFont = QFont("Monospace", 10);
  Config::editorSemantic = true;

  Config::shColor.clear();
  Config::shColor["sql_basics"]     = Qt::black;
  Config::shColor["sql_functions"]  = QColor("#644a9b");
  Config::shColor["sql_types"]      = QColor("#0057ae");
  Config::shColor["strings"]        = QColor("#bf0303");
  Config::shColor["numbers"]        = QColor("#b07e00");
  Config::shColor["comments"]       = QColor("#888786");
  Config::shColor["ctxt_table"]     = QColor("#006e26");
  Config::shColor["ctxt_field"]     = Qt::black;

  Config::shFormat.clear();
  Config::shFormat["sql_basics"]    = QTextCharFormat();
  Config::shFormat["sql_basics"].setFontWeight(QFont::Bold);
  Config::shFormat["sql_functions"] = QTextCharFormat();
  Config::shFormat["sql_functions"].setFontItalic(true);
  Config::shFormat["sql_types"]     = QTextCharFormat();
  Config::shFormat["strings"]       = QTextCharFormat();
  Config::shFormat["numbers"]       = QTextCharFormat();
  Config::shFormat["comments"]      = QTextCharFormat();
  Config::shFormat["ctxt_table"]    = QTextCharFormat();
  Config::shFormat["ctxt_field"]    = QTextCharFormat();
}

void FirstLaunchWizard::accept() {
  Config::defaultDriver = dbComboBox->currentDriverName();
  Config::save();

  emit accepted();
  close();
}
