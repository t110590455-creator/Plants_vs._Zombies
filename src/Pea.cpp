//
// Created by hankl on 2026/3/10.
//
#include "Pea.hpp"

Pea::Pea(int row, const glm::vec2& position)
    : Projectile(RESOURCE_DIR "/pea.png", row, position, 20, 250.0f) {
}

void Pea::Update() {
    Projectile::Update();
}