#pragma once
#include <random>

#include "Combat.hpp"

namespace arene {

enum class Difficulte { Facile, Normal, Difficile };

class IA {
public:
    explicit IA(Difficulte d = Difficulte::Normal, unsigned graine = 12345);

    void reinitialiser(Difficulte d);
    Commande decider(const Combat& combat, int moi, double dt);

private:
    enum class Intention { Avancer, Attaquer, Reculer, Contourner, Parer, Tirer };

    double alea() { return loi_(moteur_); }

    std::mt19937 moteur_;
    std::uniform_real_distribution<double> loi_{0.0, 1.0};
    Intention intention_ = Intention::Avancer;
    double minuteur_ = 0, reaction_ = 0.35, probParade_ = 0.35, cote_ = 1;
};

}  // namespace arene
