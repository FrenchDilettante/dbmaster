Démarrage
=========


Installation
------------

### Linux ###

#### Debian & dérivées (Ubuntu) ####

L'installation du paquet .deb est entièrement automatique.

### Windows ###

L'archive binaire `dbmaster-0.8.zip` contient tout ce qui est nécessaire pour utiliser DbMaster. Une fois extraite, vous pouvez exécuter `dbmaster.exe`.


Premier lancement
-----------------

La première fois que vous exécutez DbMaster, un assistant s'affichera à l'écran afin de déterminer les paramètres par défaut.
Il vous sera demandé quel driver par défaut utiliser pour les nouvelles connexions. Par exemple, un développeur web aura souvent besoin de MySQL. Ce choix n'est ni définitif ni exclusif. Vous pourrez modifier ce réglage ultérieurement, de même que les autres drivers seront toujours accessibles.

### Mise à jour ###

Si vous utilisiez DbMaster 0.7, vos paramètres seront importés automatiquement.


### Ajouter une connexion ###

Le premier bouton de la barre d'outil (accessible aussi dans Fichier→Nouvelle→Connexion) lance l'assistant d'ajout de connexion.
Une fois le SGBD choisi, vous pouvez utiliser le driver natif ou passer par ODBC en cochant la case adéquate.
L'adresse n'est pas obligatoire à renseigner pour certains SGBD (SQLite notamment).
Sur la page suivante, vous pouvez tester les informations saisies.

Lors de la validation de l'ajout, la connexion est disponible dans le volet de gauche.

Une fois la connexion enregistrée, vous pouvez vous connecter en double-cliquant dessus ou par un clic-droit→Connecter.


### Première requête ###

Pour lancer l'éditeur SQL, cliquez sur Fichier→Nouvelle→Requête. Vous pouvez saisir votre requête dans le nouvel onglet, par exemple `SELECT CURRENT_DATE`, puis l'exécuter.

S'il n'y a pas d'erreur, le résultat s'affichera en bas de l'éditeur.
