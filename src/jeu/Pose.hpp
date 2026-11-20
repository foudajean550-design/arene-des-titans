#pragma once
#include <cmath>

#include "../noyau/Maths.hpp"

namespace arene {

struct Pose {
    double t = 0;         // temps global (s)
    double marche = 0;    // phase du cycle de marche (rad)
    double allure = 0;    // intensite de la marche, 0..1
    double coup = 0;      // progression de l'attaque, 0..1 (0 si inactif)
    double special = 0;   // progression du special, 0..1
    double bloc = 0;      // garde, 0..1
    double touche = 0;    // recul apres un coup, 0..1
    double mort = 0;      // chute, 0..1
    double victoire = 0;  // 1 si vainqueur
};

inline double bosse(double p) { return (p > 0 && p < 1) ? std::sin(PI * p) : 0.0; }

inline double enveloppeCoup(double p) {
    if (p <= 0 || p >= 1) return 0;
    return p < 0.4 ? p / 0.4 : (1 - p) / 0.6;
}

}  // namespace arene
