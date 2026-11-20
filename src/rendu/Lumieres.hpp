#pragma once

namespace arene::rendu {

void initLumieres();
void placerLumieres(double angleLumiere);  // a appeler apres la camera
void dessinerLampe(double angleLumiere);
void basculerSoleil();
void basculerSpot();

}  // namespace arene::rendu
