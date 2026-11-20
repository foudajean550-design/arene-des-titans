#include "Camera.hpp"

#include <cstdlib>

#include "GL.hpp"

namespace arene::rendu {

namespace {
double alea() { return std::rand() / static_cast<double>(RAND_MAX) - 0.5; }
}

void Camera::mettreAJour(double dt) {
    double dh = std::fmod(cibleH - angleH, 360.0);
    if (dh > 180) dh -= 360;
    if (dh < -180) dh += 360;
    angleH += dh * (1.0 - std::exp(-4.0 * dt));
    angleV = lisser(angleV, cibleV, 4, dt);
    distance = lisser(distance, cibleDistance, 3, dt);
    cx = lisser(cx, tx, 6, dt);
    cy = lisser(cy, ty, 6, dt);
    cz = lisser(cz, tz, 6, dt);
    tremblement = std::max(0.0, tremblement - dt * 1.5);
}

void Camera::saut() {
    angleH = cibleH;
    angleV = cibleV;
    distance = cibleDistance;
    cx = tx;
    cy = ty;
    cz = tz;
}

void Camera::secouer(double amplitude) { tremblement = std::max(tremblement, amplitude); }

void Camera::placer() const {
    double h = radians(angleH), v = radians(angleV);
    double x = distance * std::cos(v) * std::sin(h);
    double y = distance * std::sin(v);
    double z = distance * std::cos(v) * std::cos(h);
    double sx = alea() * tremblement, sy = alea() * tremblement;
    gluLookAt(cx + x + sx, cy + y + sy, cz + z, cx, cy, cz, 0, 1, 0);
}

Vec2 Camera::droite() const {
    double h = radians(angleH);
    return {std::cos(h), -std::sin(h)};
}

Vec2 Camera::avant() const {
    double h = radians(angleH);
    return {-std::sin(h), -std::cos(h)};
}

}  // namespace arene::rendu
