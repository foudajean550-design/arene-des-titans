#include "Particules.hpp"

#include <algorithm>
#include <cstdlib>

#include "GL.hpp"

namespace arene::rendu {

namespace {
double alea() { return std::rand() / static_cast<double>(RAND_MAX) * 2.0 - 1.0; }
}

void Particules::jaillir(Vec2 pos, double y, Couleur c, int nombre, double vitesse) {
    for (int i = 0; i < nombre; ++i) {
        double vie = 0.4 + 0.4 * std::fabs(alea());
        liste_.push_back({pos.x, y, pos.z, alea() * vitesse, std::fabs(alea()) * vitesse + 1.0, alea() * vitesse,
                          vie, vie, 4.0 + 5.0 * std::fabs(alea()), c});
    }
}

void Particules::mettreAJour(double dt) {
    for (P& p : liste_) {
        p.vy -= 9.0 * dt;
        p.x += p.vx * dt;
        p.y += p.vy * dt;
        p.z += p.vz * dt;
        p.vie -= dt;
    }
    liste_.erase(std::remove_if(liste_.begin(), liste_.end(), [](const P& p) { return p.vie <= 0 || p.y < 0; }),
                 liste_.end());
}

void Particules::dessiner() const {
    if (liste_.empty()) return;
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glEnable(GL_POINT_SMOOTH);
    glDepthMask(GL_FALSE);
    for (const P& p : liste_) {
        glPointSize(static_cast<float>(p.taille));
        glBegin(GL_POINTS);
        glColor4f(p.c.r, p.c.g, p.c.b, static_cast<float>(p.vie / p.vieMax));
        glVertex3d(p.x, p.y, p.z);
        glEnd();
    }
    glDepthMask(GL_TRUE);
    glDisable(GL_POINT_SMOOTH);
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

}  // namespace arene::rendu
