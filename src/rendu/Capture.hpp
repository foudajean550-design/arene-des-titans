#pragma once

namespace arene::rendu {

// Enregistre le tampon d'image courant au format PPM (outil de developpement).
bool sauverPPM(const char* chemin, int largeur, int hauteur);

}  // namespace arene::rendu
