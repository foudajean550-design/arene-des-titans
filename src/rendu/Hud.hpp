#pragma once
#include <string>

#include "../noyau/Maths.hpp"

namespace arene::rendu::hud {

void* policeNormale();
void* policeGrande();

void debut(int largeur, int hauteur);  // repere pixels, origine en haut a gauche
void fin();

void rect(double x, double y, double w, double h, Couleur c, float a = 1.0f);
void cadre(double x, double y, double w, double h, Couleur c, float epaisseur = 2.0f, float a = 1.0f);
void texte(double x, double y, const std::string& s, Couleur c, void* police = nullptr, float a = 1.0f);
void texteCentre(double cx, double y, const std::string& s, Couleur c, void* police = nullptr, float a = 1.0f);
void titre(double cx, double y, const std::string& s, double hauteurPx, Couleur c, float a = 1.0f);
void barre(double x, double y, double w, double h, double ratio, double fantome, Couleur c, bool inverse);

}  // namespace arene::rendu::hud
