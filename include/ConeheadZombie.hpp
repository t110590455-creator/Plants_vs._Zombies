//
// Created by hankl on 2026/3/20.
//

#ifndef CONEHEADZOMBIE_HPP
#define CONEHEADZOMBIE_HPP

#include "Zombie.hpp"

class ConeheadZombie : public Zombie {
public:
    ConeheadZombie(int row, const glm::vec2& position);

    void Update() override;
};

#endif //CONEHEADZOMBIE_HPP
