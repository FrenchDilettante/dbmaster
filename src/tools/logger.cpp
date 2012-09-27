#include "logger.h"

Logger* Logger::instance = NULL;

Logger::Logger(QObject *parent)
  : QObject(parent) {
  instance = this;
  output = NULL;
}

void Logger::log(QSqlQuery *query) {

}

void Logger::log(QString text) {
  if (output == NULL) {
    return;
  }

  QDateTime dt = QDateTime::currentDateTime();

  QString out = "[%t] %x";

  output->append(out
                 .replace("%t", dt.time().toString(Qt::DefaultLocaleShortDate))
                 .replace("%x", text));

  emit entryAdded(dt, text);
}
