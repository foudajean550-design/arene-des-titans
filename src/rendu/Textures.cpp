#include "Textures.hpp"

#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

#include "GL.hpp"
#include "../noyau/Maths.hpp"

#ifndef DOSSIER_ASSETS
#define DOSSIER_ASSETS "assets"
#endif

namespace arene::rendu {

namespace {

constexpr int T = 64;
GLuint ids[static_cast<int>(Matiere::NB)] = {};

struct RVB {
    double r, g, b;
};

unsigned hachage(int x, int y, int graine) {
    unsigned h = static_cast<unsigned>(x) * 374761393u + static_cast<unsigned>(y) * 668265263u +
                 static_cast<unsigned>(graine) * 2246822519u;
    h = (h ^ (h >> 13)) * 1274126177u;
    return h ^ (h >> 16);
}

double alea(int x, int y, int g) { return (hachage(x, y, g) & 0xFFFF) / 65535.0; }

int modulo(int a, int n) { return ((a % n) + n) % n; }

// bruit de valeur periodique sur T pixels
double bruit(double x, double y, int periode, int g) {
    double u = x * periode / T, v = y * periode / T;
    int x0 = static_cast<int>(std::floor(u)), y0 = static_cast<int>(std::floor(v));
    double fx = u - x0, fy = v - y0;
    fx = fx * fx * (3 - 2 * fx);
    fy = fy * fy * (3 - 2 * fy);
    int xa = modulo(x0, periode), xb = modulo(x0 + 1, periode);
    int ya = modulo(y0, periode), yb = modulo(y0 + 1, periode);
    double a = alea(xa, ya, g), b = alea(xb, ya, g), c = alea(xa, yb, g), d = alea(xb, yb, g);
    return (a + (b - a) * fx) * (1 - fy) + (c + (d - c) * fx) * fy;
}

RVB mul(RVB c, double k) { return {c.r * k, c.g * k, c.b * k}; }

RVB ecailles(int x, int y, RVB base, int cellules, int g) {
    int w = T / cellules;
    int ligne = y / w;
    int decal = (ligne % 2) * (w / 2);
    int xx = (x + decal) % T;
    double fx = (xx % w) / static_cast<double>(w);
    double fy = (y % w) / static_cast<double>(w);
    double d = std::hypot(fx - 0.5, (fy - 0.35) * 1.1);
    double ombre = borner(1.0 - d * 1.4, 0.3, 1.0);
    double n = alea(xx / w, ligne, g);
    return mul(base, (0.55 + 0.45 * ombre) * (0.82 + 0.3 * n));
}

RVB motif(Matiere m, int x, int y) {
    switch (m) {
        case Matiere::Ecailles: return ecailles(x, y, {0.30, 0.62, 0.28}, 8, 1);
        case Matiere::Dragon: return ecailles(x, y, {0.75, 0.18, 0.12}, 8, 2);
        case Matiere::Chitine: {
            RVB c = ecailles(x, y, {0.55, 0.32, 0.75}, 16, 3);
            double l = 0.9 + 0.2 * bruit(x, y, 8, 4);
            return mul(c, l);
        }
        case Matiere::Peau: {
            double n = 0.75 + 0.35 * bruit(x, y, 4, 5) + 0.12 * bruit(x, y, 16, 6);
            return mul({0.80, 0.68, 0.45}, n);
        }
        case Matiere::Metal: {
            double rayure = 0.88 + 0.12 * alea(x / 8, y, 7);
            double bord = (x % 32 < 2 || y % 32 < 2) ? 0.45 : 1.0;
            bool rivet = (x % 32 >= 4 && x % 32 < 7 && y % 32 >= 4 && y % 32 < 7);
            double k = rayure * bord * (rivet ? 1.35 : 1.0);
            return mul({0.62, 0.68, 0.78}, k);
        }
        case Matiere::Pierre: {
            double n = 0.55 * bruit(x, y, 4, 8) + 0.3 * bruit(x, y, 8, 9) + 0.2 * bruit(x, y, 16, 10);
            double fissure = std::fabs(bruit(x, y, 8, 11) - 0.5) < 0.025 ? 0.5 : 1.0;
            return mul({0.62, 0.58, 0.52}, (0.6 + 0.6 * n) * fissure);
        }
        case Matiere::Membrane: {
            double v = 0.8 + 0.2 * std::sin((x + y) * 0.39 + 6 * bruit(x, y, 4, 12));
            return mul({0.85, 0.30, 0.18}, v * (0.9 + 0.2 * bruit(x, y, 8, 13)));
        }
        case Matiere::Sol: {
            int tx = x / 16, ty = y / 16;
            bool joint = (x % 16 < 1) || (y % 16 < 1);
            double n = 0.75 + 0.25 * alea(tx, ty, 14) + 0.15 * (alea(x, y, 15) - 0.5);
            return mul({0.42, 0.40, 0.45}, joint ? 0.35 : n);
        }
        default: return {1, 1, 1};
    }
}

int lireEntier(FILE* f) {
    int c = fgetc(f);
    while (c == '#' || std::isspace(c)) {
        if (c == '#') {
            while (c != '\n' && c != EOF) c = fgetc(f);
        } else {
            c = fgetc(f);
        }
    }
    int v = 0;
    while (std::isdigit(c)) {
        v = v * 10 + (c - '0');
        c = fgetc(f);
    }
    return v;
}

bool chargerPPM(const std::string& chemin, std::vector<unsigned char>& pixels, int& w, int& h) {
    FILE* f = std::fopen(chemin.c_str(), "rb");
    if (!f) return false;
    bool ok = false;
    if (std::fgetc(f) == 'P' && std::fgetc(f) == '6') {
        w = lireEntier(f);
        h = lireEntier(f);
        lireEntier(f);
        if (w > 0 && h > 0) {
            pixels.resize(static_cast<size_t>(w) * h * 3);
            ok = std::fread(pixels.data(), 1, pixels.size(), f) == pixels.size();
        }
    }
    std::fclose(f);
    return ok;
}

const char* nomFichier(Matiere m) {
    static const char* noms[] = {"", "ecailles", "peau", "metal", "chitine", "pierre", "dragon", "membrane", "sol"};
    return noms[static_cast<int>(m)];
}

GLuint creer(Matiere m) {
    std::vector<unsigned char> pixels;
    int w = T, h = T;
    std::string chemin = std::string(DOSSIER_ASSETS) + "/textures/" + nomFichier(m) + ".ppm";
    if (!chargerPPM(chemin, pixels, w, h)) {
        w = h = T;
        pixels.resize(T * T * 3);
        for (int y = 0; y < T; y++)
            for (int x = 0; x < T; x++) {
                RVB c = motif(m, x, y);
                unsigned char* p = &pixels[(y * T + x) * 3];
                p[0] = static_cast<unsigned char>(borner(c.r, 0, 1) * 255);
                p[1] = static_cast<unsigned char>(borner(c.g, 0, 1) * 255);
                p[2] = static_cast<unsigned char>(borner(c.b, 0, 1) * 255);
            }
    }
    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, w, h, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
    return id;
}

}  // namespace

void chargerTextures() {
    for (int i = 1; i < static_cast<int>(Matiere::NB); i++) ids[i] = creer(static_cast<Matiere>(i));
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

void utiliserTexture(Matiere m) {
    if (m == Matiere::Aucune) {
        glDisable(GL_TEXTURE_2D);
    } else {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, ids[static_cast<int>(m)]);
    }
}

}  // namespace arene::rendu
