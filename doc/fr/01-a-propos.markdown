À propos
========

DbMaster est un outil libre de gestion de bases de données et d'édition de requêtes.
Actuellement en plein développement, il est avant tout destiné aux développeurs et aux étudiants. Son objectif est de fournir une alternative libre aux produits propriétaires comme AnySQL.

À la différences d'autres outils spécifiques (pgAdmin, MySQL Workbench, etc.) les fonctionnalités ne dépendent pas du SGBD utilisé.

Compatibilité SGBD
------------------

DbMaster est bâti sur la technologie Qt, qui offre un framework pour la connexion aux base de données relationnelles : [QtSql](http://doc.qt.nokia.com/latest/qtsql.html).
Nokia fournit les drivers pour les SGBD suivants :

* IBM DB2
* Borland InterBase (compatible Firebird)
* MySQL
* Oracle Call Interface Driver
* Open Database Connectivity (ODBC) (driver générique)
* PostgreSQL
* SQLite
* Sybase Adaptive Server (obsolète)

Les SGBD suivants ont été testés et validés :

* Firebird
* MySQL
* PostgreSQL
* SQLite

Les autres n'ont pu être testés.

Installation
------------

### Linux ###

#### Debian & dérivées (Ubuntu) ####

L'installation du paquet .deb est entièrement automatique.

### Windows ###

L'archive binaire dbmaster-0.7.zip contient tout ce qui est nécessaire pour utiliser DbMaster. Une fois extraite, vous pouvez exécuter dbmaster.exe.

Auteurs
-------

Développement :

- Emmanuel BOURGERIE

Traduction :

[fr] - Emmanuel BOURGERIE

Outils tiers :

- QColorButton : Florian BECKER

Soutenir le projet
------------------

DbMaster est un projet libre développé bénévolement. Si vous souhaitez participer au projet, rendez-vous sur le site (http://projets.developpez.com/projects/dbmaster).
