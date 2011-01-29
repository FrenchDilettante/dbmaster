Gestion des connexions
======================

DbMaster apporte la possibilité de stocker et d'utiliser les connexions vers des SGBD dans une fenêtre unique.

Vous pouvez avoir en simultané autant de connexions que vous le souhaitez, vers autant de SGBD différents. Ces connexions seront enregistrées et de nouveau disponible au prochain démarrage de DbMaster, un peu à la manière des marque-pages pour les navigateurs web.


Pilotes
-------

Avant d'aller plus loin dans cette section, il est nécessaire de prendre connaissance des limitations de DbMaster.

DbMaster s'appuie sur la technologie Qt pour se connecter aux SGBD. S'il n'y a pas de pilote spécifique disponible pour votre SGBD, vous pourrez vous reporter à cette [page](http://doc.qt.nokia.com/4.7/sql-driver.html).

À l'heure actuelle, il n'y a pas de limite fixée au nombre de connexions simultanées.


Opérations
----------


### Ajouter une connexion ###

Vous avez vu précédemment que le premier bouton de la barre d'outil lance l'assistant d'ajout de connexion. Vous trouverez cet assistant dans le menu Fichier → Nouvelle → Connexion, dans le menu contextuel du volet de connexions (clic droit) ou dans le gestionnaire de connexions (voir plus loin).

Les différents champs de l'assistant sont en saisie libre (à part le nom de base) afin de rester compatible avec des SGBD comme SQLite qui, souvent, n'ont besoin que du chemin vers le fichier de base de données.


### Modifier une connexion ###

La modification est accessible dans le menu contextuel de la liste à gauche ou directement dans le gestionnaire.

Il y a deux restrictions : vous ne pouvez ni modifier une connexion active, ni changer le driver.


### Supprimer une connexion ###

Comme pour la modification, par le menu contextuel ou le gestionnaire.

Vous ne pouvez pas supprimer une connexion active.


### Tester une connexion ###

À l'ajout comme à la modification, vous pouvez tester vos paramètres. DbMaster créera une connexion temporaire pour savoir si le serveur répond ou non.


Gestionnaire de connexions
--------------------------

Le gestionnaire est disponible dans le menu Outils. Toutes les opérations possibles sur les connexions y sont intégrées.

Vous pouvez aussi activer ou désactiver une connexion (rappel : elle doit être inactive pour être éditée).
