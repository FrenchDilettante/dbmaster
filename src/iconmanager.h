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
