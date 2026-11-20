#pragma once
#include <vector>

#include "../noyau/Maths.hpp"

namespace arene::rendu {

class Particules {
public:
    void jaillir(Vec2 pos, double y, Couleur c, int nombre, double vitesse);
    void mettreAJour(double dt);
    void dessiner() const;
    void vider() { liste_.clear(); }

private:
    struct P {
        double x, y, z, vx, vy, vz, vie, vieMax, taille;
        Couleur c;
    };
    std::vector<P> liste_;
};

}  // namespace arene::rendu
