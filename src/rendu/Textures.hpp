#pragma once

namespace arene::rendu {

enum class Matiere { Aucune, Ecailles, Peau, Metal, Chitine, Pierre, Dragon, Membrane, Sol, NB };

// Charge assets/textures/<nom>.ppm s'il existe, sinon genere une texture procedurale.
void chargerTextures();
void utiliserTexture(Matiere m);

}  // namespace arene::rendu
