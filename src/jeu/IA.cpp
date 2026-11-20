#include "IA.hpp"

namespace arene {

IA::IA(Difficulte d, unsigned graine) : moteur_(graine) { reinitialiser(d); }

void IA::reinitialiser(Difficulte d) {
    switch (d) {
        case Difficulte::Facile: reaction_ = 0.55; probParade_ = 0.10; break;
        case Difficulte::Normal: reaction_ = 0.35; probParade_ = 0.35; break;
        case Difficulte::Difficile: reaction_ = 0.18; probParade_ = 0.65; break;
    }
    minuteur_ = 0;
    intention_ = Intention::Avancer;
}

Commande IA::decider(const Combat& combat, int moi, double dt) {
    const Combattant& a = combat.combattant(moi);
    const Combattant& o = combat.combattant(1 - moi);
    Commande cmd;

    Vec2 vers = o.pos - a.pos;
    double d = vers.longueur();
    Vec2 dir = d > 1e-6 ? vers * (1.0 / d) : Vec2{1, 0};
    Vec2 lat{-dir.z, dir.x};
    double portee = a.fiche().portee;

    minuteur_ -= dt;
    if (minuteur_ <= 0) {
        minuteur_ = reaction_ * (0.7 + 0.6 * alea());
        double r = alea();
        if (o.etat == Etat::Attaque && d < o.fiche().portee + 0.8 && alea() < probParade_) {
            intention_ = Intention::Parer;
            minuteur_ = 0.45;
        } else if (d > portee * 0.9) {
            bool tir = a.energie >= a.fiche().coutSpecial && d > 3.5 && alea() < 0.4;
            intention_ = tir ? Intention::Tirer : Intention::Avancer;
        } else if (r < 0.6) {
            intention_ = Intention::Attaquer;
        } else if (r < 0.8) {
            intention_ = Intention::Contourner;
            cote_ = alea() < 0.5 ? -1 : 1;
        } else {
            intention_ = Intention::Reculer;
        }
    }

    switch (intention_) {
        case Intention::Avancer: cmd.direction = dir + lat * (0.25 * cote_); break;
        case Intention::Attaquer:
            if (d > portee * 0.95) cmd.direction = dir;
            else cmd.attaque = true;
            break;
        case Intention::Reculer: cmd.direction = dir * -1.0; break;
        case Intention::Contourner: cmd.direction = lat * cote_; break;
        case Intention::Parer: cmd.bloc = true; break;
        case Intention::Tirer: cmd.special = true; break;
    }
    return cmd;
}

}  // namespace arene
