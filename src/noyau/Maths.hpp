#pragma once
#include <cmath>

namespace arene {

constexpr double PI = 3.14159265358979323846;

inline double radians(double d) { return d * PI / 180.0; }
inline double degres(double r) { return r * 180.0 / PI; }
inline double borner(double v, double a, double b) { return v < a ? a : (v > b ? b : v); }
inline double melange(double a, double b, double t) { return a + (b - a) * t; }
inline double lisser(double actuel, double cible, double raideur, double dt) {
    return actuel + (cible - actuel) * (1.0 - std::exp(-raideur * dt));
}

struct Couleur {
    float r, g, b;
};

struct Vec2 {
    double x = 0, z = 0;
    Vec2() = default;
    Vec2(double x_, double z_) : x(x_), z(z_) {}
    Vec2 operator+(Vec2 o) const { return {x + o.x, z + o.z}; }
    Vec2 operator-(Vec2 o) const { return {x - o.x, z - o.z}; }
    Vec2 operator*(double k) const { return {x * k, z * k}; }
    double longueur() const { return std::sqrt(x * x + z * z); }
    Vec2 normalise() const {
        double l = longueur();
        return l > 1e-9 ? Vec2{x / l, z / l} : Vec2{};
    }
};

inline double distance(Vec2 a, Vec2 b) { return (a - b).longueur(); }

}  // namespace arene
