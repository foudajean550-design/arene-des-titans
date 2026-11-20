#include "Partie.hpp"

#include <algorithm>
#include <cmath>

#include "../noyau/Config.hpp"
#include "../rendu/Ecrans.hpp"
#include "../rendu/GL.hpp"
#include "../rendu/Lumieres.hpp"
#include "../rendu/Scene.hpp"
#include "../rendu/Textures.hpp"

namespace arene {

Partie::Partie() : largeur_(config::LARGEUR_FENETRE), hauteur_(config::HAUTEUR_FENETRE) {}

void Partie::initialiserRendu() {
    glClearColor(0.03f, 0.04f, 0.08f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    rendu::chargerTextures();
    rendu::initLumieres();
    aller(Ecran::Titre);
    camera_.saut();
}

void Partie::redimensionner(int l, int h) {
    largeur_ = std::max(1, l);
    hauteur_ = std::max(1, h);
    glViewport(0, 0, largeur_, hauteur_);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(50, static_cast<double>(largeur_) / hauteur_, 0.1, 120);
    glMatrixMode(GL_MODELVIEW);
}

void Partie::aller(Ecran e) {
    ecran_ = e;
    if (e == Ecran::Titre) {
        camera_.cibleV = 18;
        camera_.cibleDistance = 12;
        camera_.tx = 0;
        camera_.tz = 0;
        entrees_.partagerClavier(false);
    }
    if (e == Ecran::Selection) {
        camera_.cibleV = 12;
        camera_.cibleDistance = 10.5;
        camera_.ty = 2.0;
        camera_.tx = camera_.tz = 0;
    }
}

void Partie::demarrerDirect(int p0, int p1, bool solo) {
    solo_ = solo;
    choix_[0] = p0;
    choix_[1] = p1;
    entrees_.partagerClavier(solo_);
    lancerMatch();
    camera_.saut();
}

void Partie::choisirCurseur(int perso) { curseur_ = perso; }

void Partie::mettreAJour(double dtBrut) {
    double dt = std::min(dtBrut, config::DT_MAX);
    temps_ += dt;
    angleLumiere_ += 40 * dt;

    if (entrees_.touchePressee(TOUCHE_F2)) filaire_ = !filaire_;
    if (entrees_.touchePressee(TOUCHE_F3)) rendu::basculerSoleil();
    if (entrees_.touchePressee(TOUCHE_F4)) rendu::basculerSpot();

    switch (ecran_) {
        case Ecran::Titre: majTitre(); break;
        case Ecran::Selection: majSelection(dt); break;
        case Ecran::Combat: majCombat(dt); break;
        case Ecran::Pause:
            if (entrees_.touchePressee('p') || entrees_.nav(Nav::Valider)) ecran_ = Ecran::Combat;
            if (entrees_.nav(Nav::Retour)) aller(Ecran::Titre);
            break;
        case Ecran::FinMatch: majFinMatch(); break;
    }

    camera_.mettreAJour(dt);
    particules_.mettreAJour(dt);
    entrees_.finImage();
}

void Partie::majTitre() {
    camera_.cibleH = temps_ * 9;
    if (entrees_.nav(Nav::Haut)) menuIndex_ = (menuIndex_ + 3) % 4;
    if (entrees_.nav(Nav::Bas)) menuIndex_ = (menuIndex_ + 1) % 4;
    int d = static_cast<int>(difficulte_);
    if (menuIndex_ == 2) {
        if (entrees_.nav(Nav::Gauche)) d = std::max(0, d - 1);
        if (entrees_.nav(Nav::Droite)) d = std::min(2, d + 1);
        difficulte_ = static_cast<Difficulte>(d);
    }
    if (entrees_.nav(Nav::Retour)) quitter_ = true;
    if (entrees_.nav(Nav::Valider)) {
        switch (menuIndex_) {
            case 0:
            case 1:
                solo_ = menuIndex_ == 0;
                phaseSel_ = 0;
                curseur_ = 0;
                aller(Ecran::Selection);
                break;
            case 2: difficulte_ = static_cast<Difficulte>((d + 1) % 3); break;
            case 3: quitter_ = true; break;
        }
    }
}

void Partie::majSelection(double dt) {
    camera_.cibleH += 20 * dt;
    if (entrees_.nav(Nav::Gauche)) curseur_ = (curseur_ + NB_PERSONNAGES - 1) % NB_PERSONNAGES;
    if (entrees_.nav(Nav::Droite)) curseur_ = (curseur_ + 1) % NB_PERSONNAGES;
    if (entrees_.nav(Nav::Retour)) {
        if (phaseSel_ > 0) {
            --phaseSel_;
            curseur_ = choix_[phaseSel_];
        } else {
            aller(Ecran::Titre);
        }
        return;
    }
    if (entrees_.nav(Nav::Valider)) {
        choix_[phaseSel_] = curseur_;
        if (phaseSel_ == 0) {
            phaseSel_ = 1;
            curseur_ = (curseur_ + 1) % NB_PERSONNAGES;
        } else {
            entrees_.partagerClavier(solo_);
            lancerMatch();
        }
    }
}

void Partie::lancerMatch() {
    manches_[0] = manches_[1] = 0;
    numeroManche_ = 1;
    lancerRound();
}

void Partie::lancerRound() {
    combat_ = std::make_unique<Combat>(choix_[0], choix_[1]);
    ia_.reinitialiser(difficulte_);
    pvAff_[0] = pvAff_[1] = 1.0;
    echelleTemps_ = 1;
    dureeRalenti_ = 0;
    particules_.vider();
    ecran_ = Ecran::Combat;
    suivreCombat();
}

void Partie::finRound() {
    int v = combat_->vainqueur();
    if (v >= 0) manches_[v]++;
    if (v >= 0 && manches_[v] >= config::MANCHES_POUR_GAGNER) {
        vainqueurMatch_ = v;
        ecran_ = Ecran::FinMatch;
        return;
    }
    if (v >= 0) ++numeroManche_;
    lancerRound();
}

Commande Partie::lireCommande(int j) const {
    Vec2 d = entrees_.direction(j);
    Commande c;
    c.direction = camera_.droite() * d.x + camera_.avant() * d.z;
    c.attaque = entrees_.tenue(j, Action::Attaque);
    c.special = entrees_.tenue(j, Action::Special);
    c.bloc = entrees_.tenue(j, Action::Bloc);
    return c;
}

void Partie::majCombat(double dtBrut) {
    if (entrees_.touchePressee('p') || entrees_.nav(Nav::Retour)) {
        ecran_ = Ecran::Pause;
        return;
    }
    if (entrees_.touchePressee('c')) presetCamera_ = (presetCamera_ + 1) % 3;

    if (dureeRalenti_ > 0) {
        dureeRalenti_ -= dtBrut;
        echelleTemps_ = 0.35;
    } else {
        echelleTemps_ = 1;
    }
    double dt = dtBrut * echelleTemps_;

    Commande cmd[2];
    cmd[0] = lireCommande(0);
    cmd[1] = solo_ ? ia_.decider(*combat_, 1, dt) : lireCommande(1);
    combat_->mettreAJour(dt, cmd);

    traiterEvenements();
    for (int i = 0; i < 2; ++i) {
        double cible = combat_->combattant(i).fractionPv();
        pvAff_[i] = pvAff_[i] > cible ? std::max(cible, pvAff_[i] - 0.45 * dtBrut) : cible;
    }
    suivreCombat();
    if (combat_->termine()) finRound();
}

void Partie::traiterEvenements() {
    for (const Evenement& e : combat_->evenements()) {
        switch (e.type) {
            case TypeEvt::Impact:
                particules_.jaillir(e.pos, e.y, e.couleur, 28, 5.0);
                camera_.secouer(0.22);
                break;
            case TypeEvt::Pare:
                particules_.jaillir(e.pos, e.y, {1, 1, 0.8f}, 12, 3.0);
                camera_.secouer(0.07);
                break;
            case TypeEvt::Tir: particules_.jaillir(e.pos, e.y, e.couleur, 12, 2.5); break;
            case TypeEvt::KO:
                particules_.jaillir(e.pos, e.y, {1, 0.8f, 0.3f}, 90, 8.0);
                camera_.secouer(0.5);
                dureeRalenti_ = 1.4;
                break;
            case TypeEvt::Debut: break;
        }
    }
}

void Partie::suivreCombat() {
    const Combattant& a = combat_->combattant(0);
    const Combattant& b = combat_->combattant(1);
    Vec2 mid = (a.pos + b.pos) * 0.5;
    double ecart = distance(a.pos, b.pos);
    camera_.tx = mid.x;
    camera_.tz = mid.z;
    camera_.ty = 1.4;

    Vec2 axe = (b.pos - a.pos).normalise();
    double psi = degres(std::atan2(axe.z, axe.x));
    double c1 = -psi, c2 = 180 - psi;
    auto ecartAng = [&](double c) {
        double d = std::fmod(c - camera_.angleH, 360.0);
        if (d > 180) d -= 360;
        if (d < -180) d += 360;
        return std::fabs(d);
    };
    double base = ecartAng(c1) <= ecartAng(c2) ? c1 : c2;
    if (ecart < 0.5) base = camera_.angleH;

    static const double decalH[3] = {0, 0, 28};
    static const double hauteurV[3] = {14, 38, 10};
    camera_.cibleH = base + decalH[presetCamera_];
    camera_.cibleV = hauteurV[presetCamera_];
    camera_.cibleDistance = borner(7.5 + ecart * 0.8, 10.0, 13.5);
    if (combat_->phase() == Phase::Fin) camera_.cibleDistance *= 0.8;
}

void Partie::majFinMatch() {
    if (entrees_.nav(Nav::Valider)) lancerMatch();
    if (entrees_.nav(Nav::Retour)) aller(Ecran::Titre);
}

void Partie::dessinerMonde() {
    rendu::scene::ciel(temps_);
    rendu::scene::arene(temps_);
    rendu::dessinerLampe(angleLumiere_);

    switch (ecran_) {
        case Ecran::Titre: {
            int paire = static_cast<int>(temps_ / 6.0);
            int a = paire % NB_PERSONNAGES, b = (paire + 2) % NB_PERSONNAGES;
            Pose p;
            p.t = temps_;
            rendu::scene::personnage(a, {-2.6, 0}, 0, p, 0);
            rendu::scene::personnage(b, {2.6, 0}, 180, p, 0);
            break;
        }
        case Ecran::Selection: {
            Pose p;
            p.t = temps_;
            double c = std::fmod(temps_, 4.0);
            if (c > 2.0 && c < 2.6) p.coup = (c - 2.0) / 0.6;
            if (c > 3.0 && c < 3.7) p.special = (c - 3.0) / 0.7;
            if (poseForcee_) p = pose_;
            rendu::scene::personnage(curseur_, {0, 0}, 0, p, 0);
            break;
        }
        default:
            if (combat_) {
                rendu::scene::combattant(combat_->combattant(0), temps_);
                rendu::scene::combattant(combat_->combattant(1), temps_);
                rendu::scene::projectiles(*combat_, temps_);
            }
    }
}

void Partie::afficher() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, filaire_ ? GL_LINE : GL_FILL);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    camera_.placer();
    rendu::placerLumieres(angleLumiere_);
    dessinerMonde();
    particules_.dessiner();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    switch (ecran_) {
        case Ecran::Titre: rendu::ecrans::titre(largeur_, hauteur_, menuIndex_, difficulte_, temps_); break;
        case Ecran::Selection: rendu::ecrans::selection(largeur_, hauteur_, curseur_, phaseSel_, solo_, temps_); break;
        case Ecran::Combat:
        case Ecran::Pause:
        case Ecran::FinMatch:
            if (combat_) rendu::ecrans::combat(largeur_, hauteur_, *combat_, pvAff_, manches_, solo_, numeroManche_);
            if (ecran_ == Ecran::Pause) rendu::ecrans::pause(largeur_, hauteur_);
            if (ecran_ == Ecran::FinMatch)
                rendu::ecrans::finMatch(largeur_, hauteur_, vainqueurMatch_, solo_, choix_, manches_);
            break;
    }
}

}  // namespace arene
