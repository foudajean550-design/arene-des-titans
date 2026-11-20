#pragma once
#include "../jeu/Combat.hpp"
#include "../jeu/IA.hpp"

namespace arene::rendu::ecrans {

void titre(int l, int h, int index, Difficulte diff, double t);
void selection(int l, int h, int curseur, int phase, bool solo, double t);
void combat(int l, int h, const Combat& c, const double pvAff[2], const int manches[2], bool solo,
            int numeroManche);
void pause(int l, int h);
void finMatch(int l, int h, int vainqueur, bool solo, const int choix[2], const int manches[2]);

}  // namespace arene::rendu::ecrans
