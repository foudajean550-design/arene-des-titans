#include "Hud.hpp"

#include "GL.hpp"

namespace arene::rendu::hud {

namespace {

std::string latin1(const std::string& s) {
    std::string o;
    for (size_t i = 0; i < s.size(); ++i) {
        unsigned char c = static_cast<unsigned char>(s[i]);
        if (c < 0x80) {
            o += static_cast<char>(c);
        } else if ((c == 0xC3 || c == 0xC2) && i + 1 < s.size()) {
            unsigned char d = static_cast<unsigned char>(s[++i]);
            o += static_cast<char>(((c & 0x03) << 6) | (d & 0x3F));
        } else {
            o += '?';
        }
    }
    return o;
}

void* choisir(void* p) { return p ? p : policeNormale(); }

}  // namespace

void* policeNormale() { return GLUT_BITMAP_HELVETICA_18; }
void* policeGrande() { return GLUT_BITMAP_TIMES_ROMAN_24; }

void debut(int largeur, int hauteur) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, largeur, hauteur, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
}

void fin() {
    glDisable(GL_LINE_SMOOTH);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void rect(double x, double y, double w, double h, Couleur c, float a) {
    glColor4f(c.r, c.g, c.b, a);
    glBegin(GL_QUADS);
    glVertex2d(x, y);
    glVertex2d(x + w, y);
    glVertex2d(x + w, y + h);
    glVertex2d(x, y + h);
    glEnd();
}

void cadre(double x, double y, double w, double h, Couleur c, float epaisseur, float a) {
    glLineWidth(epaisseur);
    glColor4f(c.r, c.g, c.b, a);
    glBegin(GL_LINE_LOOP);
    glVertex2d(x, y);
    glVertex2d(x + w, y);
    glVertex2d(x + w, y + h);
    glVertex2d(x, y + h);
    glEnd();
}

void texte(double x, double y, const std::string& s, Couleur c, void* police, float a) {
    std::string l = latin1(s);
    glColor4f(c.r, c.g, c.b, a);
    glRasterPos2d(x, y);
    for (unsigned char ch : l) glutBitmapCharacter(choisir(police), ch);
}

void texteCentre(double cx, double y, const std::string& s, Couleur c, void* police, float a) {
    std::string l = latin1(s);
    int w = glutBitmapLength(choisir(police), reinterpret_cast<const unsigned char*>(l.c_str()));
    texte(cx - w / 2.0, y, s, c, police, a);
}

void titre(double cx, double y, const std::string& s, double hauteurPx, Couleur c, float a) {
    double echelle = hauteurPx / 119.0;
    double largeur = 0;
    for (unsigned char ch : s) largeur += glutStrokeWidth(GLUT_STROKE_ROMAN, ch);
    for (int passe = 0; passe < 2; ++passe) {
        glPushMatrix();
        glTranslated(cx - largeur * echelle / 2.0, y, 0);
        glScaled(echelle, -echelle, 1);
        if (passe == 0) {
            glLineWidth(static_cast<float>(hauteurPx / 6.0 + 2));
            glColor4f(c.r * 0.25f, c.g * 0.25f, c.b * 0.25f, a);
        } else {
            glLineWidth(static_cast<float>(hauteurPx / 16.0 + 1));
            glColor4f(c.r, c.g, c.b, a);
        }
        for (unsigned char ch : s) glutStrokeCharacter(GLUT_STROKE_ROMAN, ch);
        glPopMatrix();
    }
}

void barre(double x, double y, double w, double h, double ratio, double fantome, Couleur c, bool inverse) {
    ratio = borner(ratio, 0, 1);
    fantome = borner(fantome, 0, 1);
    rect(x - 3, y - 3, w + 6, h + 6, {0, 0, 0}, 0.65f);
    auto segment = [&](double r, Couleur col, float a) {
        double lw = w * r;
        rect(inverse ? x + w - lw : x, y, lw, h, col, a);
    };
    segment(fantome, {1.0f, 0.95f, 0.8f}, 0.9f);
    segment(ratio, c, 1.0f);
    cadre(x - 3, y - 3, w + 6, h + 6, {0.9f, 0.9f, 0.9f}, 2.0f, 0.9f);
}

}  // namespace arene::rendu::hud
