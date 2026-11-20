# Architecture

Le projet sépare strictement la **logique de jeu** du **rendu** : la logique ne dépend pas d'OpenGL,
ce qui permet de la tester sans fenêtre ni GPU.

```
        main.cpp  (callbacks GLUT, boucle)
            |
        app/Partie  (machine à états : Titre > Sélection > Combat > Pause > FinMatch)
         /        \
  rendu/            jeu/   <-- noyau/
  (OpenGL)          (logique pure)
```

Bibliothèques CMake : `arene_logique` (noyau + jeu), `arene_rendu` (rendu + personnages), exécutable.

## Modules

| Dossier | Rôle |
|---|---|
| `noyau/` | `Maths` (Vec2, Couleur), `Config` (constantes d'équilibrage), `Entrees` (touches tenues / front montant, liaisons par joueur) |
| `jeu/` | `Fiche` (statistiques par personnage), `Combattant` (état, déplacement, énergie), `Combat` (manche : phases, projectiles, dégâts, fin), `IA` |
| `rendu/` | `Primitives` (sphère, cône/cylindre, pavé), `Textures`, `Lumieres`, `Camera`, `Hud`, `Particules`, `Scene` (arène, projectiles), `Ecrans` |
| `personnages/` | `Modele` (interface), un fichier par créature, `Registre` |
| `app/` | `Partie` : enchaînement des écrans, manches, caméra de combat, évènements |

## Flux d'une image

1. `Entrees` reçoit les touches GLUT ; `Partie::mettreAJour(dt)` lit les commandes.
2. L'IA ou le joueur produit une `Commande` (direction monde, attaque, spécial, garde).
3. `Combat::mettreAJour` applique les règles et émet des `Evenement` (impact, parade, tir, K.O.).
4. `Partie` convertit ces évènements en effets (particules, tremblement, ralenti).
5. `afficher()` : caméra, lumières, scène 3D, puis HUD 2D en projection orthographique.

## Modèles 3D

Un `Modele` dessine une pose (`Pose` : marche, coup, spécial, garde, recul, mort, victoire).
Convention : le modèle regarde vers **-x**, ses pieds sont à **y = 0**, hauteur d'environ 3 unités.
La position et l'orientation dans l'arène sont appliquées par `Scene::personnage`.

## Règles de combat

- Un coup touche à une fraction de l'animation (`Fiche::impactAttaque`) si l'adversaire est à portée.
- La garde réduit les dégâts à 25 % et annule les effets.
- Un coup reçu interrompt l'action, applique un recul et une courte invulnérabilité.
- L'énergie se recharge avec le temps et augmente en touchant.
- Une manche se termine par un K.O. ou à la fin du chronomètre (plus de vie restante gagne).

## Ajouter un personnage

1. Ajouter une valeur à l'enum `Perso` et une entrée dans le tableau de `jeu/Fiches.cpp`.
2. Créer `personnages/MonPerso.cpp` implémentant `Modele::dessiner(const Pose&)`.
3. Déclarer la classe dans `personnages/Modeles.hpp`, l'ajouter dans `Registre.cpp` et `CMakeLists.txt`.

Aucun autre fichier n'est à modifier : sélection, IA, HUD et combat utilisent la fiche.

## Outil de capture

`arene-des-titans --capture <dossier>` joue un scénario scripté et enregistre des images PPM
(utile pour générer les captures du README en intégration continue avec Xvfb).
