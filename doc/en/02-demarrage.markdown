First launch
============


Installation
------------

### Linux ###

#### Debian & debian-based (Ubuntu) ####

Installing the .deb is entirely automatic.

### Windows ###

The binary archive `dbmaster-0.8.zip` provides everything necessary to run DbMaster. Just extract it and run `dbmaster.exe`.


First launch
------------

The first time you run DbMaster, a wizard will pop up in order to set defaults parameters.

It will be asked which driver you will use for new connections. For example, a web developper might often use MySQL. This parameter can be easily modified. Besides, the other drivers remain available.

### Upgrade ###

Si you used to run DbMaster 0.7, your parameters will be imported automatically.


### Add a connection ###

Le premier bouton de la barre d'outil (accessible aussi dans Fichier→Nouvelle→Connexion) lance l'assistant d'ajout de connexion.
Une fois le SGBD choisi, vous pouvez utiliser le driver natif ou passer par ODBC en cochant la case adéquate.
L'adresse n'est pas obligatoire à renseigner pour certains SGBD (SQLite notamment).
Sur la page suivante, vous pouvez tester les informations saisies.

Lors de la validation de l'ajout, la connexion est disponible dans le volet de gauche.

Une fois la connexion enregistrée, vous pouvez vous connecter en double-cliquant dessus ou par un clic-droit→Connecter.


### Première requête ###

Pour lancer l'éditeur SQL, cliquez sur Fichier→Nouvelle→Requête. Vous pouvez saisir votre requête dans le nouvel onglet, par exemple `SELECT CURRENT_DATE`, puis l'exécuter.

S'il n'y a pas d'erreur, le résultat s'affichera en bas de l'éditeur.
