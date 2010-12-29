Éditeur SQL
===========

Nous allons étudier ici les différentes fonctionnalités apportées par l'éditeur SQL pour assister l'écriture de requêtes.


Saisie et exécution
-------------------

Ouvrez un nouvel onglet d'édition (Ctrl+N) et saisissez la requête suivante :

	SELECT nom FROM utilisateur
	WHERE dateinscription = CURRENT_DATE

Les mots clés `SELECT`, `FROM` et `WHERE` sont en noir, tandis que `CURRENT_DATE` est en violet ; c'est la coloration syntaxique. Vous aurez noté qu'au fûr et à mesure de la saisie, une petite boîte proposait des mots en fonction de ce que vous tapiez (par exemple `SELECT` pour `SEL` ; c'est l'auto-complément.

Avant d'exécuter cette requête, vous devez sélectionner une connexion active dans la liste de sélection à la gauche de l'onglet.

Il est fort propable que la requête échoue, à moins que vous ayez une table nommée `utilisateur` avec une colonne `dateinscription`.

Saisissez une autre requête avec des noms de table et de colonne que vous auriez, afin d'avoir un résultat.

Dans les deux cas, une fois la requête exécutée, deux onglets apparaissent en bas, 'Déboguage' et 'Résultat', le premier donnant un historique des opérations, le second le résultat de la requête.


Analyse sémantique
------------------

Une composante commune à la coloration syntaxique et à l'auto-complément est l'analyse sémantique : un outil d'extraction de mots-clés supplémentaires en fonction du contexte.

Concrêtement, DbMaster compile les noms de table et de colonne de la connexion en cours afin de les proposer à la saisie & l'affichage.


Coloration syntaxique
---------------------

La coloration syntaxique prend en compte les mots clés SQL standards. DbMaster ne gère par les syntaxes particulières, par exemple le mot clé `AUTO_INCREMENT` utilisé par MySQL.

Vous pouvez visualiser les mots-clés et régler les couleurs associées dans les préférences (voir dans la section associée).

**Note :** la coloration n'est pas sensible à la casse. `SELECT` et `select` seront colorés indifféremment.

### Ajouter manuellement des mots-clés ###

Les mots-clés sont stockés dans les fichiers en texte plain. Dans le code source, ils sont disponibles dans le dossier `src/share/`.
Sur Windows, dans le dossier `share\sqlsyntax\`, sur Linux `/usr/share/dbmaster/sqlsyntax/` par défaut.

Vous y trouverez 4 fichiers :

* `sql_basics` mots-clés de base (`SELECT`, `FROM`, ...)
* `sql_functions` fonctions (`CURRENT_DATE`, `UPPER`, ...)
* `sql_operators` opérateurs logiques (`OR`, `AND`, ...)
* `sql_types` types normalisés (`INT`, `CHAR`, ...)


Auto-complément
---------------

L'auto-complément reprend les mêmes mots-clés que la coloration syntaxique. De même, vous pouvez le configurer dans les préférences.

Par défaut, il se déclenche à partir de trois caractères saisis. Vous pouvez aussi l'activer manuellement en pressant Ctrl+Espace.


Affichage du résultat
---------------------


### Export ###

#### CSV ####

#### HTML ####
