#include "Combat.hpp"

#include <algorithm>

#include "../noyau/Config.hpp"

namespace arene {

namespace {
double yawVers(Vec2 de, Vec2 vers, double defaut) {
    Vec2 d = vers - de;
    if (d.longueur() < 1e-6) return defaut;
    return degres(std::atan2(d.z, -d.x));
}
}  // namespace

Combat::Combat(int p0, int p1)
    : f_{Combattant(p0, {-3.5, 0}, 0), Combattant(p1, {3.5, 0}, 180)}, chrono_(config::DUREE_ROUND) {}

void Combat::mettreAJour(double dt, const Commande commandes[2]) {
    evenements_.clear();
    tempsPhase_ += dt;
    Commande vide;

    if (phase_ == Phase::Intro && tempsPhase_ >= config::DUREE_COMPTE) {
        phase_ = Phase::Action;
        tempsPhase_ = 0;
        evenements_.push_back({TypeEvt::Debut, {}, 0, {1, 1, 1}, -1});
    }
    if (phase_ == Phase::Action) chrono_ = std::max(0.0, chrono_ - dt);

    for (int i = 0; i < 2; ++i) {
        f_[i].mettreAJour(phase_ == Phase::Action ? commandes[i] : vide, dt);
        if (f_[i].vivant()) f_[i].yaw = yawVers(f_[i].pos, f_[1 - i].pos, f_[i].yaw);
    }

    for (int i = 0; i < 2; ++i) {
        Combattant& a = f_[i];
        if (a.etat == Etat::Attaque && !a.impactFait &&
            a.tEtat >= a.fiche().impactAttaque * a.fiche().dureeAttaque) {
            a.impactFait = true;
            resoudreCoup(i);
        }
        if (a.etat == Etat::Special && !a.tirFait && a.tEtat >= config::IMPACT_SPECIAL * config::DUREE_SPECIAL) {
            a.tirFait = true;
            lancerProjectile(i);
        }
    }

    majProjectiles(dt);
    contraindre();
    if (phase_ == Phase::Action) verifierFin();

    if (phase_ == Phase::Fin && tempsPhase_ >= config::DUREE_FIN_ROUND) termine_ = true;
}

bool Combat::infliger(int cible, int source, double degats, double recul, Effet effet, Vec2 origine, Couleur c) {
    Combattant& t = f_[cible];
    if (!t.vivant() || t.invuln > 0) return false;
    bool pare = t.bloque;
    if (pare) {
        degats *= config::COEF_PARADE;
        recul *= 0.3;
    }
    t.recevoirCoup(degats, origine, recul, effet, pare);
    f_[source].energie = std::min(config::ENERGIE_MAX, f_[source].energie + 10);
    evenements_.push_back({pare ? TypeEvt::Pare : TypeEvt::Impact, t.pos, 1.4, c, cible});
    if (!t.vivant()) evenements_.push_back({TypeEvt::KO, t.pos, 1.4, c, cible});
    return true;
}

void Combat::resoudreCoup(int i) {
    Combattant& a = f_[i];
    if (distance(a.pos, f_[1 - i].pos) > a.fiche().portee + 0.3) return;
    double d = a.fiche().degats;
    infliger(1 - i, i, d, 2.0 + d * 0.5, Effet::Aucun, a.pos, a.fiche().accent);
}

void Combat::lancerProjectile(int i) {
    const Combattant& a = f_[i];
    const Fiche& fi = a.fiche();
    Vec2 dir = (f_[1 - i].pos - a.pos).normalise();
    if (dir.longueur() < 0.5) dir = {i == 0 ? 1.0 : -1.0, 0};
    Projectile p;
    p.pos = a.pos + dir * 1.0;
    p.dir = dir;
    p.vitesse = fi.vitesseProjectile;
    p.rayon = fi.rayonProjectile;
    p.degats = fi.degatsSpecial;
    p.hauteur = fi.hauteurProjectile;
    p.source = i;
    p.effet = fi.effet;
    p.couleur = fi.couleurProjectile;
    projectiles_.push_back(p);
    evenements_.push_back({TypeEvt::Tir, p.pos, p.hauteur, p.couleur, i});
}

void Combat::majProjectiles(double dt) {
    for (Projectile& p : projectiles_) {
        p.age += dt;
        p.pos = p.pos + p.dir * (p.vitesse * dt);
        int cible = 1 - p.source;
        if (distance(p.pos, f_[cible].pos) < p.rayon + 0.8) {
            if (infliger(cible, p.source, p.degats, 6.0, p.effet, p.pos - p.dir, p.couleur)) p.actif = false;
        }
        if (p.pos.longueur() > config::RAYON_ARENE + 3.0 || p.age > 4.0) p.actif = false;
    }
    projectiles_.erase(std::remove_if(projectiles_.begin(), projectiles_.end(),
                                      [](const Projectile& p) { return !p.actif; }),
                       projectiles_.end());
}

void Combat::contraindre() {
    Vec2 d = f_[1].pos - f_[0].pos;
    double l = d.longueur();
    if (l < config::DISTANCE_MIN) {
        Vec2 n = l > 1e-6 ? d * (1.0 / l) : Vec2{1, 0};
        double pousse = (config::DISTANCE_MIN - l) * 0.5;
        f_[0].pos = f_[0].pos - n * pousse;
        f_[1].pos = f_[1].pos + n * pousse;
    }
    for (Combattant& c : f_) {
        double r = c.pos.longueur();
        if (r > config::RAYON_ARENE) c.pos = c.pos * (config::RAYON_ARENE / r);
    }
}

void Combat::finir(int gagnant) {
    phase_ = Phase::Fin;
    tempsPhase_ = 0;
    vainqueur_ = gagnant;
    if (gagnant >= 0) f_[gagnant].victorieux = true;
}

void Combat::verifierFin() {
    bool m0 = !f_[0].vivant(), m1 = !f_[1].vivant();
    if (m0 || m1) {
        finir(m0 && m1 ? -1 : (m0 ? 1 : 0));
    } else if (chrono_ <= 0) {
        double a = f_[0].fractionPv(), b = f_[1].fractionPv();
        finir(std::abs(a - b) < 1e-9 ? -1 : (a > b ? 0 : 1));
    }
}

}  // namespace arene
