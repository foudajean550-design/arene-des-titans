#pragma once
#include "Modele.hpp"

namespace arene::rendu {

class Dinosaure : public Modele {
public:
    void dessiner(const Pose& p) const override;
};
class Robot : public Modele {
public:
    void dessiner(const Pose& p) const override;
};
class Araignee : public Modele {
public:
    void dessiner(const Pose& p) const override;
};
class Dragon : public Modele {
public:
    void dessiner(const Pose& p) const override;
};
class Golem : public Modele {
public:
    void dessiner(const Pose& p) const override;
};

}  // namespace arene::rendu
