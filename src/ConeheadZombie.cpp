//
// Created by hankl on 2026/3/20.
//
#include "ConeheadZombie.hpp"

ConeheadZombie::ConeheadZombie(int row, const glm::vec2& position)
    : Zombie(RESOURCE_DIR "/ConeheadZombie/ConeheadZombie_0.png", row, position, 220, 30.0f) {
}

void ConeheadZombie::Update() {
    Zombie::Update();
}