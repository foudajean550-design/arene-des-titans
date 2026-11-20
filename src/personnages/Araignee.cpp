#include <algorithm>
#include <cmath>

#include "../rendu/GL.hpp"
#include "../rendu/Primitives.hpp"
#include "../rendu/Textures.hpp"
#include "Modeles.hpp"

namespace arene::rendu {

namespace {

constexpr Couleur VIOLET = {0.75f, 0.55f, 0.95f};
constexpr Couleur NOIR = {0.18f, 0.1f, 0.25f};

void patte(double x, double z, double alpha, double phase, double allure, double leve, double bloc) {
    double balan = 16 * allure * std::sin(phase);
    double soulev = 28 * allure * std::max(0.0, std::cos(phase)) + leve - 15 * bloc;
    glPushMatrix();
    glTranslatef(x, 0.95, z);
    glRotatef(-alpha + balan, 0, 1, 0);
    utiliserTexture(Matiere::Aucune);
    couleur(NOIR);
    sphere(0.17, 10, 6);
    utiliserTexture(Matiere::Chitine);
    couleur(VIOLET);
    double e1 = 55 + soulev, e2 = -65;
    glRotatef(e1 - 90, 0, 0, 1);
    cocy(0.1, 0.08, 1.0, 8);
    glTranslatef(0, 1.0, 0);
    utiliserTexture(Matiere::Aucune);
    couleur(NOIR);
    sphere(0.11, 8, 6);
    utiliserTexture(Matiere::Chitine);
    couleur(VIOLET);
    glRotatef(e2 - e1, 0, 0, 1);
    cocy(0.08, 0.035, 1.9, 8);
    glPopMatrix();
}

}  // namespace

void Araignee::dessiner(const Pose& p) const {
    double e = enveloppeCoup(p.coup);
    double s = bosse(p.special);

    glPushMatrix();
    glTranslatef(-0.9 * e, 0.03 * std::sin(3 * p.t), 0);
    glTranslatef(1.0, 0, 0);
    glRotatef(-28 * s + 8 * p.touche, 0, 0, 1);
    glTranslatef(-1.0, 0, 0);

    const double ang[4] = {127, 101, 75, 45};
    const double px[4] = {-0.95, -0.55, -0.1, 0.35};
    for (int i = 0; i < 4; i++)
        for (int c = -1; c <= 1; c += 2) {
            double phase = p.marche + i * PI / 2 + (c > 0 ? PI : 0);
            double leve = (i < 2 ? 38 * e : 0) + (i == 0 ? 20 * s : 0);
            double bx = px[i] * 0.8;
            patte(bx, c * 0.35, c > 0 ? ang[i] : -ang[i], phase, p.allure, leve, p.bloc);
        }

    utiliserTexture(Matiere::Chitine);
    couleur(VIOLET);
    glPushMatrix();
    glTranslatef(-0.75, 1.05, 0);
    glScalef(1.0, 0.75, 0.95);
    sphere(0.62, 16, 10);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.75, 1.2, 0);
    glScalef(1.2, 0.9, 1.0);
    sphere(0.95, 18, 10);
    glPopMatrix();

    utiliserTexture(Matiere::Aucune);
    couleur({0.9f, 0.1f, 0.1f});
    glPushMatrix();
    glTranslatef(0.55, 2.0, 0);
    glScalef(1.4, 0.15, 0.7);
    sphere(0.4, 10, 6);
    glPopMatrix();

    lueur(1.0f, 0.1f, 0.1f);
    couleur({1.0f, 0.15f, 0.15f});
    const double yeux[6][3] = {{-1.25, 1.25, 0.18}, {-1.25, 1.25, -0.18}, {-1.15, 1.38, 0.32},
                               {-1.15, 1.38, -0.32}, {-1.3, 1.12, 0.38}, {-1.3, 1.12, -0.38}};
    for (auto& y : yeux) {
        glPushMatrix();
        glTranslatef(y[0], y[1], y[2]);
        sphere(0.07, 8, 6);
        glPopMatrix();
    }
    finLueur();

    couleur({0.95f, 0.9f, 0.8f});
    double ouv = 25 * e + 35 * s;
    for (int c = -1; c <= 1; c += 2) {
        glPushMatrix();
        glTranslatef(-1.3, 0.85, c * 0.22);
        glRotatef(180 - ouv * 0.5, 0, 0, 1);
        glRotatef(-c * ouv * 0.4, 1, 0, 0);
        cocy(0.1, 0, 0.55, 8);
        glPopMatrix();
    }
    if (s > 0.1) {
        lueur(0.3f, 1.0f * (float)s, 0.2f);
        couleur({0.6f, 1.0f, 0.4f});
        glPushMatrix();
        glTranslatef(-1.45, 0.8, 0);
        sphere(0.12 + 0.2 * s, 10, 8);
        glPopMatrix();
        finLueur();
    }
    glPopMatrix();
    utiliserTexture(Matiere::Aucune);
}

}  // namespace arene::rendu
