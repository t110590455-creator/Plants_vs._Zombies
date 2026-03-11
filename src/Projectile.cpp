//
// Created by hankl on 2026/3/10.
//
#include "Projectile.hpp"

#include "Util/Time.hpp"

Projectile::Projectile(const std::string& imagePath,
                       int row,
                       const glm::vec2& position,
                       int damage,
                       float speed)
    : Util::GameObject(std::make_shared<Util::Image>(imagePath), 11.0f),
      m_Row(row),
      m_Damage(damage),
      m_Speed(speed),
      m_Alive(true) {
    m_Transform.translation = position;
}

void Projectile::Update() {
    const float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;
    m_Transform.translation.x += m_Speed * deltaTime;
}