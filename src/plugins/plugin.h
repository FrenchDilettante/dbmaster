#ifndef PLUGIN_H
#define PLUGIN_H

#include <QDialog>
#include <QString>
#include <QStringList>
#include <QtPlugin>

class Plugin {
public:
  /** Boîte de dialogue de configuration. */
  virtual QDialog* configDialog() { return NULL; };

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

  /** Enregistre la configuration actuelle du plugin. */
  virtual void save() {};

  /**
   * Nom à afficher (traduisible), par ex. "Export au format CSV" ou "Adaptateur
   * MySQL"
   */
  virtual QString title()   =0;

  virtual QString vendor()  =0;

  /** Laissée à l'appreciation du développeur. */
  virtual QString version() =0;
};

Q_DECLARE_INTERFACE(Plugin, "dbmaster.Plugin/0.8")

#endif // PLUGIN_H
