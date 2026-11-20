#pragma once
#include "../jeu/Combat.hpp"

namespace arene::rendu::scene {

void ciel(double t);
void arene(double t);
void personnage(int id, Vec2 pos, double yaw, const Pose& pose, double flash);
void combattant(const Combattant& c, double t);
void projectiles(const Combat& combat, double t);

}  // namespace arene::rendu::scene
