//
// Created by hankl on 2026/3/20.
//
#include "LawnMower.hpp"

#include "Util/Time.hpp"

LawnMower::LawnMower(int row, const glm::vec2& position)
    : Util::GameObject(std::make_shared<Util::Image>(RESOURCE_DIR "/lawn_mower.png"), 9.0f),
      m_Row(row),
      m_Activated(false),
      m_Alive(true),
      m_Speed(500.0f) {
    m_Transform.translation = position;
}

void LawnMower::Update() {
    if (!m_Alive || !m_Activated) {
        return;
    }

    const float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;
    m_Transform.translation.x += m_Speed * deltaTime;
}

void LawnMower::Activate() {
    if (!m_Alive) {
        return;
    }
    m_Activated = true;
}

void LawnMower::Destroy() {
    m_Alive = false;
}