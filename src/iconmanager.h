#ifndef ICONMANAGER_H
#define ICONMANAGER_H

#include <QIcon>
#include <QMap>
#include <QString>

class IconManager {
public:
  static QIcon get(QString name);
  static void init();

private:
  static QIcon fromRessources(QString name);
  static QIcon customFolder(QIcon over);

  static QMap<QString, QIcon> m_icons;
};

#endif // ICONMANAGER_H
