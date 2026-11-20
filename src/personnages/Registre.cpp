#include "Modele.hpp"
#include "Modeles.hpp"
#include "../jeu/Fiche.hpp"

namespace arene::rendu {

const Modele& modele(int id) {
    static const Dinosaure dino;
    static const Robot robot;
    static const Araignee araignee;
    static const Dragon dragon;
    static const Golem golem;
    switch (id) {
        case ROBOT: return robot;
        case ARAIGNEE: return araignee;
        case DRAGON: return dragon;
        case GOLEM: return golem;
        default: return dino;
    }
}

}  // namespace arene::rendu
