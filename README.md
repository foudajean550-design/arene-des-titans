# Arène des Titans

Jeu de combat 3D en arène, écrit en C++17 avec OpenGL et freeglut. Les cinq combattants sont modélisés à la main.

![Combat](docs/img/combat.png)

## Fonctionnalités
- 5 combattants jouables : Rex, R-7, Arachna, Pyrax et Granit.
- Combat solo contre une IA à trois niveaux ou deux joueurs sur le même clavier.
- Attaques, coups spéciaux, garde, projectiles, effets et caméra dynamique.
- Écran de sélection 3D, HUD, particules et textures procédurales.

## Compilation
Prérequis : compilateur C++17, CMake, OpenGL, GLU et freeglut.
Sur Debian ou Ubuntu : `sudo apt install build-essential cmake freeglut3-dev libglu1-mesa-dev`.

Configure et compile avec `cmake -S . -B build` puis `cmake --build build -j`.
Lance le jeu avec `./build/arene-des-titans`.

## Tests
`cd build && ctest --output-on-failure`

## Commandes
Joueur 1 : ZQSD pour se déplacer, F attaque, G spécial, H garde.
Joueur 2 : flèches, K attaque, L spécial, O garde.
C change l’angle de caméra ; P ou Échap met en pause.

## Textures personnalisées
Dépose des fichiers PPM P6 dans `assets/textures/` pour remplacer les textures générées.

## Licence
MIT, voir [LICENSE](LICENSE).
