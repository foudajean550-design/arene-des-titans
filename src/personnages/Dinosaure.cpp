#include <algorithm>
#include <cmath>

#include "../rendu/GL.hpp"
#include "../rendu/Primitives.hpp"
#include "../rendu/Textures.hpp"
#include "Modeles.hpp"

namespace arene::rendu {

namespace {

struct Anim {
    double bras, queue, lumiere, jambe, yeux, machoire, bloc;
};

void bras(int cote, const Anim& a) {
    glPushMatrix();
    glTranslatef(-0.35, 2.3, cote * 0.42);
    utiliserTexture(Matiere::Ecailles);
    glColor3f(1, 1, 1);
    sphere(0.2, 12, 8);
    glRotatef(120 - 50 * a.bloc + cote * a.bras, 0, 0, 1);
    cocy(0.1, 0.1, 0.9, 10);
    glTranslatef(0, 0.9, 0);
    sphere(0.14, 12, 8);
    glRotatef(-60 - 40 * a.bloc + cote * a.bras, 0, 0, 1);
    cocy(0.08, 0.08, 0.7, 10);
    glTranslatef(0, 0.7, 0);
    utiliserTexture(Matiere::Peau);
    glPushMatrix();
    glTranslatef(0, 0.15, 0);
    pave(0.22, 0.3, 0.28);
    glPopMatrix();
    utiliserTexture(Matiere::Aucune);
    glColor3f(0.9, 0.9, 0.8);
    for (int k = -1; k <= 1; k++) {
        glPushMatrix();
        glTranslatef(0, 0.3, k * 0.09);
        cocy(0.04, 0, 0.22, 6);
        glPopMatrix();
    }
    glPopMatrix();
}

void jambe(int cote, const Anim& a) {
    double balan = cote * a.jambe;
    glPushMatrix();
    glTranslatef(0, 0.3, cote * 0.45);
    utiliserTexture(Matiere::Ecailles);
    glColor3f(1, 1, 1);
    sphere(0.3, 14, 8);
    glRotatef(165 + balan, 0, 0, 1);
    cocy(0.18, 0.15, 1.4, 12);
    glTranslatef(0, 1.4, 0);
    sphere(0.22, 12, 8);
    glRotatef(30, 0, 0, 1);
    cocy(0.15, 0.12, 1.3, 12);
    glTranslatef(0, 1.3, 0);
    sphere(0.16, 12, 8);
    glRotatef(-195 - balan, 0, 0, 1);
    utiliserTexture(Matiere::Peau);
    glPushMatrix();
    glTranslatef(-0.25, -0.1, 0);
    pave(0.9, 0.2, 0.4);
    glPopMatrix();
    utiliserTexture(Matiere::Aucune);
    glColor3f(0.9, 0.9, 0.8);
    for (int k = -1; k <= 1; k++) {
        glPushMatrix();
        glTranslatef(-0.7, -0.1, k * 0.12);
        glRotatef(90, 0, 0, 1);
        cocy(0.06, 0, 0.25, 6);
        glPopMatrix();
    }
    glPopMatrix();
}

void queue(const Anim& a) {
    glPushMatrix();
    glTranslatef(0.55, 0.3, 0);
    glRotatef(a.queue, 0, 1, 0);
    glRotatef(-80, 0, 0, 1);
    utiliserTexture(Matiere::Ecailles);
    glColor3f(1, 1, 1);
    sphere(0.4, 14, 8);
    cocy(0.4, 0.25, 1.6, 14);
    utiliserTexture(Matiere::Aucune);
    glColor3f(0, 1, 0);
    for (int i = 1; i <= 3; i++) {
        double y = i * 0.4;
        double ray = 0.4 + (y / 1.6) * (0.25 - 0.4);
        glPushMatrix();
        glTranslatef(-ray, y, 0);
        glRotatef(90, 0, 0, 1);
        cocy(0.12, 0, 0.35, 5);
        glPopMatrix();
    }
    glTranslatef(0, 1.6, 0);
    utiliserTexture(Matiere::Ecailles);
    glColor3f(1, 1, 1);
    sphere(0.25, 12, 8);
    glRotatef(80, 0, 0, 1);
    glRotatef(a.queue * 0.7, 0, 1, 0);
    glRotatef(-60, 0, 0, 1);
    cocy(0.25, 0, 1.4, 14);
    utiliserTexture(Matiere::Aucune);
    glColor3f(0, 1, 0);
    for (int i = 1; i <= 2; i++) {
        double y = i * 0.4;
        double ray = 0.25 * (1 - y / 1.4);
        glPushMatrix();
        glTranslatef(-ray, y, 0);
        glRotatef(90, 0, 0, 1);
        cocy(0.1, 0, 0.3, 5);
        glPopMatrix();
    }
    glPopMatrix();
}

void oeil(double px, double py, double pz, const Anim& a) {
    glPushMatrix();
    glTranslatef(px, py, pz);
    orienter(px, py, pz);
    double h = 0.4 * a.yeux;
    if (h > 0.01) {
        utiliserTexture(Matiere::Ecailles);
        glColor3f(1, 1, 1);
        cocy(0.05, 0.05, h, 8);
    }
    glTranslatef(0, h, 0);
    utiliserTexture(Matiere::Aucune);
    glColor3f(1, 1, 0.8);
    sphere(0.14, 12, 8);
    glTranslatef(0, 0.1, 0);
    glColor3f(0, 0, 0);
    sphere(0.07, 8, 6);
    glPopMatrix();
}

void corps(const Anim& a) {
    utiliserTexture(Matiere::Ecailles);
    glColor3f(1, 1, 1);
    cocy(0.75, 0.5, 3, 20);
    glPushMatrix();
    glScalef(1, 0.6, 1);
    sphere(0.75, 20, 10);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0, 3, 0);
    glScalef(1, 0.35, 1);
    sphere(0.5, 20, 8);
    glPopMatrix();
    utiliserTexture(Matiere::Aucune);
    for (int i = 0; i < 9; i++) {
        glPushMatrix();
        double y = 0.3 + i * 0.3;
        double ray = 0.75 + (y / 3) * (0.5 - 0.75);
        glColor3f(0, 1, 0);
        glTranslatef(ray, y, 0);
        glRotatef(-90, 0, 0, 1);
        cocy(0.25, 0, 0.3, 5);
        glPopMatrix();
    }
    bras(1, a);
    bras(-1, a);
    jambe(1, a);
    jambe(-1, a);
    queue(a);
}

void tete(const Anim& a) {
    glColor3f(1, 1, 1);
    utiliserTexture(Matiere::Ecailles);
    sphere(0.65, 24, 12);
    glPushMatrix();
    glTranslatef(0, -0.7, 0);
    cocy(0.5, 0.30, 0.4, 15);
    sphere(0.38, 12, 8);
    glPopMatrix();

    utiliserTexture(Matiere::Aucune);
    glColor3f(0, 1, 0);
    const double piq[2][3] = {{0.35, 0.55, 0}, {0.58, 0.3, 0}};
    for (int i = 0; i < 2; i++) {
        glPushMatrix();
        glTranslatef(piq[i][0], piq[i][1], piq[i][2]);
        orienter(piq[i][0], piq[i][1], piq[i][2]);
        cocy(0.1, 0, 0.3, 5);
        glPopMatrix();
    }

    oeil(-0.3, 0.35, 0.45, a);
    oeil(-0.3, 0.35, -0.45, a);

    utiliserTexture(Matiere::Peau);
    glColor3f(1, 1, 1);
    glPushMatrix();
    glTranslatef(-1.0, -0.05, 0);
    pave(1.6, 0.45, 0.7);
    glPopMatrix();
    utiliserTexture(Matiere::Aucune);
    glColor3f(0.95, 0.95, 0.85);
    for (int i = 0; i < 5; i++)
        for (int k = -1; k <= 1; k += 2) {
            glPushMatrix();
            glTranslatef(-0.4 - i * 0.3, -0.275, k * 0.25);
            glRotatef(180, 1, 0, 0);
            cocy(0.06, 0, 0.18, 5);
            glPopMatrix();
        }

    glPushMatrix();
    glTranslatef(-0.35, -0.5, 0);
    utiliserTexture(Matiere::Ecailles);
    glColor3f(1, 1, 1);
    sphere(0.13, 10, 6);
    glRotatef(a.machoire, 0, 0, 1);
    glTranslatef(-0.75, 0, 0);
    utiliserTexture(Matiere::Peau);
    pave(1.5, 0.3, 0.6);
    utiliserTexture(Matiere::Aucune);
    glColor3f(0.95, 0.95, 0.85);
    for (int i = 0; i < 5; i++)
        for (int k = -1; k <= 1; k += 2) {
            glPushMatrix();
            glTranslatef(0.5 - i * 0.3, 0.15, k * 0.2);
            cocy(0.06, 0, 0.18, 5);
            glPopMatrix();
        }
    glPopMatrix();
}

}  // namespace

