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


#ifndef WRAPPER_H
#define WRAPPER_H

#include "plugin.h"
#include "../db_enum.h"

#include <QList>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QString>
#include <QStringList>

class SqlWrapper : public Plugin {
public:
  enum WrapperFeature {
    BasicFeatures     = 0x0000,
    CustomTypes       = 0x0001,
    Dumps             = 0x0002,
    ForeignKeys       = 0x0004,
    Indexes           = 0x0008,
    ODBC              = 0x0010,
    Schemas           = 0x0020,
    StoredProcedures  = 0x0040,
    Triggers          = 0x0080,
    Views             = 0x0100
  };
  Q_DECLARE_FLAGS(WrapperFeatures, WrapperFeature)


  /// @deprecated
  virtual SqlWrapper* newInstance(QSqlDatabase db) =0;

  virtual QList<SqlColumn> columns(QString table) { return QList<SqlColumn>(); };

  virtual QList<SqlConstraint> constraints(QString table) { return QList<SqlConstraint>(); };

  virtual QString driver() =0;
  virtual QString driverName() { return ""; };
  virtual QString driverIconCode() { return ""; };

  virtual WrapperFeatures features() =0;

  /**
   * Liste des index pour une table donnée.
   */
  virtual QList<SqlIndex> indexes(QString table) { return QList<SqlIndex>(); };

  virtual bool isIndexed() { return true; };
  virtual bool isRemote() { return true; };

  /**
   * Spécifie si le SGBD fonctionne par le réseau ou non. Si oui, il aura besoin
   * d'un nom d'hôte.
   */
  virtual bool requiresHostname() { return true; };

  /** Extrait un schéma selon son nom */
  virtual SqlSchema schema(QString s) { return SqlSchema(); };

  /**
   * Retourne la liste des schémas disponibles sur cette connexion.
   *
   * @param fillTables
   *    Cette option permet à l'adaptateur de remplir la liste des tables pour
   *    chaque schéma. Voir la variable tables du type SqlSchema.
   *    Noter que cette option est par défaut à vrai.
   *
   * @return une liste vide si la fonctionnalité n'est pas supportée.
   */
  virtual QList<SqlSchema> schemas() { return QList<SqlSchema>(); };

  /** Extrait une table depuis son nom */
  virtual SqlTable table(QString t) =0;

  virtual QList<SqlTable> tables() { return QList<SqlTable>(); };
  virtual QList<SqlTable> tables(QString schema) { return QList<SqlTable>(); };

  QSqlDatabase db() { return m_db; };

protected:
  QSqlDatabase m_db;
};

Q_DECLARE_INTERFACE(SqlWrapper, "dbmaster.Wrapper")
Q_DECLARE_OPERATORS_FOR_FLAGS(SqlWrapper::WrapperFeatures)

#endif // WRAPPER_H
