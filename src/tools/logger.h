#ifndef LOGGER_H
#define LOGGER_H

#include <QDateTime>
#include <QObject>
#include <QTextBrowser>

class Logger : public QObject {
Q_OBJECT
public:
  explicit Logger(QObject *parent = 0);
  void setTextBrowser(QTextBrowser *output) { this->output = output; };

  static Logger *instance;

signals:
  void entryAdded(QDateTime dt, QString entry);
  void error();

public slots:
  void log(QString text);
  void logError(QString text);

private:
  QTextBrowser *output;
};

#endif // LOGGER_H
