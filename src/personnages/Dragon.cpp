#include <algorithm>
#include <cmath>

#include "../rendu/GL.hpp"
#include "../rendu/Primitives.hpp"
#include "../rendu/Textures.hpp"
#include "Modeles.hpp"

namespace arene::rendu {

namespace {

constexpr Couleur ROUGE = {1.0f, 0.85f, 0.8f};
constexpr Couleur OS = {0.95f, 0.9f, 0.75f};

void aile(int cote, double battement) {
    glPushMatrix();
    glTranslatef(0.2, 2.35, cote * 0.55);
    glRotatef(cote * (55 + battement), 1, 0, 0);
    utiliserTexture(Matiere::Dragon);
    couleur(ROUGE);
    cocy(0.1, 0.07, 2.0, 8);
    utiliserTexture(Matiere::Membrane);
    couleur({1, 1, 1});
    glNormal3f(0, 0, 1);
    glBegin(GL_TRIANGLE_FAN);
    glTexCoord2f(0.1f, 0.0f);
    glVertex3f(0.1f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(0.0f, 2.0f, 0.0f);
    glTexCoord2f(0.6f, 0.9f);
    glVertex3f(0.8f, 1.7f, 0.0f);
    glTexCoord2f(1.0f, 0.6f);
    glVertex3f(1.5f, 1.1f, 0.0f);
    glTexCoord2f(1.0f, 0.2f);
    glVertex3f(1.8f, 0.4f, 0.0f);
    glTexCoord2f(0.6f, 0.0f);
    glVertex3f(1.2f, -0.2f, 0.0f);
    glEnd();
    utiliserTexture(Matiere::Aucune);
    couleur(OS);
    for (double a : {-12.0, -38.0, -62.0}) {
        glPushMatrix();
        glTranslatef(0, 2.0, 0);
        glRotatef(a, 0, 0, 1);
        cocy(0.045, 0.02, 1.0 + (a < -50 ? 0.3 : 0.0), 6);
        glPopMatrix();
    }
    glPopMatrix();
}

void patte(double x, double z, bool arriere, double balan) {
    glPushMatrix();
    glTranslatef(x, 1.35, z);
    utiliserTexture(Matiere::Dragon);
    couleur(ROUGE);
    sphere(arriere ? 0.38 : 0.3, 12, 8);
    double cuisse = arriere ? 0.75 : 0.7;
    glRotatef(180 + (arriere ? 25 : -10) + balan, 0, 0, 1);
    cocy(arriere ? 0.28 : 0.2, 0.16, cuisse, 10);
    glTranslatef(0, cuisse, 0);
    sphere(0.17, 8, 6);
    glRotatef(arriere ? -60 : 35, 0, 0, 1);
    cocy(0.14, 0.1, 0.65, 10);
    glTranslatef(0, 0.65, 0);
    glRotatef(-(180 + (arriere ? 25 : -10) + balan + (arriere ? -60 : 35)), 0, 0, 1);
    utiliserTexture(Matiere::Aucune);
    couleur(OS);
    glPushMatrix();
    glTranslatef(-0.2, 0.0, 0);
    pave(0.55, 0.12, 0.3);
    glPopMatrix();
    for (int k = -1; k <= 1; k++) {
        glPushMatrix();
        glTranslatef(-0.5, 0.0, k * 0.1);
        glRotatef(90, 0, 0, 1);
        cocy(0.04, 0, 0.18, 5);
        glPopMatrix();
    }
    glPopMatrix();
}

void tete(double machoire, double feu) {
    utiliserTexture(Matiere::Dragon);
    couleur(ROUGE);
    sphere(0.4, 14, 10);
    glPushMatrix();
    glTranslatef(-0.75, -0.05, 0);
    pave(0.9, 0.25, 0.4);
    glPopMatrix();
    utiliserTexture(Matiere::Aucune);
    couleur(OS);
    for (int k = -1; k <= 1; k += 2) {
        glPushMatrix();
        glTranslatef(0.2, 0.3, k * 0.22);
        glRotatef(-60, 0, 0, 1);
        glRotatef(-k * 15, 1, 0, 0);
        cocy(0.09, 0, 0.7, 6);
        glPopMatrix();
        for (int i = 0; i < 4; i++) {
            glPushMatrix();
            glTranslatef(-0.45 - i * 0.2, -0.18, k * 0.15);
            glRotatef(180, 1, 0, 0);
            cocy(0.035, 0, 0.12, 5);
            glPopMatrix();
        }
    }
    lueur(1.0f, 0.85f, 0.1f);
    couleur({1.0f, 0.9f, 0.2f});
    for (int k = -1; k <= 1; k += 2) {
        glPushMatrix();
        glTranslatef(-0.28, 0.17, k * 0.3);
        sphere(0.08, 8, 6);
        glPopMatrix();
    }
    finLueur();

    glPushMatrix();
    glTranslatef(-0.2, -0.25, 0);
    glRotatef(machoire, 0, 0, 1);
    utiliserTexture(Matiere::Dragon);
    couleur(ROUGE);
    glTranslatef(-0.45, 0, 0);
    pave(0.85, 0.15, 0.32);
    glPopMatrix();

    if (feu > 0.05) {
        lueur(1.0f, 0.5f * (float)feu, 0.0f);
        couleur({1.0f, 0.6f, 0.1f});
        utiliserTexture(Matiere::Aucune);
        glPushMatrix();
        glTranslatef(-0.95, -0.2, 0);
        sphere(0.1 + 0.3 * feu, 10, 8);
        glPopMatrix();
        finLueur();
    }
}

}  // namespace

void Dragon::dessiner(const Pose& p) const {
    double e = enveloppeCoup(p.coup);
    double s = bosse(p.special);
    double marche = p.allure * std::sin(p.marche);
    double battement = 22 * std::sin(2.2 * p.t) * (1 - 0.5 * p.allure) + 35 * s + 25 * e - 20 * p.bloc;
    double queueSwing = 18 * std::sin(1.4 * p.t) + 20 * p.allure * std::sin(p.marche * 0.5);

    glPushMatrix();
    glScalef(0.85f, 0.85f, 0.85f);
    glTranslatef(-0.4 * e, 0.04 * std::fabs(std::sin(p.marche)) * p.allure, 0);
    glTranslatef(0.3, 1.0, 0);
    glRotatef(-8 * p.touche - 10 * s, 0, 0, 1);
    glTranslatef(-0.3, -1.0, 0);

    patte(0.55, 0.6, true, 20 * marche);
    patte(0.55, -0.6, true, -20 * marche);
    patte(-0.7, 0.55, false, -18 * marche);
    patte(-0.7, -0.55, false, 18 * marche);

    utiliserTexture(Matiere::Dragon);
    couleur(ROUGE);
    glPushMatrix();
    glTranslatef(0.1, 1.85, 0);
    glScalef(1.6, 0.85, 0.75);
    sphere(1.0, 20, 12);
    glPopMatrix();

    aile(1, battement);
    aile(-1, battement);

    utiliserTexture(Matiere::Aucune);
    couleur(OS);
    for (int i = 0; i < 5; i++) {
        glPushMatrix();
        glTranslatef(1.2 - i * 0.5, 2.55 - 0.1 * std::fabs(i - 2) * 0.5, 0);
        glRotatef(10, 0, 0, 1);
        cocy(0.1, 0, 0.35, 5);
        glPopMatrix();
    }

    glPushMatrix();
    glTranslatef(-1.2, 2.1, 0);
    double pliage = 28 - 22 * s + 32 * e;
    for (int i = 0; i < 3; i++) {
        glRotatef(pliage * (i == 0 ? 1.0 : 0.45) * (i == 0 ? 1 : 1) + (i > 0 ? -8 * s : 0), 0, 0, 1);
        utiliserTexture(Matiere::Dragon);
        couleur(ROUGE);
        sphere(0.3 - i * 0.03, 10, 8);
        cocy(0.28 - i * 0.03, 0.25 - i * 0.03, 0.65, 10);
        utiliserTexture(Matiere::Aucune);
        couleur(OS);
        glPushMatrix();
        glTranslatef(0.2, 0.4, 0);
        glRotatef(-70, 0, 0, 1);
        cocy(0.07, 0, 0.25, 5);
        glPopMatrix();
        glTranslatef(0, 0.65, 0);
    }
    glRotatef(-65 - 10 * e, 0, 0, 1);
    tete(18 * e + 38 * s + 2 * std::sin(3 * p.t), s);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.4, 1.65, 0);
    glRotatef(queueSwing * 0.4, 0, 1, 0);
    glRotatef(-95, 0, 0, 1);
    for (int i = 0; i < 4; i++) {
        glRotatef(queueSwing * 0.25 * (i ? 1 : 0), 0, 1, 0);
        glRotatef(i ? 6 : 0, 0, 0, 1);
        utiliserTexture(Matiere::Dragon);
        couleur(ROUGE);
        double r0 = 0.5 - i * 0.1, r1 = 0.4 - i * 0.1;
        sphere(r0, 10, 8);
        cocy(r0, r1 > 0.05 ? r1 : 0.05, 0.7, 10);
        utiliserTexture(Matiere::Aucune);
        couleur(OS);
        glPushMatrix();
        glTranslatef(-r0 * 0.9, 0.35, 0);
        glRotatef(90, 0, 0, 1);
        cocy(0.08, 0, 0.25, 5);
        glPopMatrix();
        glTranslatef(0, 0.7, 0);
    }
    glPopMatrix();

    glPopMatrix();
    utiliserTexture(Matiere::Aucune);
}

}  // namespace arene::rendu
