#ifndef CONFIG_H
#define CONFIG_H

#include <QFont>
#include <QMap>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QStringListIterator>
#include <QTextCharFormat>

class Config {
public:
  static void init();
  static void loadDefaults();
  static void reload();
  static void save();
  static QString updateIndentation(bool saveSettings=true);

  static int              compCharCount;
  static QString          defaultDriver;
  static QFont            editorFont;
  static bool             editorAutoSave;
  static QString          editorEncoding;
  static QString editorIndentation;
  static bool             editorSemantic;
  static QMap<QString,QColor> shColor;
  static QMap<QString,QTextCharFormat> shFormat;
  static QStringList      shGroupList;
  static int              editorTabSize;
  static bool editorTabUseSpaces;
};

#endif // CONFIG_H
