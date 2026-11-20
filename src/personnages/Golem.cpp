#include <algorithm>
#include <cmath>

#include "../rendu/GL.hpp"
#include "../rendu/Primitives.hpp"
#include "../rendu/Textures.hpp"
#include "Modeles.hpp"

namespace arene::rendu {

namespace {

constexpr Couleur ROCHE = {0.85f, 0.8f, 0.75f};
constexpr Couleur BRAISE = {1.0f, 0.55f, 0.1f};

void jambe(int cote, double swing) {
    glPushMatrix();
    glTranslatef(0, 1.55, cote * 0.55);
    utiliserTexture(Matiere::Pierre);
    couleur(ROCHE);
    sphere(0.45, 10, 8);
    glRotatef(180 + cote * swing, 0, 0, 1);
    cocy(0.4, 0.34, 0.8, 10);
    glTranslatef(0, 0.8, 0);
    sphere(0.36, 10, 8);
    cocy(0.34, 0.38, 0.7, 10);
    glTranslatef(0, 0.7, 0);
    glRotatef(-(180 + cote * swing), 0, 0, 1);
    glPushMatrix();
    glTranslatef(-0.2, 0.15, 0);
    pave(1.1, 0.35, 0.8);
    glPopMatrix();
    glPopMatrix();
}

double angleBras(double p) {
    if (p < 0.45) return 180 + 170 * p / 0.45;
    if (p < 0.6) return 350 + 110 * (p - 0.45) / 0.15;
    return 460 + 80 * (p - 0.6) / 0.4;
}

void bras(int cote, double angle, double cote_swing) {
    glPushMatrix();
    glTranslatef(0, 3.0, cote * 1.1);
    utiliserTexture(Matiere::Pierre);
    couleur(ROCHE);
    glPushMatrix();
    glScalef(1.0, 0.85, 1.0);
    sphere(0.6, 12, 8);
    glPopMatrix();
    glRotatef(angle + cote * cote_swing, 0, 0, 1);
    cocy(0.32, 0.28, 1.0, 10);
    glTranslatef(0, 1.0, 0);
    sphere(0.32, 10, 8);
    glRotatef(-18, 0, 0, 1);
    cocy(0.3, 0.36, 0.9, 10);
    glTranslatef(0, 1.05, 0);
    pave(0.75, 0.75, 0.75);
    utiliserTexture(Matiere::Aucune);
    lueur(0.8f, 0.35f, 0.0f);
    couleur(BRAISE);
    glPushMatrix();
    glTranslatef(0, 0, 0.0);
    pave(0.78, 0.1, 0.78);
    glPopMatrix();
    finLueur();
    glPopMatrix();
}

}  // namespace

void Golem::dessiner(const Pose& p) const {
    double swing = 22 * p.allure * std::sin(p.marche);
    double angle = 180;
    double lever = 0;
    bool rocher = false;
    if (p.coup > 0) {
        angle = angleBras(p.coup);
    } else if (p.special > 0) {
        double q = p.special;
        angle = q < 0.5 ? 180 + 170 * (q / 0.5) : 350 + 110 * (q - 0.5) / 0.5;
        rocher = q < 0.5;
    }
    angle -= 70 * p.bloc;
    lever = std::fabs(std::sin(p.marche)) * p.allure * 0.08;

    glPushMatrix();
    glTranslatef(0, lever + 0.03 * std::sin(p.t * 1.5), 0);
    glTranslatef(0, 1.5, 0);
    double pench = 0;
    if (p.coup > 0.45 && p.coup < 0.9) pench = 14;
    glRotatef(pench - 6 * p.touche, 0, 0, 1);
    glTranslatef(0, -1.5, 0);

    jambe(1, swing);
    jambe(-1, swing);

    utiliserTexture(Matiere::Pierre);
    couleur(ROCHE);
    glPushMatrix();
    glTranslatef(0, 1.7, 0);
    pave(1.2, 0.4, 1.4);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.05, 2.55, 0);
    pave(1.7, 1.5, 1.7);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.35, 3.35, 0);
    pave(1.0, 0.5, 1.5);
    glPopMatrix();

    utiliserTexture(Matiere::Aucune);
    double pulse = 0.7 + 0.3 * std::sin(3 * p.t);
    lueur(0.9f * (float)pulse, 0.4f * (float)pulse, 0.0f);
    couleur(BRAISE);
    glPushMatrix();
    glTranslatef(-0.85, 2.55, 0);
    glRotatef(90, 0, 0, 1);
    cocy(0.0001, 0.35, 0.3, 4);
    glTranslatef(0, 0.3, 0);
    glRotatef(180, 0, 0, 1);
    cocy(0.0001, 0.35, 0.3, 4);
    glPopMatrix();
    finLueur();

    utiliserTexture(Matiere::Pierre);
    couleur(ROCHE);
    glPushMatrix();
    glTranslatef(-0.2, 3.75, 0);
    pave(0.8, 0.65, 0.8);
    utiliserTexture(Matiere::Aucune);
    lueur(1.0f, 0.5f, 0.0f);
    couleur({1.0f, 0.6f, 0.1f});
    for (int k = -1; k <= 1; k += 2) {
        glPushMatrix();
        glTranslatef(-0.41, 0.08, k * 0.2);
        pave(0.05, 0.1, 0.18);
        glPopMatrix();
    }
    finLueur();
    glPopMatrix();

    bras(1, angle, 0.0);
    bras(-1, angle, 0.0);
    if (rocher) {
        utiliserTexture(Matiere::Pierre);
        couleur(ROCHE);
        glPushMatrix();
        glTranslatef(0.2, 4.7, 0);
        glScalef(1.0, 0.85, 1.1);
        sphere(0.75, 12, 8);
        glPopMatrix();
    }
    glPopMatrix();
    utiliserTexture(Matiere::Aucune);
}

}  // namespace arene::rendu