void Dinosaure::dessiner(const Pose& p) const {
    double e = enveloppeCoup(p.coup);
    double s = bosse(p.special);
    Anim a;
    a.bras = 12 * std::sin(2 * p.t) + 25 * p.allure * std::sin(p.marche) + 40 * e;
    a.queue = 20 * std::sin(1.5 * p.t) + 25 * p.allure * std::sin(p.marche * 0.5);
    a.jambe = 22 * p.allure * std::sin(p.marche);
    a.yeux = s > 0.15 ? std::min(1.0, s * 1.5) : 0.0;
    a.machoire = 40 * e + 35 * s + 3 * std::sin(3 * p.t);
    a.bloc = p.bloc;
    a.lumiere = 0;

    glPushMatrix();
    glScalef(0.45f, 0.45f, 0.45f);
    glTranslatef(-1.2 * e, 0, 0);
    glRotatef(14 * e - 12 * p.touche - 6 * p.bloc - 10 * s, 0, 0, 1);
    glTranslatef(0, 3.41 + 0.05 * std::fabs(std::sin(p.marche)) * p.allure, 0);
    glTranslatef(0, 2.8, 0);
    tete(a);
    glPushMatrix();
    glTranslatef(0, -3.7, 0);
    corps(a);
    glPopMatrix();
    glPopMatrix();
    utiliserTexture(Matiere::Aucune);
}

}  // namespace arene::rendu
