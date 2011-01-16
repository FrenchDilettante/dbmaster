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


#ifndef PLUGIN_H
#define PLUGIN_H

#include <QString>
#include <QStringList>
#include <QtPlugin>

class Plugin {
public:
  /**
   * Liste des fichiers accompagnant le plugin.
   * Il est vivement recommandé de ne mettre que des noms de fichiers relatifs.
   */
  virtual QStringList files() { return QStringList(); };

  /**
   * Nom de code (ou identifiant) unique du plugin, par ex. DBM.MYSQL.WRAPPER.
   *
   * Doit respecter le format suivant :
   *   [Nom du fournisseur/éditeur][Spécificité du plugin][Catégorie]
   *   DBM.                        MYSQL.                 WRAPPER
   *   DBM.                        CSV.                   EXPORTENGINE
   * Ne doit contenir que des caractères alphanumériques et '_' ou '.'
   */
  virtual QString plid()    =0;

  /**
   * Nom à afficher (traduisible), par ex. "Export au format CSV" ou "Adaptateur
   * MySQL"
   */
  virtual QString title()   =0;

  /**
   * Laissée à l'appreciation du développeur.
   */
  virtual QString version() =0;
};

Q_DECLARE_INTERFACE(Plugin, "dbmaster.Plugin/0.8")

#endif // PLUGIN_H
