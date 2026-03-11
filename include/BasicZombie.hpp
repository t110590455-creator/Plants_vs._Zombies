//
// Created by hankl on 2026/3/10.
//

#ifndef BASICZOMBIE_HPP
#define BASICZOMBIE_HPP

#include "Zombie.hpp"

class BasicZombie : public Zombie {
public:
    BasicZombie(int row, const glm::vec2& position);

    void Update() override;
};


#endif //BASICZOMBIE_HPP
