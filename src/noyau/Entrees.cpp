#include "Entrees.hpp"

namespace arene {

namespace {
constexpr int idx(Action a) { return static_cast<int>(a); }
}

Entrees::Entrees() {
    liaisons_[0][idx(Action::Haut)] = {'z', 'w'};
    liaisons_[0][idx(Action::Bas)] = {'s'};
    liaisons_[0][idx(Action::Gauche)] = {'q', 'a'};
    liaisons_[0][idx(Action::Droite)] = {'d'};
    liaisons_[0][idx(Action::Attaque)] = {'f'};
    liaisons_[0][idx(Action::Special)] = {'g'};
    liaisons_[0][idx(Action::Bloc)] = {'h'};

    liaisons_[1][idx(Action::Haut)] = {TOUCHE_HAUT};
    liaisons_[1][idx(Action::Bas)] = {TOUCHE_BAS};
    liaisons_[1][idx(Action::Gauche)] = {TOUCHE_GAUCHE};
    liaisons_[1][idx(Action::Droite)] = {TOUCHE_DROITE};
    liaisons_[1][idx(Action::Attaque)] = {'k'};
    liaisons_[1][idx(Action::Special)] = {'l'};
    liaisons_[1][idx(Action::Bloc)] = {'o'};
}

int Entrees::normaliser(int t) {
    if (t >= 'A' && t <= 'Z') return t - 'A' + 'a';
    return t;
}

void Entrees::appuyer(int touche) {
    int t = normaliser(touche);
    if (t < 0 || t >= NB_TOUCHES) return;
    if (!tenue_[t]) front_[t] = true;
    tenue_[t] = true;
}

void Entrees::relacher(int touche) {
    int t = normaliser(touche);
    if (t < 0 || t >= NB_TOUCHES) return;
    tenue_[t] = false;
}

void Entrees::finImage() {
    for (bool& f : front_) f = false;
}

bool Entrees::etat(const std::vector<int>& l, const bool* tab) const {
    for (int t : l)
        if (tab[t]) return true;
    return false;
}

bool Entrees::tenue(int joueur, Action a) const {
    if (etat(liaisons_[joueur][idx(a)], tenue_)) return true;
    return joueur == 0 && partage_ && etat(liaisons_[1][idx(a)], tenue_);
}

bool Entrees::pressee(int joueur, Action a) const {
    if (etat(liaisons_[joueur][idx(a)], front_)) return true;
    return joueur == 0 && partage_ && etat(liaisons_[1][idx(a)], front_);
}

Vec2 Entrees::direction(int joueur) const {
    double x = (tenue(joueur, Action::Droite) ? 1 : 0) - (tenue(joueur, Action::Gauche) ? 1 : 0);
    double z = (tenue(joueur, Action::Haut) ? 1 : 0) - (tenue(joueur, Action::Bas) ? 1 : 0);
    return {x, z};
}

bool Entrees::nav(Nav n) const {
    switch (n) {
        case Nav::Gauche: return front_[TOUCHE_GAUCHE] || front_['q'] || front_['a'];
        case Nav::Droite: return front_[TOUCHE_DROITE] || front_['d'];
        case Nav::Haut: return front_[TOUCHE_HAUT] || front_['z'] || front_['w'];
        case Nav::Bas: return front_[TOUCHE_BAS] || front_['s'];
        case Nav::Valider: return front_[13] || front_[' '];
        case Nav::Retour: return front_[27];
    }
    return false;
}

bool Entrees::touchePressee(int touche) const {
    int t = normaliser(touche);
    return t >= 0 && t < NB_TOUCHES && front_[t];
}

}  // namespace arene
