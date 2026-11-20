#pragma once
#include "../jeu/Pose.hpp"

namespace arene::rendu {

// Convention : le modele regarde vers -x, ses pieds sont en y = 0, hauteur ~3.
class Modele {
public:
    virtual ~Modele() = default;
    virtual void dessiner(const Pose& p) const = 0;
};

const Modele& modele(int id);

}  // namespace arene::rendu
