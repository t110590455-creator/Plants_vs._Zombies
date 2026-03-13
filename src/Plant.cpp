//
// Created by hankl on 2026/3/10.
//
#include "Plant.hpp"

Plant::Plant(const std::string& imagePath,
             int row,
             int col,
             const glm::vec2& position,
             int hp,
             int cost)
    : Util::GameObject(std::make_shared<Util::Image>(imagePath), 10.0f),
      m_Row(row),
      m_Col(col),
      m_HP(hp),
      m_Cost(cost),
      m_Alive(true) {
    m_Transform.translation = position;
}

void Plant::TakeDamage(int damage) {
    if (!m_Alive) {
        return;
    }

    m_HP -= damage;
    if (m_HP <= 0) {
        m_HP = 0;
        m_Alive = false;
    }
}