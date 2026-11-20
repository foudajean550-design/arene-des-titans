#include "Fiche.hpp"

namespace arene {

namespace {

const Fiche FICHES[NB_PERSONNAGES] = {
    {"Rex", "Le Dinosaure", "Souffle de feu",
     "Equilibre : morsure puissante et souffle enflamme.",
     100, 4.2, 12, 2.4, 0.50, 0.40, 0.50,
     18, 40, 11, 0.35, 1.5, Effet::Aucun,
     {0.35f, 0.80f, 0.30f}, {1.0f, 0.5f, 0.1f}},

    {"R-7", "Unite de combat", "Rayon laser",
     "Tireur precis : laser tres rapide, peu couteux.",
     110, 3.8, 10, 2.3, 0.45, 0.40, 0.40,
     15, 30, 20, 0.22, 1.9, Effet::Aucun,
     {0.30f, 0.70f, 1.00f}, {0.3f, 0.9f, 1.0f}},

    {"Arachna", "La Veuve", "Toile collante",
     "Rapide et fragile : sa toile ralentit l'adversaire.",
     80, 5.6, 8, 2.1, 0.35, 0.40, 0.30,
     6, 35, 9, 0.40, 1.0, Effet::Ralentissement,
     {0.70f, 0.30f, 0.90f}, {0.6f, 1.0f, 0.4f}},

    {"Pyrax", "Le Dragon", "Brasier",
     "Grande allonge : son feu brule pendant plusieurs secondes.",
     90, 4.0, 11, 3.0, 0.55, 0.45, 0.60,
     12, 50, 10, 0.50, 2.2, Effet::Brulure,
     {1.00f, 0.35f, 0.20f}, {1.0f, 0.25f, 0.05f}},

    {"Granit", "Le Golem", "Jet de rocher",
     "Lent mais colossal : ses coups etourdissent.",
     150, 2.8, 20, 2.8, 0.80, 0.58, 0.90,
     22, 60, 7, 0.55, 2.0, Effet::Etourdissement,
     {0.90f, 0.60f, 0.20f}, {0.55f, 0.45f, 0.35f}},
};

}  // namespace

const Fiche& fiche(int id) {
    if (id < 0 || id >= NB_PERSONNAGES) id = 0;
    return FICHES[id];
}

}  // namespace arene
