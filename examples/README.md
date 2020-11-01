# MLOD PROJECT

![alt text](https://github.com/PierreBourdeau/TP7/blob/master/raylib_180x180.png)RayLib Multiplayer snake - Pierre Bourdeau

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
* Barre [ESPACE] pour lancer la partie avec les options séléctionnées
* [ESCAPE] pour fermer l'application
* [O] pour ouvrir le menu d'option

#### Options :

Le menu d'option permets de choisir les parametres de jeu :
Pour naviguer dans le menu, utilisez : [▲] et [▼]
[ENTREE] pour enregistrer les paramètres actuels et revenir au menu
* Séléctionner la difficulté : [1] - [2] - [3]  (ou "&", "é", """)
* Une ou trois vie : [Y] : 3 vies / [N] : 1 vie
* Traverser les bords de carte : [Y] : Oui / [N] : Non
* Seul ou multijoueurs : [Y] : Multijoueurs / [N] : Seul

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

#### Controles :
Le joueur 1 incarne le serpent bleu :
* [▲] : Haut
* [▼] : Bas
* [◀] : Gauche
* [▶] : Droite

Le joueur 2 incarne le serpent Orange :
* [R] : Haut
* [F] : Bas
* [D] : Gauche
* [G] : Droite

Une partie peut être mise en pause en appuyant sur [P]

Le menu de pause permet également de revenir au menu principal : [E]
