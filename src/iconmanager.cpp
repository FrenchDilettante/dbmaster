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

#include <QApplication>
#include <QPainter>
#include <QPixmap>
#include <QStyle>

QMap<QString, QIcon> IconManager::m_icons;

/**
 * Créé une icône de dossier personnalisée.
 *
 * @param over
 *    L'icône à superposer
 */
QIcon IconManager::customFolder(QIcon over) {
  QPixmap baseIcon = ((QApplication*) QApplication::instance())->style()
                   ->standardIcon(QStyle::SP_DirIcon).pixmap(16, 16);

  QSize overSize = QSize(12, 12);
  QPoint topLeft = QPoint(16 - overSize.width(), 16 - overSize.height());
  QPainter painter;
  painter.begin(&baseIcon);
  painter.drawPixmap(QRect(topLeft, overSize),
                     over.pixmap(overSize));
  painter.end();

  return QIcon(baseIcon);
}

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

  if(QIcon::hasThemeIcon("gtk-preferences"))
    m_icons["preferences"] = QIcon::fromTheme("gtk-preferences");

  m_icons["folder_tables"] = QIcon(":/img/table.png");
  m_icons["folder_views"] = QIcon(":/img/table_lightning.png");
  m_icons["folder_systemtables"] = QIcon(":/img/table_gear.png");
#endif
}
