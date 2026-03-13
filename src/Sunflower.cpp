//
// Created by hankl on 2026/3/13.
//
#include "Sunflower.hpp"

Sunflower::Sunflower(int row, int col, const glm::vec2& position)
    : Plant(RESOURCE_DIR "/sunflower.png", row, col, position, 80, 50),
      m_GenerateInterval(6.0f),
      m_LastGenerateTime(-999.0f) {
}

void Sunflower::Update() {
}

bool Sunflower::CanGenerateSun(float currentTime) const {
    return currentTime - m_LastGenerateTime >= m_GenerateInterval;
}

void Sunflower::RecordGenerateTime(float currentTime) {
    m_LastGenerateTime = currentTime;
}