#pragma once
#include "../noyau/Maths.hpp"

namespace arene::rendu {

class Camera {
public:
    void mettreAJour(double dt);
    void placer() const;
    void secouer(double amplitude);
    void saut();  // positionne directement sur les valeurs cibles

    Vec2 droite() const;  // vecteur ecran-droite dans le plan xz
    Vec2 avant() const;   // vecteur ecran-avant dans le plan xz

    double angleH = 0, angleV = 15, distance = 14;
    double cibleH = 0, cibleV = 15, cibleDistance = 14;
    double cx = 0, cy = 1.4, cz = 0;
    double tx = 0, ty = 1.4, tz = 0;
    double tremblement = 0;
};

}  // namespace arene::rendu
