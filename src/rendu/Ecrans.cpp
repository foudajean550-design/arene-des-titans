#include "Ecrans.hpp"

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

#include "../noyau/Config.hpp"
#include "Hud.hpp"

namespace arene::rendu::ecrans {

namespace {

constexpr Couleur BLANC = {1, 1, 1};
constexpr Couleur GRIS = {0.7f, 0.7f, 0.75f};
constexpr Couleur OR = {1.0f, 0.8f, 0.25f};
constexpr Couleur ENERGIE = {0.25f, 0.65f, 1.0f};
constexpr Couleur VIE = {0.3f, 0.85f, 0.35f};

std::string nomJoueur(int i, bool solo) { return i == 0 ? "J1" : (solo ? "IA" : "J2"); }

Couleur couleurVie(double r) {
    if (r > 0.5) return {0.3f, 0.85f, 0.35f};
    if (r > 0.25) return {0.95f, 0.75f, 0.2f};
    return {0.9f, 0.25f, 0.2f};
}

void statistique(double x, double y, double w, const char* nom, double ratio, Couleur c) {
    hud::texte(x, y + 14, nom, GRIS);
    hud::rect(x + 100, y, w - 100, 14, {0, 0, 0}, 0.6f);
    hud::rect(x + 100, y, (w - 100) * borner(ratio, 0, 1), 14, c);
    hud::cadre(x + 100, y, w - 100, 14, {0.8f, 0.8f, 0.8f}, 1.5f, 0.7f);
}

std::vector<std::string> decouper(const std::string& s, size_t max) {
    std::vector<std::string> lignes;
    std::string cur, mot;
    for (size_t i = 0; i <= s.size(); ++i) {
        if (i == s.size() || s[i] == ' ') {
            if (cur.size() + mot.size() + 1 > max && !cur.empty()) {
                lignes.push_back(cur);
                cur.clear();
            }
            cur += (cur.empty() ? "" : " ") + mot;
            mot.clear();
        } else {
            mot += s[i];
        }
    }
    if (!cur.empty()) lignes.push_back(cur);
    return lignes;
}

}  // namespace

void titre(int l, int h, int index, Difficulte diff, double t) {
    hud::debut(l, h);
    hud::rect(0, 0, l, h * 0.42, {0, 0, 0}, 0.45f);
    hud::titre(l / 2.0, h * 0.2, "ARENE DES TITANS", h * 0.11, OR);
    hud::texteCentre(l / 2.0, h * 0.29, "Combat 3D en arène - 5 créatures, 1 vainqueur", GRIS, hud::policeGrande());

    const char* dif[] = {"Facile", "Normal", "Difficile"};
    std::string items[4] = {"Solo contre l'IA", "Deux joueurs", std::string("Difficulté : < ") + dif[static_cast<int>(diff)] + " >",
                            "Quitter"};
    hud::rect(l / 2.0 - 200, h * 0.62, 400, 4 * 46 + 20, {0, 0, 0}, 0.55f);
    for (int i = 0; i < 4; ++i) {
        double y = h * 0.62 + 46 * (i + 1);
        bool sel = i == index;
        Couleur c = sel ? OR : BLANC;
        hud::texteCentre(l / 2.0, y, (sel ? "> " : "") + items[i] + (sel ? " <" : ""), c, hud::policeGrande());
    }
    double clig = 0.5 + 0.5 * std::sin(t * 4);
    hud::texteCentre(l / 2.0, h - 26, "Haut/Bas : choisir    Entrée : valider    Échap : quitter", GRIS, nullptr,
                     static_cast<float>(0.6 + 0.4 * clig));
    hud::fin();
}

void selection(int l, int h, int curseur, int phase, bool solo, double) {
    hud::debut(l, h);
    const Fiche& f = fiche(curseur);

    std::string titre = phase == 0 ? (solo ? "VOTRE COMBATTANT" : "JOUEUR 1 : CHOISISSEZ")
                                   : (solo ? "CHOISISSEZ VOTRE ADVERSAIRE" : "JOUEUR 2 : CHOISISSEZ");
    hud::rect(0, 0, l, 70, {0, 0, 0}, 0.55f);
    hud::titre(l / 2.0, 52, titre, 38, OR);

    double px = l - 380, py = 100;
    hud::rect(px, py, 350, 400, {0, 0, 0}, 0.6f);
    hud::cadre(px, py, 350, 400, f.accent, 2.5f);
    hud::texte(px + 20, py + 40, f.nom, f.accent, hud::policeGrande());
    hud::texte(px + 20, py + 66, f.titre, GRIS);
    statistique(px + 20, py + 92, 310, "Vie", f.pv / 150.0, VIE);
    statistique(px + 20, py + 120, 310, "Vitesse", f.vitesse / 5.6, {0.4f, 0.8f, 1.0f});
    statistique(px + 20, py + 148, 310, "Force", f.degats / 20.0, {0.95f, 0.4f, 0.3f});
    statistique(px + 20, py + 176, 310, "Allonge", f.portee / 3.0, {0.9f, 0.8f, 0.3f});
    hud::texte(px + 20, py + 225, std::string("Spécial : ") + f.special, OR);
    hud::texte(px + 20, py + 251, "Coût " + std::to_string(static_cast<int>(f.coutSpecial)) +
                                      " - dégâts " + std::to_string(static_cast<int>(f.degatsSpecial)),
               GRIS);
    double ly = py + 300;
    for (const std::string& l : decouper(f.description, 36)) {
        hud::texte(px + 20, ly, l, BLANC);
        ly += 24;
    }

    double tw = 150, gap = 14, x0 = l / 2.0 - (NB_PERSONNAGES * tw + (NB_PERSONNAGES - 1) * gap) / 2.0;
    for (int i = 0; i < NB_PERSONNAGES; ++i) {
        double x = x0 + i * (tw + gap), y = h - 120;
        bool sel = i == curseur;
        hud::rect(x, y, tw, 70, fiche(i).accent, sel ? 0.55f : 0.18f);
        hud::cadre(x, y, tw, 70, sel ? BLANC : GRIS, sel ? 4.0f : 1.5f, sel ? 1.0f : 0.5f);
        hud::texteCentre(x + tw / 2, y + 42, fiche(i).nom, BLANC);
    }
    hud::texteCentre(l / 2.0, h - 20, "Gauche/Droite : changer    Entrée : valider    Échap : retour", GRIS);
    hud::fin();
}

void combat(int l, int h, const Combat& c, const double pvAff[2], const int manches[2], bool solo,
            int numeroManche) {
    hud::debut(l, h);
    double bw = std::min(l * 0.38, 520.0), bh = 28, marge = 40, y = 48;

    for (int i = 0; i < 2; ++i) {
        const Combattant& f = c.combattant(i);
        bool droite = i == 1;
        double x = droite ? l - marge - bw : marge;
        hud::barre(x, y, bw, bh, f.fractionPv(), pvAff[i], couleurVie(f.fractionPv()), droite);
        hud::rect(x, y + bh + 8, bw, 9, {0, 0, 0}, 0.6f);
        double er = f.energie / config::ENERGIE_MAX;
        bool pret = f.energie >= f.fiche().coutSpecial;
        double ex = droite ? x + bw - bw * er : x;
        hud::rect(ex, y + bh + 8, bw * er, 9, pret ? Couleur{0.4f, 0.85f, 1.0f} : Couleur{0.2f, 0.4f, 0.7f});
        double cx = droite ? x + bw * (1 - f.fiche().coutSpecial / config::ENERGIE_MAX)
                           : x + bw * f.fiche().coutSpecial / config::ENERGIE_MAX;
        hud::rect(cx - 1, y + bh + 6, 2, 13, BLANC, 0.8f);
        std::string nom = nomJoueur(i, solo) + " - " + f.fiche().nom;
        if (droite) {
            hud::texte(x + bw - 8.0 * nom.size(), y - 14, nom, f.fiche().accent);
        } else {
            hud::texte(x, y - 14, nom, f.fiche().accent);
        }
        for (int m = 0; m < config::MANCHES_POUR_GAGNER; ++m) {
            double mx = droite ? x + bw - 18 - m * 24 : x + m * 24;
            hud::rect(mx, y + bh + 26, 16, 16, m < manches[i] ? OR : Couleur{0, 0, 0}, m < manches[i] ? 1.0f : 0.5f);
            hud::cadre(mx, y + bh + 26, 16, 16, BLANC, 1.5f, 0.8f);
        }
    }

    hud::rect(l / 2.0 - 40, 28, 80, 54, {0, 0, 0}, 0.6f);
    hud::texteCentre(l / 2.0, 62, std::to_string(static_cast<int>(std::ceil(c.chrono()))), BLANC, hud::policeGrande());
    hud::texteCentre(l / 2.0, 100, "Manche " + std::to_string(numeroManche), GRIS);

    if (c.phase() == Phase::Intro) {
        int n = static_cast<int>(std::ceil(config::DUREE_COMPTE - c.tempsPhase()));
        double frac = 1.0 - (std::ceil(config::DUREE_COMPTE - c.tempsPhase()) - (config::DUREE_COMPTE - c.tempsPhase()));
        hud::titre(l / 2.0, h * 0.42, std::to_string(std::max(1, n)), h * (0.2 + 0.08 * (1 - frac)), OR);
    } else if (c.phase() == Phase::Action && c.tempsPhase() < 1.0) {
        hud::titre(l / 2.0, h * 0.42, "COMBAT !", h * 0.17, {1.0f, 0.4f, 0.2f}, static_cast<float>(1.0 - c.tempsPhase()));
    } else if (c.phase() == Phase::Fin) {
        int v = c.vainqueur();
        bool ko = !c.combattant(0).vivant() || !c.combattant(1).vivant();
        hud::titre(l / 2.0, h * 0.38, ko ? "KO !" : (v < 0 ? "EGALITE" : "TEMPS ECOULE"), h * 0.2, {1.0f, 0.3f, 0.25f});
        if (v >= 0)
            hud::texteCentre(l / 2.0, h * 0.38 + 50,
                             nomJoueur(v, solo) + " - " + c.combattant(v).fiche().nom + " remporte la manche", BLANC,
                             hud::policeGrande());
        else
            hud::texteCentre(l / 2.0, h * 0.38 + 50, "Manche à rejouer", BLANC, hud::policeGrande());
    }

    hud::texteCentre(l / 2.0, h - 14,
                     "J1 : ZQSD / F attaque / G spécial / H garde     J2 : flèches / K L O     C caméra   P pause", GRIS,
                     nullptr, 0.8f);
    hud::fin();
}

void pause(int l, int h) {
    hud::debut(l, h);
    hud::rect(0, 0, l, h, {0, 0, 0}, 0.55f);
    hud::titre(l / 2.0, h * 0.4, "PAUSE", h * 0.14, OR);
    hud::texteCentre(l / 2.0, h * 0.55, "P / Entrée : reprendre      Échap : menu principal", BLANC, hud::policeGrande());
    hud::fin();
}

void finMatch(int l, int h, int vainqueur, bool solo, const int choix[2], const int manches[2]) {
    hud::debut(l, h);
    hud::rect(0, 0, l, h, {0, 0, 0}, 0.6f);
    bool defaite = solo && vainqueur == 1;
    hud::titre(l / 2.0, h * 0.3, defaite ? "DEFAITE" : "VICTOIRE", h * 0.16,
               defaite ? Couleur{0.9f, 0.3f, 0.25f} : OR);
    hud::texteCentre(l / 2.0, h * 0.3 + 50,
                     nomJoueur(vainqueur, solo) + " - " + fiche(choix[vainqueur]).nom + " remporte le match", BLANC,
                     hud::policeGrande());
    hud::texteCentre(l / 2.0, h * 0.3 + 90, std::to_string(manches[0]) + " - " + std::to_string(manches[1]), OR,
                     hud::policeGrande());
    hud::texteCentre(l / 2.0, h * 0.7, "Entrée : revanche      Échap : menu principal", GRIS, hud::policeGrande());
    hud::fin();
}

}  // namespace arene::rendu::ecrans
