//
// Created by hankl on 2026/3/10.
//

#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include "pch.hpp"

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"

class Projectile : public Util::GameObject {
public:
    Projectile(const std::string& imagePath,
               int row,
               const glm::vec2& position,
               int damage,
               float speed);

    virtual ~Projectile() = default;

    virtual void Update();
    virtual void Destroy() { m_Alive = false; }

    int GetRow() const { return m_Row; }
    int GetDamage() const { return m_Damage; }
    bool IsAlive() const { return m_Alive; }

protected:
    int m_Row;
    int m_Damage;
    float m_Speed;
    bool m_Alive;
};

#endif //PROJECTILE_HPP
