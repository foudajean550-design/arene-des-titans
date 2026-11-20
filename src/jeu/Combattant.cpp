#include "Combattant.hpp"

#include <algorithm>

#include "../noyau/Config.hpp"

namespace arene {

double Combattant::config_invuln() { return config::DUREE_INVULNERABLE; }

Combattant::Combattant(int id_, Vec2 position, double yaw_)
    : id(id_), pos(position), yaw(yaw_), pv(arene::fiche(id_).pv), energie(30), fiche_(arene::fiche(id_)) {}

Pose Combattant::pose(double t) const {
    Pose p;
    p.t = t;
    p.marche = phaseMarche;
    p.allure = allure;
    if (etat == Etat::Attaque) p.coup = std::max(1e-4, borner(tEtat / fiche_.dureeAttaque, 0, 1));
    if (etat == Etat::Special) p.special = std::max(1e-4, borner(tEtat / config::DUREE_SPECIAL, 0, 1));
    if (etat == Etat::Touche) p.touche = 1.0 - borner(tEtat / config::DUREE_TOUCHE, 0, 1);
    p.bloc = bloc01;
    if (etat == Etat::Mort) p.mort = borner(tEtat / 0.9, 0, 1);
    p.victoire = victorieux ? 1.0 : 0.0;
    return p;
}

void Combattant::mourir() {
    etat = Etat::Mort;
    tEtat = 0;
    pv = 0;
    brulure = ralenti = etourdi = 0;
    bloque = false;
}

void Combattant::recevoirCoup(double degats, Vec2 origine, double recul, Effet effet, bool pare) {
    pv -= degats;
    if (pare) {
        energie = std::max(0.0, energie - 4);
        impulsion = (pos - origine).normalise() * recul;
        if (pv <= 0) mourir();
        return;
    }
    energie = std::min(config::ENERGIE_MAX, energie + 4);
    if (pv <= 0) {
        mourir();
        return;
    }
    etat = Etat::Touche;
    tEtat = 0;
    invuln = config::DUREE_INVULNERABLE;
    impulsion = (pos - origine).normalise() * recul;
    switch (effet) {
        case Effet::Brulure: brulure = config::DUREE_BRULURE; break;
        case Effet::Ralentissement: ralenti = config::DUREE_RALENTI; break;
        case Effet::Etourdissement: etourdi = config::DUREE_ETOURDI; break;
        case Effet::Aucun: break;
    }
}

void Combattant::mettreAJour(const Commande& c, double dt) {
    tEtat += dt;
    recharge = std::max(0.0, recharge - dt);
    invuln = std::max(0.0, invuln - dt);
    ralenti = std::max(0.0, ralenti - dt);
    etourdi = std::max(0.0, etourdi - dt);

    pos = pos + impulsion * dt;
    impulsion = impulsion * std::exp(-8.0 * dt);

    if (etat == Etat::Mort) {
        bloc01 = lisser(bloc01, 0, 10, dt);
        allure = lisser(allure, 0, 10, dt);
        return;
    }

    if (brulure > 0) {
        brulure = std::max(0.0, brulure - dt);
        pv -= config::DEGATS_BRULURE * dt;
        if (pv <= 0) {
            mourir();
            return;
        }
    }

    if (etat == Etat::Attaque && tEtat >= fiche_.dureeAttaque) etat = Etat::Repos;
    if (etat == Etat::Special && tEtat >= config::DUREE_SPECIAL) etat = Etat::Repos;
    if (etat == Etat::Touche && tEtat >= config::DUREE_TOUCHE) etat = Etat::Repos;

    bool libre = (etat == Etat::Repos) && etourdi <= 0;
    bloque = libre && c.bloc;
    double deplace = 0;

    if (libre && !bloque) {
        if (c.attaque && recharge <= 0) {
            etat = Etat::Attaque;
            tEtat = 0;
            impactFait = false;
            recharge = fiche_.recharge + fiche_.dureeAttaque;
        } else if (c.special && energie >= fiche_.coutSpecial) {
            etat = Etat::Special;
            tEtat = 0;
            tirFait = false;
            energie -= fiche_.coutSpecial;
        } else {
            double force = std::min(1.0, c.direction.longueur());
            if (force > 0.01) {
                double v = fiche_.vitesse * (ralenti > 0 ? 0.5 : 1.0);
                deplace = v * force;
                pos = pos + c.direction.normalise() * (deplace * dt);
            }
        }
    }

    double cibleAllure = borner(deplace / fiche_.vitesse, 0, 1);
    allure = lisser(allure, cibleAllure, 12, dt);
    phaseMarche += allure * dt * 9.0;
    bloc01 = lisser(bloc01, bloque ? 1.0 : 0.0, 14, dt);

    if (!bloque) energie = std::min(config::ENERGIE_MAX, energie + config::REGEN_ENERGIE * dt);
}

}  // namespace arene
