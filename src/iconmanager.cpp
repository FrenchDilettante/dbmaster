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

  QSize overSize = QSize(13, 13);
  QPoint topLeft = QPoint(16 - overSize.width(), 16 - overSize.height());
  QPainter painter;
  painter.begin(&baseIcon);
  painter.drawPixmap(QRect(topLeft, overSize),
                     over.pixmap(overSize));
  painter.end();

  return QIcon(baseIcon);
}

QIcon IconManager::get(QString name) {
  if (m_icons.contains(name)) {
    return m_icons[name];
  }
  if (QIcon::hasThemeIcon(name)) {
    return QIcon::fromTheme(name);
  }

  return fromRessources(name);
}

QIcon IconManager::fromRessources(QString name) {
  return QIcon(QString(":/img/%1.png").arg(name));
}

void IconManager::init() {
  m_icons["connect_creating"]     = fromRessources("connect_creating");
  m_icons["connect_established"]  = fromRessources("connect_established");
  m_icons["connect_no"]           = fromRessources("connect_no");

  if (QIcon::hasThemeIcon("gtk-preferences")) {
    m_icons["preferences"] = QIcon::fromTheme("gtk-preferences");
  }

  m_icons["folder_columns"] = customFolder(QIcon(":/img/column.png"));
  m_icons["folder_schemas"] = customFolder(QIcon(":/img/schema.png"));
  m_icons["folder_systemtables"] = customFolder(QIcon(":/img/table_gear.png"));
  m_icons["folder_tables"] = customFolder(QIcon(":/img/table.png"));
  m_icons["folder_views"] = customFolder(QIcon(":/img/table_lightning.png"));
}
