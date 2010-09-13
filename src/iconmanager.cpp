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


#include "iconmanager.h"

#include <QPainter>
#include <QPixmap>

QMap<QString, QIcon> IconManager::m_icons;

QIcon IconManager::get(QString name)
{
  if(m_icons.contains(name))
    return m_icons[name];
#if QT_VERSION >= 0x040600
  if(QIcon::hasThemeIcon(name))
    return QIcon::fromTheme(name);
#endif

  return fromRessources(name);
}

QIcon IconManager::fromRessources(QString name)
{
  return QIcon(QString(":/img/%1.png").arg(name));
}

void IconManager::init()
{
#if QT_VERSION >= 0x040600

  m_icons["connect_creating"]     = fromRessources("connect_creating");
  m_icons["connect_established"]  = fromRessources("connect_established");
  m_icons["connect_no"]           = fromRessources("connect_no");

  QPainter painter;
  if(QIcon::hasThemeIcon("gtk-preferences"))
    m_icons["preferences"] = QIcon::fromTheme("gtk-preferences");

  if(QIcon::hasThemeIcon("stock_database"))
  {
    QPixmap db_add(32, 32);
    painter.begin(&db_add);
//    painter.drawImage(QRect(0, 0, 32, 32),
//                      QIcon::fromTheme("stock_database").pixmap(32, 32)
//                      .toImage());
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(db_add.rect());
    painter.drawLine(0, 0, 32, 32);
    painter.end();

//    m_icons["db_add"] = QIcon(db_add);
  }

  if(QIcon::hasThemeIcon("folder"))
  {
    QPixmap folder_tables(16, 16);
    painter.begin(&folder_tables);
    painter.drawImage(QRect(0, 0, 16, 16),
                      QIcon::fromTheme("folder").pixmap(16, 16)
                      .toImage());
    painter.end();

//    m_icons["folder_tables"] = QIcon(folder_tables);

    QPixmap folder_views(16, 16);
    painter.begin(&folder_views);
    painter.drawImage(QRect(0, 0, 16, 16),
                      QIcon::fromTheme("folder").pixmap(16, 16)
                      .toImage());
    painter.drawText(QRect(8, 8, 8, 8), "V");
    painter.end();
  } else {
    m_icons["folder_tables"] = IconManager::get("folder");
  }

  QPixmap folder_view(16, 16);
  painter.begin(&folder_view);
  painter.end();
#endif
}
