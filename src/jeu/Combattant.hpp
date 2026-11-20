#pragma once
#include "Fiche.hpp"
#include "Pose.hpp"

namespace arene {

struct Commande {
    Vec2 direction;  // repere monde
    bool attaque = false;
    bool special = false;
    bool bloc = false;
};

enum class Etat { Repos, Attaque, Special, Touche, Mort };

class Combattant {
public:
    Combattant(int id, Vec2 position, double yaw);

    const Fiche& fiche() const { return fiche_; }
    bool vivant() const { return etat != Etat::Mort; }
    double fractionPv() const { return pv / fiche_.pv; }
    double flash() const { return invuln > 0 ? invuln / config_invuln() : 0.0; }
    Pose pose(double t) const;

    void mettreAJour(const Commande& c, double dt);
    void recevoirCoup(double degats, Vec2 origine, double recul, Effet effet, bool pare);
    void mourir();

    int id;
    Vec2 pos;
    double yaw;
    double pv, energie = 0;
    Etat etat = Etat::Repos;
    double tEtat = 0;
    bool bloque = false, impactFait = false, tirFait = false, victorieux = false;
    double recharge = 0, invuln = 0, brulure = 0, ralenti = 0, etourdi = 0;
    Vec2 impulsion;
    double phaseMarche = 0, allure = 0, bloc01 = 0;

private:
    static double config_invuln();
    const Fiche& fiche_;
};

}  // namespace arene
