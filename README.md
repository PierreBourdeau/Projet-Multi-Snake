# MLOD PROJECT

![alt text](https://github.com/PierreBourdeau/TP7/blob/master/raylib_180x180.png)  __RayLib Multiplayer snake - Pierre Bourdeau__

  Original Sample game developed by Ian Eito, Albert Martos and Ramon Santamaria

  Modified by : Pierre Bourdeau

  This game has been created using raylib v1.3 (www.raylib.com)
  raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)

  Copyright (c) 2015 Ramon Santamaria (@raysan5)

___

## RayLib Multiplayer snake
======
project extented by : Pierre Bourdeau 

### Présentation globale :

Le projet est issue du jeu snake disponible sur le site officiel de [RayLib](https://www.raylib.com/games.html).
De nombreuses extensions ont été ajoutées :
* Generation d'obstacle sur le terrain
* Capacité de traverser les murs d'un bout à l'autre
* 3 niveaux de difficultés qui seront détaillés ci-après
* Des interfaces de menus et d'options pour configurer les parties
* Un mode multijoueurs (2 joueurs)
* Un système de scoring : avec les résumés des scores de la dernière partie jouée, le hiscore...
* Un système de vies
* Affichage des points, vies, scores

Les 3 niveaux de difficulté sont établis tels que :
* Le niveau 1 correspond au Snake classique, pas d'obstacle sur le terrain (1 ou 2 joueurs).
* Le niveau 2 permet d'introduire la génération aléatoire d'obstacles sur le terrain (1 ou 2 joueurs).
* Le niveau 3 permet d'introduire la génération aléatoire d'obstacles sur le terrain et le changement de disposition de ceux-ci et une accélération `(+10 fps)` tous les 10 fruits mangés par un des joueurs. La vitesse est limitée à 100 fps

Dans tous les modes de jeu, il est possible via le menu d'option de pramétrer :
* 1 ou 3 vies
* La possibilité de traverser les bords du terrain
* Jouer seul ou à deux
___

### Fonctionnement du jeu : 
#### Menu :

Le jeu s'ouvre sur la page de menu ou est affiché :
* Le dernier score des joueurs
* Le record de points
* Barre __[ESPACE]__ pour lancer la partie avec les options séléctionnées
* __[ESCAPE]__ pour fermer l'application
* __[O]__ pour ouvrir le menu d'option

#### Options :

Le menu d'option permets de choisir les parametres de jeu :
Pour naviguer dans le menu, utilisez : __[▲]__ et __[▼]__

__[ENTREE]__ pour enregistrer les paramètres actuels et revenir au menu principal
* Séléctionner la difficulté : __[1]__ - __[2]__ - __[3]__  (ou "&", "é", """)
* Une ou trois vie : __[Y]__ : 3 vies / __[N]__ : 1 vie
* Traverser les bords de carte : __[Y]__ : Oui / __[N]__ : Non
* Seul ou multijoueurs : __[Y]__ : Multijoueurs / __[N]__ : Seul

#### Jeu :

Durant la partie sont affichés les informations suivantes :
* Points actuel de chaque joueur
* Vies de chaque joueur
* La vitesse actuelle du jeu (FPS), par défaut 60
* Le hiscore

Le gagnant de la partie est le dernier joueur présent sur le terrain. 
Pour marquer des points, il faut manger les fruits (points bleus) qui apparaissent aléatoirement sur le terrain. Ceci a pour effet de faire grandir le serpent.
Un joueur meurt ou pert une vie si :
* Il avance contre un mur
* Il depasse les limites du terrains lorsque la règle permettant de les traverser est désactivée
* Il avance contre son propre serpent
* Il avance contre le serpent adverse : dans ce cas seul lui est affecté

Si deux joueurs se rencontre avec la tête du serpent, les deux joueurs perdent.

Lorsque l'un des joueurs ou les deux joueurs n'ont plus de vie, un menu Game Over apparaît. Il permet de revenir au menu principal via : __[E]__ ou de rejouer la partie avec les paramètres actuels via : __[ENTREE]__

#### Controles :
Le joueur 1 incarne le serpent bleu :
* __[▲]__ : Haut
* __[▼]__ : Bas
* __[◄]__ : Gauche
* __[►]__ : Droite

Le joueur 2 incarne le serpent Orange :
* __[R]__ : Haut
* __[F]__ : Bas
* __[D]__ : Gauche
* __[G]__ : Droite

Une partie peut être mise en pause en appuyant sur __[P]__

Le menu de pause permet également de revenir au menu principal : __[E]__

### Fichier et code :

Le code source est contenu dans les fichiers : 
* `main_snake_solo.c`: Il a été la V1 qui a permit la mise en place des fonctionnalités. Jouable seul uniquement.
* `main_snake_multi.c` : Il est le projet final. Jouable seul comme à deux.

Le projet fonctionne avec la 'library RayLib'. Le code original, socle de ce projet se trouve : [ici](https://www.raylib.com/games.html)

### Dynamique du projet :

Tous les aspects et fonctionnalitées ennoncés ci-dessus sont complets et fonctionnels. J'avais pour idée d'implémenter d'autre fonctionnalités telles que :
* Des fruit offrant aléatoirement des bonus ou malus (Vitesse (+ ou -), capacité de traverser les murs, vie supplémentaaire...
* Un mode multijoueur en ligne
* Un tableau de score (ladder) avec les noms des joueurs et les scores
* ...

Ces fonctionnalités seront peut-être ajoutée par la suite.

¯\_(ツ)_/¯
