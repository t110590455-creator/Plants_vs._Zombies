//
// Created by hankl on 2026/3/10.
//
#include "Peashooter.hpp"

Peashooter::Peashooter(int row, int col, const glm::vec2& position)
    : Plant(RESOURCE_DIR "/plant.png", row, col, position, 100, 100),
      m_ShootInterval(1.4f),
      m_LastShootTime(-999.0f) {
}

void Peashooter::Update() {
}

bool Peashooter::CanShoot(float currentTime) const {
    return currentTime - m_LastShootTime >= m_ShootInterval;
}

void Peashooter::RecordShootTime(float currentTime) {
    m_LastShootTime = currentTime;
}