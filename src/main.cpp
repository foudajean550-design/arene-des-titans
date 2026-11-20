#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <string>
#include <vector>

#include "app/Partie.hpp"
#include "noyau/Config.hpp"
#include "rendu/Capture.hpp"
#include "rendu/GL.hpp"

using namespace arene;

namespace {

Partie* partie = nullptr;
int dernierTemps = 0;

// --- mode capture (outil de developpement : --capture <dossier>) ---
struct Etape {
    std::string nom;
    int images;
    std::function<void(Partie&)> action;
};
std::vector<Etape> scenario;
std::string dossierCapture;
size_t etape = 0;
int compteur = 0;
bool attenteCapture = false;
bool modeCapture = false;

int toucheSpeciale(int k) {
    switch (k) {
        case GLUT_KEY_LEFT: return TOUCHE_GAUCHE;
        case GLUT_KEY_RIGHT: return TOUCHE_DROITE;
        case GLUT_KEY_UP: return TOUCHE_HAUT;
        case GLUT_KEY_DOWN: return TOUCHE_BAS;
        case GLUT_KEY_F2: return TOUCHE_F2;
        case GLUT_KEY_F3: return TOUCHE_F3;
        case GLUT_KEY_F4: return TOUCHE_F4;
    }
    return -1;
}

void clavier(unsigned char k, int, int) { partie->entrees().appuyer(k); }
void clavierRelache(unsigned char k, int, int) { partie->entrees().relacher(k); }
void special(int k, int, int) { partie->entrees().appuyer(toucheSpeciale(k)); }
void specialRelache(int k, int, int) { partie->entrees().relacher(toucheSpeciale(k)); }
void reshape(int l, int h) { partie->redimensionner(l, h); }

void affichage() {
    partie->afficher();
    if (modeCapture && attenteCapture) {
        const Etape& e = scenario[etape];
        if (!e.nom.empty()) {
            std::string chemin = dossierCapture + "/" + e.nom + ".ppm";
            rendu::sauverPPM(chemin.c_str(), glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
        }
        attenteCapture = false;
        ++etape;
        compteur = 0;
    }
    glutSwapBuffers();
}

void idle() {
    if (modeCapture) {
        if (attenteCapture) {
            glutPostRedisplay();
            return;
        }
        if (etape >= scenario.size()) std::exit(0);
        const Etape& e = scenario[etape];
        if (compteur == 0 && e.action) e.action(*partie);
        partie->mettreAJour(1.0 / 30.0);
        if (++compteur > e.images) attenteCapture = true;
    } else {
        int t = glutGet(GLUT_ELAPSED_TIME);
        partie->mettreAJour((t - dernierTemps) / 1000.0);
        dernierTemps = t;
    }
    if (partie->quitter()) std::exit(0);
    glutPostRedisplay();
}

void construireScenario() {
    auto touche = [](int k) { return [k](Partie& p) { p.entrees().appuyer(k); p.entrees().relacher(k); }; };
    scenario.push_back({"titre", 20, nullptr});
    scenario.push_back({"", 2, touche(13)});
    for (int i = 0; i < NB_PERSONNAGES; ++i) {
        std::string n = std::to_string(i);
        auto base = [i](Partie& p) { p.choisirCurseur(i); Pose q; q.t = 1.0; p.forcerPose(q); };
        scenario.push_back({"perso" + n + "_repos", 3, base});
        scenario.push_back({"perso" + n + "_coup", 3, [i](Partie& p) { Pose q; q.t = 1.0; q.coup = 0.42; p.forcerPose(q); }});
        scenario.push_back({"perso" + n + "_special", 3, [i](Partie& p) { Pose q; q.t = 1.0; q.special = 0.5; p.forcerPose(q); }});
        scenario.push_back({"perso" + n + "_marche", 3, [i](Partie& p) { Pose q; q.t = 1.0; q.allure = 1; q.marche = 1.0; p.forcerPose(q); }});
        scenario.push_back({"perso" + n + "_bloc", 6, [i](Partie& p) { Pose q; q.t = 1.0; q.bloc = 1; p.forcerPose(q); }});
    }
    scenario.push_back({"selection", 3, [](Partie& p) { p.libererPose(); p.choisirCurseur(3); }});
    scenario.push_back({"", 1, [](Partie& p) { p.demarrerDirect(DINOSAURE, DRAGON, true); }});
    scenario.push_back({"", 95, nullptr});
    scenario.push_back({"combat_debut", 1, nullptr});
    scenario.push_back({"", 30, [](Partie& p) {
        p.combat()->combattant(0).pos = {-1.2, 0};
        p.combat()->combattant(1).pos = {1.2, 0};
        p.combat()->combattant(0).energie = 100;
        p.entrees().appuyer('g');
    }});
    scenario.push_back({"combat_special", 1, [](Partie& p) { p.entrees().relacher('g'); }});
    scenario.push_back({"", 10, [](Partie& p) { p.entrees().appuyer('f'); }});
    scenario.push_back({"combat_impact", 1, nullptr});
    scenario.push_back({"", 60, [](Partie& p) { p.entrees().relacher('f'); p.combat()->combattant(1).pv = 3; p.entrees().appuyer('f'); }});
    scenario.push_back({"combat_ko", 1, nullptr});
    scenario.push_back({"", 160, nullptr});
    scenario.push_back({"fin_manche", 1, nullptr});
}

}  // namespace

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowPosition(100, 80);
    glutInitWindowSize(config::LARGEUR_FENETRE, config::HAUTEUR_FENETRE);
    glutCreateWindow("Arène des Titans");

    for (int i = 1; i + 1 < argc; ++i)
        if (!std::strcmp(argv[i], "--capture")) {
            modeCapture = true;
            dossierCapture = argv[i + 1];
        }

    Partie p;
    partie = &p;
    p.initialiserRendu();
    p.redimensionner(config::LARGEUR_FENETRE, config::HAUTEUR_FENETRE);
    if (modeCapture) construireScenario();

    glutIgnoreKeyRepeat(1);
    glutDisplayFunc(affichage);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(clavier);
    glutKeyboardUpFunc(clavierRelache);
    glutSpecialFunc(special);
    glutSpecialUpFunc(specialRelache);
    glutIdleFunc(idle);
    dernierTemps = glutGet(GLUT_ELAPSED_TIME);
    glutMainLoop();
    return 0;
}
