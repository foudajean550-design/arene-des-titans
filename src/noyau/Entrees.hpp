#pragma once
#include <vector>

#include "Maths.hpp"

namespace arene {

enum Touche : int {
    TOUCHE_GAUCHE = 256,
    TOUCHE_DROITE,
    TOUCHE_HAUT,
    TOUCHE_BAS,
    TOUCHE_F2,
    TOUCHE_F3,
    TOUCHE_F4,
    NB_TOUCHES
};

enum class Action { Haut, Bas, Gauche, Droite, Attaque, Special, Bloc, NB };
enum class Nav { Gauche, Droite, Haut, Bas, Valider, Retour };

class Entrees {
public:
    Entrees();

    void appuyer(int touche);
    void relacher(int touche);
    void finImage();

    bool tenue(int joueur, Action a) const;
    bool pressee(int joueur, Action a) const;
    Vec2 direction(int joueur) const;  // x : droite - gauche, z : haut - bas

    bool nav(Nav n) const;
    bool touchePressee(int touche) const;

    void partagerClavier(bool actif) { partage_ = actif; }

private:
    static int normaliser(int touche);
    bool etat(const std::vector<int>& l, const bool* tab) const;

    bool tenue_[NB_TOUCHES] = {};
    bool front_[NB_TOUCHES] = {};
    std::vector<int> liaisons_[2][static_cast<int>(Action::NB)];
    bool partage_ = false;
};

}  // namespace arene
