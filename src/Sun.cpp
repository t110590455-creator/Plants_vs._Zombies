//
// Created by hankl on 2026/3/13.
//
#include "Sun.hpp"

Sun::Sun(const glm::vec2& position, int value)
    : Util::GameObject(std::make_shared<Util::Image>(RESOURCE_DIR "/sun.png"), 12.0f),
      m_Value(value),
      m_Alive(true),
      m_HalfWidth(40.0f),
      m_HalfHeight(40.0f) {
    m_Transform.translation = position;
}

void Sun::Collect() {
    m_Alive = false;
}

bool Sun::ContainsPoint(const glm::vec2& point) const {
    const float left = m_Transform.translation.x - m_HalfWidth;
    const float right = m_Transform.translation.x + m_HalfWidth;
    const float top = m_Transform.translation.y - m_HalfHeight;
    const float bottom = m_Transform.translation.y + m_HalfHeight;

    return point.x >= left && point.x <= right &&
           point.y >= top && point.y <= bottom;
}