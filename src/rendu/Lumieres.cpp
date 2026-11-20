#include "Lumieres.hpp"

#include <cmath>

#include "../noyau/Maths.hpp"
#include "GL.hpp"
#include "Primitives.hpp"

namespace arene::rendu {

namespace {
bool soleil = true, spot = true;
}

void initLumieres() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    GLfloat specMat[] = {0.3f, 0.3f, 0.3f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specMat);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 30.0f);

    GLfloat amb0[] = {0.28f, 0.28f, 0.32f, 1.0f};
    GLfloat dif0[] = {0.75f, 0.75f, 0.8f, 1.0f};
    GLfloat spe0[] = {0.5f, 0.5f, 0.5f, 1.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT, amb0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, dif0);
    glLightfv(GL_LIGHT0, GL_SPECULAR, spe0);

    GLfloat amb1[] = {0.0f, 0.0f, 0.0f, 1.0f};
    GLfloat dif1[] = {1.0f, 0.75f, 0.4f, 1.0f};
    GLfloat spe1[] = {1.0f, 0.8f, 0.5f, 1.0f};
    glLightfv(GL_LIGHT1, GL_AMBIENT, amb1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, dif1);
    glLightfv(GL_LIGHT1, GL_SPECULAR, spe1);
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 55.0f);
    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 3.0f);
}

void placerLumieres(double angleLumiere) {
    GLfloat pos0[] = {-0.6f, 1.0f, 1.0f, 0.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, pos0);
    double a = radians(angleLumiere);
    GLfloat pos1[] = {static_cast<GLfloat>(7 * std::cos(a)), 7.0f, static_cast<GLfloat>(7 * std::sin(a)), 1.0f};
    GLfloat dir1[] = {-pos1[0], -pos1[1], -pos1[2]};
    glLightfv(GL_LIGHT1, GL_POSITION, pos1);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, dir1);
}

void dessinerLampe(double angleLumiere) {
    double a = radians(angleLumiere);
    glPushMatrix();
    glTranslatef(7 * std::cos(a), 7.0, 7 * std::sin(a));
    glColor3f(1.0f, 0.9f, 0.4f);
    sphere(0.2, 10, 6);
    glPopMatrix();
}

void basculerSoleil() {
    soleil = !soleil;
    if (soleil) glEnable(GL_LIGHT0);
    else glDisable(GL_LIGHT0);
}

void basculerSpot() {
    spot = !spot;
    if (spot) glEnable(GL_LIGHT1);
    else glDisable(GL_LIGHT1);
}

}  // namespace arene::rendu
