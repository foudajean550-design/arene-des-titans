#pragma once
#include <memory>

#include "../jeu/Combat.hpp"
#include "../jeu/IA.hpp"
#include "../noyau/Entrees.hpp"
#include "../rendu/Camera.hpp"
#include "../rendu/Particules.hpp"

namespace arene {

enum class Ecran { Titre, Selection, Combat, Pause, FinMatch };

class Partie {
public:
    Partie();

    void initialiserRendu();
    void redimensionner(int largeur, int hauteur);
    void mettreAJour(double dt);
    void afficher();

    Entrees& entrees() { return entrees_; }
    bool quitter() const { return quitter_; }

    // outils de developpement (captures, demonstrations)
    void demarrerDirect(int perso0, int perso1, bool solo);
    void choisirCurseur(int perso);
    void forcerPose(const Pose& p) { pose_ = p; poseForcee_ = true; }
    void libererPose() { poseForcee_ = false; }
    Ecran ecran() const { return ecran_; }
    Combat* combat() { return combat_.get(); }

private:
    void majTitre();
    void majSelection(double dt);
    void majCombat(double dt);
    void majFinMatch();
    void lancerMatch();
    void lancerRound();
    void finRound();
    void traiterEvenements();
    void suivreCombat();
    void aller(Ecran e);
    Commande lireCommande(int joueur) const;

    void dessinerMonde();

    Ecran ecran_ = Ecran::Titre;
    Entrees entrees_;
    rendu::Camera camera_;
    rendu::Particules particules_;
    std::unique_ptr<Combat> combat_;
    IA ia_;

    int largeur_, hauteur_;
    double temps_ = 0, angleLumiere_ = 0;
    int menuIndex_ = 0;
    Difficulte difficulte_ = Difficulte::Normal;
    bool solo_ = true;
    int curseur_ = 0, phaseSel_ = 0, choix_[2] = {0, 1};
    int manches_[2] = {0, 0}, numeroManche_ = 1, vainqueurMatch_ = 0;
    double pvAff_[2] = {1, 1};
    double echelleTemps_ = 1, dureeRalenti_ = 0;
    int presetCamera_ = 0;
    bool filaire_ = false, quitter_ = false;
    bool poseForcee_ = false;
    Pose pose_;
};

}  // namespace arene
