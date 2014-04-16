#include "logger.h"

Logger* Logger::instance = NULL;

Logger::Logger(QObject *parent)
  : QObject(parent) {
  instance = this;
  output = NULL;
}

void Logger::log(QString text) {
  if (output == NULL) {
    return;
  }

  QDateTime dt = QDateTime::currentDateTime();

  QString out = "<b>[%1]</b> %2";

  output->append(out.arg(dt.time().toString()).arg(text));

  emit entryAdded(dt, text);
}

void Logger::logError(QString text) {
  log(QString("<span style=\"color: red\">%1</span>").arg(text));
  emit error();
}
