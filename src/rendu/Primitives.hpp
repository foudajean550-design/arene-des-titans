#pragma once
#include "../noyau/Maths.hpp"

namespace arene::rendu {

void sphere(double ray, int lon, int lat);
void cocy(double raybas, double rayhaut, double h, int n);  // cylindre / cone le long de y
void disque(double ray, double h, int n, int etage);
void pave(double lx, double ly, double lz);
void orienter(double dx, double dy, double dz);  // l'axe y pointe vers (dx,dy,dz)

void couleur(Couleur c);
void emissionBase(float intensite);  // lueur rouge globale (coup recu)
void lueur(float r, float g, float b);
void finLueur();

}  // namespace arene::rendu
