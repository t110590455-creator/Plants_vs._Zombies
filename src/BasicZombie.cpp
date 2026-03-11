//
// Created by hankl on 2026/3/10.
//
#include "BasicZombie.hpp"

BasicZombie::BasicZombie(int row, const glm::vec2& position)
    : Zombie(RESOURCE_DIR "/zombie.png", row, position, 100, 35.0f) {
}

void BasicZombie::Update() {
    Zombie::Update();
}