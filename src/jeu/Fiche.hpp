#pragma once
#include "../noyau/Maths.hpp"

namespace arene {

enum class Effet { Aucun, Brulure, Ralentissement, Etourdissement };

enum Perso { DINOSAURE, ROBOT, ARAIGNEE, DRAGON, GOLEM, NB_PERSONNAGES };

struct Fiche {
    const char* nom;
    const char* titre;
    const char* special;
    const char* description;

    double pv, vitesse, degats, portee;
    double dureeAttaque, impactAttaque, recharge;

    double degatsSpecial, coutSpecial;
    double vitesseProjectile, rayonProjectile, hauteurProjectile;
    Effet effet;

    Couleur accent, couleurProjectile;
};

const Fiche& fiche(int id);

}  // namespace arene
