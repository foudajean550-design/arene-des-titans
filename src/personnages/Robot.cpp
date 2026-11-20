#include <algorithm>
#include <cmath>

#include "../rendu/GL.hpp"
#include "../rendu/Primitives.hpp"
#include "../rendu/Textures.hpp"
#include "Modeles.hpp"

namespace arene::rendu {

namespace {

constexpr Couleur ACIER = {0.78f, 0.82f, 0.92f};
constexpr Couleur ORANGE = {1.0f, 0.5f, 0.1f};
constexpr Couleur SOMBRE = {0.22f, 0.24f, 0.30f};

void jambe(int cote, double swing) {
    double flex = std::max(0.0, -cote * swing) * 0.8;
    glPushMatrix();
    glTranslatef(0, 1.5, cote * 0.38);
    utiliserTexture(Matiere::Aucune);
    couleur(SOMBRE);
    sphere(0.19, 12, 8);
    utiliserTexture(Matiere::Metal);
    couleur(ACIER);
    glRotatef(180 + cote * swing, 0, 0, 1);
    cocy(0.15, 0.13, 0.7, 12);
    glTranslatef(0, 0.7, 0);
    utiliserTexture(Matiere::Aucune);
    couleur(SOMBRE);
    sphere(0.15, 10, 8);
    glRotatef(-flex, 0, 0, 1);
    utiliserTexture(Matiere::Metal);
    couleur(ACIER);
    cocy(0.13, 0.11, 0.6, 12);
    glTranslatef(0, 0.6, 0);
    glRotatef(-(180 + cote * swing - flex), 0, 0, 1);
    couleur(ORANGE);
    utiliserTexture(Matiere::Aucune);
    glPushMatrix();
    glTranslatef(-0.18, 0.0, 0);
    pave(0.75, 0.18, 0.46);
    glPopMatrix();
    glPopMatrix();
}

void bras(int cote, const Pose& p, double swing, double e, double s) {
    glPushMatrix();
    glTranslatef(0, 2.65, cote * 0.78);
    utiliserTexture(Matiere::Metal);
    couleur(ORANGE);
    sphere(0.3, 12, 8);
    couleur(ACIER);
    double haut = 180 + cote * swing;
    double coude = -25;
    if (cote == 1) {
        haut -= 90 * std::max(e, s) * 0.9;
        coude = -25 * (1 - s);
    }
    haut -= 60 * p.bloc;
    coude -= 90 * p.bloc;
    glRotatef(haut, 0, 0, 1);
    cocy(0.14, 0.12, 0.7, 12);
    glTranslatef(0, 0.7, 0);
    utiliserTexture(Matiere::Aucune);
    couleur(SOMBRE);
    sphere(0.15, 10, 8);
    glRotatef(coude, 0, 0, 1);
    utiliserTexture(Matiere::Metal);
    couleur(ACIER);
    if (cote == 1 && (s > 0 || e > 0)) {
        cocy(0.19, 0.22, 0.7, 12);
        glTranslatef(0, 0.7, 0);
        utiliserTexture(Matiere::Aucune);
        lueur(0.1f * s, 0.8f * s, 1.0f * s);
        couleur({0.3f, 0.9f, 1.0f});
        sphere(0.1 + 0.18 * s, 12, 8);
        finLueur();
    } else {
        cocy(0.12, 0.17, 0.6, 12);
        glTranslatef(0, 0.6, 0);
        utiliserTexture(Matiere::Aucune);
        couleur(SOMBRE);
        pave(0.3, 0.3, 0.3);
    }
    glPopMatrix();
}

}  // namespace

void Robot::dessiner(const Pose& p) const {
    double e = enveloppeCoup(p.coup);
    double s = bosse(p.special);
    double swing = 32 * p.allure * std::sin(p.marche);
    double rebond = 0.05 * std::fabs(std::sin(p.marche)) * p.allure + 0.02 * std::sin(2 * p.t);

    glPushMatrix();
    glTranslatef(-0.5 * e, rebond, 0);
    glTranslatef(0, 1.5, 0);
    glRotatef(10 * e - 10 * p.touche - 4 * p.bloc, 0, 0, 1);
    glTranslatef(0, -1.5, 0);

    jambe(1, swing);
    jambe(-1, swing);

    utiliserTexture(Matiere::Metal);
    couleur(ACIER);
    glPushMatrix();
    glTranslatef(0, 1.55, 0);
    pave(0.7, 0.25, 0.9);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0, 2.3, 0);
    pave(0.95, 1.1, 1.25);
    glPopMatrix();

    utiliserTexture(Matiere::Aucune);
    lueur(0.1f, 0.5f + 0.4f * (float)s, 1.0f);
    couleur({0.3f, 0.8f, 1.0f});
    glPushMatrix();
    glTranslatef(-0.5, 2.3, 0);
    sphere(0.2, 12, 8);
    glPopMatrix();
    finLueur();

    utiliserTexture(Matiere::Metal);
    couleur(ACIER);
    glPushMatrix();
    glTranslatef(0, 2.85, 0);
    cocy(0.17, 0.17, 0.2, 10);
    glTranslatef(0, 0.2, 0);
    pave(0.6, 0.5, 0.6);
    utiliserTexture(Matiere::Aucune);
    lueur(1.0f, 0.1f, 0.1f);
    couleur({1.0f, 0.2f, 0.2f});
    glPushMatrix();
    glTranslatef(-0.31, 0.03, 0);
    pave(0.06, 0.12, 0.46);
    glPopMatrix();
    finLueur();
    couleur(SOMBRE);
    glPushMatrix();
    glTranslatef(0.1, 0.25, 0.15);
    cocy(0.03, 0.02, 0.35, 6);
    glTranslatef(0, 0.35, 0);
    couleur(ORANGE);
    sphere(0.06, 8, 6);
    glPopMatrix();
    glPopMatrix();

    bras(1, p, swing, e, s);
    bras(-1, p, swing, e, s);
    glPopMatrix();
    utiliserTexture(Matiere::Aucune);
}

}  // namespace arene::rendu
