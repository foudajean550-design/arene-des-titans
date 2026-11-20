#include "Primitives.hpp"

#include <cmath>

#include "GL.hpp"

namespace arene::rendu {

namespace {

struct Sommet {
    double x, y, z;
};

const Sommet pCube[8] = {{-0.5, -0.5, 0.5}, {0.5, -0.5, 0.5},  {0.5, -0.5, -0.5}, {-0.5, -0.5, -0.5},
                         {-0.5, 0.5, 0.5},  {0.5, 0.5, 0.5},   {0.5, 0.5, -0.5},  {-0.5, 0.5, -0.5}};
const int fCube[6][4] = {{0, 3, 2, 1}, {0, 1, 5, 4}, {1, 2, 6, 5}, {2, 3, 7, 6}, {0, 4, 7, 3}, {4, 5, 6, 7}};
const float nCube[6][3] = {{0, -1, 0}, {0, 0, 1}, {1, 0, 0}, {0, 0, -1}, {-1, 0, 0}, {0, 1, 0}};
const int uvCube[4][2] = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};

float baseEmission[4] = {0, 0, 0, 1};

void psph(double ray, double u, double v) {
    double x = ray * std::cos(v) * std::cos(u);
    double y = ray * std::sin(v);
    double z = ray * std::cos(v) * std::sin(u);
    glNormal3f(x, y, z);
    glTexCoord2f(u / (2 * PI), (v + PI / 2) / PI);
    glVertex3f(x, y, z);
}

void pscyl(double raybas, double rayhaut, double h, double u, double v) {
    double rayon = raybas + v * (rayhaut - raybas);
    double lon = std::sqrt(h * h + (raybas - rayhaut) * (raybas - rayhaut));
    glNormal3f(h * std::cos(u) / lon, (raybas - rayhaut) / lon, h * std::sin(u) / lon);
    glTexCoord2f(u / (2 * PI), v);
    glVertex3f(rayon * std::cos(u), h * v, rayon * std::sin(u));
}

}  // namespace

void sphere(double ray, int lon, int lat) {
    glBegin(GL_QUADS);
    for (int i = 0; i < lon; i++) {
        double u0 = i * 2 * PI / lon;
        double u1 = (i + 1) * 2 * PI / lon;
        for (int j = 0; j < lat; j++) {
            double v0 = -PI / 2 + j * PI / lat;
            double v1 = -PI / 2 + (j + 1) * PI / lat;
            psph(ray, u0, v0);
            psph(ray, u1, v0);
            psph(ray, u1, v1);
            psph(ray, u0, v1);
        }
    }
    glEnd();
}

void disque(double ray, double h, int n, int etage) {
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, etage == 1 ? 1 : -1, 0);
    glTexCoord2f(0.5, 0.5);
    glVertex3f(0, h, 0);
    for (int i = 0; i <= n; i++) {
        double u = (etage == 1 ? i : -i) * 2 * PI / n;
        glTexCoord2f(0.5 + 0.5 * std::cos(u), 0.5 + 0.5 * std::sin(u));
        glVertex3f(ray * std::cos(u), h, ray * std::sin(u));
    }
    glEnd();
}

void cocy(double raybas, double rayhaut, double h, int n) {
    glBegin(GL_QUADS);
    for (int i = 0; i < n; i++) {
        double u0 = i * 2 * PI / n;
        double u1 = (i + 1) * 2 * PI / n;
        pscyl(raybas, rayhaut, h, u0, 0);
        pscyl(raybas, rayhaut, h, u1, 0);
        pscyl(raybas, rayhaut, h, u1, 1);
        pscyl(raybas, rayhaut, h, u0, 1);
    }
    glEnd();
    disque(raybas, 0, n, 0);
    if (rayhaut > 0) disque(rayhaut, h, n, 1);
}

void pave(double lx, double ly, double lz) {
    for (int i = 0; i < 6; i++) {
        glNormal3f(nCube[i][0], nCube[i][1], nCube[i][2]);
        glBegin(GL_QUADS);
        for (int j = 0; j < 4; j++) {
            glTexCoord2f(uvCube[j][0], uvCube[j][1]);
            const Sommet& s = pCube[fCube[i][j]];
            glVertex3f(s.x * lx, s.y * ly, s.z * lz);
        }
        glEnd();
    }
}

void orienter(double dx, double dy, double dz) {
    double n = std::sqrt(dx * dx + dy * dy + dz * dz);
    double ang = std::acos(dy / n) * 180 / PI;
    if (std::fabs(dx) + std::fabs(dz) > 1e-6)
        glRotatef(ang, dz, 0, -dx);
    else if (dy < 0)
        glRotatef(180, 1, 0, 0);
}

void couleur(Couleur c) { glColor3f(c.r, c.g, c.b); }

void emissionBase(float intensite) {
    baseEmission[0] = intensite;
    baseEmission[1] = intensite * 0.1f;
    baseEmission[2] = intensite * 0.1f;
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, baseEmission);
}

void lueur(float r, float g, float b) {
    const GLfloat e[4] = {r, g, b, 1};
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, e);
}

void finLueur() { glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, baseEmission); }

}  // namespace arene::rendu
