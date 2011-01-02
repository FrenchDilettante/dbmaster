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

**Note :** DbMaster utilise le même mécanisme pour afficher le résultat d'une requête et d'une table.

Afin de faciliter la lecture, les résultats sont "paginés", c'est à dire qu'une requête retournant 100 lignes sera découpée en 3 pages de 30 lignes et une de 10 lignes.

Le nombre de lignes par page est réglable.

Les quatres boutons de parcours permettent, dans l'ordre, d'accéder à :

* La première page
* La page précédente
* La page suivante
* La dernière page

Le dernier bouton permet de relancer l'exécution de la requête (pour prendre en compte d'éventuelles modifications dans la base).


Export
------

Afin de pouvoir réutiliser le résultat d'une requête dans d'autres logiciels, DbMaster permet de l'exporter directement aux formats `CSV` ou `HTML`.

L'assistant est disponible en faisant un clic droit sur le tableau des résultats, puis "Exporter".

Voici les spécificités pour chaque format :


### CSV ###

Quelques mots de vocabulaires sont à connaître. Une valeur est encadrée par le *délimiteur*, puis un *séparateur* est placé entre chaque valeur.

Exemple :

	SELECT usr_id, usr_name
	FROM utilisateur

Donnera :

	"3";"jdupond";

Ici, le délimiteur est `"`, le séparateur `;`.

La ligne d'en-tête ne fait que reprendre les noms de colonnes (`usr_id`, `usr_name`).


### HTML ###

L'export au format HTML a un but de présentation, il inclut donc deux options dans ce sens.

"Exporter la requête" inclura la requête avant le résultat.

Vous pouvez aussi n'exporter qu'une partie du résultat s'il est long.
