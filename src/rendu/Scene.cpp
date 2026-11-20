#include "Scene.hpp"

#include <cmath>

#include "../noyau/Config.hpp"
#include "../personnages/Modele.hpp"
#include "GL.hpp"
#include "Primitives.hpp"
#include "Textures.hpp"

namespace arene::rendu::scene {

namespace {

void sol(double rayon, int n) {
    utiliserTexture(Matiere::Sol);
    glColor3f(1, 1, 1);
    glNormal3f(0, 1, 0);
    glBegin(GL_TRIANGLE_FAN);
    glTexCoord2f(0, 0);
    glVertex3f(0, 0, 0);
    for (int i = 0; i <= n; i++) {
        double u = i * 2 * PI / n;
        double x = rayon * std::cos(u), z = rayon * std::sin(u);
        glTexCoord2f(x * 0.18, z * 0.18);
        glVertex3f(x, 0, z);
    }
    glEnd();
}

void anneau(double r0, double r1, int n, float r, float g, float b) {
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glColor3f(r, g, b);
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= n; i++) {
        double u = i * 2 * PI / n;
        glVertex3f(r0 * std::cos(u), 0.03, r0 * std::sin(u));
        glVertex3f(r1 * std::cos(u), 0.03, r1 * std::sin(u));
    }
    glEnd();
    glEnable(GL_LIGHTING);
}

void ombre(double rayon) {
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);
    glColor4f(0, 0, 0, 0.4f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0, 0.02, 0);
    for (int i = 0; i <= 24; i++) {
        double u = i * 2 * PI / 24;
        glVertex3f(rayon * std::cos(u), 0.02, rayon * std::sin(u));
    }
    glEnd();
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

}  // namespace

void ciel(double) {
    static double etoiles[200][3];
    static bool pret = false;
    if (!pret) {
        for (auto& e : etoiles) {
            double a = (std::rand() / (double)RAND_MAX) * 2 * PI;
            double h = 0.1 + (std::rand() / (double)RAND_MAX) * 0.9;
            e[0] = 60 * std::cos(a) * std::sqrt(1 - h * h);
            e[1] = 60 * h;
            e[2] = 60 * std::sin(a) * std::sqrt(1 - h * h);
        }
        pret = true;
    }
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glPointSize(2.0f);
    glColor3f(0.8f, 0.85f, 1.0f);
    glBegin(GL_POINTS);
    for (auto& e : etoiles) glVertex3dv(e);
    glEnd();
    glEnable(GL_LIGHTING);
}

void arene(double t) {
    const double R = config::RAYON_ARENE + 1.2;
    sol(R, 64);

    utiliserTexture(Matiere::Pierre);
    glColor3f(0.8f, 0.78f, 0.75f);
    glPushMatrix();
    glTranslatef(0, -0.64, 0);
    cocy(R + 0.3, R + 0.3, 0.6, 64);
    glPopMatrix();

    utiliserTexture(Matiere::Aucune);
    glColor3f(0.06f, 0.06f, 0.08f);
    disque(19, -0.66, 48, 1);

    anneau(config::RAYON_ARENE, config::RAYON_ARENE + 0.15, 64, 1.0f, 0.45f, 0.1f);

    for (int i = 0; i < 8; i++) {
        double a = i * 2 * PI / 8 + PI / 8;
        double r = 14.5;
        glPushMatrix();
        glTranslatef(r * std::cos(a), -0.66, r * std::sin(a));
        utiliserTexture(Matiere::Pierre);
        glColor3f(0.85f, 0.82f, 0.78f);
        cocy(0.8, 0.65, 6.0, 14);
        glTranslatef(0, 6.0, 0);
        cocy(0.8, 0.8, 0.3, 14);
        glTranslatef(0, 0.3, 0);
        utiliserTexture(Matiere::Aucune);
        double f = 0.8 + 0.2 * std::sin(t * 7 + i * 1.7);
        glDisable(GL_LIGHTING);
        glColor3f(1.0f, 0.55f * (float)f, 0.1f);
        sphere(0.45 * f + 0.15, 10, 8);
        glEnable(GL_LIGHTING);
        glPopMatrix();
    }
    utiliserTexture(Matiere::Aucune);
}

void personnage(int id, Vec2 pos, double yaw, const Pose& pose, double flash) {
    glPushMatrix();
    glTranslatef(pos.x, 0, pos.z);
    glRotatef(yaw, 0, 1, 0);

    utiliserTexture(Matiere::Aucune);
    ombre(id == ARAIGNEE ? 1.9 : (id == GOLEM ? 1.5 : 1.2));

    glPushMatrix();
    double rebond = pose.victoire > 0 ? 0.35 * std::fabs(std::sin(pose.t * 6)) : 0.0;
    glTranslatef(0, rebond, 0);
    if (pose.mort > 0) {
        glRotatef(-90 * pose.mort, 0, 0, 1);
        glTranslatef(0, -0.15 * pose.mort, 0);
    }
    emissionBase(static_cast<float>(flash));
    rendu::modele(id).dessiner(pose);
    emissionBase(0.0f);
    glPopMatrix();
    glPopMatrix();
}

void combattant(const Combattant& c, double t) { personnage(c.id, c.pos, c.yaw, c.pose(t), c.flash()); }

void projectiles(const Combat& combat, double t) {
    for (const Projectile& p : combat.projectiles()) {
        glPushMatrix();
        glTranslatef(p.pos.x, p.hauteur, p.pos.z);
        bool roche = p.effet == Effet::Etourdissement;
        if (roche) {
            glRotatef(t * 300, 1, 0.5, 0);
            utiliserTexture(Matiere::Pierre);
            glColor3f(1, 1, 1);
            glScalef(1.0, 0.85, 1.1);
            sphere(p.rayon + 0.25, 12, 8);
            utiliserTexture(Matiere::Aucune);
        } else {
            glDisable(GL_LIGHTING);
            glDisable(GL_TEXTURE_2D);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            glDepthMask(GL_FALSE);
            for (int i = 0; i < 5; i++) {
                double k = 1.0 - i * 0.18;
                glPushMatrix();
                glTranslatef(-p.dir.x * i * 0.35, 0, -p.dir.z * i * 0.35);
                glColor4f(p.couleur.r, p.couleur.g, p.couleur.b, 0.5f * (float)k);
                sphere(p.rayon * 1.7 * k, 10, 6);
                glPopMatrix();
            }
            glDepthMask(GL_TRUE);
            glDisable(GL_BLEND);
            glColor3f(1.0f, 1.0f, 1.0f);
            sphere(p.rayon * 0.6, 10, 6);
            glEnable(GL_LIGHTING);
        }
        glPopMatrix();
    }
}

}  // namespace arene::rendu::scene
