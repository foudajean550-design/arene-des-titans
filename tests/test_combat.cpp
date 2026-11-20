#include <cstdio>

#include "../src/jeu/Combat.hpp"
#include "../src/jeu/IA.hpp"
#include "../src/noyau/Config.hpp"
#include "../src/noyau/Entrees.hpp"

using namespace arene;

namespace {
int echecs = 0;
int total = 0;

#define VERIF(cond)                                                          \
    do {                                                                     \
        ++total;                                                             \
        if (!(cond)) {                                                       \
            ++echecs;                                                        \
            std::printf("ECHEC %s:%d  %s\n", __FILE__, __LINE__, #cond);     \
        }                                                                    \
    } while (0)

bool proche(double a, double b, double eps = 1e-6) { return std::abs(a - b) < eps; }

void avancer(Combat& c, double secondes, const Commande cmd[2]) {
    const double dt = 1.0 / 60.0;
    for (double t = 0; t < secondes; t += dt) c.mettreAJour(dt, cmd);
}

Combat enAction(int a, int b) {
    Combat c(a, b);
    Commande rien[2];
    avancer(c, config::DUREE_COMPTE + 0.1, rien);
    return c;
}

void testIntro() {
    Combat c(DINOSAURE, ROBOT);
    Commande cmd[2];
    cmd[0].attaque = true;
    avancer(c, 1.0, cmd);
    VERIF(c.phase() == Phase::Intro);
    VERIF(proche(c.combattant(1).pv, c.combattant(1).fiche().pv));
    avancer(c, 2.2, cmd);
    VERIF(c.phase() == Phase::Action);
}

void testAttaque() {
    Combat c = enAction(DINOSAURE, ROBOT);
    c.combattant(0).pos = {-1, 0};
    c.combattant(1).pos = {1, 0};
    Commande cmd[2];
    cmd[0].attaque = true;
    avancer(c, 0.6, cmd);
    VERIF(proche(c.combattant(1).pv, 110 - 12));
}

void testHorsPortee() {
    Combat c = enAction(DINOSAURE, ROBOT);
    c.combattant(0).pos = {-5, 0};
    c.combattant(1).pos = {5, 0};
    Commande cmd[2];
    cmd[0].attaque = true;
    avancer(c, 0.05, cmd);
    cmd[0].attaque = false;
    avancer(c, 0.6, cmd);
    VERIF(proche(c.combattant(1).pv, 110));
}

void testParade() {
    Combat c = enAction(DINOSAURE, ROBOT);
    c.combattant(0).pos = {-1, 0};
    c.combattant(1).pos = {1, 0};
    Commande cmd[2];
    cmd[0].attaque = true;
    cmd[1].bloc = true;
    avancer(c, 0.6, cmd);
    VERIF(proche(c.combattant(1).pv, 110 - 12 * config::COEF_PARADE));
}

void testSpecial() {
    Combat c = enAction(DINOSAURE, ROBOT);
    c.combattant(0).pos = {-3, 0};
    c.combattant(1).pos = {3, 0};
    c.combattant(0).energie = 100;
    Commande cmd[2];
    cmd[0].special = true;
    avancer(c, 0.05, cmd);
    cmd[0].special = false;
    VERIF(c.combattant(0).energie < 100 - 39);
    avancer(c, 1.5, cmd);
    VERIF(c.combattant(1).pv < 110);
}

void testSpecialSansEnergie() {
    Combat c = enAction(DINOSAURE, ROBOT);
    c.combattant(0).energie = 0;
    Commande cmd[2];
    cmd[0].special = true;
    avancer(c, 0.1, cmd);
    VERIF(c.combattant(0).etat == Etat::Repos);
}

void testBrulure() {
    Combat c = enAction(DRAGON, ROBOT);
    c.combattant(1).recevoirCoup(1, {0, 0}, 0, Effet::Brulure, false);
    double avant = c.combattant(1).pv;
    Commande cmd[2];
    avancer(c, 1.0, cmd);
    VERIF(avant - c.combattant(1).pv > 2.5);
}

void testKO() {
    Combat c = enAction(GOLEM, ARAIGNEE);
    c.combattant(0).pos = {-1, 0};
    c.combattant(1).pos = {1, 0};
    c.combattant(1).pv = 5;
    Commande cmd[2];
    cmd[0].attaque = true;
    avancer(c, 1.0, cmd);
    VERIF(!c.combattant(1).vivant());
    VERIF(c.phase() == Phase::Fin);
    VERIF(c.vainqueur() == 0);
    avancer(c, config::DUREE_FIN_ROUND + 0.1, cmd);
    VERIF(c.termine());
}

void testTempsEcoule() {
    Combat c = enAction(DINOSAURE, DINOSAURE);
    c.combattant(1).pv = 40;
    Commande cmd[2];
    avancer(c, config::DUREE_ROUND + 0.5, cmd);
    VERIF(c.phase() == Phase::Fin);
    VERIF(c.vainqueur() == 0);
}

void testArene() {
    Combat c = enAction(DINOSAURE, ROBOT);
    Commande cmd[2];
    cmd[0].direction = {-1, 0};
    avancer(c, 6.0, cmd);
    VERIF(c.combattant(0).pos.longueur() <= config::RAYON_ARENE + 1e-6);
}

void testDistanceMin() {
    Combat c = enAction(DINOSAURE, ROBOT);
    c.combattant(0).pos = {0, 0};
    c.combattant(1).pos = {0.2, 0};
    Commande cmd[2];
    avancer(c, 0.05, cmd);
    VERIF(distance(c.combattant(0).pos, c.combattant(1).pos) >= config::DISTANCE_MIN - 1e-6);
}

void testIA() {
    Combat c = enAction(DINOSAURE, DRAGON);
    IA ia(Difficulte::Difficile, 7);
    Commande cmd[2];
    for (int i = 0; i < 60 * 20 && c.phase() == Phase::Action; ++i) {
        cmd[0] = ia.decider(c, 0, 1.0 / 60);
        cmd[1] = Commande{};
        c.mettreAJour(1.0 / 60, cmd);
    }
    VERIF(c.combattant(1).pv < c.combattant(1).fiche().pv);
}

void testEntrees() {
    Entrees e;
    e.appuyer('Z');
    VERIF(e.tenue(0, Action::Haut));
    VERIF(e.pressee(0, Action::Haut));
    e.finImage();
    VERIF(!e.pressee(0, Action::Haut));
    e.relacher('z');
    VERIF(!e.tenue(0, Action::Haut));
    e.appuyer(TOUCHE_GAUCHE);
    VERIF(e.tenue(1, Action::Gauche));
    VERIF(!e.tenue(0, Action::Gauche));
    e.partagerClavier(true);
    VERIF(e.tenue(0, Action::Gauche));
}

void testFiches() {
    for (int i = 0; i < NB_PERSONNAGES; ++i) {
        const Fiche& f = fiche(i);
        VERIF(f.pv > 0 && f.vitesse > 0 && f.degats > 0 && f.portee > 0);
        VERIF(f.coutSpecial <= config::ENERGIE_MAX);
    }
}

}  // namespace

int main() {
    testIntro();
    testAttaque();
    testHorsPortee();
    testParade();
    testSpecial();
    testSpecialSansEnergie();
    testBrulure();
    testKO();
    testTempsEcoule();
    testArene();
    testDistanceMin();
    testIA();
    testEntrees();
    testFiches();
    std::printf("%d verifications, %d echec(s)\n", total, echecs);
    return echecs == 0 ? 0 : 1;
}
