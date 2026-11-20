#pragma once
#include <vector>

#include "Combattant.hpp"

namespace arene {

struct Projectile {
    Vec2 pos, dir;
    double vitesse, rayon, degats, hauteur;
    int source;
    Effet effet;
    Couleur couleur;
    double age = 0;
    bool actif = true;
};

enum class Phase { Intro, Action, Fin };
enum class TypeEvt { Debut, Impact, Pare, Tir, KO };

struct Evenement {
    TypeEvt type;
    Vec2 pos;
    double y;
    Couleur couleur;
    int joueur;
};

class Combat {
public:
    Combat(int persoJ0, int persoJ1);

    void mettreAJour(double dt, const Commande commandes[2]);

    Combattant& combattant(int i) { return f_[i]; }
    const Combattant& combattant(int i) const { return f_[i]; }
    const std::vector<Projectile>& projectiles() const { return projectiles_; }
    const std::vector<Evenement>& evenements() const { return evenements_; }

    Phase phase() const { return phase_; }
    double tempsPhase() const { return tempsPhase_; }
    double chrono() const { return chrono_; }
    int vainqueur() const { return vainqueur_; }  // -1 : aucun / egalite
    bool termine() const { return termine_; }

private:
    bool infliger(int cible, int source, double degats, double recul, Effet effet, Vec2 origine, Couleur c);
    void resoudreCoup(int i);
    void lancerProjectile(int i);
    void majProjectiles(double dt);
    void contraindre();
    void verifierFin();
    void finir(int gagnant);

    Combattant f_[2];
    std::vector<Projectile> projectiles_;
    std::vector<Evenement> evenements_;
    Phase phase_ = Phase::Intro;
    double tempsPhase_ = 0;
    double chrono_;
    int vainqueur_ = -1;
    bool termine_ = false;
};

}  // namespace arene
