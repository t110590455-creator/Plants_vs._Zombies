//
// Created by hankl on 2026/3/10.
//

#ifndef PEA_HPP
#define PEA_HPP

#include "Projectile.hpp"

class Pea : public Projectile {
public:
    Pea(int row, const glm::vec2& position);

    void Update() override;
};

#endif //PEA_HPP
